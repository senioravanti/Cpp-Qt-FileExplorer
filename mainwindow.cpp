#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QFileInfo>
#include <QDirIterator>

#include <QIcon>

#include <QPushButton>

#include <QFileInfo>

#include <QDesktopServices>
#include <QUrl>

#include <QMouseEvent>
#include <QMenu>
#include <QInputDialog>

#include <QLineEdit>
#include "explorertab.h"


MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent), ui (new Ui::MainWindow) {
  ui->setupUi (this);

  process = new QProcess();

  homePath = QDir::homePath();

  visitedPaths = QList<QString>(homePath);
  poppedPath = "";
  ui->rightArrowButton->setEnabled(false);
  ui->leftArrowButton->setEnabled(false);

  on_itemsSelected(false);
  ui->actionPaste->setEnabled(false);

  fileSystemModel = new QFileSystemModel(this);
  fileSystemModel->setRootPath("");
  fileSystemModel->setReadOnly(false);

  ui->treeView->setModel(fileSystemModel);
  ui->treeView->setRootIndex(fileSystemModel->index(""));

  // treeViewDelegate = new MyDelegate(ui->treeView);
  // ui->treeView->setItemDelegate(treeViewDelegate);

  // Правильно скрываем лишние столбцы
  QHeaderView * header = ui->treeView->header();
  for (int i = 1; i < header->count(); ++i) {
    header->hideSection(i);
  }
  ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);

  // Путь
  ui->adressBar->setText(QDir::homePath());

  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 3);

  ui->tabWidget->clear();
  initContextMenu();
  on_addTabAction_triggered();

  // --- Сигналы и слоты
  connect(ui->adressBar, &QLineEdit::returnPressed, this, [=](){
    on_changeDir(ui->adressBar->text(), ChangeDirSource::AdressBar);
  });
  connect(ui->upArrowButton, &QPushButton::pressed, this, [=](){
    QDir dir(ui->adressBar->text());
    dir.cdUp();
    ui->adressBar->setText(dir.absolutePath());
    on_changeDir(dir.absolutePath(), ChangeDirSource::NavButton);
  });
  connect(ui->treeView, &QAbstractItemView::clicked, this, [=](const QModelIndex &index){
    on_changeDir(fileSystemModel->filePath(index), ChangeDirSource::TreeView);
  });

  // undo & redo
  connect(ui->leftArrowButton, &QPushButton::clicked, this, [=](){

    // Один элемент в списке у меня есть всегда, а на момент этой операции в списке у меня не меньше чем 2 элемента.
    poppedPath = visitedPaths.last();
    visitedPaths.removeLast();
    // Нажали на кнопку один раз -- включили ->
    if (!ui->rightArrowButton->isEnabled())
      ui->rightArrowButton->setEnabled(true);

    if (visitedPaths.size() == 1)
      ui->leftArrowButton->setEnabled(false);

    // Последний элемент есть всегда
    on_changeDir(visitedPaths.last(), ChangeDirSource::NavButton);
  });
  connect(ui->rightArrowButton, &QPushButton::clicked, this, [=](){
    visitedPaths.append(poppedPath);
    ui->rightArrowButton->setEnabled(false);

    if (!ui->leftArrowButton->isEnabled()) {
      ui->leftArrowButton->setEnabled(true);
    }
    // Переходим по извлеченному пути
    on_changeDir(poppedPath, ChangeDirSource::NavButton);
  });
  // Операции с файлами

  connect(ui->actionRenameItem, &QAction::triggered, this, [=](){
    curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());
    MyListView * listView = curTab->getListView();
    QModelIndex curIndex = listView->currentIndex();
    if (curIndex.isValid())
      listView->edit(curIndex);
  });
  connect(ui->actionNewFolder, &QAction::triggered, this, &MainWindow::on_createFolderAction);
  connect(ui->actionNewFile, &QAction::triggered, this, &MainWindow::on_createFileAction);

  // Удаляем файлы и каталоги
  connect(ui->actionToTrash, &QAction::triggered, this, [=](){
    removeFilesAndDirectoriesRecursively(false);
  });
  connect(ui->actionRemove, &QAction::triggered, this, [=](){
    removeFilesAndDirectoriesRecursively(true);
  });

  // Копируем и перемещаем файлы и каталоги
  connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::on_copyAction_triggered);
  connect(ui->actionCut, &QAction::triggered, this, &MainWindow::on_cutAction_triggered);
  connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::on_pasteAction_triggered);

  // Работа с вкладками (сигналы)
  connect(ui->actionNewTab, &QAction::triggered, this, &MainWindow::on_addTabAction_triggered);
  // Привязанный сигнал вызывается когда на вкладке нажата кнопка "закрыть"
  connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::on_closeTabAction_triggered);
  connect(ui->actionCloseTab, &QAction::triggered, this, [=](){

    int index = ui->tabWidget->currentIndex();

    if (index != -1) {
      on_closeTabAction_triggered(index);
    }

  });

  connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::on_currentTabChanged);

  // Открываем терминал в текущем каталоге.
  connect(ui->actionOpenInTerminal, &QAction::triggered, this, &MainWindow::on_openInTerminalAction_triggered);

  // Открываю элемент файловой системы по двойному щелчку мыши.
  // Вид
  connect(ui->actionStandartView, &QAction::triggered, this, [=](){
    on_changeView(QListView::ListMode);
  });
  connect(ui->actionIconsView, &QAction::triggered, this, [=](){
    on_changeView(QListView::IconMode);
  });

}

void MainWindow::on_createFolderAction() {

  QString folderName = "", prefix = "Новый каталог";
  QModelIndex newFolderIndex;
  int i = 0;
  do {
    if (i) folderName = prefix + " " + QString::number(i);
    else folderName = prefix;
    newFolderIndex = fileSystemModel->mkdir(fileSystemModel->index(ui->adressBar->text()), folderName);
    i++;

  } while(!newFolderIndex.isValid());

  curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());
  curTab->getListView()->edit(newFolderIndex);
}

void MainWindow::on_createFileAction() {
  QString fileName = "", prefix = "Новый файл";
  QString curDir = ui->adressBar->text();
  QFile newFile; //QFileInfo newFileInfo;
  // Генерируем уникальное стандартное имя файла
  int i = 0;
  do {
    if (i)
      fileName = curDir + "/" + prefix + " " + QString::number(i);
    else
      fileName = curDir + "/" + prefix;

    newFile.setFileName(fileName);
    i++;

  } while(newFile.exists());

  if (newFile.open(QIODeviceBase::WriteOnly)) {
    newFile.close();
    curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());
    curTab->getListView()->edit(fileSystemModel->index(fileName));
  }
}

void
MainWindow::moveDirectoryContentsToTrashRecursively (const QString& dirPath) {
  QDir dir(dirPath);
  if (!dir.exists()) return;

  QList<QString> files = dir.entryList(QDir::Files), dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  QFile tmpFile;
  for (const auto& fileName : files) {
    tmpFile.setFileName(dir.absoluteFilePath(fileName));
    tmpFile.moveToTrash();
  }

  for (const auto& dirName : dirs) {
    moveDirectoryContentsToTrashRecursively(dir.absolutePath() + "/" + dirName);
  }

  dir.rmdir(dir.absolutePath());
}


void
MainWindow::removeFilesAndDirectoriesRecursively (bool isPermanently) {
  curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());

  QFileInfo info; QFile curFile;
  for (const auto& path : curTab->getSelection()) {

    info.setFile(path);

    if (info.isFile()) {
      curFile.setFileName(info.absoluteFilePath());

      if (isPermanently) {
        curFile.remove();
      } else {
        curFile.moveToTrash();
      }

    } else if (info.isDir()) {

      if (isPermanently) {
        QDir curDir = QDir(info.absoluteFilePath());
        curDir.removeRecursively();
      } else {
        moveDirectoryContentsToTrashRecursively(info.absoluteFilePath());
      }

    }
  }
}

// Работа с вкладками (методы)
void MainWindow::on_addTabAction_triggered()
{
  curTab = new ExplorerTab(fileSystemModel, rootFolderMenu, selectedItemMenu, homePath);

  ui->tabWidget->addTab(curTab, "Домашний каталог пользователя");

  connect(curTab->getListView(), &MyListView::itemsSelected, this, &MainWindow::on_itemsSelected);

  connect(curTab, &ExplorerTab::itemOpened, this, [=](QModelIndex index){
    on_changeDir(fileSystemModel->filePath(index), ChangeDirSource::ListView);
  });

}

void MainWindow::on_closeTabAction_triggered(int index) {
  QWidget * widget = ui->tabWidget->widget(index);
  if (widget) {

    ui->tabWidget->removeTab(index);
    widget->deleteLater();

  }
}

void MainWindow::on_currentTabChanged(int index) {
  curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());
  if (curTab) {
    const QString& curPath = curTab->getCurDir();
    ui->adressBar->setText(curPath);
    locateInTreeView(curPath);
  }
}

void
MainWindow::on_itemsSelected (bool isEmpty) {
  ui->actionCopy->setEnabled(isEmpty);
  ui->actionCut->setEnabled(isEmpty);
  ui->actionRenameItem->setEnabled(isEmpty);
  ui->actionRemove->setEnabled(isEmpty);
  ui->actionToTrash->setEnabled(isEmpty);
}

void
MainWindow::on_copyAction_triggered () {
  curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());

  if (curTab) {
    itemsToMove.clear();
    itemsToCopy = curTab->getSelection();

    ui->actionPaste->setEnabled(true);
  }
}

void
MainWindow::on_cutAction_triggered () {
  curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());

  if (curTab) {
    itemsToCopy.clear();
    itemsToMove = curTab->getSelection();

    ui->actionPaste->setEnabled(true);
  }
}

void
MainWindow::copyOrMoveDirectorySubtree (const QString & from, const QString & to, bool isOverwrite, bool isMove) {
  QDir fromDir(from);
  QDir toDir(to);

  const auto sourcePathLength = fromDir.absolutePath().length();

  toDir.mkdir(fromDir.dirName());
  toDir.cd(fromDir.dirName());

  QDirIterator it(from, QDirIterator::Subdirectories);

  while (it.hasNext()) {
    const QFileInfo fileInfo = it.nextFileInfo();


    if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") {
      continue;
    }
    // canonicalPath -> возвращает путь без символьных ссылок.
    // Мы копируем файл на который ссылается символическая ссылка, а не ссылку и соответственно с другой стороны тоже надо вставить файл в каталог, на который ссылается ссылка, т.е. разрешить ссылку.
    QString subPath = fileInfo.absoluteFilePath().mid(sourcePathLength);
    const QString curDirName = toDir.dirName();
    // Решаем проблему рекурсивного копирования в случае когда from == to. Проблема: в toDir создается каталог с тем же именем что и в sourcePath и обновленное содержимое from (в котором создан каталог toDir) копируется в каталог toDir и в результате получаем в каталоге toDir каталог toDir ))
    if (curDirName == subPath.mid(1)) {
      subPath = subPath.mid(curDirName.length() + 1);
    }

    const QString newPath = toDir.canonicalPath() + subPath;

    if (fileInfo.isFile()) {

      // Статический метод copy по умолчанию не перезаписывает существующий файл, он просто вернет false.
      if (isOverwrite) {
        QFile::remove(newPath);
      }

      QFile::copy(fileInfo.absoluteFilePath(), newPath);

    } else if (fileInfo.isDir()) {

      toDir.mkpath(newPath);

    }

  }
  // Если мы перемещаем каталог, то мы должны удалить исходный каталог.
  if (isMove) {
    fromDir.removeRecursively();
  }


}

void
MainWindow::on_pasteAction_triggered () {
  curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());
  if (!curTab) {
    qDebug() << "MainWindow::on_pasteAction_triggered -> curTab = 0";
    return;
  }

  QFileInfo itemInfo;

  // Нужно определить каталог назначения (текущий или выделенный)
  QString destDir = ui->adressBar->text();
  // Решить проблему рекурсивного копирования.

  for (const QString& path : itemsToCopy) {
    itemInfo.setFile(path);

    if (itemInfo.isFile()) {

      QFile::copy(path, destDir + "/" + itemInfo.fileName());

    } else if (itemInfo.isDir()) {

      copyOrMoveDirectorySubtree(path, destDir, false, false);

    }

  }

  for (const QString& path : itemsToMove) {
    itemInfo.setFile(path);

    if (itemInfo.isFile()) {

      QFile::copy(path, ui->adressBar->text() + "/" + itemInfo.fileName());
      QFile::remove(path);

    } else if (itemInfo.isDir()) {

      copyOrMoveDirectorySubtree(path, ui->adressBar->text(), true, true);

    }

  }

  ui->actionPaste->setEnabled(false);
}

void
MainWindow::on_openInTerminalAction_triggered () {

#if defined Q_OS_WIN
  process->startCommand("wt.exe -d \"" + ui->adressBar->text() + "\"");
#elif defined Q_OS_MACOS
  process->startCommand("open -a Terminal \"" + ui->adressBar->text() + "\"");
#endif

}

void MainWindow::on_changeView(QListView::ViewMode mode)
{
  curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());

  if (!curTab) {
    qDebug() << "curTab = 0";
  }

  MyListView * listView = curTab->getListView();
  listView->setViewMode(mode);
}


void MainWindow::initContextMenu() {

  rootFolderMenu = new QMenu();
  selectedItemMenu = new QMenu();

  QMenu * changeView = new QMenu("Вид");
  changeView->setIcon(QIcon(":/icons/view.svg"));
  changeView->addAction(ui->actionIconsView);
  changeView->addAction(ui->actionStandartView);

  // TODO
  QMenu * sortBy = new QMenu("Сортировать по");

  rootFolderMenu->addAction(ui->actionNewFolder);
  rootFolderMenu->addAction(ui->actionNewFile);
  rootFolderMenu->addSeparator();
  rootFolderMenu->addAction(ui->actionPaste);
  rootFolderMenu->addSeparator();
  rootFolderMenu->addAction(ui->actionItemInfo);
  rootFolderMenu->addSeparator();
  rootFolderMenu->addMenu(changeView);
  rootFolderMenu->addSeparator();
  rootFolderMenu->addAction(ui->actionOpenInTerminal);

  selectedItemMenu->addAction(ui->actionRenameItem);
  selectedItemMenu->addSeparator();
  selectedItemMenu->addAction(ui->actionCopy);
  selectedItemMenu->addAction(ui->actionCut);
  selectedItemMenu->addSeparator();
  selectedItemMenu->addAction(ui->actionPaste);
  selectedItemMenu->addSeparator();
  selectedItemMenu->addAction(ui->actionToTrash);
  selectedItemMenu->addAction(ui->actionRemove);
}


// Общий функционал при обработке событий связанных с изменением текущего каталога одним из доступных способов
// Частный функционал должен быть прикручен к соотв. сигналам.

void MainWindow::on_changeDir(const QString& path, ChangeDirSource source)
{
  // Если выбранный элемент файловой системы не является каталогом, то делегируем его открытие на ОС

  // К текущей вкладке можем получить доступ через QTabWidget, посему хранить ее в качестве свойств не имеет смысла.
  QDir dir(path);
  if (dir.exists()) {
    // QDir::setCurrent(path);
    if (source != ChangeDirSource::TreeView) {
      // Каталог изменен любым способом кроме treeView, в treeView надо отобразить изменения
      locateInTreeView(path);
    }

    if (source != ChangeDirSource::AdressBar) {
      // Отражаем изменения в adressBar
      ui->adressBar->setText(dir.absolutePath());
    }

    // if (source != ChangeDirSource::ListView) {
      // Надо изменить отображаемый индекс в текущей вкладке (состояние других вкладок не должно быть изменено).
    curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());
    curTab->changeDir(dir.absolutePath());

    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), dir.dirName());
    // }

    if (visitedPaths.last() != path)
      visitedPaths.append(path);
    if (!ui->leftArrowButton->isEnabled() && visitedPaths.size() > 1) {
      ui->leftArrowButton->setEnabled(true);
    }

  } else {
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
  }
}

void MainWindow::locateInTreeView(const QString& path)
{
  QDir dir(path);
  while(dir.cdUp()) {
    ui->treeView->expand(fileSystemModel->index(dir.absolutePath()));
  }

  // Сбросит предыдущее выделение и выделит новый элемент treeView.
  ui->treeView->selectionModel()->select(fileSystemModel->index(path), QItemSelectionModel::ClearAndSelect);

}
MainWindow::~MainWindow () { delete ui; }
