#pragma once
#include<opencv2\opencv.hpp>
#include<iostream>
using namespace cv;
#define MIN_HALF_SIZE 32;
#define MAX_SIZE 65525;

//Rect rectTemp[32];//use for note the public range on template
//Rect rectCurrent[32];//note public range for Mat_in

class jsyt{
private:
	int method;
public:

	jsyt() :method(0) {};
	jsyt(int n) { method = n; };
	bool split(Mat &in, Mat &out);
	struct move_para {
		int xoff;
		int yoff;
		float scale;
	};
	struct tss_para{
		Mat src1;
		Mat src2;
		int xoffset,yoffset;
		int xstep, ystep;
		_int64 result;
		float scale;
		bool iscolor;
	};
	__int64 tss(tss_para &pp);
	__int64 tss(Mat &src1, Mat &src2, int xoff, int yoff, float scale);
	void scaleAdjust(Mat &input, Size &target);
	move_para findMovePara(Mat &src1,Mat &src2,Rect &mat_in);
	void allignment(std::vector<Mat> &matIn);
	void laplace_method(std::vector<Mat>&matIn,Mat &dst);
	void self_assert(Mat &in, int size);
	int findmax(Mat &kernel, int size);
	void highpass(Mat &in,int Threshold=0);
	double findthreshold(Mat &in);
	int getopticsize(int w);
	void island(Mat &in);
	Mat fuseWithLut(std::vector<Mat>&in,Mat &lut,std::vector<Mat>&lap);
	void removeStep(Mat &lut, std::vector<Mat> &matIn);
	Mat getlut(std::vector<Mat>&lap);
	Mat lapPyramin(std::vector<Mat>splitMat, int level);
	int split(Mat &in, std::vector<Mat> &split);
};

