#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>

#include <QProcess>

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
  void on_createFileAction();

  void on_addTabAction_triggered();
  void on_closeTabAction_triggered(int index);
  void on_currentTabChanged(int index);

  void on_itemsSelected(bool);

  void on_copyAction_triggered();
  void on_cutAction_triggered();
  void on_pasteAction_triggered();

  void on_openInTerminalAction_triggered();

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

  // Копирование, перемещение, вставка
  QList<QString> itemsToCopy;
  QList<QString> itemsToMove;

  QProcess * process;

  QMap<QString, QString> colors;

  // Методы
protected:
  void initContextMenu();
  void on_changeDir(const QString&, ChangeDirSource);
  void on_changeView(QListView::ViewMode);
  void locateInTreeView(const QString&);

  void moveDirectoryContentsToTrashRecursively(const QString &);
  void removeFilesAndDirectoriesRecursively(bool);
  void copyOrMoveDirectorySubtree(const QString & from, const QString & to, bool isOverwrite, bool isMove);

};
#endif // MAINWINDOW_H
