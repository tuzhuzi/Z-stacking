#include"stdafx.h"
#include"jingshenyantuo.h"
#include"wavelet.h"
#include<iostream>
using namespace cv;
static jingshenyantuo method = jingshenyantuo();
static wavelet DWT = wavelet(3);
int aeroblast() {
	Mat src1 = imread("1.jpg", 1);
	
	Mat src2 = imread("2.jpg", 1);
	
	Mat src3 = imread("10.jpg", 1);
	Mat src4 = imread("11.jpg", 1);
	std::vector<Mat_<uchar>> RGB1;
	split(src1, RGB1);
	std::vector<Mat_<uchar>> RGB2;
	split(src2, RGB2);
	std::vector<Mat_<uchar>> RGB3;
	split(src3, RGB3);
	std::vector<Mat_<uchar>> RGB4;
	split(src4, RGB4);
	std::vector<Mat_<uchar>> RGNimg[4] = { RGB1,RGB2,RGB3,RGB4 };
	Mat_<Vec3b> src[4] = { src1,src2,src3,src4 };
	Mat dst = Mat(src1.size(), src1.type());
	Mat_<uchar> maxdiff[3];
	Mat_<uchar> count[3];
	for (int channel = 0; channel < 3; channel++) {
		maxdiff[channel] = Mat::zeros(src1.size(), CV_8UC1);
		count[channel] = Mat::zeros(src1.size(), CV_8UC1);
		Mat_<uchar> detail[4];
		for (int i = 0; i < 4; i++) {
			Mat waveletimg= DWT.swavelett(RGNimg[i][channel]);
			Mat Vreigin = Mat(waveletimg, Rect(0, 0, waveletimg.cols / 8, waveletimg.rows / 8));
			Mat diff = method.SWT(Vreigin);
			diff.copyTo(Vreigin);
			detail[i] = DWT.deswt(waveletimg);
		}
 		for (int x = 0; x < detail[0].rows; x++) {
			for (int y = 0; y < detail[0].cols; y++) {
				uchar num = 0;
				uchar maxP = 0;
				for (int t = 0; t < 4; t++) {
					if (maxP < detail[t].at<uchar>(x, y)) {
						maxP = detail[t].at<uchar>(x, y);
						num = t;
					}
					
				}
				maxdiff[channel].at<uchar>(x, y) = maxP;
				count[channel].at<uchar>(x, y) = num;
			}
		}
	}
	for (int x = 0; x < count[0].rows; x++) {
		for (int y = 0; y < count[0].cols; y++) {
			uchar num = 0;
			uchar maxP = 0;
			for (int t = 0; t < 3; t++) {
				if (maxP < maxdiff[t].at<uchar>(x, y)) {
					maxP = maxdiff[t].at<uchar>(x, y);
					num = t;
				}
			}
			dst.at<Vec3b>(x, y)[0] = src[num].at<Vec3b>(x, y)[0];
			dst.at<Vec3b>(x, y)[1] = src[num].at<Vec3b>(x, y)[1];
			dst.at<Vec3b>(x, y)[2] = src[num].at<Vec3b>(x, y)[2];
		}
	}
	imwrite("multi_wavalet_and_origiPixel.jpg", dst);
	return 0;


}