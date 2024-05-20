#include "mylistview.h"

MyListView::MyListView (QMenu * rootFolderMenu, QMenu * selectedItemMenu, QWidget * parent)
:
  QListView(parent),
  rootFolderMenu(rootFolderMenu),
  selectedItemMenu(selectedItemMenu)
{
}

void MyListView::contextMenuEvent(QContextMenuEvent * event) {

  if (event->reason() == QContextMenuEvent::Mouse) {
    if (this->indexAt(event->pos()).isValid()) {
      this->selectedItemMenu->exec(event->globalPos());
    } else {
      this->rootFolderMenu->exec(event->globalPos());
    }
  }

}

void
MyListView::selectionChanged (const QItemSelection& selected,
                              const QItemSelection& deselected) {
  QListView::selectionChanged(selected, deselected);

  if (selected.isEmpty()) {
    emit itemsSelected(false);
  } else {
    emit itemsSelected(true);
  }

}


