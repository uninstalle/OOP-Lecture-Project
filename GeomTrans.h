#pragma once
#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

Mat Rotate(Mat src, int angle);
Mat Scale(Mat src, double scale);//等比例
Mat Resize(Mat src, int width, int height);//任意比例
Mat Translate(Mat src, int offx, int offy);//平移
Mat Flip(Mat src, int flipCode);//翻转
Mat Shear(Mat src, double ratio);//错切

