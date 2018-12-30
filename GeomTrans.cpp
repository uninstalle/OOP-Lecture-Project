#include"GeomTrans.h"

Mat Rotate(Mat src, int angle)
{
	Point2f centre(src.rows / 2, src.cols / 2);
	Mat dst;
	Mat rotMatrix = getRotationMatrix2D(centre, angle, 1);
	Rect newRect = RotatedRect(centre, src.size(), angle).boundingRect();
	rotMatrix.at<double>(0, 2) += newRect.width / 2 - centre.x;
	rotMatrix.at<double>(1, 2) += newRect.height / 2 - centre.y;
	warpAffine(src, dst, rotMatrix, newRect.size());

	return dst;


}

Mat Scale(Mat src, double scale)
{
	Mat dst;
	resize(src, dst, Size(), scale, scale, 1);
	return dst;
}

Mat Resize(Mat src, int width, int height)
{
	Mat dst;
	resize(src, dst, Size(width, height), 0, 0, 1);
	return dst;
}

Mat Flip(Mat src, int flipCode)
{
	Mat dst;
	flip(src, dst, flipCode);
	return dst;
}

Mat Translate(cv::Mat src, int offx, int offy)
{
	cv::Mat trans = cv::Mat::zeros(2, 3, CV_32FC1);
	cv::Mat dst;
	trans.at<float>(0, 0) = 1;
	trans.at<float>(0, 2) = offx;
	trans.at<float>(1, 1) = 1;
	trans.at<float>(1, 2) = offy;
	warpAffine(src, dst, trans, Size(src.cols + offx, src.rows + offy));
	return dst;
}

Mat Shear(Mat src, double ratio)
{
	Mat trans = Mat::zeros(2, 3, CV_32FC1);
	Mat dst;
	trans.at<float>(0, 0) = 1;
	trans.at<float>(0, 1) = ratio;
	trans.at<float>(1, 1) = 1;
	warpAffine(src, dst, trans, Size(trans.cols + trans.rows*ratio, trans.rows));
	return dst;
}

