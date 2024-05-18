#include "lineeditdelegate.h"

LineEditDelegate::LineEditDelegate(QWidget * parent)
:
  QStyledItemDelegate(parent)
{

}

QWidget*
LineEditDelegate::createEditor (QWidget* parent,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
  QLineEdit * editor = new QLineEdit(parent);
  return editor;
}

void LineEditDelegate::updateEditorGeometry (QWidget * editor,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const {
  editor->setGeometry(option.rect);
}

void
LineEditDelegate::setModelData (
    QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index
  ) const
{
  QLineEdit * lineEdit = qobject_cast<QLineEdit * >(editor);
  model->setData(index, lineEdit->text());
}

void
LineEditDelegate::setEditorData (
  QWidget * editor, const QModelIndex & index) const
{
  QString data = index.model()->data(index).toString();

  QLineEdit * lineEdit = qobject_cast<QLineEdit * >(editor);
  lineEdit->setText(data);


}
