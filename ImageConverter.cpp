#include "ImageConverter.h"
#include <opencv2/opencv.hpp>
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
	return *Mat.mat;
}

MAT ImageConverter::packMAT(cv::Mat mat)
{
	return { std::make_shared<cv::Mat>(mat) };
}


QImage ImageConverter::MatToQImage(MAT Mat)
{
	auto mat = parseMAT(Mat);

	switch (mat.type())
	{
	//8 bits, 1 channel
	case CV_8UC1:
	{
	// A tip for readers:
	// QImage constructed in this way(using existing data) will not delete
	// its data after this QImage itself destructed.
	// Thus using it in converting is safe.
		QImage image(mat.data, mat.cols, mat.rows, QImage::Format_Indexed8);
		initializeColorTable();
		image.setColorTable(ColorTable);
		return image;
	}

	//8 bits, 3 channels
	case CV_8UC3:
	{
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}

	//8 bits, 4 channels
	case CV_8UC4:
	{
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image;
	}

	default:
		throw std::runtime_error("Mat to QImage failed.Image type not supported by converter.");
	}
}

QImage ImageConverter::MatCopyToQImage(MAT Mat)
{
	return MatToQImage(Mat).copy();
}

MAT ImageConverter::QImageToMat(QImage& image)
{
	// Using existing data means Mat will not automatically deallocate
	// these memories. Remember to free.
	switch (image.format())
	{
	case QImage::Format_Indexed8:
	{
		MAT retMat{ std::make_shared<cv::Mat>(cv::Mat(image.height(), image.width(), CV_8UC1, reinterpret_cast<void*>(image.bits()), image.bytesPerLine())) };
		return retMat;
	}
	case QImage::Format_RGB888:
	{
		MAT retMat{ std::make_shared<cv::Mat>(cv::Mat(image.height(), image.width(), CV_8UC3, reinterpret_cast<void*>(image.bits()), image.bytesPerLine())) };
		cv::cvtColor(parseMAT(retMat), parseMAT(retMat), cv::COLOR_BGR2RGB);
		return retMat;
	}
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
	{
		MAT retMat{ std::make_shared<cv::Mat>(cv::Mat(image.height(), image.width(), CV_8UC4, reinterpret_cast<void*>(image.bits()), image.bytesPerLine())) };
		return retMat;
	}
	default:
		throw std::runtime_error("QImage to Mat failed.Image type not supported by converter.");
	}
}

MAT ImageConverter::QImageCopyToMat(QImage& image)
{
	MAT retMat = QImageToMat(image);
	parseMAT(retMat) = parseMAT(retMat).clone();
	return retMat;
}
