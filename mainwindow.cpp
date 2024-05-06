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

  ui->listView->setModel(listModel);

  ui->currentPath->setText(QDir::homePath());
  ui->listView->setViewMode(QListView::IconMode);

  on_currentPath_changed();

  ui->listView->setUniformItemSizes(true);
  ui->listView->setTextElideMode(Qt::ElideNone);
  ui->listView->setWordWrap(true);

  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 3);




  // --- Сигналы и слоты
  connect(ui->currentPath, &QLineEdit::returnPressed, this, &MainWindow::on_currentPath_changed);
  connect(ui->upArrowButton, &QPushButton::pressed, this, [=](){
    QDir dir(ui->currentPath->text()); dir.cdUp();
    ui->currentPath->setText(dir.absolutePath());
    on_currentPath_changed();
  });
  connect(ui->listView, &QAbstractItemView::doubleClicked, this, [=](const QModelIndex &index){
    QString curPath = listModel->filePath(index);

    QFileInfo fileInfo(curPath);
    if (fileInfo.isDir()) {
      ui->listView->setRootIndex(index);
      ui->currentPath->setText(curPath);
    } else {
      QDesktopServices::openUrl(QUrl::fromLocalFile(curPath));
    }

  });

}

void MainWindow::on_currentPath_changed(){
  listModel->setRootPath(ui->currentPath->text());
  ui->listView->setRootIndex(listModel->index(ui->currentPath->text()));
}

MainWindow::~MainWindow () { delete ui; }
