#include "mainwindow.h"

#include <QApplication>

QString readTextFile(const QString& path) {
  QFile file(path);

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    return in.readAll();
  }

  return "";

}

int
main (int argc, char *argv[]) {
  QApplication a (argc, argv);

  MainWindow w;

  // --- Стилизуем приложение
  QString css = readTextFile(":/styles/style.css");
  if (!css.isEmpty()) {
    a.setStyleSheet(css);
  }

  w.show ();
  return a.exec ();
}
