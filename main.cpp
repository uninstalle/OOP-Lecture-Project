#include "ImageProcess.h"
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
	auto image = cv::imread("2chtex.png");
	ImageProcess::generate3ChannelsNormalTexture(&image);
	cv::imshow("test", image);
	cv::waitKey();
}

