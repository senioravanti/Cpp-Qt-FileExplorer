#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QListView>
#include <QObject>
#include <QWidget>
#include <QContextMenuEvent>

class MyListView : public QListView {
  Q_OBJECT
public:
  MyListView (QMenu *, QMenu * , QWidget * parent = nullptr);

signals:
  void itemsSelected(bool);

private:
  QMenu * rootFolderMenu, * selectedItemMenu;

protected:
  void contextMenuEvent(QContextMenuEvent *) override;
  void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

};

#endif // MYLISTVIEW_H
