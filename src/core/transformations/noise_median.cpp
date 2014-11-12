#include "noise_median.h"

NoiseMedian::NoiseMedian(PNM* img) :
    Convolution(img)
{
}

NoiseMedian::NoiseMedian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseMedian::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

	if (image->format() == QImage::Format_Indexed8) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int value = getMedian(i, j, LChannel);
				newImage->setPixel(i, j, value);
			}
		}
	}
	else if (image->format() == QImage::Format_RGB32) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int valueR = getMedian(i, j, RChannel);
				int valueG = getMedian(i, j, GChannel);
				int valueB = getMedian(i, j, BChannel);
				QColor newPixel = QColor(valueR, valueG, valueB);
				newImage->setPixel(i, j, newPixel.rgb());
			}
		}
	}

    return newImage;
}

// pomocnicza funkcja wykorzystywana w quicksorcie
int compare(const void *a, const void *b)
{
	int value = *(int*)a - *(int*)b;
	return value;
}

int NoiseMedian::getMedian(int x, int y, Channel channel)
{
    int radius = getParameter("radius").toInt();
	int rozmiar = (radius * 2) + 1;

	math::matrix<float> window(rozmiar, rozmiar);
	window = getWindow(x, y, rozmiar, channel, RepeatEdge);

	int *tablica = new int[rozmiar * rozmiar];

	for (int i = 0; i < rozmiar; i++) {
		for (int j = 0; j < rozmiar; j++) {
			tablica[(i*rozmiar) + j] = window(i, j);
		}
	}

	qsort(tablica, rozmiar * rozmiar, sizeof(int), compare);

	int mediana = tablica[(rozmiar * rozmiar) / 2];
	delete[] tablica;
	return mediana;
}
