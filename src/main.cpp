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
	QObject::connect(win.addFilter("box blur"),      &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(filters::box);       else img.resetImage(); });
	QObject::connect(win.addFilter("gaussian 3x3"),  &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(filters::gaussian3); else img.resetImage(); });
	QObject::connect(win.addFilter("gaussian 5x5"),  &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(filters::gaussian5); else img.resetImage(); });
	QObject::connect(win.addFilter("edge (H)"),      &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(filters::edgesH);    else img.resetImage(); });
	QObject::connect(win.addFilter("edge (V)"),      &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(filters::edgesV);    else img.resetImage(); });
	QObject::connect(win.addFilter("sharpen"),       &QAbstractButton::toggled, [&](bool checked) { if (checked) img.filter(filters::sharpen);   else img.resetImage(); });

	img.load(QImage("test.png"));

	win.show();

	return app.exec();
}
