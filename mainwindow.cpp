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

#include <QAction>
#include <QCursor>

#include <QLineEdit>

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

}

void MainWindow::on_currentPath_changed(){
  listModel->setRootPath(ui->currentPath->text());
  listView->setRootIndex(listModel->index(ui->currentPath->text()));
}




MainWindow::~MainWindow () { delete ui; }
