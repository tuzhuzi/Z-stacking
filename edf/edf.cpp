// edf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<opencv2\imgcodecs.hpp>
#include<opencv2\imgproc.hpp>
#include<opencv2\highgui.hpp>

int kyrouk()
{
	cv::Mat src = cv::imread("Fig0604(a).tif", 1);
	cv::namedWindow("origin",CV_WINDOW_AUTOSIZE);
	cv::imshow("origin", src);
    return 0;
}

