#ifndef _IMAGE_PROCESS_H
#define _IMAGE_PROCESS_H
/*
 * 如何使用ImageProcess和ImageConverter：
 * ImageProcess.h隐藏了cv::Mat的实现，因此前端不需要准备OpenCV环境即可包含头文件
 * 前端调试时，可以自行将ImageProcess.cpp中的实现修改为不需要OpenCV。
 * 例如QImageToMat转换函数不对QImage做任何转换，只是返回QImage的指针；MatToQImage也只是解析指针
 * 当前后端合并时，再把ImageProcess.cpp改为当前的有作用的实现。
 * 由于ImageProcess.cpp不包含Qt头文件，因此后端不需要拥有Qt环境。
 * 
 * ImageConvertor.h同样隐藏了cv::Mat的实现
 * 便于前端调试时脱离OpenCV环境使用。
 * 后端没有使用需求，因此在头文件中直接使用了Qt类。
 */

namespace cv {
	class Mat;
}

class ImageProcess
{
	using MAT = cv::Mat*;
	static cv::Mat& parseMAT(MAT Mat);
public:
	static MAT generate3ChannelsNormalTexture(MAT Mat);
};

#endif