#include"局部处理.h"


Mat RegularPart(Mat src, Point2d centre,ShapeType imgType,int w, int h)
{

	Mat tmp=src.clone();
	Mat backRegion, foreRegion;
	GaussianBlur(tmp,tmp, Size(7, 7),12);//do some operations
	//选取区域和其余区域分离开
	Mat cutArea = GetRegularRegion(src, centre,imgType, w, h);
	tmp.copyTo(foreRegion,cutArea);
	Mat mask = Mat(src.size(), CV_8UC1, Scalar(255));
	subtract(mask,cutArea,mask);
	src.copyTo(backRegion, mask);
	Mat dst;
	//addWeighted(backRegion, 1,foreRegion, 1, 0, dst);//将处理过后的提取区域合并至背景图像
	return dst;
}


Mat IrregulatPart(Mat src, std::vector<Vertex>ppos, const int npt)//前端传入绘制图像时的点集
{
	
	Mat tmp = src.clone();
	Mat backRegion, foreRegion;
	GaussianBlur(tmp, tmp, Size(7, 7), 12);//Only an example
	Mat cutArea = GetIrregularRegion(src,ppos,npt);
	tmp.copyTo(foreRegion, cutArea);
	Mat mask = Mat(src.size(), CV_8UC1, Scalar(255));
	subtract(mask, cutArea, mask);
	src.copyTo(backRegion, mask);
	Mat dst;
	//addWeighted(backRegion, 1, foreRegion, 1, 0, dst);//将处理过后的提取区域合并至背景图像
	return dst;
}




Mat FilledPart(Mat src,int threshold,Point2d seed)
{
	if (src.type() != CV_8UC3 && src.type() != CV_8UC4)
	throw std::runtime_error("This function doesn't support for a grayscale image.");
	
	Mat tmp = src.clone();
	Mat backRegion, foreRegion;
	GaussianBlur(tmp, tmp, Size(7, 7), 12);//do some operations
	Mat cutArea = GetFilledRegion(src,threshold,seed);
	tmp.copyTo(foreRegion, cutArea);
	Mat mask = Mat(src.size(), CV_8UC1, Scalar(255));
	subtract(mask, cutArea, mask);
	src.copyTo(backRegion, mask);
	Mat dst;
	//addWeighted(backRegion, 1, foreRegion, 1, 0, dst);//将处理过后的提取区域合并至背景图像
	return dst;
}


Mat GetRegularRegion(Mat src, Point2d centre, ShapeType imgType, int a, int b)//提取roi区域,注意传入的都是中心坐标
{
	Mat mask = Mat::zeros(src.size(), CV_8UC1);
	Mat dst;
	if (imgType == ELLIPSE)
		ellipse(mask, centre, Size(a, b), 360, 0, 360, Scalar(255), -1);
	else if (imgType == RECTANGLE)
		rectangle(mask, Rect(centre.x - a, centre.y - b, 2 * a, 2 * b), Scalar(255), -1);
	else
	{
		throw std::runtime_error("It isn't a valid shape.");
	}

	return mask;
}



Mat GetIrregularRegion(Mat src, std::vector<Vertex>ppos, const int npt)//任意区域传入点集
{
	Mat dst;
	int index;
	Mat mask = Mat::zeros(src.size(), CV_8UC1);
	std::vector<std::vector<cv::Point>>ppt(1);
	for (int i = 0; i < npt; i++)
		ppt[0].push_back(Point(ppos[i].x, ppos[i].y));
	drawContours(mask,ppt,0,Scalar(255), -1);
	return mask;
}

Mat GetFilledRegion(Mat src, int threshold, Point2d seed)//不是通过边界信息，而是通过深搜，即魔棒效果
{
	
	Mat mask = Mat::zeros(src.size(), CV_8UC1);
	Mat*img = &src;
	int start = 0;
	int end = 1;
	int channel, i;
	int imgChannel = src.channels();
	int dist;
	Pixel*first = new Pixel;
	first->x = seed.x;
	first->y = seed.y;
	first->next = NULL;
	Pixel*last = first;
	int* target = new int[imgChannel];
	uchar *p = src.ptr<uchar>(seed.y);
	for (i = 0; i < imgChannel; i++)
	target[i] = p[first->x*imgChannel + i];
	mask.at<uchar>(first->y, first->x) = 255;
	while (end - start > 0)
	{
		int x = first->x;
		int y = first->y;
		int a[4][2] = { -1,0,0,1,0,-1,1,0 };
		for (i = 0; i < 4; i++)
		{
			int cx = x + a[i][0];
			int cy = y + a[i][1];
			if (cx >= 0 && cx < src.cols && cy >= 0 && cy < src.rows)
			{
				uchar *p = src.ptr<uchar>(cy);
				dist = 0;
				for (channel = 0; channel < imgChannel; channel++)
				dist += (p[imgChannel* cx + channel] - target[channel]) *(p[imgChannel* cx + channel] - target[channel]);//颜色距离小于阈值
				if (dist < threshold*threshold && !mask.at<uchar>(cy, cx))
				{
					Pixel*node = new Pixel;
					node->x = cx;
					node->y = cy;
					node->next = NULL;
					end++;
					last->next = node;
					last = node;
					for (channel = 0; channel<imgChannel; channel++)
						p[imgChannel* cx + channel] = 255 - p[imgChannel* cx + channel];
					mask.at<uchar>(cy, cx) = 255;//use to find contours
				}
			}
		}
		Pixel* temp = first;
		first = first->next;
		delete temp;
		start++;
	}
	/*std::vector<std::vector<Point>> allContours;
	findContours(mask, allContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	drawContours(src, allContours, 0, Scalar::all(255), 1);//连通点集->绘制成边界,为了更好显示效果，可以在原图上绘制出魔棒边界*/
	return mask;
}

Mat GetMinRect(Mat cutOut,Mat mask)//一个副本图层，框出包含选取区域的最小矩形，并绘制选取区域轮廓产生相应视觉效果
{
	Mat copy = cutOut.clone();
	std::vector<std::vector<Point>> allContours;
	findContours(mask, allContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	Rect boundRect;
	boundRect =  boundingRect(Mat(allContours[0]));
	drawContours(copy, allContours, 0, Scalar::all(255), 2);
	Mat cutRect = copy(boundRect);//获得的最小区域
	return cutRect;
}

