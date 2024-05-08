#ifndef EXPLORERTAB_H
#define EXPLORERTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QMenu>
#include <QFileSystemModel>
#include "mylistview.h"

class ExplorerTab : public QWidget {
  Q_OBJECT
public:
  explicit ExplorerTab(QFileSystemModel * model, QMenu * rootFolderContextMenu, QMenu * selectedItemMenu, const QString& path = "", QWidget *parent = nullptr);

  MyListView * getListView();
  void changeDir(const QString&);
  QString getCurDir();

  ~ExplorerTab();

// public slots:
  // void on_openItem(const QModelIndex&);
signals:
  void itemOpened(const QModelIndex&);
private:
  QVBoxLayout * mainLayout;
  // QDir * curDir;
  QFileSystemModel * model;
  // MainWindow * mainWindow;

  MyListView * listView;
  QMenu * rootFolderMenu, * selectedItemMenu;

};

#endif // EXPLORERTAB_H
