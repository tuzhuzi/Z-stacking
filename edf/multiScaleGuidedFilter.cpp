//杨航, 吴笑天, 贺柏根,等. 基于多尺度导引滤波的图像融合方法[J]. 光电子·激光, 2015(1):170-176.
#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<opencv2\ximgproc.hpp>
#include<opencv2\opencv_modules.hpp>
#include<iostream>
#define LEVEL 3;
using namespace cv;
using namespace std;
vector<Mat> findGuideFilter(vector<Mat> in, int size) {
	vector<Mat> inT(size);
	for (int i = 0; i < size; i++) {
		boxFilter(in[i], inT[i], CV_32FC1, Size(3,3),Point(-1,-1));
		absdiff(inT[i], Mat::zeros(inT[i].size(), inT[i].type()), inT[i]);
	}
	int w = in[0].cols;
	int h = in[0].rows;
	uchar num;
	double max;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			max = inT[0].at<float>(i, j);
			if (max > 0) {
				inT[0].at<float>(i, j) = 1.0f;
			}
			else
				inT[0].at<float>(i, j) = 0;
			num = 0;
			for (int n = 1; n < size; n++) {
				if (inT[n].at<float>(i, j) > max) {
				
						inT[num].at<float>(i, j) = 0;
						max = inT[n].at<float>(i, j);
						inT[n].at<float>(i, j) = 1.0;
						num = n;
				}
				else
					inT[n].at<float>(i, j) = 0;
			}

		}
	}
	return inT;
}
Mat guidefilter(Mat &Filter, Mat &in) {
	int r = 15;
	double eps = 0.01;
	Mat I;
	Filter.convertTo(I, CV_32FC1, 1.0f / 255.0f);
	Mat P;
	in.convertTo(P, CV_32FC1, 1.0f / 255.0f);
	Mat mean_I;
	boxFilter(I, mean_I, CV_64FC1, Size(r, r));
	Mat mean_P;
	boxFilter(P, mean_P, CV_64FC1,Size(r,r));
	Mat mean_IP;
	boxFilter(I.mul(P), mean_IP, CV_64FC1, Size(r, r));
	Mat cov;
	cov = mean_IP - mean_I.mul(mean_P);
	Mat mean_II;
	boxFilter(I.mul(I), mean_II, CV_64FC1, Size(r, r));
	Mat var_I;
	var_I = mean_II - mean_I.mul(mean_I);
	Mat a;
	boxFilter(cov / (var_I + eps), a, CV_64FC1,Size(r,r));
	Mat b;
	b = mean_P - a.mul(mean_I);
	boxFilter(b, b, CV_64FC1, Size(r, r));
	Mat q;
	q = P.mul(a) + b;
	return q;


}
void decompose(vector<Mat> &in, int level = 3) {
	Mat temp;
	for (int i = 0; i < level; i++) {
		pyrDown(in[i], in[i + 1]);
		pyrUp(in[i + 1], temp);
		in[i] -= temp;
	}
}
int fusionFlame() {
	int level = LEVEL;
	int r = 15;
	double eps = 0.0001;
	vector<Mat> matIn(4);
	int size = 4;
	for (int i = 0; i < size; i++) {
		string Matname = "E:\\works\\edf\\experiment\\pyrmid_laplace\\align" + to_string(i) + ".jpg";
		matIn[i] = imread(Matname, 0);
	}
	vector<Mat> in1(level + 1), in2(level + 1), in3(level + 1), in4(level + 1);
	matIn[0].convertTo(in1[0], CV_32F, 1.0f / 255.0f);
	decompose(in1, level);
	Mat in1lap;
	Laplacian(in1[level], in1lap, CV_32FC1, 1);
	matIn[1].convertTo(in2[0], CV_32F, 1.0f / 255.0f);
	decompose(in2, level);
	Mat in2lap;
	Laplacian(in2[level], in2lap, CV_32FC1, 1);
	matIn[2].convertTo(in3[0], CV_32F, 1.0f / 255.0f);
	decompose(in3, level);
	Mat in3lap;
	Laplacian(in3[level], in3lap, CV_32FC1, 1);
	matIn[3].convertTo(in4[0], CV_32F, 1.0f / 255.0f);
	decompose(in4, level);
	Mat in4lap;
	Laplacian(in4[level], in4lap, CV_32FC1, 1);
	vector<Mat> current_Mat{in1lap,in2lap,in3lap,in4lap};
	vector<Mat> guideFilter(4);
	guideFilter = findGuideFilter(current_Mat, size);
	vector<Mat> dst(level + 1);
	dst[level] = Mat::zeros(in1[2].size(), CV_32FC1);
	vector<Mat> W(4);
	cv::ximgproc::guidedFilter(guideFilter[0], in1[level], W[0], r, eps);
	cv::ximgproc::guidedFilter(guideFilter[1], in2[level], W[1], r, eps);
	cv::ximgproc::guidedFilter(guideFilter[2], in3[level], W[2], r, eps);
	cv::ximgproc::guidedFilter(guideFilter[3], in4[level], W[3], r, eps);
	Mat div = W[0] + W[1] + W[2] + W[3];
	dst[level] = W[0].mul(in1[level]) + W[1].mul(in2[level]) + W[2].mul(in3[level]) + W[3].mul(in4[level]);
	cv::divide(dst[level], div, dst[level], 1, -1);
	for (int i = level-1; i>=0; i--) {
		dst[i] = Mat::zeros(in1[i].size(), CV_32FC1);
		current_Mat = { in1[i],in2[i],in3[i],in4[i] };
		guideFilter = findGuideFilter(current_Mat, size);
		for (int n = 0; n < size; n++) {
			cv::ximgproc::guidedFilter(guideFilter[n], current_Mat[n], W[n], r, eps);
			dst[i] += W[n].mul(current_Mat[n]);
		}
		div = W[0] + W[1] + W[2] + W[3];
		divide(dst[i], div, dst[i]);
	}
	for (int i = level; i > 0; i--) {
		pyrUp(dst[level], dst[level]);
		dst[level] += dst[i - 1];
	}
	imshow("dst", dst[level]);
	waitKey(0);
	imwrite("multi_guide_filter.jpg", dst[level]);
	return 0;
	
}

