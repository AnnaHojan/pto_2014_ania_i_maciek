#include "edge_canny.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

EdgeCanny::EdgeCanny(PNM* img) :
    Convolution(img)
{
}

EdgeCanny::EdgeCanny(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* EdgeCanny::transform()
{
    int width  = image->width(),
        height = image->height();

    int upper_thresh = getParameter("upper_threshold").toInt(),
        lower_thresh = getParameter("lower_threshold").toInt();

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

	PNM* img = ConversionGrayscale::ConversionGrayscale(image).transform();

	BlurGaussian blurGaussian(img);
	blurGaussian.setParameter("sigma", 1.6);
	blurGaussian.setParameter("size", 3);
	img = blurGaussian.transform();

	EdgeSobel sob(img);
	math::matrix<float>* G_x = sob.rawHorizontalDetection();
	math::matrix<float>* G_y = sob.rawVerticalDetection();


	math::matrix<float> m = math::matrix<float>(width, height);
	math::matrix<float> o = math::matrix<float>(width, height);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			m(i, j) = sqrt(((*G_x)(i, j)*(*G_x)(i, j)) + ((*G_y)(i, j)*(*G_y)(i, j)));
			float arctan = atan2((*G_y)(i, j), (*G_x)(i, j)) / M_PI;
			if (arctan < 0) {
				arctan = -arctan + 180;
			}
			o(i, j) = arctan;
		}
	}


	std::list<int> iksy;
	std::list<int> igreki;

	for (int i = 1; i < width - 1; i++) {
		for (int j = 1; j < height - 1; j++) {

			int p1, p2;
			int kier = kierunek(o(i, j));
			if (kier == 0) {
				p1 = m(i, j - 1);
				p2 = m(i, j + 1);
			}
			else if (kier == 1) {
				p1 = m(i + 1, j - 1);
				p2 = m(i - 1, j + 1);
			}
			else if (kier == 2) {
				p1 = m(i - 1, j);
				p2 = m(i + 1, j);
			}
			else if (kier == 3) {
				p1 = m(i - 1, j - 1);
				p2 = m(i + 1, j + 1);
			}

			if (m(i, j) > p1 && m(i, j) > p2 && m(i, j) > upper_thresh) {
				newImage->setPixel(i, j, 255);
				iksy.push_back(i);
				igreki.push_back(j);
			}
			else {
				newImage->setPixel(i, j, 0);
			}
		}
	}


	while (!(iksy.empty() && igreki.empty()))
	{
		int iks = iksy.back();
		iksy.pop_back();
		int igrek = igreki.back();
		igreki.pop_back();

		int kier = kierunek(o(iks, igrek));

		int poss_x[7] = { -1, -1, 0, 0, 1, 1, 1 };
		int poss_y[7] = { 0, 1, -1, 1, -1, 0, 1 };


		for (int i = 0; i <= 1; i++)
		{
			int x_mod;
			int y_mod;

			if (i == 0)
			{
				if (kier == 0)
				{
					x_mod = 0;
					y_mod = -1;
				}
				else if (kier == 1)
				{
					x_mod = 1;
					y_mod = -1;
				}
				else if (kier == 2)
				{
					x_mod = -1;
					y_mod = 0;
				}
				else if (kier == 3)
				{
					x_mod = -1;
					y_mod = -1;
				}
			}
			else
			{
				if (kier == 0)
				{
					x_mod = 0;
					y_mod = 1;
				}
				else if (kier == 1)
				{
					x_mod = -1;
					y_mod = 1;
				}
				else if (kier == 2)
				{
					x_mod = 1;
					y_mod = 0;
				}
				else if (kier == 3)
				{
					x_mod = 1;
					y_mod = 1;
				}
			}

			int x = iks;
			int y = igrek;

			while (true)
			{
				int nx = x + x_mod;
				int ny = y + y_mod;


				if (nx == width - 1 || nx == 0 || ny == height - 1 || ny == 0
					|| newImage->pixel(nx, ny) == 255 || m(nx, ny) < lower_thresh
					|| kierunek((int) o(nx, ny)) != kier) {
					break;
				}


				bool max = true;
				for (int i = 0; i < 7; i++)
				{
					int x_sum = nx + poss_x[i];
					int y_sum = ny + poss_y[i];

					if ((x_sum == x && y_sum == y) || kierunek((int) o(x_sum, y_sum)) != kier) {
						continue;
					}

					if (m(x_sum, y_sum) >= m(nx, ny))
					{
						max = false;
						break;
					}
				}

				if (!max) {
					break;
				}


				newImage->setPixel(nx, ny, 255);

				x = nx;
				y = ny;
			}
		}
	}





	return newImage;
}

int EdgeCanny::kierunek(float o) {

	if ((o < 22.5) || (o >= 157.5 && o < 202.5) || (o >= 337.5)) {
		return 0;
	}
	else if ((o >= 22.5 && o < 67.5) || (o >= 202.5 && o < 247.5)) {
		return 1;
	}
	else if ((o >= 67.5 && o < 112.5) || (o >= 247.5 && o < 292.5)) {
		return 2;
	}
	else if ((o >= 112.5 && o < 157.5) || (o >= 292.5 && o < 337.5)) {
		return 3;
	}

}