#include "corner_harris.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

CornerHarris::CornerHarris(PNM* img) :
    Convolution(img)
{
}

CornerHarris::CornerHarris(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* CornerHarris::transform()
{
    int    threshold    = getParameter("threshold").toInt();
    double sigma        = getParameter("sigma").toDouble(),
           sigma_weight = getParameter("sigma_weight").toDouble(),
           k_param      = getParameter("k").toDouble();

    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

	math::matrix<double> Ixx(width, height);
	math::matrix<double> Ixy(width, height);
	math::matrix<double> Iyy(width, height);
	math::matrix<double> corner_candidates(width, height);
	math::matrix<double> corner_nonmax_suppress(width, height);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Ixx(i, j) = 0;
			Ixy(i, j) = 0;
			Iyy(i, j) = 0;
			corner_candidates(i, j) = 0;
			corner_nonmax_suppress(i, j) = 0;
		}
	}

	PNM* img = ConversionGrayscale::ConversionGrayscale(image).transform();

	BlurGaussian blurGaussian(img);
	blurGaussian.setParameter("sigma", 1.6);
	blurGaussian.setParameter("size", 3);
	img = blurGaussian.transform();

	EdgeSobel Sobel(img);
	math::matrix<float>* Gx = Sobel.rawHorizontalDetection();
	math::matrix<float>* Gy = Sobel.rawVerticalDetection();

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Ixx(i, j) = (*Gx)(i, j) * (*Gx)(i, j);
			Iyy(i, j) = (*Gy)(i, j) * (*Gy)(i, j);
			Ixy(i, j) = (*Gx)(i, j) * (*Gy)(i, j);
		}
	}

	for (int i = 1; i < width-1; i++) {
		for (int j = 1; j < height-1; j++) {

			float licznikxx = 0;
			float licznikyy = 0;
			float licznikxy = 0;

			for (int k = -1; k <= 1; k++) {
				for (int l = -1; l <= 1; l++) {

					float gaus = BlurGaussian::getGauss(k, l, sigma);

					licznikxx += Ixx(i + k, j + l) * gaus;
					licznikyy += Iyy(i + k, j + l) * gaus;
					licznikxy += Ixy(i + k, j + l) * gaus;

				}
			}

			float Sxx = licznikxx / sigma_weight;
			float Syy = licznikyy / sigma_weight;
			float Sxy = licznikxy / sigma_weight;

			math::matrix<double> H(2, 2);

			H(0, 0) = Sxx;
			H(0, 1) = Sxy;
			H(1, 1) = Syy;
			H(1, 0) = Sxy;

			float r = (H(0, 0) * H(1, 1)) - (H(0, 1) * H(1, 0)) - ((k_param * (H(0, 0) + H(1, 1)) * (H(0, 0) + H(1, 1))));
			if (r > threshold) {
				corner_candidates(i, j) = r;
			}

		}
	}

	bool search = true;
	while (search)
	{
		search = false;

		for (int i = 1; i < width-1; i++) {
			for (int j = 1; j < height-1; j++) {

				double ij_value = corner_candidates(i, j);
				if (ij_value > corner_candidates(i - 1, j - 1)
					&& ij_value > corner_candidates(i - 1, j)
					&& ij_value > corner_candidates(i - 1, j + 1)
					&& ij_value > corner_candidates(i, j - 1)
					&& ij_value > corner_candidates(i, j + 1)
					&& ij_value > corner_candidates(i + 1, j - 1)
					&& ij_value > corner_candidates(i + 1, j)
					&& ij_value > corner_candidates(i + 1, j + 1))
				{
					corner_nonmax_suppress(i, j) = ij_value;
				}
				else
				{
					if (ij_value > 0)
						search = true;
					corner_nonmax_suppress(i, j) = 0;
				}
			}
		}
		corner_candidates = corner_nonmax_suppress;
	}

	for (int i = 1; i < width - 1; i++) {
		for (int j = 1; j < height - 1; j++) {

			if (corner_candidates(i, j) == 0)
				newImage->setPixel(i, j, 0);
			else
				newImage->setPixel(i, j, 1);
		}
	}

    return newImage;
}
