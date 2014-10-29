#include "blur_linear.h"

BlurLinear::BlurLinear(PNM* img) :
    Convolution(img)
{
}

BlurLinear::BlurLinear(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurLinear::transform()
{
    int maskSize = getParameter("size").toInt();
    QList<QVariant> tmpMask = getParameter("mask").toList();
    bool normalize = getParameter("normalize").toBool();

    math::matrix<float> mask(maskSize, maskSize);


	/*
		dzieje si� dziwna rzecz:
		przyk�ad zamieszczony na eduwiki rozmywa si� nie w t� stron� co powinien
		kiedy jednak maska zostaje przystosowana do poprawnego przekszta�cenia przyk�adu, pozosta�e przekszta�cenia
		wykonuj� si� w obr�cone o 90 stopni
		mo�e wi�c przyk�ad jest niepoprawny?
		z tego powodu na wszelki wypadek dok�adamy poni�szy parametr
	*/
	bool wersja_przeksztalcajaca_prawidlowo_przyklad_z_eduwiki = false;




	int licznik = 0;
	for (int i = 0; i < maskSize; i++)
	{
		for (int j = 0; j < maskSize; j++)
		{
			float x = tmpMask.at(licznik).toDouble();
			licznik++;
			if (!wersja_przeksztalcajaca_prawidlowo_przyklad_z_eduwiki)
				mask(i, j) = x;
			else
				mask(j, i) = x;
		}
	}
	if (normalize)
	{
		float suma = sum(mask);
		for (int i = 0; i < maskSize; i++)
		{
			for (int j = 0; j < maskSize; j++)
			{
				mask(i, j) = mask(i, j) / suma;
			}
		}
	}
	
	if (!wersja_przeksztalcajaca_prawidlowo_przyklad_z_eduwiki)
		mask = reflection(mask);

    return convolute(mask, RepeatEdge);
}
