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
				k[y][x] = img.pixel(newcoord);
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
	//Check if pixel is inside the image bounds
	if (coords.x() >= img.size().width() || coords.x() <= 0)
		return;
	if (coords.y() >= img.size().height() || coords.y() <= 0)
		return;

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

	//dithering template pattern matrix
	const Kernel<4, 4> pattern = {
		1,  9,  3,  11,
		13, 5,  15, 7,
		4,  12, 2,  10,
		16, 8,  14, 6
	};

	switch (mode)
	{
		case Dithering::ERROR_DIFFUSION:
		{
			//intensity error
			int error = 0;

			for (int j = 0; j < m_a.height(); j++)
			{
				for (int i = 0; i < m_a.width(); i++)
				{
					//If row is even move left -> right, otherwise right -> left.
					const int pos = (j % 2 == 0) ? i : m_a.width() - (i + 1);

					QPoint coords(pos, j);
					//QPoint coords(i, j);

					int curp = QColor(img.pixel(coords)).blue(); //current pixel value
					curp += error;
					int newp = (curp < threshold) ? 0 : maxIntensity; //thresholded pixel value
					error = curp - newp; //pass error onto next pixel
				
					out.setPixel(coords, qRgb(newp, newp, newp));
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

					int curp = QColor(img.pixel(coords)).blue();       //current pixel value
					int newp = (curp < threshold) ? 0 : maxIntensity;  //thresholded pixel value
					int error = curp - newp;

					/*
						---|x|a|--
						-|b|g|d|--
						----------

						Pass error of current pixel onto neighbouring pixels
					*/
					addError(img, coords + QPoint(+1, 0), (int)(error * 7.0f / 16)); //alpha
					addError(img, coords + QPoint(-1, 1), (int)(error * 3.0f / 16)); //beta
					addError(img, coords + QPoint(+0, 1), (int)(error * 5.0f / 16)); //gamma
					addError(img, coords + QPoint(+1, 1), (int)(error * 1.0f / 16)); //delta

					out.setPixel(coords, qRgb(newp, newp, newp));
				}
			}

			break;
		}

		case Dithering::ORDERED:
		{
			const int n = 4;

			for (int j = 0; j < m_a.height(); j++)
			{
				for (int i = 0; i < m_a.width(); i++)
				{
					QPoint coords(i, j);

					int curp = QColor(img.pixel(coords)).blue();       //current pixel value

					//Normalized intensity of pixel 
					float intensity = (float)curp / 255;
					//Compute pattern number
					int p = std::min((int)(intensity * (n*n + 1)), n*n);

					//Compare pattern number against corresponding number in template
					int newp = (p < pattern[j % n][i % n]) ? 0 : maxIntensity;

					out.setPixel(coords, qRgb(newp, newp, newp));
				}
			}

			break;
		}

		case Dithering::PATTERN:
		{
			const int n = 4;
			const int area = n * n;

			//Foreach n*n region of pixels
			for (int j = 0; j < m_a.height(); j += n)
			{
				for (int i = 0; i < m_a.width(); i += n)
				{
					int totalIntensity = 0;

					//Get total intensity
					for (int y = 0; y < n; y++)
						for (int x = 0; x < n; x++)
							totalIntensity += QColor(img.pixel(QPoint(i + x, j + y))).blue();

					//Normalized average intensity of pixel region 
					float normIntensity = (float)(totalIntensity / area) / 255.0f;
					//Compute pattern number
					int p = std::min((int)(normIntensity * (area + 1)), area);

					//Fill in pixel region
					for (int y = 0; y < n; y++)
						for (int x = 0; x < n; x++)
						{
							QPoint coords(i + x, j + y);

							//Threshold each pixel based on pattern matrix
							int newp = (p < pattern[y][x]) ? 0 : maxIntensity;

							out.setPixel(coords, qRgb(newp, newp, newp));
						}
				}
			}
		}
	}

	//Swap image buffers
	img.swap(out);
	imageUpdated(QPixmap::fromImage(img));

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
