#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<iostream>
using namespace cv;
int thundershock() {
	Mat src = imread("lena.jpg", 0);
	Mat dst;
	int Width = src.cols;
	int Height= src.rows;
	//小波分解次数
	int depth = 1;//
	int depthcount = 1;
	//改变数据格式防止溢出
	Mat tmp = Mat::zeros(src.size(), CV_32FC1);
	Mat  wavelet = Mat::zeros(src.size(), CV_32FC1);
	Mat  imgtmp = src.clone();
	imgtmp.convertTo(imgtmp, CV_32FC1);
	//执行小波变换
	while (depthcount <= depth) {
		Width = src.cols / pow(2,depthcount-1);
		Height = src.rows/pow(2,depthcount-1);
		for (int i = 0; i < Height; i++) {
			for (int j = 0; j < Width / 2; j++) {
				tmp.at<float>(i, j) = (imgtmp.at<float>(i, 2 * j) + imgtmp.at<float>(i, 2 * j + 1)) / 2;
				tmp.at<float>(i, j + Width / 2) = (imgtmp.at<float>(i, 2 * j) - imgtmp.at<float>(i, 2 * j + 1)) / 2;
			}



		}
		for (int i = 0; i < Height / 2; i++) {
			for (int j = 0; j < Width; j++) {
				wavelet.at<float>(i, j) = (tmp.at<float>(2 * i, j) + tmp.at<float>(2 * i + 1, j)) / 2;
				wavelet.at<float>(i + Height / 2, j) = (tmp.at<float>(2 * i, j) - tmp.at<float>(2 * i + 1, j)) / 2;
			}
		}
		imgtmp = wavelet;
		depthcount++;
	}
	namedWindow("DST", WINDOW_AUTOSIZE);
	convertScaleAbs(wavelet, dst);
	
	
	imshow("DST", dst);
	//反变换
	while (depthcount > 1) {
	
		for (int i = 0; i < Height / 2; i++) {
			for (int j = 0; j < Width; j++) {
				tmp.at<float>(2*i, j) = wavelet.at<float>( i, j) + wavelet.at<float>(i+Height/2,j);
				tmp.at<float>(2*i+1, j) = wavelet.at<float>( i, j) - wavelet.at<float>(i + Height/2, j);
			}
		}
		for (int i = 0; i < Height; i++) {
			for (int j = 0; j < Width / 2; j++) {
				imgtmp.at<float>(i, 2*j) = tmp.at<float>(i,  j) + tmp.at<float>(i,  j + Width/2);
				imgtmp.at<float>(i, 2*j+1) = tmp.at<float>(i,  j) - tmp.at<float>(i,  j + Width/2);
			}
		}
		depthcount--;
		wavelet = imgtmp;
		Height *= 2;
		Width *= 2;

	}
	namedWindow("restruct", WINDOW_AUTOSIZE);
	convertScaleAbs(imgtmp, imgtmp);

	
	imshow("restruct", imgtmp);
	waitKey();
	imwrite("haar.jpg", dst);
	return 0;
}
