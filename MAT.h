#pragma once
#include <memory>

namespace cv
{
	class Mat;
}

struct MAT
{
	std::shared_ptr<cv::Mat> mat;
};


cv::Mat& parseMAT(MAT Mat);
MAT packMAT(cv::Mat mat);