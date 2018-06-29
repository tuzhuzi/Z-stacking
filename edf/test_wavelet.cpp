#include"stdafx.h"
#include"wavelet.h"
using namespace std;
int hurricane() {
	wavelet Wavelet(1);
	Mat src1 = imread("1.jpg", 0);
	Mat src2 = imread("2.jpg", 0);
	Mat src3 = imread("10.jpg", 0);
	Mat src4 = imread("11.jpg", 0);
	vector<Mat> input = { src1,src2,src3,src4 };
	Mat dst = Wavelet.wavelet_refusion(input, 4);
	equalizeHist(dst, dst);
	imshow("result", dst);
	imwrite("wavelet_energy.jpg", dst);
	waitKey();
	return 0;
}