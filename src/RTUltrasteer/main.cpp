#include "rtultrasteer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	qRegisterMetaType<QVector<double> >("QVector<double>");
	QApplication a(argc, argv);
	RTUltrasteer w;
	w.showMaximized();
	return a.exec();
}
