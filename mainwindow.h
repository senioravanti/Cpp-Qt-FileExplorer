#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>

#include <QFileSystemModel>

#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT


public slots:
  void on_currentPath_changed();

public:
  MainWindow (QWidget *parent = nullptr);
  ~MainWindow ();

private:
  // Свойства
  Ui::MainWindow * ui;

  QFileSystemModel * treeModel, * listModel;


  // Методы
};
#endif // MAINWINDOW_H
