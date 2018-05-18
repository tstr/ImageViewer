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

	const Kernel3x3 box = {
		{ 1, 1, 1 },
		{ 1, 1, 1 },
		{ 1, 1, 1 }
	};

	const Kernel3x3 edgesV = {
		{ 1, 2, 1 },
		{ 0, 0, 0 },
		{ -1,-2,-1 }
	};

	const Kernel3x3 edgesH = {
		{ 1, 0, -1 },
		{ 2, 0, -2 },
		{ 1, 0, -1 }
	};

	const Kernel3x3 gaussian = {
		{ 1, 2, 1 },
		{ 2, 4, 2 },
		{ 1, 2, 1 }
	};

	const Kernel3x3 sharpen = {
		{ -1, -1, -1 },
		{ -1,  8, -1 },
		{ -1, -1, -1 }
	};

	auto none = win.addFilter("none");
	none->setChecked(true);
	QObject::connect(none, &QAbstractButton::toggled, [&](bool checked) { if (checked) img.resetImage(); });

	//Set image to greyscale
	QObject::connect(win.addFilter("greyscale"), &QAbstractButton::toggled, [&](bool checked) {
		if (checked) img.apply([](auto img, auto coord) {
			QColor c = qGray(img.pixel(coord));
			return qRgb(c.blue(), c.blue(), c.blue());
		});
		else
			img.resetImage();
	});
	
	//Filter kernels
	QObject::connect(win.addFilter("box blur"),      &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(box);      else img.resetImage(); });
	QObject::connect(win.addFilter("gaussian blur"), &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(gaussian); else img.resetImage(); });
	QObject::connect(win.addFilter("edge (H)"),      &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(edgesH);   else img.resetImage(); });
	QObject::connect(win.addFilter("edge (V)"),      &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(edgesV);   else img.resetImage(); });
	QObject::connect(win.addFilter("sharpen"),       &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(sharpen);  else img.resetImage(); });

	img.load(QImage("test.png"));

	win.show();

	return app.exec();
}
