/*
	Image processing pipeline class
*/

#pragma once

#include <QImage>
#include <QPixmap>

#include "Utils.h"
#include "FilterKernels.h"

class ImageProcessor : public QObject
{
	Q_OBJECT

public:

	explicit ImageProcessor(QObject* parent = nullptr) : QObject(parent) {}

	//Load the given image
	void load(const QImage& img);

	//Return the current image
	const QImage& image() const { return m_a; }

	//Pixel function signature
	using PixelFunction = FunctionRef<QRgb(const QImage&, const QPoint&)>;

	//Apply a function to every pixel of an image:
	ImageProcessor& apply(const PixelFunction& func);

	//Apply a gray scale filter
	ImageProcessor& makeGrayscale();

	//Apply a filter kernel to the image
	ImageProcessor& applyFilter(const KernelView& kernel);

	//Apply a non linear filter operation to the image
	ImageProcessor& applyNonLinearFilter();

public slots:

	void resetImage();

signals:

	void imageUpdated(const QPixmap& img);

private:

	QImage m_src;
	QImage m_a;
	QImage m_b;
};
