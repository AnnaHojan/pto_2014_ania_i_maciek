#include "transformation.h"

Transformation::Transformation(PNM* image) :
    QThread(0)
{
    parameters = new QHash<QString, QVariant>;
    this->image = image;
    this->supervisor = 0;
}

Transformation::Transformation(PNM* image, ImageViewer* iv) :
    QThread(0)
{
    parameters = new QHash<QString, QVariant>;
    this->image = image;
    this->supervisor = iv;

    if (iv)
    {
        connect(this, SIGNAL(started()), iv, SLOT(transformationStarted()));
        connect(this, SIGNAL(finished()), iv, SLOT(transformationFinished()));
        connect(this, SIGNAL(message(QString)), iv, SLOT(toolMessage(QString)));
        connect(this, SIGNAL(progress(int)), iv, SLOT(toolProgress(int)));
        connect(this, SIGNAL(resultReady(PNM*)), iv, SLOT(updateImage(PNM*)));
        connect(this, SIGNAL(finished()), iv, SLOT(toolFinished()));
    }
}

Transformation::~Transformation()
{
    delete parameters;
    if (supervisor)
    {
        disconnect(this, SIGNAL(started()));
        disconnect(this, SIGNAL(message(QString)));
        disconnect(this, SIGNAL(progress(int)));
        disconnect(this, SIGNAL(resultReady(PNM*)));
        disconnect(this, SIGNAL(finished()));
    }
}

/** Sets a parameter param of the transformation
* with the value.
* If the parameter was already set it is overridden
* Returns the pointer to the Transformation object so it can be used as a factory.
*/
Transformation* Transformation::setParameter(QString param, QVariant value)
{
    parameters->insert(param, value);
    return this;
}

/** Returns the value of the given param.
* If the param isn't set returns a default QVariant()
* which is an invalid variant
* @see QVariant::QVariant()
*/
QVariant Transformation::getParameter(QString param)
{
    return parameters->value(param, QVariant());
}

void Transformation::run()
{
    PNM* image = this->transform();
    emit resultReady(image);
}

/**
* /virtual, abstract!/
* Virtual method that do some transformations (based on parameter Hash)
* in the Transformation class it returns a pointer to
* the loaded image or a new null PNM image
*/
PNM* Transformation::transform()
{
    if (image)
        return image;
    else
        return new PNM();
}

/** Returns a pointer to the image stored in the class */
PNM* Transformation::getImage()
{
    return image;
}

/** Returns a pixel value at given coordinates using different modes */
QRgb Transformation::getPixel(int x, int y, Mode mode)
{
    switch (mode) {
    case CyclicEdge:    return getPixelCyclic(x,y);
    case NullEdge:      return getPixelNull(x,y);
    case RepeatEdge:    return getPixelRepeat(x,y);
    default:            return image->pixel(x,y);
    }

}

/** Returns a pixel using the Cyclic mode:
 *  pixel(x,y) = pixel(x%width, y%width);
 */
QRgb Transformation::getPixelCyclic(int x, int y)
{
	int width = image->width();
	int height = image->height();

	if (x > width - 1)
	{
		x = x - width;
	}
	else if (x < 0)
	{
		x = x + width;
	}

	if (y > height - 1)
	{
		y = y - height;
	}
	else if (y < 0)
	{
		y = y + height;
	}

    return image->pixel(x,y);
}

/**
  * Returns a given pixel
  * If the pixel is out of image boundaries Black is returned;
  */
QRgb Transformation::getPixelNull(int x, int y)
{
	int width = image->width();
	int height = image->height();

	if (x > width - 1 || x < 0 || y > height - 1 || y < 0)
	{
		QRgb pixel = qRgb(0, 0, 0);
		return pixel;
	}
	else
	{
		return image->pixel(x, y);
	}
}

/**
  * Returns given pixel.
  * If the pixel is out of image boundaries
  * the nearest edge pixel is given
  */
QRgb Transformation::getPixelRepeat(int x, int y)
{
	int width = image->width();
	int height = image->height();

	if (x > width - 1)
	{
		x = width - 1;
	}
	else if (x < 0)
	{
		x = 0;
	}

	if (y > height - 1)
	{
		y = y - height;
	}
	else if (y < 0)
	{
		y = 0;
	}

    return image->pixel(x,y);
}

/** Returns a size x size part of the image centered around (x,y) */
math::matrix<float> Transformation::getWindow(int x, int y, int size,
                                              Channel channel,
                                              Mode mode = RepeatEdge)
{
    math::matrix<float> window(size,size);

	int sasiadow = size / 2;
	int od_x = x - sasiadow;
	int do_x = x + sasiadow;
	int od_y = y - sasiadow;
	int do_y = y + sasiadow;

	int licznik_x = 0;
	int licznik_y = 0;
	for (int i = od_x; i <= do_x; i++)
	{
		for (int j = od_y; j <= do_y; j++)
		{
			QRgb pixel = getPixel(i, j, mode);
			int value;
			if (channel == LChannel) value = qGray(pixel);
			if (channel == RChannel) value = qRed(pixel);
			if (channel == GChannel) value = qGreen(pixel);
			if (channel == BChannel) value = qBlue(pixel);
			window(licznik_x, licznik_y) = value;
			++licznik_y;
		}
		++licznik_x;
		licznik_y = 0;
	}

    return window;
}

ImageViewer* Transformation::getSupervisor()
{
    return supervisor;
}
