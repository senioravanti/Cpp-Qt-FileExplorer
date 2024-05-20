#ifndef EXPLORERTAB_H
#define EXPLORERTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QMenu>
#include <QFileSystemModel>
#include "mylistview.h"
#include "mydelegate.h"

class ExplorerTab : public QWidget {
  Q_OBJECT
public:
  explicit ExplorerTab(QFileSystemModel * model, QMenu * rootFolderContextMenu, QMenu * selectedItemMenu, const QString& path = "", QWidget *parent = nullptr);

  void changeDir(const QString&);

  // Геттеры

  MyListView * getListView();

  QList<QString> getSelection();

  QString getCurDir();



  ~ExplorerTab();

// public slots:
  // void on_openItem(const QModelIndex&);
signals:
  void itemOpened(QModelIndex index);

private:
  QVBoxLayout * mainLayout;
  // QDir * curDir;
  QFileSystemModel * fileSystemModel;
  // MainWindow * mainWindow;

  MyListView * listView;
  MyDelegate * delegate;

  QMenu * rootFolderMenu, * selectedItemMenu;

};

#endif // EXPLORERTAB_H
