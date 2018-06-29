#pragma once
#include<opencv2\opencv.hpp>
#include<cstring>
#include<iostream>

using namespace cv;
class wavelet
{
private:
	enum {min_half_size=128};
	int depth;
	bool iscolor;
	

	
public:
	wavelet():depth(0),iscolor(1) {};
	wavelet(bool b) : depth(0), iscolor(b){};
	int getdepth()const { return depth; };
	cv::Mat swavelett(Mat &src);
	cv::Mat deswt(Mat & swtimg);
	cv::Mat wavelet_refusion(Mat src1,Mat src2);
	void selfcert(Mat&tmp, int nos=2);
	cv::Mat refuse(Mat&src1, Mat &src2, Mat &lut, Mat dst);
	cv::Mat wavelet_refusion(std::vector<Mat>& input, int size);
	~wavelet();
};

