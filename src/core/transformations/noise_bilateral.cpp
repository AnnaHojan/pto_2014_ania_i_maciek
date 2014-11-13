#include "noise_bilateral.h"

NoiseBilateral::NoiseBilateral(PNM* img) :
    Convolution(img)
{
}

NoiseBilateral::NoiseBilateral(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseBilateral::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    sigma_d = getParameter("sigma_d").toInt();
    sigma_r = getParameter("sigma_r").toInt();
    radius = sigma_d;

	if (image->format() == QImage::Format_Indexed8) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int value = calcVal(i, j, LChannel);
				newImage->setPixel(i, j, value);
			}
		}
	}
	else if (image->format() == QImage::Format_RGB32) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int valueR = calcVal(i, j, RChannel);
				int valueG = calcVal(i, j, GChannel);
				int valueB = calcVal(i, j, BChannel);
				QColor newPixel = QColor(valueR, valueG, valueB);
				newImage->setPixel(i, j, newPixel.rgb());
			}
		}
	}

    return newImage;
}

int NoiseBilateral::calcVal(int x, int y, Channel channel)
{
	float licznik = 0;
	float mianownik = 0;

	int rozmiar = (radius * 2) + 1;

	math::matrix<float> window(rozmiar, rozmiar);
	window = getWindow(x, y, rozmiar, channel, RepeatEdge);

	for (int i = 0; i < rozmiar; i++) {
		for (int j = 0; j < rozmiar; j++) {
			QPoint pij(x + i - radius, y + j - radius);
			QPoint pxy(x, y);
			licznik += (window(i, j) * colorCloseness(window(i, j), window(radius, radius)) * spatialCloseness(pij, pxy)); // I(x, y) -> window(radius, radius), czyli środek okna
			mianownik += (colorCloseness(window(i, j), window(radius, radius)) * spatialCloseness(pij, pxy));
		}
	}

	float calcVal = licznik / mianownik;

	return calcVal;
}

float NoiseBilateral::colorCloseness(int val1, int val2)
{
	float value = exp(
		-(
		((val1 - val2)*(val1 - val2)) / (2 * (sigma_r * sigma_r))
		)
		);

	return value;
}

float NoiseBilateral::spatialCloseness(QPoint point1, QPoint point2)
{
	float value = exp(-(

		(((point1.x() - point2.x())*(point1.x() - point2.x())) + ((point1.y() - point2.y())*(point1.y() - point2.y()))) / (2 * (sigma_d*sigma_d))

		));

	return value;
}
