#include "bin_gradient.h"

BinarizationGradient::BinarizationGradient(PNM* img) :
    Transformation(img)
{
}

BinarizationGradient::BinarizationGradient(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationGradient::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

	double licznik=0;
	double mianownik = 0;
	QRgb pixel, pixel1, pixel2, pixel3, pixel4;
	int g1, g2;
	for (int x = 0; x<width; x++)
		for (int y = 0; y<height; y++)
		{
			pixel = getPixel(x, y, RepeatEdge);
			pixel1 = getPixel(x+1, y, RepeatEdge);
			pixel2 = getPixel(x-1, y, RepeatEdge);
			pixel3 = getPixel(x, y+1, RepeatEdge);
			pixel4 = getPixel(x, y-1, RepeatEdge);

			g1 = qGray(pixel1) - qGray(pixel2);
			g2 = qGray(pixel3) - qGray(pixel4);
			if (g1 > g2)
			{
				licznik = licznik + (g1*qGray(pixel));
				mianownik += g1;
			}
			else
			{
				licznik = licznik + (g2*qGray(pixel));
				mianownik += g2;
			}
		}
	
	double threshold = licznik / mianownik;
	for (int x = 0; x<width; x++)
		for (int y = 0; y<height; y++)
		{
		QRgb pixel = image->pixel(x, y); 

		int v = qGray(pixel);    
		newImage->setPixel(x, y, v < threshold ? Qt::color0 : Qt::color1);
		}

    return newImage;
}


