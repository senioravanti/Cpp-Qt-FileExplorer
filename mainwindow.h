#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>

#include <QFileSystemModel>

#include <QMenu>

#include <QAction>
#include <QList>
#include "explorertab.h"


QT_BEGIN_NAMESPACE
namespace Ui {

class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT


public slots:
  void on_createFolderAction();
  void on_addTabAction_triggered();

public:
  MainWindow (QWidget *parent = nullptr);

  enum class ChangeDirSource { ListView, TreeView, AdressBar, TabChange, NavButton};

  ~MainWindow ();

private:
  // Свойства
  Ui::MainWindow * ui;

  QFileSystemModel * fileSystemModel;

  QString homePath;

  ExplorerTab * curTab;
  // Операции контектсного меню
  QMenu * rootFolderMenu, * selectedItemMenu;

  // QAction * createFolder, * createFile ,  * sortItems, * getInfo, * standartView, * iconsView, * rename;

  QList<QString> visitedPaths;
  QString poppedPath;

  // Методы
protected:
  void setupListViewContextMenu();
  void on_changeDir(const QString&, ChangeDirSource);
  void on_changeView(QListView::ViewMode);
  void locateInTreeView(const QString&);
};
#endif // MAINWINDOW_H
