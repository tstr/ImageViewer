/*
	Image processing pipeline class
*/

#pragma once

#include <QImage>

class ImageProcessor : public QObject
{
	Q_OBJECT

private:
	
	QImage m_a;
	QImage m_b;

public:

	ImageProcessor() {}

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

	void load(const QString& fileName)
	{
		m_a = QImage(fileName);									//source image
		m_b = QImage(m_a.width(), m_a.height(), m_a.format());  //destination image

		imageUpdated(m_a);
	}

	const QImage& image() const { return m_a; }

signals:

	void imageUpdated(const QImage& img);

};