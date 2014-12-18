#include "hough_lines.h"

#include "bin_gradient.h"
#include "edge_laplacian.h"
#include "hough.h"

#include <QPainter>

HoughLines::HoughLines(PNM* img) :
    Transformation(img)
{
}

HoughLines::HoughLines(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HoughLines::transform()
{
    // Cut of value from the image;
    int  threshold      = getParameter("threshold").toInt();
    bool drawWholeLines = getParameter("draw_whole_lines").toBool();

	int width = image->width();
	int height = image->height();

    PNM* newImage = new PNM(image->copy());

	EdgeLaplacian* lapl = new EdgeLaplacian(image);
	lapl->setParameter("size", 3);
	PNM* img = lapl->transform();

	BinarizationGradient* binary = new BinarizationGradient(img);
	PNM*  img2 = binary->transform();

	Hough* hough = new Hough(img2);
	hough->setParameter("theta_density", 3);
	hough->setParameter("skip_edge_detection", true);
	PNM* img3 = hough->transform();

	int img3_width = img3->width();
	int img3_height = img3->height();

	QPainter* qp = new QPainter(newImage);
	qp->setPen(Qt::red);


	for (int th = 0; th < img3_width; th++) {
		for (int rh = 0; rh < img3_height; rh++) {

			int val = qGray(img3->pixel(th, rh));
			if (val > threshold) {
				int rrh = rh - (img3_height / 2);
				float rth = (((float)th / 3) * M_PI) / 180;

				int x_2 = (int)(rrh / sin(rth));
				int y_1 = width - 1;
				int y_2 = (int)((float)(rrh - (cos(rth) * y_1)) / sin(rth));

				qp->drawLine(0, x_2, y_1, y_2);
			}

		}
	}

	int img2_width = img2->width();
	int img2_height = img2->height();

	if (!drawWholeLines)
		for (int i = 0; i < img2_width; i++)
			for (int j = 0; j < img2_height; j++)
				if (qGray(img2->pixel(i, j)) == 0)
					newImage->setPixel(i, j, image->pixel(i, j));



    return newImage;
}
