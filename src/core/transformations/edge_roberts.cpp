#include "edge_roberts.h"

EdgeRoberts::EdgeRoberts(PNM* img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

EdgeRoberts::EdgeRoberts(PNM* img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

void EdgeRoberts::prepareMatrices()
{
	g_x = math::matrix<float>(3, 3); // maska 3x3 to hack na wywalanie si� dla maski 2x2
	g_y = math::matrix<float>(3, 3);

	g_x(0, 0) = 1;
	g_x(0, 1) = 0;
	g_x(1, 0) = 0;
	g_x(1, 1) = -1;

	g_y(0, 0) = 0;
	g_y(0, 1) = -1;
	g_y(1, 0) = 1;
	g_y(1, 1) = 0;
}
