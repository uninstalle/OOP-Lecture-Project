#pragma once
#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

Mat Rotate(Mat src, int angle);
Mat Scale(Mat src, double scale);//�ȱ���
Mat Resize(Mat src, int width, int height);//�������
Mat Translate(Mat src, int offx, int offy);//ƽ��
Mat Flip(Mat src, int flipCode);//��ת
Mat Shear(Mat src, double ratio);//����

