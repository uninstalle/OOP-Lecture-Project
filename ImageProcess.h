#ifndef _IMAGE_PROCESS_H
#define _IMAGE_PROCESS_H

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