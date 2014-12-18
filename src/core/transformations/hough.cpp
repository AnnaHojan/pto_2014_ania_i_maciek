#include "hough.h"

#include "conversion_grayscale.h"
#include "edge_laplacian.h"

Hough::Hough(PNM* img) :
    Transformation(img)
{
}

Hough::Hough(PNM* img, ImageViewer* super) :
    Transformation(img, super)
{
}

PNM* Hough::transform()
{   
    int thetaDensity = getParameter("theta_density").toInt();

	PNM* img = ConversionGrayscale::ConversionGrayscale(image).transform();

	if (getParameter("skip_edge_detection").toBool() == false) {
		EdgeLaplacian* lapl = new EdgeLaplacian(img);
		lapl->setParameter("size", 3);
		img = lapl->transform();
	}

	int width = image->width();
	int height = image->height();

	int pmax = (int) sqrt((height*height) + (width*width));
	int osize = 180 * thetaDensity;

	int img2_size = (pmax * 2) + 1;
	PNM* img2 = new PNM(osize, img2_size, QImage::Format_Indexed8);


	math::matrix<int> hough(osize, img2_size);
	for (int i = 0; i < osize; i++)
		for (int j = 0; j < img2_size; j++)
			hough(i, j) = 0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			QRgb I = qGray(img->pixel(i, j));

			if (qGray(I) > 0) {
				for (int k = 0; k < osize; k++) {
					float O = (float)(k * M_PI) / (float)(180 * thetaDensity);
					float p = i*cos(O) + j*sin(O);
					hough(k, int(p + pmax))++;
				}

			}
		}
	}

	int max = 0;
	int img2_width = img2->width();
	int img2_height = img2->height();
	for (int i = 0; i < img2_width; i++) {
		for (int j = 0; j < img2_height; j++) {
			if (max < hough(i, j))
				max = hough(i, j);
		}
	}

	for (int i = 0; i < img2_width; i++) {
		for (int j = 0; j < img2_height; j++) {
			img2->setPixel(i, j, (int)((hough(i, j) * 255) / max));
		}
	}

	return img2;
}
