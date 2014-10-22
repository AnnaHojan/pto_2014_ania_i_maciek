#include "histogram_stretching.h"

#include "../histogram.h"

HistogramStretching::HistogramStretching(PNM* img) :
    Transformation(img)
{
}

HistogramStretching::HistogramStretching(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramStretching::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

	if (image->format() == QImage::Format_Indexed8) {
		QHash<int, int>* szary_hist = new QHash<int, int>;
		Histogram::Channel channel = Histogram::Channel::LChannel;
		szary_hist = image->getHistogram()->get(channel);

		int min_key = PIXEL_VAL_MAX;
		int max_key = PIXEL_VAL_MIN;

		foreach(int i, szary_hist->keys())
		{
			if (i < min_key && szary_hist->value(i) > 0)
			{
				min_key = i;
			}
			if (i > max_key && szary_hist->value(i) > 0)
			{
				max_key = i;
			}
		}

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				QRgb pixel = image->pixel(x, y);
				int v = qGray(pixel);
				int new_v = (PIXEL_VAL_MAX * (v - min_key)) / (max_key - min_key);
				newImage->setPixel(x, y, new_v);
			}
		}
	}
	else if (image->format() == QImage::Format_RGB32)
	{
		QHash<int, int>* R_hist = new QHash<int, int>;
		QHash<int, int>* G_hist = new QHash<int, int>;
		QHash<int, int>* B_hist = new QHash<int, int>;
		Histogram::Channel R_channel = Histogram::Channel::RChannel;
		Histogram::Channel G_channel = Histogram::Channel::GChannel;
		Histogram::Channel B_channel = Histogram::Channel::BChannel;
		R_hist = image->getHistogram()->get(R_channel);
		G_hist = image->getHistogram()->get(G_channel);
		B_hist = image->getHistogram()->get(B_channel);

		int R_min_key = PIXEL_VAL_MAX;
		int R_max_key = PIXEL_VAL_MIN;
		int G_min_key = PIXEL_VAL_MAX;
		int G_max_key = PIXEL_VAL_MIN;
		int B_min_key = PIXEL_VAL_MAX;
		int B_max_key = PIXEL_VAL_MIN;

		foreach(int i, R_hist->keys())
		{
			if (i < R_min_key && R_hist->value(i) > 0)
			{
				R_min_key = i;
			}
			if (i > R_max_key && R_hist->value(i) > 0)
			{
				R_max_key = i;
			}
		}
		foreach(int i, G_hist->keys())
		{
			if (i < G_min_key && G_hist->value(i) > 0)
			{
				G_min_key = i;
			}
			if (i > G_max_key && G_hist->value(i) > 0)
			{
				G_max_key = i;
			}
		}
		foreach(int i, B_hist->keys())
		{
			if (i < B_min_key && B_hist->value(i) > 0)
			{
				B_min_key = i;
			}
			if (i > B_max_key && B_hist->value(i) > 0)
			{
				B_max_key = i;
			}
		}

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				QRgb pixel = image->pixel(x, y);

				int r = qRed(pixel);
				int g = qGreen(pixel);
				int b = qBlue(pixel);

				int new_r = (PIXEL_VAL_MAX * (r - R_min_key)) / (R_max_key - R_min_key);
				int new_g = (PIXEL_VAL_MAX * (g - G_min_key)) / (G_max_key - G_min_key);
				int new_b = (PIXEL_VAL_MAX * (b - B_min_key)) / (B_max_key - B_min_key);

				QColor newPixel = QColor(new_r, new_g, new_b);
				newImage->setPixel(x, y, newPixel.rgb());
			}
		}
	}

    return newImage;
}


