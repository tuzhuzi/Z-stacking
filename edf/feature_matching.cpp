//finding matched feature is to allign the image
#include"stdafx.h"
#include<opencv2\xfeatures2d.hpp>
#include<opencv2\highgui.hpp>
#include<opencv2\features2d.hpp>
#include<iostream>
using namespace cv;
using namespace xfeatures2d;
int ember() {
	Mat src1 = imread("1_gray.jpg", 0);
	Mat src2 = imread("10_gray.jpg", 0);
	Ptr<SURF> detect = SURF::create();
	std::vector<KeyPoint> keyPoint_src1;
	std::vector<KeyPoint> keyPoint_src2;
	detect->setHessianThreshold(400);
	Mat descripteor_src1, descriptor_src2;
	detect->detectAndCompute(src1, Mat(), keyPoint_src1, descripteor_src1);
	detect->detectAndCompute(src2, Mat(), keyPoint_src2, descriptor_src2);
	BFMatcher matcher(NORM_L2);
	std::vector<DMatch> matches;
	matcher.match(descripteor_src1, descriptor_src2, matches);
	Mat dst;
	drawMatches(src1, keyPoint_src1, src2, keyPoint_src2, matches, dst);
	imshow("Dst", dst);
	waitKey();
	return 0;
	
}