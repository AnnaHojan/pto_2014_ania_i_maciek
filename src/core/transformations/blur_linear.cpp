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
		dzieje siê dziwna rzecz:
		przyk³ad zamieszczony na eduwiki rozmywa siê nie w t¹ stronê co powinien
		kiedy jednak maska zostaje przystosowana do poprawnego przekszta³cenia przyk³adu, pozosta³e przekszta³cenia
		wykonuj¹ siê w obrócone o 90 stopni
		mo¿e wiêc przyk³ad jest niepoprawny?
		z tego powodu na wszelki wypadek dok³adamy poni¿szy parametr
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
