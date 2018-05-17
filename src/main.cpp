/*
	Entry point
*/

#include <QApplication>

#include "ImageWindow.h"
#include "ImageProcessor.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	ImageWindow win;
	ImageProcessor img;

	QObject::connect(&img, &ImageProcessor::imageUpdated, &win, &ImageWindow::setImage);

	img.load("test.png");

	img.apply([](const QImage& img, const QPoint& coord) {
		auto c = img.pixel(coord);
		return c * c;
	});

	win.show();

	return app.exec();
}
