#include"stdafx.h"
#include"jingshenyantuo.h"
using namespace cv;
static Mat src, dst;
static int kersize = 3;
static int max_kersize = 5;
void gauss_bblur(int, void*) {
	
	GaussianBlur(src, dst, Size(251, 251), 0, 0);
	Mat diff;
	absdiff(src, dst, diff);
	
	imshow("diff", diff);
	imshow("blured", dst);
	
}
int ceshi() {
	src = imread("10.jpg", 0);
	namedWindow("original", WINDOW_AUTOSIZE);
	namedWindow("blured", WINDOW_AUTOSIZE);
	namedWindow("diff", WINDOW_AUTOSIZE);
	createTrackbar("kernel size", "diff", &kersize, max_kersize, gauss_bblur);
	imshow("original", src);
	gauss_bblur(0, 0);
	waitKey(0);

    
	return 0;


	
}