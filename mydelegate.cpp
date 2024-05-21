#include "mydelegate.h"

MyDelegate::MyDelegate(QWidget * parent)
:
  QStyledItemDelegate(parent),
  iconSizeIconMode{56},
  iconSizeListMode{20},
  iconOffset{12},
  textOffsetListMode{3},
  elementWidth{72},
  roundingRadius{12},
  textWidth{64}
{}

void
MyDelegate::paint (QPainter* painter, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const {
  // QStyledItemDelegate::paint(painter, option, index);

  if (!index.isValid()) return;

  MyListView * listView = qobject_cast<MyListView*>(parent());
  // QTreeView * treeView = qobject_cast<QTreeView*>(parent());

  QString filePath;
  QFileInfo fileInfo;
  QBrush backgroundBrush = Qt::transparent;

  if (option.state & QStyle::State_Selected) {

    backgroundBrush = QColor::fromString("#B4CCBB");

  }

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);
  painter->setBrush(backgroundBrush);
  painter->drawRoundedRect(option.rect, roundingRadius, roundingRadius);

  QString text;
  QIcon icon;

  if (listView) {

    text = index.data(Qt::DisplayRole).toString();
    painter->drawRoundedRect(option.rect, roundingRadius, roundingRadius);

    filePath = index.data(QFileSystemModel::FilePathRole).toString();
    fileInfo.setFile(filePath);

    QFontMetrics fontMetrics(option.font);
    if (fontMetrics.horizontalAdvance(text) >= textWidth) {
      text = fontMetrics.elidedText(text, Qt::ElideRight, 64);
    }

    QTextOption textOption;
    textOption.setUseDesignMetrics(true);
    textOption.setAlignment(Qt::AlignHCenter);
    textOption.setWrapMode(QTextOption::NoWrap);

    painter->setPen(QPen(QColor::fromString("#1A1C16")));

    if (fileInfo.isDir()) {

      icon = QIcon(":/icons/folder.png");

      icon.paint(painter, QRect(option.rect.x() + (option.rect.width() - iconSizeIconMode) / 2, option.rect.y(), iconSizeIconMode, iconSizeIconMode), Qt::AlignHCenter | Qt::AlignTop);

    } else {

      icon = QFileIconProvider().icon(fileInfo);

      icon.paint(painter, QRect(option.rect.x() + (option.rect.width() - iconSizeIconMode) / 2, option.rect.y() + iconOffset, iconSizeIconMode, iconSizeIconMode), Qt::AlignHCenter | Qt::AlignTop);

    }

    painter->drawText(QRect(option.rect.x() + (option.rect.width() - textWidth) / 2, option.rect.y() + iconSizeIconMode, textWidth, option.rect.height()), text, textOption);

  }
}

QSize
MyDelegate::sizeHint (const QStyleOptionViewItem& option,
                            const QModelIndex& index) const {
  MyListView * listView = qobject_cast<MyListView * >(parent());
  if (listView) {
    // Получаем данные для отображения элемента
    QString text = index.data(Qt::DisplayRole).toString();

           // Задаем параметры отображения текста
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignHCenter);
    textOption.setWrapMode(QTextOption::NoWrap);

           // Создаем документ с необходимыми параметрами для определения высоты текста
    QTextDocument document;
    document.setDefaultTextOption(textOption);
    document.setPageSize(QSize(elementWidth, 16));
    document.setPlainText(text);

    return QSize(elementWidth, iconSizeIconMode + document.size().height());
  }// Реализация базового класса
  return QStyledItemDelegate::sizeHint(option, index);
}

QWidget*
MyDelegate::createEditor (QWidget* parent,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
  QLineEdit * editor = new QLineEdit(parent);
  return editor;
}

void MyDelegate::updateEditorGeometry (QWidget * editor,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const {
  QRect rect = option.rect;
  rect.setY(rect.y() + iconSizeIconMode - iconOffset);
  editor->setGeometry(rect);
}

void
MyDelegate::setModelData (
    QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index
  ) const
{
  QLineEdit * lineEdit = qobject_cast<QLineEdit * >(editor);
  model->setData(index, lineEdit->text());
}

void
MyDelegate::setEditorData (
  QWidget * editor, const QModelIndex & index) const
{
  QString data = index.model()->data(index).toString();

  QLineEdit * lineEdit = qobject_cast<QLineEdit * >(editor);
  lineEdit->setText(data);

}
