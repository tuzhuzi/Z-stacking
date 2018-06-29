#include"stdafx.h"

#include<opencv2\opencv.hpp>
#include<iostream>
#include<cstring>
using namespace cv;
static struct mapimf {
	uchar light;
	MatIterator_<uchar> it;
};
int getmax(mapimf diff[]) {
	int num = 4;
	int count = 0;
	uchar maxlight = 0;
	for (int i = 0; i < num; i++) {
		if (maxlight < diff[i].light) {
			maxlight = diff[i].light;
			count = i;
		}
		diff[i].it++;
		diff[i].light = *diff[i].it;
	}
	return count;
}
int overheat() {
	Mat src1 = imread("1.jpg", 0);
	Mat src2 = imread("2.jpg", 0);
	Mat src3 = imread("3.jpg", 0);
	Mat src4 = imread("4.jpg", 0);
	Mat dst=Mat::zeros(src1.size(), src1.type());
	mapimf diff[4];
	Mat src[4] = { src1,src2,src3,src4 };
	for (int i = 0; i < 4; i++) {
		diff[i].it = src[i].begin<uchar>();
		diff[i].light = *diff[i].it;
	}
	MatIterator_<uchar> it = dst.begin<uchar>();
	MatIterator_<uchar> itend = dst.end<uchar>();
	for (; it != itend; it++) {
		int count = getmax(diff);
		*it = *diff[count].it;
		std::cout << count << std::endl;
	}
	imshow("dst", dst);
	waitKey();
	return 0;

    
}