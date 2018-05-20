/*
	Image processing pipeline class
*/

#pragma once

#include <QImage>
#include <QPixmap>

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

	/*
		Apply a function to every pixel of an image:

		uint func(const QImage&, const QPoint& coord)
	*/
	template<typename Function>
	ImageProcessor& apply(const Function& func)
	{
		for (int i = 0; i < m_a.width(); i++)
		{
			for (int j = 0; j < m_a.height(); j++)
			{
				QPoint coord(i, j);

				m_b.setPixel(coord, func((const QImage&)m_a, coord));;
			}
		}

		//Swap image buffers
		m_a.swap(m_b);

		imageUpdated(QPixmap::fromImage(m_a));

		return *this;
	}

	/*
		Apply a gray scale filter
	*/
	ImageProcessor& makeGrayscale();

	/*
		Apply a filter kernel to the image
	*/
	ImageProcessor& applyFilter(const KernelView& kernel);

	/*
		Apply a non linear filter operation to the image
	*/
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