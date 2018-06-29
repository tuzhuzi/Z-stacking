#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<stdlib.h>
#include<iostream>
using namespace cv;
int metromesh() {
	Mat temp = Mat::zeros(Size(5, 5), CV_8UC1);
	for (int i = 0; i < temp.rows; i++) {
		for (int j = 0; j < temp.cols; j++) {
			temp.at<uchar>(i, j) = rand()%4;
		}
	}
	Canny(temp, temp, 2, 3);
	return 0;

}