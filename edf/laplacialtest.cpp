#include"stdafx.h"
#include<opencv2\opencv.hpp>;
#include<iostream>
using namespace cv;
int thunder() {
	Mat dst;
	Mat src = imread("lena.jpg", 0);
	
	Laplacian(src, dst, CV_16S, 3);
	convertScaleAbs(dst, dst);
	dst = src - dst;

	imshow("jieguo", dst);
	waitKey();
	return 0;
}