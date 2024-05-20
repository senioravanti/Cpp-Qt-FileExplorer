#include "mainwindow.h"

#include <QMap>
#include <QDebug>

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

  QMap<QString, QString> colors;

  colors["@bg"] = "#EDEFE0";
  colors["@list_view"] = "#FAFAEE";
  colors["@btn"] = "#FFD7E6";
  colors["@on_btn"] = "#8F4664";
  colors["@field"] = "#E1E4D5";
  colors["@border_color"] = "#586249";
  colors["@text"] = "#1A1C16";
  colors["@border_width"] = "2px";
  colors["@padding"] = "3px";
  colors["@tab_padding"] = "12px";
  colors["@vertical_scroll_bar_height"] = "18px";
  colors["@vertical_scroll_bar_border_radius"] = "5px";
  colors["@control_size"] = "36px";

  QString css = readTextFile(":/styles/style.css");
  if (!css.isEmpty()) {
    for (const auto& color : colors.keys()) {
      css.replace(color, colors[color]);
    }
    qDebug() << css;
    a.setStyleSheet(css);
  }

  w.show ();
  return a.exec ();
}
