#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QFileInfo>

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

  homePath = QDir::homePath();

  visitedPaths = QList<QString>(homePath);
  poppedPath = "";
  ui->rightArrowButton->setEnabled(false);
  ui->leftArrowButton->setEnabled(false);

  ui->actionCopy->setEnabled(false);
  ui->actionCut->setEnabled(false);
  ui->actionPaste->setEnabled(false);
  ui->actionRenameItem->setEnabled(false);
  ui->actionRemove->setEnabled(false);

  fileSystemModel = new QFileSystemModel(this);
  fileSystemModel->setRootPath("");

  ui->treeView->setModel(fileSystemModel);
  ui->treeView->setRootIndex(fileSystemModel->index(""));

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
  setupListViewContextMenu();
  on_addTabAction_triggered();

  QDir::setCurrent(homePath);


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
  // connect(listView, &QAbstractItemView::doubleClicked, this, [=](const QModelIndex &index){
  //   QString curPath = listModel->filePath(index);

  //   QFileInfo fileInfo(curPath);
  //   if (fileInfo.isDir()) {

  //     // Элемент с переданным индексом станет корневым.

  //     listView->setRootIndex(index);
  //     ui->currentPath->setText(curPath);

  //   } else {

  //     QDesktopServices::openUrl(QUrl::fromLocalFile(curPath));

  //   }

  // });
  // connect(createFolder, &QAction::triggered, this, &MainWindow::on_createFolderAction);

  // connect(rename, &QAction::triggered, this, [=](){
  //   listView->edit(listView->currentIndex());
  // });

  // Работа с вкладками
  connect(ui->actionNewTab, &QAction::triggered, this, &MainWindow::on_addTabAction_triggered);
  // Привязанный сигнал вызывается когда на вкладке нажата кнопка "закрыть"
  connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [=](int index){
    curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());

    ui->tabWidget->removeTab(index);

    delete curTab;
  });
  connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int index){
    curTab = qobject_cast<ExplorerTab * >(ui->tabWidget->currentWidget());
    if (curTab) {
      QString path = curTab->getCurDir();
      ui->adressBar->setText(path);
      locateInTreeView(path);
      // QDir::setCurrent(path);
    }
  });

  // Открываю элемент файловой системы по двойному щелчку мыши.
  connect(curTab, &ExplorerTab::itemOpened, this, [=](const QModelIndex& index){
    on_changeDir(fileSystemModel->filePath(index), ChangeDirSource::ListView);
  });

  // Вид
  connect(ui->actionStandartView, &QAction::triggered, this, [=](){
    on_changeView(QListView::ListMode);
  });
  connect(ui->actionIconsView, &QAction::triggered, this, [=](){
    on_changeView(QListView::IconMode);
  });

}


void MainWindow::on_createFolderAction() {
  bool ok;
  QString folderName = QInputDialog::getText(this, "Введите название каталога", "Название каталога", QLineEdit::Normal, "", &ok);
  if (ok && !folderName.isEmpty()) {
    QDir curDir = QDir(ui->adressBar->text());
    curDir.mkdir(folderName);
  }
}

void MainWindow::on_addTabAction_triggered()
{
  curTab = new ExplorerTab(fileSystemModel, rootFolderMenu, selectedItemMenu, homePath);
  ui->tabWidget->addTab(curTab, "Домашний каталог пользователя");
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


void MainWindow::setupListViewContextMenu() {

  rootFolderMenu = new QMenu();
  selectedItemMenu = new QMenu();

  QMenu * changeView = new QMenu("Вид");
  changeView->setIcon(QIcon(":/icons/view.svg"));

  // TODO
  QMenu * sortBy = new QMenu("Сортировать по");

  rootFolderMenu->addAction(ui->actionNewFolder);
  rootFolderMenu->addAction(ui->actionNewFile);
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
