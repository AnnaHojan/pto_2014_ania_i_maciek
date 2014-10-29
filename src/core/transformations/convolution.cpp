#include "convolution.h"

/** Overloaded constructor */
Convolution::Convolution(PNM* img) :
    Transformation(img)
{
}

Convolution::Convolution(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

/** Returns a convoluted form of the image */
PNM* Convolution::transform()
{
    return convolute(getMask(3, Normalize), RepeatEdge);
}

/** Returns a sizeXsize matrix with the center point equal 1.0 */
math::matrix<float> Convolution::getMask(int size, Mode mode = Normalize)
{
    math::matrix<float> mask(size, size);

	int srodek = size / 2;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (i == srodek && j == srodek)
				mask(i, j) = 1;
			else
				mask(i, j) = 0;
		}
	}

    return mask;
}

/** Does the convolution process for all pixels using the given mask. */
PNM* Convolution::convolute(math::matrix<float> mask, Mode mode = RepeatEdge)
{
    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

	float waga_maski = sum(mask);

	if (image->format() == QImage::Format_Indexed8) {
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				int size_okna = mask.rowno();
				math::matrix<float> okno(size_okna, size_okna);
				okno = getWindow(i, j, size_okna, LChannel, mode);

				math::matrix<float> akumulator(size_okna, size_okna);
				akumulator = join(okno, reflection(mask));

				float suma_akumulatora = sum(akumulator);
				if (waga_maski > 0.000000000001 || waga_maski < -0.000000000001) // lol, never ever bêdzie 0, bo to float
				{
					suma_akumulatora = suma_akumulatora / waga_maski;
				}
				if (suma_akumulatora < 0)
					suma_akumulatora = 0;
				else if (suma_akumulatora > 255)
					suma_akumulatora = 255;

				newImage->setPixel(i, j, suma_akumulatora);
			}
		}
	}
	else if (image->format() == QImage::Format_RGB32)
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				int size_okna = mask.rowno();
				math::matrix<float> oknor(size_okna, size_okna);
				math::matrix<float> oknog(size_okna, size_okna);
				math::matrix<float> oknob(size_okna, size_okna);
				oknor = getWindow(i, j, size_okna, RChannel, mode);
				oknog = getWindow(i, j, size_okna, GChannel, mode);
				oknob = getWindow(i, j, size_okna, BChannel, mode);

				math::matrix<float> akumulatorr(size_okna, size_okna);
				math::matrix<float> akumulatorg(size_okna, size_okna);
				math::matrix<float> akumulatorb(size_okna, size_okna);
				akumulatorr = join(oknor, reflection(mask));
				akumulatorg = join(oknog, reflection(mask));
				akumulatorb = join(oknob, reflection(mask));

				float suma_akumulatorar = sum(akumulatorr);
				float suma_akumulatorag = sum(akumulatorg);
				float suma_akumulatorab = sum(akumulatorb);
				if (waga_maski > 0.000000000001 || waga_maski < -0.000000000001) // lol, never ever bêdzie 0, bo to float
				{
					suma_akumulatorar = suma_akumulatorar / waga_maski;
					suma_akumulatorag = suma_akumulatorag / waga_maski;
					suma_akumulatorab = suma_akumulatorab / waga_maski;
				}
				if (suma_akumulatorar < 0)
					suma_akumulatorar = 0;
				else if (suma_akumulatorar > 255)
					suma_akumulatorar = 255;

				if (suma_akumulatorag < 0)
					suma_akumulatorag = 0;
				else if (suma_akumulatorag > 255)
					suma_akumulatorag = 255;

				if (suma_akumulatorab < 0)
					suma_akumulatorab = 0;
				else if (suma_akumulatorab > 255)
					suma_akumulatorab = 255;

				QColor newPixel = QColor(suma_akumulatorar, suma_akumulatorag, suma_akumulatorab);
				newImage->setPixel(i, j, newPixel.rgb());
			}
		}
	}

    return newImage;
}

/** Joins to matrices by multiplying the A[i,j] with B[i,j].
  * Warning! Both Matrices must be squares with the same size!
  */
const math::matrix<float> Convolution::join(math::matrix<float> A, math::matrix<float> B)
{
    int size = A.rowno();
	int sizey = A.colno();
	math::matrix<float> C(size, sizey);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < sizey; j++)
		{
			C(i, j) = A(i, j) * B(j, i);
		}
	}

    return C;
}

/** Sums all of the matrixes elements */
const float Convolution::sum(const math::matrix<float> A)
{
    float sum = 0.0;
	int size = A.rowno();

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			sum += A(i, j);
		}
	}

    return sum;

}


/** Returns reflected version of a matrix */
const math::matrix<float> Convolution::reflection(const math::matrix<float> A)
{
    int size = A.rowno();
	int srodek = size / 2;
    math::matrix<float> C(size, size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			int roznica_wzgledem_srodka_x = srodek - i;
			int roznica_wzgledem_srodka_y = srodek - j;
			C((roznica_wzgledem_srodka_x * 2) + i, (roznica_wzgledem_srodka_y * 2) + j) = A(i, j);
		}
	}

    return C;
}
