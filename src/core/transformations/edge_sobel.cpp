#include "edge_sobel.h"

EdgeSobel::EdgeSobel(PNM* img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

EdgeSobel::EdgeSobel(PNM* img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

void EdgeSobel::prepareMatrices()
{
	g_x = math::matrix<float>(3, 3);
	g_y = math::matrix<float>(3, 3);

	g_x(0, 0) = -1;
	g_x(0, 1) = -2;
	g_x(0, 2) = -1;
	g_x(1, 0) = 0;
	g_x(1, 1) = 0;
	g_x(1, 2) = 0;
	g_x(2, 0) = 1;
	g_x(2, 1) = 2;
	g_x(2, 2) = 1;

	g_y(0, 0) = -1;
	g_y(0, 1) = 0;
	g_y(0, 2) = 1;
	g_y(1, 0) = -2;
	g_y(1, 1) = 0;
	g_y(1, 2) = 2;
	g_y(2, 0) = -1;
	g_y(2, 1) = 0;
	g_y(2, 2) = 1;
}

math::matrix<float>* EdgeSobel::rawHorizontalDetection()
{
    math::matrix<float>* x_gradient = new math::matrix<float>(this->image->width(), this->image->height());

	for (int i = 0; i < x_gradient->rowno(); i++) {
		for (int j = 0; j < x_gradient->colno(); j++) {
			math::matrix<float> maska = getWindow(i, j, 3, LChannel, RepeatEdge);
			(*x_gradient)(i, j) = sum(join(g_x, maska));
		}
	}

    return x_gradient;
}

math::matrix<float>* EdgeSobel::rawVerticalDetection()
{
    math::matrix<float>* y_gradient = new  math::matrix<float>(this->image->width(), this->image->height());

	for (int i = 0; i < y_gradient->rowno(); i++) {
		for (int j = 0; j < y_gradient->colno(); j++) {
			math::matrix<float> maska = getWindow(i, j, 3, LChannel, RepeatEdge);
			(*y_gradient)(i, j) = sum(join(g_y, maska));
		}
	}

    return y_gradient;
}
