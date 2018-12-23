#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

namespace Method
{
	class Filter
	{
		Mat NostalgicHueFilter(Mat src);							//怀旧色调滤镜
		Mat StrongLightFilter(Mat src);								//强光滤镜
		Mat DarkTownFilter(Mat src, double DarkDegree = 1.0);		//暗调滤镜，DarkDegree为暗度
		Mat FeatherFilter(Mat src, double VagueRatio = 0.5);		//羽化滤镜，VagueRatio只能取0到1，越大其边缘模糊比率越高（0相当于未羽化） 
		Mat MosaicFilter(Mat src, int size = 5);					//马赛克滤镜，size越大，打码越模糊
		Mat SculptureFilter(Mat src);								//浮雕滤镜
		Mat DiffusionFilter(Mat src);								//扩散（毛玻璃）滤镜
		Mat GaussianBlurFilter(Mat src, int size);					//高斯模糊滤镜，size越大越模糊
		Mat WindRilter(Mat src, int strength);						//风滤镜，strength为风的强度，强度越大，效果越明显
	};
}