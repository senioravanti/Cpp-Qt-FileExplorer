#include "explorertab.h"

ExplorerTab::ExplorerTab(QFileSystemModel * model, QMenu * rootFolderMenu, QMenu * selectedItemMenu, const QString& path, QWidget *parent)
:

  QWidget{parent}, fileSystemModel{model},
  rootFolderMenu{rootFolderMenu},
  selectedItemMenu{selectedItemMenu}

{
  mainLayout = new QVBoxLayout(this);

  listView = new MyListView(rootFolderMenu, selectedItemMenu);

  mainLayout->addWidget(listView);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  listView->setModel(model);

  listView->setSelectionRectVisible(true);
  listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  //
  listView->setEditTriggers(QAbstractItemView::EditKeyPressed);
  // ?
  listView->setTextElideMode(Qt::ElideNone);
  // Внешний вид элементов
  listView->setViewMode(QListView::IconMode);
  listView->setResizeMode(QListView::Adjust);
  listView->setMovement(QListView::Free);
  listView->setUniformItemSizes(true);
  listView->setWordWrap(true);
  listView->setWrapping(true);

  if (!path.isEmpty()) {
    changeDir(path);
  }
  // Сигналы и слоты
  connect(listView, &QAbstractItemView::doubleClicked, this, [=](QModelIndex index){
    emit itemOpened(index);
  });
}

MyListView * ExplorerTab::getListView()
{
  return this->listView;
}

QList<QString>
ExplorerTab::getSelection () {
  QList<QString> res;
  for (const auto& index : listView->selectionModel()->selectedIndexes()) {
    res.append(fileSystemModel->filePath(index));
  }
  return res;
}

void ExplorerTab::changeDir(const QString& path)
{
  listView->setRootIndex(fileSystemModel->index(path));
}

QString ExplorerTab::getCurDir()
{
  return fileSystemModel->filePath(listView->rootIndex());
}

// Будет вызываться всякий раз при закрытии вкладки
ExplorerTab::~ExplorerTab()
{
  // Очищаем память, выделенную под локальные для вкладки экземпляры
  delete listView;
  delete mainLayout;
}

