#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileSystemModel>
#include <QSplitter>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent), ui (new Ui::MainWindow) {
  ui->setupUi (this);

  QVBoxLayout * topLevelLayout = new QVBoxLayout();

  topLevelLayout->setContentsMargins(12, 12, 12, 12);
  topLevelLayout->setSpacing(12);

  ui->centralwidget->setLayout(topLevelLayout);

  QLineEdit * lineEdit = new QLineEdit();
  lineEdit->setText(QDir::homePath());

  QSplitter * splitter = new QSplitter();
  splitter->setHandleWidth(0);

  topLevelLayout->addWidget(lineEdit);
  topLevelLayout->addWidget(splitter);

  QTreeView * mainTreeView = new QTreeView(splitter), * curTreeView = new QTreeView(splitter);

  QFileSystemModel * model = new QFileSystemModel;
  model->setRootPath(QDir::homePath());
  mainTreeView->setModel(model);

  mainTreeView->setRootIndex(model->index(QDir::homePath()));

}

MainWindow::~MainWindow () { delete ui; }
