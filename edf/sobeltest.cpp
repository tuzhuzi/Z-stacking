#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<iostream>
using namespace cv;

int outrage() {
	Mat src = imread("2.jpg", IMREAD_GRAYSCALE);
	int ksize = 3;
	Mat dst_x = src.clone();
	Mat dst_y(src.size(), CV_16S);
	namedWindow("lena", WINDOW_AUTOSIZE);
	for (;;) {
		std::cout << "now ksize=" << ksize << std::endl;
		imshow("lena", dst_x);
		char c = (char)waitKey(0);
		if (c == 27)
			break;
		else if (c =='b')
			ksize += 2;
		else
			ksize = 3;
		Sobel(src, dst_x, CV_16S, 1, 0, ksize);
		Sobel(src, dst_y, CV_16S, 0, 1, ksize);
		addWeighted(dst_x, 0.5, dst_y,0.5,1.0,dst_x);
		convertScaleAbs(dst_x, dst_x);
		
	}
	return 0;

	
}