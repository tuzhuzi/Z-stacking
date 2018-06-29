#include"stdafx.h"
#include"wavelet.h"
#include"jingshenyantuo.h"
#include<iostream>
#include<opencv2\opencv.hpp>
using namespace cv;
static jingshenyantuo method = jingshenyantuo();
static wavelet DWT = wavelet(2);
int thunderbolt() {
	Mat src = imread("1.jpg", 1);
	std::vector<Mat_<uchar>> RGBimg;
	split(src, RGBimg);
	Mat_<uchar> count[3];
	Mat_<uchar> maxdiff[3];
	for (int channel = 0; channel < 3; channel++) {
		Mat_<float> decpt = DWT.swavelett(RGBimg[channel]);
 		Mat_<float> Vrigin = Mat(decpt, Rect(0, 0, decpt.cols / 4, decpt.rows / 4));
		Mat diff = method.SWT(Vrigin);
		diff.copyTo(Vrigin);
		maxdiff[channel] = DWT.deswt(decpt);
	}
	Mat dst;
	merge(maxdiff, 3, dst);
	Mat tuoyanshijian;
	return 0;

}