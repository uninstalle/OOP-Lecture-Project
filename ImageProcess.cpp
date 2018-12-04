#include "ImageProcess.h"
#include <opencv2/opencv.hpp>

cv::Mat& ImageProcess::parseMAT(MAT Mat)
{
	return *Mat;
}


ImageProcess::MAT ImageProcess::generate3ChannelsNormalTexture(MAT Mat)
{
	auto mat = parseMAT(Mat);

	//Warning : This is just a test program. Mat here must be CV_8UC4.
	//In real case, You need to process depend on the type of mat.
	//This function isn't that useful in fact. But my CG project happened to need it.

	for (int i = 0; i < mat.rows; ++i)
		for (int j = 0; j < mat.cols; ++j)
		{
			double x = (mat.at<cv::Vec3b>(i, j)[2] / 255.0) * 2 - 1;
			double y = (mat.at<cv::Vec3b>(i, j)[1] / 255.0) * 2 - 1;
			double z = cv::sqrt(1 - x * x - y * y);
			mat.at<cv::Vec3b>(i, j)[0] = (z*0.5 + 0.5) * 255;
		}

	return Mat;
}
