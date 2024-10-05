#include "MainWidget.h"
#include "ScreenManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("ScreenSessionManager");

    qRegisterMetaType<ScreenSession>("ScreenSession");
    qRegisterMetaType<QList<ScreenSession>>("QList<ScreenSession>");

    MainWidget w;
    w.show();
    return a.exec();
}
