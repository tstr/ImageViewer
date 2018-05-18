/*
	Image processing pipeline class
*/

#pragma once

#include <QImage>

using Kernel3x3 = int[3][3];
using Kernel5x5 = int[5][5];

class ImageProcessor : public QObject
{
	Q_OBJECT

public:

	explicit ImageProcessor(QObject* parent = nullptr) : QObject(parent) {}

	/*
		Apply a function to every pixel of an image:

		uint func(const QImage&, const QPoint& coord)
	*/
	template<typename Function>
	void apply(const Function& func)
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

		imageUpdated(m_a);
	}

	/*
		Apply a filter kernel to the image
	*/
	void filter(const Kernel3x3& kernel);

	void load(const QImage& img);

	const QImage& image() const { return m_a; }

public slots:

	void resetImage();

signals:

	void imageUpdated(const QImage& img);

private:

	QImage m_src;
	QImage m_a;
	QImage m_b;
};