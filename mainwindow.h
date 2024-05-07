#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>

#include <QFileSystemModel>

#include <QMenu>

#include <QAction>

#include "mylistview.h"

QT_BEGIN_NAMESPACE
namespace Ui {

class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT


public slots:
  void on_currentPath_changed();
  void on_createFolderAction();

public:
  MainWindow (QWidget *parent = nullptr);

  ~MainWindow ();

private:
  // Свойства
  Ui::MainWindow * ui;

  QFileSystemModel * treeModel, * listModel;
  MyListView * listView;

  // Операции контектсного меню
  QAction * createFolder, * changeView, * sortItems, * getInfo;

  // Методы
protected:
  void setupListViewContextMenu();
};
#endif // MAINWINDOW_H
