#include "MainWid.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWid w;
	w.show();
	return a.exec();
}