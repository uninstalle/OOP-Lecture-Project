#include "ImageConverter.h"
#include <QImage>

static QVector<QRgb> ColorTable;

static void initializeColorTable()
{
	if (ColorTable.empty())
		for (int i = 0; i < 256; ++i)
			ColorTable.push_back(qRgb(i, i, i));
}

cv::Mat& ImageConverter::parseMAT(MAT Mat)
{
	return *Mat;
}


QImage ImageConverter::MatToQImage(MAT Mat)
{
	QImage *ptr = (QImage*)Mat;
	return *ptr;
}

QImage ImageConverter::MatCopyToQImage(MAT Mat)
{
	return MatToQImage(Mat).copy();
}

ImageConverter::MAT ImageConverter::QImageToMat(QImage& image)
{
	void * retVal = &image;
	return MAT(retVal);
}

ImageConverter::MAT ImageConverter::QImageCopyToMat(QImage& image)
{
	return QImageToMat(image);
}
