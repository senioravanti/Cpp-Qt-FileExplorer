#include "mylistview.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QListView>
#include <QAction>

MyListView::MyListView (QWidget * parent) : QListView(parent)
{
  // --- Инициализация контекстного меню
  contextMenu = new QMenu(this);
}

void MyListView::contextMenuEvent(QContextMenuEvent * event) {

  if (event->reason() == QContextMenuEvent::Mouse) {
    contextMenu->exec(event->globalPos());
  }

}

QMenu * MyListView::getContextMenu() {
  return this->contextMenu;
}
