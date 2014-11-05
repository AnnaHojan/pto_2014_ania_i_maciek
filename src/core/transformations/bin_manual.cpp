#include "bin_manual.h"

BinarizationManual::BinarizationManual(PNM* img) :
    Transformation(img)
{
}

BinarizationManual::BinarizationManual(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationManual::transform()
{
    int threshold = getParameter("threshold").toInt();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

		for (int x = 0; x<width; x++)
			for (int y = 0; y<height; y++)
			{
			QRgb pixel = image->pixel(x, y); // Getting the pixel(x,y) value

			int v = qGray(pixel);    // Get the 0-255 value of the L channel
			newImage->setPixel(x, y, v < threshold ? Qt::color0 : Qt::color1);
			}

    return newImage;
}




