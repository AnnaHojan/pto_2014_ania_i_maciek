#include "map_height.h"

MapHeight::MapHeight(PNM* img) :
    Transformation(img)
{
}

MapHeight::MapHeight(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* MapHeight::transform()
{
    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

	// copypasta z conversion_grayscale::transform
	if (image->format() == QImage::Format_Mono)
	{
		// Iterate over image space
		for (int x = 0; x<width; x++)
			for (int y = 0; y<height; y++)
			{
			QColor color = QColor::fromRgb(image->pixel(x, y)); // Getting the pixel(x,y) value

			newImage->setPixel(x, y, color == Qt::white ? PIXEL_VAL_MAX : PIXEL_VAL_MIN);
			}
	}
	else // if (image->format() == QImage::Format_RGB32)
	{
		// Iterate over image space
		for (int x = 0; x<width; x++)
			for (int y = 0; y<height; y++)
			{
			QRgb pixel = image->pixel(x, y); // Getting the pixel(x,y) value

			int r = qRed(pixel);    // Get the 0-255 value of the R channel
			int g = qGreen(pixel);  // Get the 0-255 value of the G channel
			int b = qBlue(pixel);   // Get the 0-255 value of the B channel
			int new_color = (int)((0.6*r) + (0.3*g) + (0.1*g));
			newImage->setPixel(x, y, new_color);
			}
	}

    return newImage;
}
