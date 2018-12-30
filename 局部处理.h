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
Mat GetIrregularRegion(Mat tmp, vector<Vertex>ppos, const int npt);//多边形套索，传入点集
Mat GetFilledRegion(Mat src, int threshold, Point2d seed);
Mat GetMinRect(Mat cutOut, Mat mask);//一个副本图层，框出包含轮廓的最小区域，并绘制轮廓产生相应视觉效果
