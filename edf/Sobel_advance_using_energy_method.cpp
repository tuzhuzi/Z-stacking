#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<iostream>
using namespace cv;
//�������ʹ���ݶȣ���sqrt(dI/dX^2+dI/dY^2),����ֵ���õ��Ľ��
int punish() {
	Mat src = imread("lena.jpg", 0);
	Mat SobelX, SobelY;
	Mat norm, dir;
	Sobel(src, SobelX, CV_32F, 1, 0, 3);
	Sobel(src, SobelY, CV_32F, 0, 1);
	Mat sobelImg;
	cartToPolar(SobelX, SobelY, norm, dir);
	double sobelmin, sobelmax;
	minMaxLoc(norm, &sobelmin, &sobelmax);
	norm.convertTo(sobelImg, CV_8U, -255. / sobelmax, 255);
	imshow("�ݶ�", sobelImg);
	waitKey();
	return 0;
}