#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QListView>
#include <QObject>
#include <QWidget>
#include <QContextMenuEvent>

class MyListView : public QListView {
  Q_OBJECT
public:
  MyListView (QWidget * parent = nullptr);

  QMenu * getContextMenu();

private:
  QMenu * contextMenu;

protected:
  void contextMenuEvent(QContextMenuEvent *) override;
};

#endif // MYLISTVIEW_H
