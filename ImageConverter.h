#ifndef _IMAGE_CONVERTER_H
#define _IMAGE_CONVERTER_H
#include <QImage>
#include "MAT.h"


class ImageConverter {
	
	static cv::Mat& parseMAT(MAT Mat);
	static MAT packMAT(cv::Mat mat);
public:
	//ATTENTION: This function does shallow copy.
	static QImage MatToQImage(MAT Mat);
	static MAT QImageToMat(QImage &image);

	//ATTENTION: This function does deep copy. (needed?)
	static QImage MatCopyToQImage(MAT Mat);
	static MAT QImageCopyToMat(QImage &image);
};

#endif