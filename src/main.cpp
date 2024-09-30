#include "MainWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("ScreenSessionManager");

    MainWidget w;
    w.show();
    return a.exec();
}
