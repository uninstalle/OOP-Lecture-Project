#include "Filter.h"

Mat NostalgicHueFilter(Mat src) {
	
	int height = src.rows;
	int width = src.cols;
	double R, G, B;
	double NewR, NewG, NewB;

	for (int y = 0; y < height; y++) {
		uchar *P = src.ptr<uchar>(y);

		for (int x = 0; x < width; x++) {
			B = P[3 * x];
			G = P[3 * x + 1];
			R = P[3 * x + 2];

			NewB = 0.272*R + 0.534*G + 0.131*B;
			NewG = 0.349*R + 0.686*G + 0.168*B;
			NewR = 0.393*R + 0.769*G + 0.189*B;

			NewB = NewB > 255 ? 255 : NewB;
			NewB = NewB < 0 ? 0 : NewB;
			NewG = NewG > 255 ? 255 : NewG;
			NewG = NewG < 0 ? 0 : NewG;
			NewR = NewR > 255 ? 255 : NewR;
			NewR = NewR < 0 ? 0 : NewR;

			P[3 * x] = (uchar)NewB;
			P[3 * x + 1] = (uchar)NewG;
			P[3 * x + 2] = (uchar)NewR;
		}
	}
	return src;
}

Mat StrongLightFilter(Mat src) {

	int height = src.rows;
	int width = src.cols;
	double temp;

	for (int y = 0; y < height; y++) {
		
		uchar *P = src.ptr<uchar>(y);

		for (int x = 0; x < width; x++) {

			for (int k = 0; k < 3; k++) {
				temp = P[3 * x + k];
				if (temp > 127.5) {
					temp = temp + (255 - temp)*(temp - 127.5) / 127.5;
				}
				else {
					temp = temp * temp / 127.5;
				}
				temp = temp > 255 ? 255 : temp;
				temp = temp < 0 ? 0 : temp;
				P[3 * x + k] = (uchar)temp;
			}
		}
	}
	return src;
}

Mat DarkTownFilter(Mat src, double DarkDegree) {

	int height = src.rows;
	int width = src.cols;
	double temp;

	for (int y = 0; y < height; y++) {
		uchar *P = src.ptr<uchar>(y);
		for (int x = 0; x < width; x++) {
			for (int k = 0; k < 3; k++) {
				temp = P[3 * x + k];
				temp *= pow(temp / 255.0, DarkDegree);
				P[3 * x + k] = (uchar)(temp);
			}
		}
	}
	return src;
}

Mat FeatherFilter(Mat src, double VagueRatio) {

	int height = src.rows;
	int width = src.cols;
	int centerX = width / 2;
	int centerY = height / 2;

	double Vmax = centerX * centerX + centerY * centerY;
	double Vmin = VagueRatio * Vmax;
	double diffV = Vmax - Vmin;
	double xyRatio = width > height ? (double)width / (double)height : (double)height / (double)width;
	double temp, V;

	for (int y = 0; y < height; y++) {
		uchar *P = src.ptr<uchar>(y);
		for (int x = 0; x < width; x++) {

			double dx = (double)(centerX - x);
			double dy = (double)(centerY - y);

			if (width < height) {
				dx = dx * xyRatio;
			}
			else{
				dy = dy * xyRatio;
			}

			V = (dx * dx + dy * dy) / diffV * 255.0;	//use Vmax - Vmin to control the feather degree

			for (int k = 0; k < 3; k++) {
				temp = P[3 * x + k] + V;
				temp = temp > 255 ? 255 : temp;
				P[3 * x + k] = (uchar)temp;
			}
		}
	}
	return src;
}

Mat MosaicFilter(Mat src, int size) {
	
	int height = src.rows;
	int width = src.cols;
	double B, G, R;
	int mUpper, nUpper;

	for (int y = 0; y < height; y++) {
		uchar *P = src.ptr<uchar>(y);
		for (int x = 0; x < width; x += size) {
			B = P[3 * x];
			G = P[3 * x + 1];
			R = P[3 * x + 2];

			if (y + size - 1 >= height) {
				mUpper = height;
			}
			else mUpper = y + size;

			if (x + size - 1 >= width) {
				nUpper = width;
			}
			else nUpper = x + size;

			for (int m = y; m < mUpper; m++) {
				uchar *Q = src.ptr<uchar>(m);
				for (int n = x; n < nUpper; n++) {
					Q[3 * n] = (uchar)B;
					Q[3 * n + 1] = (uchar)G;
					Q[3 * n + 2] = (uchar)R;
				}
			}
		}
	}
	return src;
}

Mat SculptureFilter(Mat src) {
	
	Mat NewSrc = src.clone();
	int height = src.rows;
	int width = src.cols;

	for (int y = 1; y < height - 1; y++) {
		uchar *P1 = src.ptr<uchar>(y - 1);
		uchar *P2 = src.ptr<uchar>(y + 1);
		uchar *Q = NewSrc.ptr<uchar>(y);
		for (int x = 1; x < width - 1; x++) {
			for (int k = 0; k < 3; k++) {
				int temp = P2[3 * (x + 1) + k] - P1[3 * (x - 1) + k] + 128;
				temp = (temp < 0 ? 0 : temp);
				temp = (temp > 255 ? 255 : temp);
				Q[3 * x + k] = temp;
			}
		}
	}
	return NewSrc;
}

Mat DiffusionFilter(Mat src) {

	Mat NewSrc = src;
	int height = src.rows;
	int width = src.cols;
	cv::RNG rng;

	for (int y = 0; y < height; y++) {
		uchar *Q = src.ptr<uchar>(y);
		for (int x = 1; x < width - 1; x++) {
			int m = rng.uniform(0, 3) - 1;
			int n = rng.uniform(0, 3) - 1;
			uchar *P = src.ptr<uchar>(y + m);
			for (int k = 0; k < 3; k++) {
				Q[3 * x + k] = P[3 * (x + n) + k];
			}
		}
	}
	return NewSrc;
}

Mat GaussianBlurFilter(Mat src, int size) {
	
	Mat NewSrc = src;
	GaussianBlur(src, NewSrc, cv::Size(2 * size - 1, 2 * size - 1), 0);
	return NewSrc;

}

Mat WindFilter(Mat src, int strength) {

	int height = src.rows;
	int width = src.cols;
	cv::RNG rng;

	int density = width / (strength * 20);
	int length = 2 * strength;

	density < 2 ? 2 : density;
	for (int y = 0; y < height; y++) {
		uchar *P = src.ptr<uchar>(y);
		uchar *Q = src.ptr<uchar>(y);
		for (int x = 0; x < width; x += density) {
			int tmp = rng.uniform(0, density);
			int root = x + tmp;
			root = root < 255 ? root : 255;
			for (int k = 1; k < length; k++) {
				Q[3 * (root + k)] = P[3 * root];
				Q[3 * (root + k) + 1] = P[3 * root + 1];
				Q[3 * (root + k) + 2] = P[3 * root + 2];
			}
		}
	}
	return src;

}