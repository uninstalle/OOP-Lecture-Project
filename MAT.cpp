#include "MAT.h"
#include <opencv2/opencv.hpp>

cv::Mat& parseMAT(MAT Mat)
{
	return *Mat.mat;
}

MAT packMAT(cv::Mat mat)
{
	return { std::make_shared<cv::Mat>(mat) };
}