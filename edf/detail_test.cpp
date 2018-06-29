#include"stdafx.h"
#include<opencv2\imgproc.hpp>
#include<opencv2\opencv.hpp>
using namespace cv;
int thunderpunch() {
	Mat src = imread("lena.jpg",1);
	Mat temp = Mat::zeros(src.size(), src.type());
	Mat mask = (Mat_<float>(3, 3) << 1, 2, 1, 2, 4, 2, 1, 2, 1);
	mask /= 16;
	filter2D(src, temp,src.depth(), mask);
	filter2D(temp, temp,src.depth(), mask);
	imshow("detail result", temp);
	waitKey(0);
	return 0;
	

}