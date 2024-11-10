#include <QApplication>
#include <qapplication.h>
#include <mainwindow.h>
#include <QtPlugin>


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;

  w.show();

  return a.exec();
}
