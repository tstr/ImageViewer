/*
	Entry point
*/

#include <QApplication>

#include "ImageWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	ImageWindow win;
	win.loadImage("test.png");
	win.show();

	return app.exec();
}
