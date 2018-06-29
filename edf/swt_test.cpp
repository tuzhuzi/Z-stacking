#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<iostream>
using namespace cv;
int multiattack() {
	Mat h0 = (Mat_<float>(1, 2) << 0.5, 0.5);
	Mat h1 = (Mat_<float>(1, 2) << 0.5, -0.5);
	Mat g0 = (Mat_<float>(2, 1) << 0.5, 0.5);
	Mat g1 = (Mat_<float>(2, 1) << 0.5, -0.5);
	Mat src = imread("10.jpg", 0);
	Mat vsrc (src.size(), CV_32FC1);
	src.convertTo(vsrc, CV_32FC1);
	int depth = 3;
	std::vector<Mat_<float>> wavelet(3);
	std::vector<std::vector<Mat_<float>>> Kwavelet(depth);
	for (int i = 0; i < depth; i++) {
		Mat tmp1, tmp2;
		filter2D(vsrc, tmp1, CV_32FC1, h0, Point(0, 0));
		filter2D(vsrc, tmp2, CV_32FC1, h1, Point(0, 0));
		filter2D(tmp1, vsrc, CV_32FC1, g0, Point(0, 0));
		filter2D(tmp1, wavelet[0], CV_32FC1, g1, Point(0, 0));
		filter2D(tmp2, wavelet[1], CV_32FC1, g0, Point(0, 0));
		filter2D(tmp2, wavelet[2], CV_32FC1, g1, Point(0, 0));

		Kwavelet[i] = wavelet;


	}
	Mat M = (Mat_<float>(3, 3) << 8.0 / 35.0, (double)4 / 35, (double)2 / 35,
		(double)8 / 35, (double)4 / 35, (double)2 / 35,
		(double)4 / 35, (double)2 / 35, (double)2 / 35);
	Mat dst = Mat::zeros(src.size(), CV_32FC1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			dst = Kwavelet[i][j]*M.at<float>(i,j)+dst;
		}
	}
	
	
	

	return 0;

}