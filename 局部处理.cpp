#include"�ֲ�����.h"


Mat RegularPart(Mat src, Point2d centre,ShapeType imgType,int w, int h,double alpha)
{

	Mat tmp=src.clone();
	Mat backRegion, foreRegion;
	GaussianBlur(tmp,tmp, Size(7, 7),12);//do some operations
	//ѡȡ���������������뿪
	Mat cutArea = GetRegularRegion(src, centre,imgType, w, h);
	tmp.copyTo(foreRegion,cutArea);
	bitwise_not(cutArea,cutArea);
	src.copyTo(backRegion,cutArea);
	Mat dst;
	add(foreRegion, backRegion, dst);//�ڴ�ͼ����ֱ�ӽ��в�����������������ȡ����ϲ�������ͼ��
	
	return dst;
}


Mat IrregulatPart(Mat src, std::vector<Vertex>ppos, const int npt)//ǰ�˴������ͼ��ʱ�ĵ㼯
{
	
	Mat tmp = src.clone();
	Mat backRegion, foreRegion;
	GaussianBlur(tmp, tmp, Size(7, 7), 12);//Only an example
	Mat cutArea = GetIrregularRegion(src,ppos,npt);
	tmp.copyTo(foreRegion, cutArea);
	bitwise_not(cutArea, cutArea);
	src.copyTo(backRegion, cutArea);
	Mat dst;
	add(foreRegion, backRegion, dst);//������������ȡ����ϲ�������ͼ��
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
	bitwise_not(cutArea, cutArea);
	src.copyTo(backRegion, cutArea);
	Mat dst;
	add(foreRegion, backRegion, dst);//������������ȡ����ϲ�������ͼ��
	return dst;
}


Mat GetRegularRegion(Mat src, Point2d centre, ShapeType imgType, int a, int b)//��ȡroi����,ע�⴫��Ķ�����������
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



Mat GetIrregularRegion(Mat src, std::vector<Vertex>ppos, const int npt)//����������㼯
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

Mat GetFilledRegion(Mat src, int threshold, Point2d seed)//����ͨ���߽���Ϣ������ͨ�����ѣ���ħ��Ч��
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
				dist += (p[imgChannel* cx + channel] - target[channel]) *(p[imgChannel* cx + channel] - target[channel]);//��ɫ����С����ֵ
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
	std::vector<std::vector<Point>> allContours;
	findContours(mask, allContours, RETR_LIST, CHAIN_APPROX_NONE);
	drawContours(src, allContours,-1, Scalar::all(255), 1);//��ͨ�㼯->���Ƴɱ߽�,Ϊ�˸�����ʾЧ����������ԭͼ�ϻ��Ƴ�ħ���߽�*/
	return mask;
}

Mat GetMinRect(Mat cutOut,Mat mask)//һ������ͼ�㣬�������ѡȡ�������С���Σ�������ѡȡ��������������Ӧ�Ӿ�Ч��
{
	Mat copy = cutOut.clone();
	std::vector<std::vector<Point>> allContours;
	findContours(mask, allContours, RETR_LIST, CHAIN_APPROX_NONE);
	Rect boundRect;
	boundRect =  boundingRect(Mat(allContours[0]));
	drawContours(copy, allContours,-1, Scalar::all(255));
	Mat cutRect = copy(boundRect);//��õ���С����
	return cutRect;
	
}

void ConvertedOperation(Mat mask)//����ѡ����
{
	bitwise_not(mask, mask);
}

Mat BlendRegion(Mat src, Mat addimg, Mat mask,double alpha)
{
	if (src.type() != addimg.type())
		throw std::runtime_error("Not of the same type.");
	int i, j;
	int imgchannel = src.channels();
	int channel;
	Mat dst=Mat(src.size(),src.type());
	uchar*p, *q, *d;
	for (i = 0; i < src.rows; i++)
	{
		if (mask.at<uchar>(i, j))//ѡ���ڵ�������Ҫ���
		{
			for (j = 0; j < src.cols; j++)
			for (channel = 0; channel < imgchannel; channel++)
			d[imgchannel*j + channel] = static_cast<int>(alpha*p[imgchannel*j + channel] + (1 - alpha)*q[imgchannel*j + channel]);
		}
		
	}

}