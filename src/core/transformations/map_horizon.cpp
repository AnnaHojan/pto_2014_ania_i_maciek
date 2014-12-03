#include "map_horizon.h"

#include "map_height.h"

MapHorizon::MapHorizon(PNM* img) :
    Transformation(img)
{
}

MapHorizon::MapHorizon(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* MapHorizon::transform()
{
    int width  = image->width(),
        height = image->height();

    double scale     = getParameter("scale").toDouble();
    int    sun_alpha = getParameter("alpha").toInt();
    int dx, dy;

    switch (getParameter("direction").toInt())
    {
    case NORTH: dx = 0; dy = -1; break;
    case SOUTH: dx = 0; dy = 1; break;
    case EAST:  dx = 1; dy = 0; break;
    case WEST:  dx = -1; dy = 0; break;
    default:
        qWarning() << "Unknown direction!";
        dx =  0;
        dy = -1;
    }

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);


	PNM* mapa_wys = MapHeight(image).transform();

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			float alfa = 0;
			int current_h = qGray(mapa_wys->pixel(i, j));

			for (
				int k = i + dx, l = j + dy;
				k < width && l < height && k >= 0 && l >= 0;
				k = k + dx, l = l + dy
			) {

				int ray_h = qGray(mapa_wys->pixel(k, l));
				if (current_h < ray_h) {

					float dist = sqrt(((k - i)*(k - i)) + ((l - j)*(l - j))) * scale;
					float ray_alfa = atan((ray_h - current_h) / dist);

					if (ray_alfa > alfa) {
						alfa = ray_alfa;
					}
				}
			}

			float delta = alfa - ((sun_alpha * M_PI) / 180);
			if (delta > 0) {
				newImage->setPixel(i, j, cos(delta) * 255);
			}
			else {
				newImage->setPixel(i, j, 255);
			}

		}
	}

    return newImage;
}
