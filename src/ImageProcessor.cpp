/*
	Image processing
*/

#include "ImageProcessor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImageProcessor::load(const QImage& img)
{
	m_src = img;
	m_a = m_src;
	m_b = QImage(m_a.width(), m_a.height(), m_a.format());

	imageUpdated(m_a);
}

void ImageProcessor::resetImage()
{
	m_a = m_src;
	imageUpdated(m_a);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImageProcessor::filter(const int* kernel, int n, int m)
{
	//Apply 3x3 filter kernel to the image
	apply([&](auto img, auto coord) {

		int newchannels[] = { 0, 0, 0 };

		//For each colour channel of the input image
		for (int c = 0; c < 3; c++)
		{
			int accum = 0;
			int factor = 0;

			//Apply kernel
			for (int y = 0; y < m; y++)
			{
				for (int x = 0; x < n; x++)
				{
					QPoint newcoord = coord + QPoint(x - (n >> 1), y - (m >> 1));

					newcoord.setX(std::max(0, std::min(img.size().width() - 1, newcoord.x())));
					newcoord.setY(std::max(0, std::min(img.size().height() - 1, newcoord.y())));

					//Read input image
					const QColor colour = img.pixel(newcoord);
					const int channels[] = { colour.red(), colour.green(), colour.blue() };

					const int weight = kernel[x + y*m];

					accum += channels[c] * weight;
					factor += weight;
				}
			}

			accum /= std::max(factor, 1);
			newchannels[c] = std::min(std::max(accum, 0), 255);
		}

		return QColor(newchannels[0], newchannels[1], newchannels[2]).rgb();
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
