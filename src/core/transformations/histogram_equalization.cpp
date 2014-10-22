#include "histogram_equalization.h"

#include "../histogram.h"

HistogramEqualization::HistogramEqualization(PNM* img) :
    Transformation(img)
{
}

HistogramEqualization::HistogramEqualization(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramEqualization::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

	if (image->format() == QImage::Format_Indexed8) {
		double p[256] = {};
		for (int i = 0; i <= 255; i++)
		{
			p[i] = 0.0;
		}

		QHash<int, int>* szary_hist = new QHash<int, int>;
		Histogram::Channel channel = Histogram::Channel::LChannel;
		szary_hist = image->getHistogram()->get(channel);

		foreach(int i, szary_hist->keys())
		{
			p[i] = (double)szary_hist->value(i) / (double)(width * height);
		}

		double D[256] = {};
		for (int i = 0; i <= 255; i++)
		{
			if (i == 0)
				D[i] = p[i];
			else
				D[i] = D[i - 1] + p[i];
		}
		for (int i = 0; i <= 255; i++)
		{
			D[i] = D[i] * 255;
		}

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				QRgb pixel = image->pixel(x, y);
				int v = qGray(pixel);
				newImage->setPixel(x, y, (int)D[v]);
			}
		}
	}
	else if (image->format() == QImage::Format_RGB32)
	{
		double pr[256] = {};
		double pg[256] = {};
		double pb[256] = {};
		for (int i = 0; i <= 255; i++)
		{
			pr[i] = 0.0;
			pg[i] = 0.0;
			pb[i] = 0.0;
		}

		QHash<int, int>* histr = new QHash<int, int>;
		QHash<int, int>* histg = new QHash<int, int>;
		QHash<int, int>* histb = new QHash<int, int>;
		Histogram::Channel channelr = Histogram::Channel::RChannel;
		Histogram::Channel channelg = Histogram::Channel::GChannel;
		Histogram::Channel channelb = Histogram::Channel::BChannel;
		histr = image->getHistogram()->get(channelr);
		histg = image->getHistogram()->get(channelg);
		histb = image->getHistogram()->get(channelb);

		foreach(int i, histr->keys())
		{
			pr[i] = (double)histr->value(i) / (double)(width * height);
		}
		foreach(int i, histg->keys())
		{
			pg[i] = (double)histg->value(i) / (double)(width * height);
		}
		foreach(int i, histb->keys())
		{
			pb[i] = (double)histb->value(i) / (double)(width * height);
		}

		double Dr[256] = {};
		double Dg[256] = {};
		double Db[256] = {};
		for (int i = 0; i <= 255; i++)
		{
			if (i == 0)
			{
				Dr[i] = pr[i];
				Dg[i] = pg[i];
				Db[i] = pb[i];
			}
			else
			{
				Dr[i] = Dr[i - 1] + pr[i];
				Dg[i] = Dg[i - 1] + pg[i];
				Db[i] = Db[i - 1] + pb[i];
			}
		}
		for (int i = 0; i <= 255; i++)
		{
			Dr[i] = Dr[i] * 255;
			Dg[i] = Dg[i] * 255;
			Db[i] = Db[i] * 255;
		}

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				QRgb pixel = image->pixel(x, y);
				int r = qRed(pixel);
				int g = qGreen(pixel);
				int b = qBlue(pixel);

				int new_r = (int)Dr[r];
				int new_g = (int)Dg[g];
				int new_b = (int)Db[b];

				QColor newPixel = QColor(new_r, new_g, new_b);
				newImage->setPixel(x, y, newPixel.rgb());
			}
		}
	}

    return newImage;
}

