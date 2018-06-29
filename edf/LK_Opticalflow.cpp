//using mouseclick to choose feature Point
#include"stdafx.h"
#include<opencv2\features2d.hpp>
#include<opencv2\highgui.hpp>
#include<opencv2\imgproc.hpp>
#include<opencv2\xfeatures2d.hpp>
#include<opencv2\opencv.hpp>
using namespace cv;
int magicleaf() {
	Mat src1, src2;
	src1 = imread("1.jpg", 0);
	src2 = imread("4.jpg", 0);
	Mat temp = imread("4.jpg", 1);
	std::vector<Point2f> corner_1, corner_2;
	goodFeaturesToTrack(src1, corner_1, 100, 0.01, 10, Mat(), 7, 0, 0.04);
	cornerSubPix(src1, corner_1, Size(10, 10), Size(-1, -1), TermCriteria(TermCriteria::MAX_ITER, 20, 0.03));
	std::vector<uchar> featurefound;
	calcOpticalFlowPyrLK(src1, src2, corner_1, corner_2, featurefound, noArray(), Size(10, 10));
	for (int i = 0; i < corner_2.size(); i++) {
		if (featurefound[i])
			line(temp, corner_1[i], corner_2[i], Scalar(0, 0, 255), 2, 8);

	}
	imshow("souce1", src1);
	imshow("shift", temp);
	waitKey();
	return 0;

}