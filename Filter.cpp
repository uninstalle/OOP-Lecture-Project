#include "Filter.h"

Mat NostalgicHueFilter(Mat src) {
	int height = src.rows;
	int width = src.cols;
	int Channels = src.channels();
	double R, G, B;
	double NewR, NewG, NewB;
	Mat NewSrc;
	src.copyTo(NewSrc);

	if (Channels == 3 || Channels == 4) {
		for (int y = 0; y < height; y++) {
			uchar *P = NewSrc.ptr<uchar>(y);

			for (int x = 0; x < width; x++) {
				B = P[Channels * x];
				G = P[Channels * x + 1];
				R = P[Channels * x + 2];

				NewB = 0.272*R + 0.534*G + 0.131*B;
				NewG = 0.349*R + 0.686*G + 0.168*B;
				NewR = 0.393*R + 0.769*G + 0.189*B;

				NewB = NewB > 255 ? 255 : NewB;
				NewB = NewB < 0 ? 0 : NewB;
				NewG = NewG > 255 ? 255 : NewG;
				NewG = NewG < 0 ? 0 : NewG;
				NewR = NewR > 255 ? 255 : NewR;
				NewR = NewR < 0 ? 0 : NewR;

				P[Channels * x] = (uchar)NewB;
				P[Channels * x + 1] = (uchar)NewG;
				P[Channels * x + 2] = (uchar)NewR;
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
	
}

Mat StrongLightFilter(Mat src) {

	int height = src.rows;
	int width = src.cols;
	double temp;
	Mat NewSrc;
	src.copyTo(NewSrc);
	int Channels = src.channels();

	if (Channels >= 1 && Channels <= 4) {
		int flag = (Channels >= 3) ? 3 : Channels;
		for (int y = 0; y < height; y++) {
			uchar *P = NewSrc.ptr<uchar>(y);
			for (int x = 0; x < width; x++) {
				for (int k = 0; k < flag; k++) {
					temp = P[Channels * x + k];
					if (temp > 127.5) {
						temp = temp + (255 - temp)*(temp - 127.5) / 127.5;
					}
					else {
						temp = temp * temp / 127.5;
					}
					temp = temp > 255 ? 255 : temp;
					temp = temp < 0 ? 0 : temp;
					P[Channels * x + k] = (uchar)temp;
				}
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
}

Mat DarkTownFilter(Mat src, double DarkDegree) {

	int height = src.rows;
	int width = src.cols;
	double temp;
	Mat NewSrc;
	src.copyTo(NewSrc);
	int Channels = src.channels();


	if (Channels >= 1 && Channels <= 4) {
		int flag = (Channels >= 3) ? 3 : Channels;
		for (int y = 0; y < height; y++) {
			uchar *P = NewSrc.ptr<uchar>(y);
			for (int x = 0; x < width; x++) {
				for (int k = 0; k < flag; k++) {
					temp = P[Channels * x + k];
					temp *= pow(temp / 255.0, DarkDegree);
					P[Channels * x + k] = (uchar)(temp);
				}
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
	
}

Mat FeatherFilter(Mat src, double VagueRatio) {
	Mat NewSrc;
	src.copyTo(NewSrc);

	int height = src.rows;
	int width = src.cols;
	int centerX = width / 2;
	int centerY = height / 2;
	int Channels = src.channels();

	double Vmax = centerX * centerX + centerY * centerY;
	double Vmin = VagueRatio * Vmax;
	double diffV = Vmax - Vmin;
	double xyRatio = width > height ? (double)width / (double)height : (double)height / (double)width;
	double temp, V;

	if (Channels >= 1 && Channels <= 4) {
		int flag = (Channels >= 3) ? 3 : Channels;
		for (int y = 0; y < height; y++) {
			uchar *P = NewSrc.ptr<uchar>(y);
			for (int x = 0; x < width; x++) {

				double dx = (double)(centerX - x);
				double dy = (double)(centerY - y);

				if (width < height) {
					dx = dx * xyRatio;
				}
				else {
					dy = dy * xyRatio;
				}

				V = (dx * dx + dy * dy) / diffV * 255.0;	//use Vmax - Vmin to control the feather degree

				for (int k = 0; k < flag; k++) {
					temp = P[Channels * x + k] + V;
					temp = temp > 255 ? 255 : temp;
					P[Channels * x + k] = (uchar)temp;
				}
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
	
}

Mat MosaicFilter(Mat src, int size) {
	int height = src.rows;
	int width = src.cols;
	double temp;
	int mUpper, nUpper;
	Mat NewSrc;
	src.copyTo(NewSrc);
	int Channels = src.channels();

	if (Channels >= 1 && Channels <= 4) {
		int flag = (Channels >= 3) ? 3 : Channels;
		for (int y = 0; y < height; y += size) {

			for (int x = 0; x < width; x += size) {

				uchar *P = src.ptr<uchar>(y, x);
				if (y + size - 1 >= height) {
					mUpper = height;
				}
				else mUpper = y + size;

				if (x + size - 1 >= width) {
					nUpper = width;
				}
				else nUpper = x + size;

				for (int m = y; m < mUpper; m++) {
					
					uchar *Q = NewSrc.ptr<uchar>(m);
					for (int n = x; n < nUpper; n++) {

						for (int k = 0; k < flag; k++) {
							Q[Channels*n + k] = P[k];
						}
					}
				}
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
	
}

Mat SculptureFilter(Mat src) {

	Mat NewSrc;
	src.copyTo(NewSrc);
	int height = src.rows;
	int width = src.cols;
	int Channels = src.channels();
	
	if (Channels >= 3 && Channels <= 4) {
		int flag = (Channels >= 3) ? 3 : Channels;
		for (int y = 1; y < height - 1; y++) {
			for (int x = 1; x < width - 1; x++) {
				uchar *P1 = src.ptr<uchar>(y - 1, x - 1);
				uchar *P2 = src.ptr<uchar>(y + 1, x + 1);
				uchar *Q = NewSrc.ptr<uchar>(y, x);
				for (int k = 0; k < flag; k++) {
					int temp = P2[k] - P1[k] + 128;
					temp = (temp < 0 ? 0 : temp);
					temp = (temp > 255 ? 255 : temp);
					Q[k] = (uchar)temp;
				}
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
	
}

Mat DiffusionFilter(Mat src) {

	Mat NewSrc;
	src.copyTo(NewSrc);
	int height = src.rows;
	int width = src.cols;
	cv::RNG rng;

	int Channels = src.channels();

	if (Channels >= 1 && Channels <= 4) {
		int flag = (Channels >= 3) ? 3 : Channels;
		for (int y = 1; y < height - 1; y++) {
			for (int x = 1; x < width - 1; x++) {
				uchar *Q = src.ptr<uchar>(y, x);
				int m = rng.uniform(0, 3) - 1;
				int n = rng.uniform(0, 3) - 1;
				uchar *P = src.ptr<uchar>(y + m, x + n);
				for (int k = 0; k < flag; k++) {
					Q[k] = P[k];
				}
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
}

Mat WindFilter(Mat src, int strength) {

	Mat NewSrc;
	src.copyTo(NewSrc);
	int height = src.rows;
	int width = src.cols;
	cv::RNG rng;
	int Channels = src.channels();
	int density = strength /2 + strength * 2;
	int length = 2 * strength - strength / 2;

	density < 2 ? 2 : density;

	if (Channels >= 1 && Channels <= 4) {
		int flag = (Channels >= 3) ? 3 : Channels;
		for (int y = 0; y < height; y++) {
			uchar *P = src.ptr<uchar>(y);
			uchar *Q = NewSrc.ptr<uchar>(y);
			for (int x = 0; x < width; x += density) {
				int tmp = rng.uniform(0, density);
				int root = x + tmp;
				root = root < width ? root : width;
				for (int k = 1; k < length; k++) {
					for (int m = 0; m < flag; m++) {
						Q[Channels*(root + k) + m] = P[Channels*root + m];
					}
				}
			}
		}
		return NewSrc;
	}
	else {
		throw process_error(FilterChannelsError, "This filter does not support the number of the channels of the picture.");
	}
	

}