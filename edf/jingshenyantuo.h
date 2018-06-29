#pragma once
#include<opencv2\imgcodecs.hpp>
#include<opencv2\imgproc.hpp>
#include<opencv2\highgui.hpp>
#include<opencv2\core.hpp>
using namespace cv;

class jingshenyantuo
{
     int  sigma;
	


public:
	jingshenyantuo();
	jingshenyantuo(int a);
	
	cv::Mat getYchannel(cv::Mat &img);
	cv::Mat getGaussblur(int row,int col);
	cv::Mat freqgauss(cv::Mat &img);
	cv::Mat imgfusion(cv::Mat diff[], cv::Mat src[], int n,int size=0);
	cv::Mat laplacialfilter(cv::Mat &img);
	cv::Mat gaussblur(cv::Mat & img);
	cv::Mat sobleDev(cv::Mat &img, int ksize=5);
	cv::Mat SWT(cv::Mat &img);
	

	~jingshenyantuo();
};

