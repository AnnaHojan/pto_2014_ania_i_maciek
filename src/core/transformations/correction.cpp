#include "correction.h"

Correction::Correction(PNM* img) :
    Transformation(img)
{
}

Correction::Correction(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* Correction::transform()
{

    float shift  = getParameter("shift").toFloat();
    float factor = getParameter("factor").toFloat();
    float gamma  = getParameter("gamma").toFloat();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

	for (int i = 0; i <= 255; i++)
	{
		LUT[i] = i;
	}
	for (int i = 0; i <= 255; i++)
	{
		LUT[i] = int(LUT[i] + shift);
		LUT[i] = int(LUT[i] * factor);
		LUT[i] = int(pow(LUT[i], gamma));
		if (LUT[i] > 255)
		{
			LUT[i] = 255;
		}
		else if (LUT[i] < 0)
		{
			LUT[i] = 0;
		}
	}
	// Iterate over image space
	if (image->format() == QImage::Format_RGB32)
	{
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
			{
				QRgb pixel = image->pixel(x, y); // Getting the pixel(x,y) value
				int r = qRed(pixel);    // Get the 0-255 value of the R channel
				int g = qGreen(pixel);  // Get the 0-255 value of the G channel
				int b = qBlue(pixel);   // Get the 0-255 value of the B channel
				r = LUT[r];
				g = LUT[g];
				b = LUT[b];
				QColor newPixel = QColor(r, g, b);
				newImage->setPixel(x, y, newPixel.rgb());
			}
	}
	else if (image->format() == QImage::Format_Indexed8)
	{
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
			{
				QRgb pixel = image->pixel(x, y); // Getting the pixel(x,y) value
				int v = qGray(pixel);    
				v = LUT[v];
				newImage->setPixel(x, y, v);
			}
	}
    return newImage;
}
