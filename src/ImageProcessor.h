/*
	Image processing pipeline class
*/

#pragma once

#include <QImage>
#include <QPixmap>

#include "Utils.h"
#include "FilterKernels.h"

class ImageAccessor;

enum class Dithering
{
	ERROR_DIFFUSION = 1,
	FLOYD_STEINBERG = 2,
	PATTERN         = 3,
	ORDERED         = 4,
};

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

	//Apply gamma correction
	ImageProcessor& setGamma(float gamma);

	//Apply a filter kernel to the image
	ImageProcessor& applyFilter(const KernelView& kernel);

	//Apply a non linear filter operation to the image
	ImageProcessor& applyNonLinearFilter();

	//Apply thresholding
	ImageProcessor& applyThresholding();

	//Apply error diffusion dithering
	ImageProcessor& applyDithering(Dithering mode);

public slots:

	void resetImage();

signals:

	void imageUpdated(const QPixmap& img);

private:

	QImage m_src;
	QImage m_a;
	QImage m_b;
};
