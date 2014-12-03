#include "map_normal.h"

#include "edge_sobel.h"
#include "map_height.h"

MapNormal::MapNormal(PNM* img) :
    Convolution(img)
{
}

MapNormal::MapNormal(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* MapNormal::transform()
{
    int width  = image->width(),
        height = image->height();

    double strength = getParameter("strength").toDouble();

    PNM* newImage = new PNM(width, height, image->format());


	PNM* mapa_wys = MapHeight(image).transform();
	EdgeSobel sobel(mapa_wys);

	math::matrix<float>* Gx = sobel.rawHorizontalDetection();
	math::matrix<float>* Gy = sobel.rawVerticalDetection();

	for (int i = 0; i < width; i++) {
		for (int j = 0; j <height; j++) {

			float dx = (*Gx)(i, j) / 255;
			float dy = (*Gy)(i, j) / 255;
			float dz = 1 / strength;

			float normalizacja = sqrt((dx*dx) + (dy*dy) + (dz*dz));

			dx = dx / normalizacja;
			dy = dy / normalizacja;
			dz = dz / normalizacja;

			dx = (dx + 1.0) * (255 / strength);
			dy = (dy + 1.0) * (255 / strength);
			dz = (dz + 1.0) * (255 / strength);


			newImage->setPixel(i, j, qRgb(dx, dy, dz));
		}
	}

    return newImage;
}
