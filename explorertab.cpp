#include "explorertab.h"

ExplorerTab::ExplorerTab(QFileSystemModel * model, QMenu * rootFolderMenu, QMenu * selectedItemMenu, const QString& path, QWidget *parent)
:

  QWidget{parent}, model{model},
  rootFolderMenu{rootFolderMenu},
  selectedItemMenu{selectedItemMenu}

{
  mainLayout = new QVBoxLayout(this);

  listView = new MyListView();
  listView->setModel(model);

  listView->setContextMenu(rootFolderMenu);
  mainLayout->addWidget(listView);

  listView->setSelectionRectVisible(true);
  listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
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
  connect(listView, &QAbstractItemView::doubleClicked, this, [=](const QModelIndex& index){
    emit itemOpened(index);
  });
}

MyListView * ExplorerTab::getListView()
{
  return this->listView;
}

void ExplorerTab::changeDir(const QString& path)
{
  listView->setRootIndex(model->index(path));
}

QString ExplorerTab::getCurDir()
{
  return model->filePath(listView->rootIndex());
}

// Будет вызываться всякий раз при закрытии вкладки
ExplorerTab::~ExplorerTab()
{
  // Очищаем память, выделенную под локальные для вкладки экземпляры
  delete listView;
  delete mainLayout;
}

