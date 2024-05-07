#include "mylistview.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QListView>
#include <QAction>

MyListView::MyListView (QWidget * parent) : QListView(parent)
{
  // --- Инициализация контекстного меню
  contextMenu = new QMenu(this);
  QAction * newFolderAction = new QAction(tr("Создать папку"));

  contextMenu->addAction(newFolderAction);
}

void MyListView::contextMenuEvent(QContextMenuEvent * event) {

  if (event->reason() == QContextMenuEvent::Mouse) {
    contextMenu->exec(QCursor::pos());
  }

}
