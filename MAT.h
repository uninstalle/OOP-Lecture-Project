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