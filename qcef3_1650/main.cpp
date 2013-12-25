#include "mainwindow.h"
#include <QApplication>
#include "cefclient/cefclient.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  int result = CefInit(argc, argv);
  if (result >= 0)
    return result;

  MainWindow w;
  w.show();

  //a.setQuitOnLastWindowClosed(false);
  result = a.exec();

  CefQuit();

  return result;
}
