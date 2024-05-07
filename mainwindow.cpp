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
#include <QKeySequence>



MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent), ui (new Ui::MainWindow) {
  ui->setupUi (this);

  treeModel = new QFileSystemModel();
  listModel = new QFileSystemModel();

  treeModel->setRootPath(QDir::rootPath());
  ui->treeView->setModel(treeModel);
  ui->treeView->setRootIndex(treeModel->index(QDir::rootPath()));

  ui->treeView->hideColumn(1);
  ui->treeView->hideColumn(2);
  ui->treeView->hideColumn(3);

  // QObject * obj = ui->listView;
  listView = new MyListView();
  this->ui->splitter->addWidget(listView);
  // ui->listView = new MyListView(ui->splitter);

  listView->setModel(listModel);

  ui->currentPath->setText(QDir::homePath());
  listView->setViewMode(QListView::IconMode);

  on_currentPath_changed();

  listView->setUniformItemSizes(true);
  listView->setTextElideMode(Qt::ElideNone);
  listView->setWordWrap(true);

  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 3);

  setupListViewContextMenu();

  // --- Сигналы и слоты
  connect(ui->currentPath, &QLineEdit::returnPressed, this, &MainWindow::on_currentPath_changed);
  connect(ui->upArrowButton, &QPushButton::pressed, this, [=](){
    QDir dir(ui->currentPath->text()); dir.cdUp();
    ui->currentPath->setText(dir.absolutePath());
    on_currentPath_changed();
  });
  connect(listView, &QAbstractItemView::doubleClicked, this, [=](const QModelIndex &index){
    QString curPath = listModel->filePath(index);

    QFileInfo fileInfo(curPath);
    if (fileInfo.isDir()) {
      listView->setRootIndex(index);
      ui->currentPath->setText(curPath);
    } else {
      QDesktopServices::openUrl(QUrl::fromLocalFile(curPath));
    }

  });
  connect(createFolder, &QAction::triggered, this, &MainWindow::on_createFolderAction);

}

void MainWindow::on_currentPath_changed(){
  listModel->setRootPath(ui->currentPath->text());
  listView->setRootIndex(listModel->index(ui->currentPath->text()));
}

void MainWindow::on_createFolderAction() {
  bool ok;
  QString folderName = QInputDialog::getText(this, "Введите название каталога", "Название каталога", QLineEdit::Normal, "", &ok);
  if (ok && !folderName.isEmpty()) {
    QDir curDir = QDir(ui->currentPath->text());
    if (curDir.mkdir(folderName)) {
      on_currentPath_changed();
    }
  }
}

void MainWindow::setupListViewContextMenu() {
  QMenu * menu = listView->getContextMenu();

  createFolder = new QAction("Создать каталог");
  createFolder->setShortcut(QKeySequence(""));

  getInfo = new QAction("Получить информацию");

  changeView = new QAction("Вид");
  sortItems = new QAction("Сортировать по");

  menu->addAction(createFolder);
  menu->addSeparator();
  menu->addAction(getInfo);
  menu->addSeparator();
  menu->addAction(changeView);
  menu->addAction(sortItems);


}


MainWindow::~MainWindow () { delete ui; }
