#include "rtultrasteer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RTUltrasteer w;
    w.showMaximized();
    return a.exec();
}
