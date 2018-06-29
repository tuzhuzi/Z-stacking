#include"stdafx.h"
#include"wavelet.h"
#include<opencv2\opencv.hpp>
using namespace cv;
static wavelet H(1);

int watergun() {
	Mat src1 = imread("1.jpg", 0);
	Mat src2 = imread("10.jpg", 0);
	Mat src3 = imread("2.jpg", 0);
	Mat src4 = imread("11.jpg", 0);
	Mat dst = src1.clone();
	Mat src[4] = { src1,src2,src3,src4 };
	for (int i = 1; i < 4; i++)
		dst = H.wavelet_refusion(dst, src[i]);
	dst = H.deswt(dst);
	Mat filtdst = Mat::zeros(dst.size(), dst.type());
	bilateralFilter(dst, filtdst, 9, 15, 15);
	imwrite("su_gray_wavelet.jpg", dst);
	return 0;

}