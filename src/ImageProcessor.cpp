/*
	Image processing
*/

#include <algorithm>

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

ImageProcessor& ImageProcessor::apply(const PixelFunction& func)
{
	for (int j = 0; j < m_a.height(); j++)
	{
		for (int i = 0; i < m_a.width(); i++)
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

ImageProcessor& ImageProcessor::makeGrayscale()
{
	return apply([](auto img, auto coord) {
		QColor c = qGray(img.pixel(coord));
		return qRgb(c.blue(), c.blue(), c.blue());
	});
}

ImageProcessor& ImageProcessor::applyFilter(const KernelView& kernel)
{
	//Apply 3x3 filter kernel to each pixel
	return apply([&kernel](auto img, auto coord) {

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
}

ImageProcessor& ImageProcessor::applyNonLinearFilter()
{
	return apply([](auto img, auto coord) {

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
}

ImageProcessor& ImageProcessor::applyThresholding()
{
	return apply([](auto img, auto coord) {

		//Make grey
		uint8_t c = img.pixel(coord);
		
		c = (c < 128) ? 0 : 255;

		return qRgb(c, c, c);
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void addError(QImage& img, const QPoint& coords, int error)
{
	int c = QColor(img.pixel(coords)).blue();
	c += error;
	img.setPixel(coords, c);
}

ImageProcessor& ImageProcessor::applyDithering(Dithering mode)
{
	//int error = 0;      //intensity error, difference between original pixel intensity and new intensity
	QImage& img = m_a;
	QImage& out = m_b;

	apply([](auto img, auto point) {
		return qGray(img.pixel(point));
	});

	const int threshold = 128;
	const int maxIntensity = 255;

	switch (mode)
	{
		case Dithering::ERROR_DIFFUSION:
		{
			int error = 0;

			for (int j = 0; j < m_a.height(); j++)
			{
				for (int i = 0; i < m_a.width(); i++)
				{
					//If row is even move left -> right, otherwise right -> left.
					const int pos = (j % 2 == 0) ? i : m_a.width() - (i + 1);

					//QPoint coords(pos, j);
					QPoint coords(i, j);

					int c = QColor(img.pixel(coords)).blue();
					c += error;
					int o = (c < threshold) ? 0 : maxIntensity;
					error = c - o;
				
					out.setPixel(coords, qRgb(o, o, o));
				}
			}

			break;
		}

		case Dithering::FLOYD_STEINBERG:
		{
			for (int j = 0; j < m_a.height(); j++)
			{
				for (int i = 0; i < m_a.width(); i++)
				{
					//QPoint coords(pos, j);
					QPoint coords(i, j);

					int c = QColor(img.pixel(coords)).blue();
					int o = (c < threshold) ? 0 : maxIntensity;
					int error = c - o;

					/*
						---|x|a|--
						-|b|g|d|--
						----------

						Pass error onto neighbouring pixels
					*/
					addError(img, coords + QPoint(+1, 0), error * 7 / 16); //alpha
					addError(img, coords + QPoint(-1, 1), error * 3 / 16); //beta
					addError(img, coords + QPoint(+0, 1), error * 5 / 16); //gamma
					addError(img, coords + QPoint(+1, 1), error * 1 / 16); //delta

					out.setPixel(coords, qRgb(o, o, o));
				}
			}

			break;
		}

		case Dithering::ORDERED:
		{
			break;
		}

		case Dithering::PATTERN:
		{
			break;
		}
	}

	//Swap image buffers
	img.swap(out);
	imageUpdated(QPixmap::fromImage(img));

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
