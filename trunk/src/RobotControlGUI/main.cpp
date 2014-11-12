#include "robotcontrol.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RobotControl w;
	w.show();
	return a.exec();
}
