#pragma once
#include<opencv2/opencv.hpp>

using namespace std;
using  namespace cv;

enum ShapeType { RECTANGLE, ELLIPSE };

struct Pixel
{
	int x;
	int y;
	Pixel* next;
};
struct Vertex
{
	int x;
	int y;

};

Mat RegularPart(Mat src, Point2d centre,ShapeType imgType, int w, int h);
Mat IrregulatPart(Mat src, std::vector<Vertex>ppos, const int npt);
Mat FilledPart(Mat src, int threshold, Point2d seed);
Mat GetRegularRegion(Mat tmp, Point2d centre, ShapeType imgType, int a, int b);
Mat GetIrregularRegion(Mat tmp, vector<Vertex>ppos, const int npt);//���������������㼯
Mat GetFilledRegion(Mat src, int threshold, Point2d seed);
Mat GetMinRect(Mat cutOut, Mat mask);//һ������ͼ�㣬���������������С���򣬲���������������Ӧ�Ӿ�Ч��
