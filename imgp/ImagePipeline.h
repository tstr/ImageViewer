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

class ImagePipeline : public QObject
{
	Q_OBJECT

public:

	explicit ImagePipeline(QObject* parent = nullptr) : QObject(parent) {}

	//Load the given image
	void load(const QImage& img);

	//Return the current image
	const QImage& image() const { return m_a; }

	//Pixel function signature
	using PixelFunction = FunctionRef<QRgb(const QImage&, const QPoint&)>;

	//Apply a function to every pixel of an image:
	ImagePipeline& apply(const PixelFunction& func);

	//Apply a gray scale filter
	ImagePipeline& makeGrayscale();

	//Apply gamma correction
	ImagePipeline& setGamma(float gamma);

	//Apply a filter kernel to the image
	ImagePipeline& applyFilter(const KernelView& kernel);

	//Apply a non linear filter operation to the image
	ImagePipeline& applyNonLinearFilter();

	//Apply thresholding
	ImagePipeline& applyThresholding();

	//Apply error diffusion dithering
	ImagePipeline& applyDithering(Dithering mode);

public slots:

	void resetImage();

signals:

	void imageUpdated(const QPixmap& img);

private:

	QImage m_src;
	QImage m_a;
	QImage m_b;
};
