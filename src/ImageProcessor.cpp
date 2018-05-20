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

	imageUpdated(QPixmap::fromImage(m_a));
}

void ImageProcessor::resetImage()
{
	m_a = m_src;
	imageUpdated(QPixmap::fromImage(m_a));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageProcessor& ImageProcessor::makeGrayscale()
{
	apply([](auto img, auto coord) {
		QColor c = qGray(img.pixel(coord));
		return qRgb(c.blue(), c.blue(), c.blue());
	});

	return *this;
}

ImageProcessor& ImageProcessor::applyFilter(const KernelView& kernel)
{
	//Apply 3x3 filter kernel to each pixel
	apply([&](auto img, auto coord) {

		int newchannels[] = { 0, 0, 0 };

		//For each colour channel of the input image
		for (int c = 0; c < 3; c++)
		{
			int accum = 0;
			int factor = 0;

			//Apply kernel
			for (uint y = 0; y < kernel.m; y++)
			{
				for (uint x = 0; x < kernel.n; x++)
				{
					QPoint newcoord = coord + QPoint(x - (kernel.n >> 1), y - (kernel.m >> 1));

					newcoord.setX(std::max(0, std::min(img.size().width() - 1, newcoord.x())));
					newcoord.setY(std::max(0, std::min(img.size().height() - 1, newcoord.y())));

					//Read input image
					const QColor colour = img.pixel(newcoord);
					const int channels[] = { colour.red(), colour.green(), colour.blue() };

					const int weight = kernel[y][x];

					accum += channels[c] * weight;
					factor += weight;
				}
			}

			accum /= std::max(factor, 1);
			newchannels[c] = std::min(std::max(accum, 0), 255);
		}

		return QColor(newchannels[0], newchannels[1], newchannels[2]).rgb();
	});

	return *this;
}

ImageProcessor& ImageProcessor::applyNonLinearFilter()
{
	apply([&](auto img, auto coord) {

		Kernel<3, 3> k;

		for (uint y = 0; y < 3; y++)
		{
			for (uint x = 0; x < 3; x++)
			{
				QPoint newcoord = coord + QPoint(x - 1, y - 1);

				newcoord.setX(std::max(0, std::min(img.size().width() - 1, newcoord.x())));
				newcoord.setY(std::max(0, std::min(img.size().height() - 1, newcoord.y())));

				//Read input image
				k.v[x + y * 3] = img.pixel(newcoord);
			}
		}

		std::sort(std::begin(k.v), std::end(k.v));
		return k.v[5];
	});

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
