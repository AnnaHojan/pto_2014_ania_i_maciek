#include "edge_gradient.h"

EdgeGradient::EdgeGradient(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

EdgeGradient::EdgeGradient(PNM* img) :
    Convolution(img)
{
}

PNM* EdgeGradient::verticalDetection()
{
    return convolute(g_y, RepeatEdge);
}

PNM* EdgeGradient::horizontalDetection()
{
    return convolute(g_x, RepeatEdge);
}

PNM* EdgeGradient::transform()
{
    PNM* newImage = new PNM(image->width(), image->height(), image->format());

	PNM* ImageX = horizontalDetection();
	PNM* ImageY = verticalDetection();

	if (image->format() == QImage::Format_Indexed8) {

		for (int x = 0; x < image->width(); x++)
		{
			for (int y = 0; y < image->height(); y++)
			{
				QRgb pixel_x = ImageX->pixel(x, y);
				QRgb pixel_y = ImageY->pixel(x, y);
				int v_x = qGray(pixel_x);
				int v_y = qGray(pixel_y);

				int gray = sqrt((v_x * v_x) + (v_y * v_y));
				if (gray > 255)
					gray = 255;
				else if (gray < 0)
					gray = 0;

				newImage->setPixel(x, y, gray);
			}
		}

	}
	else if (image->format() == QImage::Format_RGB32) {

		for (int x = 0; x < image->width(); x++)
		{
			for (int y = 0; y < image->height(); y++)
			{
				QRgb pixel_x = ImageX->pixel(x, y);
				QRgb pixel_y = ImageY->pixel(x, y);
				int v_x_r = qRed(pixel_x);
				int v_y_r = qRed(pixel_y);
				int v_x_g = qGreen(pixel_x);
				int v_y_g = qGreen(pixel_y);
				int v_x_b = qBlue(pixel_x);
				int v_y_b = qBlue(pixel_y);


				int one = sqrt((v_x_r * v_x_r) + (v_y_r * v_y_r));
				int two = sqrt((v_x_g * v_x_g) + (v_y_g * v_y_g));
				int three = sqrt((v_x_b * v_x_b) + (v_y_b * v_y_b));

				if (one > 255)
					one = 255;
				else if (one < 0)
					one = 0;
				if (two > 255)
					two = 255;
				else if (two < 0)
					two = 0;
				if (three > 255)
					three = 255;
				else if (three < 0)
					three = 0;

				QColor newPixel = QColor(
					one,
					two,
					three
				);

				newImage->setPixel(x, y, newPixel.rgb());
			}
		}

	}


	return newImage;
}

