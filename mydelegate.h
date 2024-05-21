#ifndef MYDELEGATE_H
#define MYDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QPainter>
#include <QTextDocument>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFontMetrics>
#include <QFileIconProvider>
#include <QTreeView>

#include "mylistview.h"

class MyDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  MyDelegate(QWidget * parent = nullptr);

  void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;


  QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

  // Вызывается в момент когда пользователь инициировал изменение элемента QListView предоставляет указатель на виджет, данные которого нужно изменить и указатель на индекс элемента модели.
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;

  // Возвращает модели измененные данные
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

  // Обеспечивает правильное отображение виджета editor по отношению к представлению элемента модели с индексом index путем изменения его линейных размеров на зн., хр. в свойстве rect параметра option.
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
  // Свойства
  int iconSizeIconMode, iconSizeListMode, iconOffset, elementWidth;
  int roundingRadius;
  int textWidth, textOffsetListMode;

};

#endif // MYDELEGATE_H
