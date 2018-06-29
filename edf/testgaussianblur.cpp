#include"stdafx.h"
#include<opencv2\highgui.hpp>
#include<opencv2\imgcodecs.hpp>
#include<opencv2\imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;
static Mat complix;
static int D0 = 200;
static Mat gaussianBlur;
static int  m, n;
static void gaussian(int,void*) {
	gaussianBlur = Mat(m, n, CV_32FC2);
	for (int i = 0; i < m; i++) {
		float *p = gaussianBlur.ptr<float>(i);
		for (int j = 0; j < n; j++) {
			float d = pow(i - m / 2, 2) + pow(j - n / 2, 2);
			p[2 * j] = expf(-d / D0);
			p[2 * j + 1] = expf(-d / D0);
		}
	}
	multiply(complix, gaussianBlur, complix);
}
int gauss() {
	Mat src = imread("lena.jpg", IMREAD_GRAYSCALE);
	if (src.empty())
		cout << "Error reading" << endl;
	m = getOptimalDFTSize(src.rows);
	n = getOptimalDFTSize(src.cols);
	Mat padded;
	copyMakeBorder(src, padded, 0, m - src.rows, 0,n-src.cols, BORDER_CONSTANT, Scalar::all(0));
	padded.convertTo(padded, CV_32FC1);
	for (int i = 0; i<padded.rows; i++)//中心化操作，其余操作和上一篇博客的介绍一样  
	{
		float *ptr = padded.ptr<float>(i);
		for (int j = 0; j<padded.cols; j++)
			ptr[j] *= pow(-1, i + j);
	}
	
	Mat plane[] = { padded,Mat::zeros(m,n,padded.type()) };

	merge(plane, 2, complix);
	dft(complix, complix);
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", src);
	namedWindow("guass demo", WINDOW_AUTOSIZE);
	createTrackbar("sigma", "guass demo", &D0, m*n / 2, gaussian);
	gaussian(0, 0);
	idft(complix, complix);
	split(complix, plane);
	magnitude(plane[0], plane[1], plane[0]);
	normalize(plane[0], plane[0], 0, 1, CV_MINMAX);
	imshow("guass demo", plane[0]);
	waitKey();
	return 0;
	
	
}