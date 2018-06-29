#include"stdafx.h"
#include"wavelet.h"
#include"jingshenyantuo.h"

#include<iostream>
using namespace cv;
static wavelet DWT = wavelet(3);
static jingshenyantuo method = jingshenyantuo();
static void selfcert(Mat &tmp, int nos) {//改善图像粉碎化问题，将结果矩阵分块化,返回图像中数量最多的像素值
	
	
	for (int n = 0; n < nos; n++) {
		int num = 0;
		
		for (int i = 0; i < tmp.rows; i++) {
			for (int j = 0; j < tmp.cols; j++) {
				if (tmp.at<uchar>(i, j) == n)
					num++;
			}
		}
		if (num >= 5)
			tmp.at<uchar>((tmp.rows - 1) / 2, (tmp.cols - 1) / 2) = n;
	}
	
}



int secretpower() {
	Mat src1 = imread("1.jpg", 1);
	cvtColor(src1, src1, CV_RGB2HSV);
	Mat src2= imread("2.jpg", 1);
	cvtColor(src2, src2, CV_RGB2HSV);
	Mat src3 = imread("10.jpg", 1);
	cvtColor(src3, src3, CV_RGB2HSV);
	Mat src4 = imread("11.jpg", 1);
	cvtColor(src4, src4, CV_RGB2HSV);
	std::vector<Mat_<uchar>> RGB1(3);
	split(src1, RGB1);
	std::vector<Mat_<uchar>> RGB2(3);
	split(src2, RGB2);
	std::vector<Mat_<uchar>> RGB3(3);
	split(src3, RGB3);
	std::vector<Mat_<uchar>> RGB4(3);
	split(src4, RGB4);
	Mat_<uchar> result[3];
	std::vector<Mat_<uchar>> RGBimg[4] = { RGB1,RGB2,RGB3,RGB4 };

	Mat_<uchar> sourcecount = Mat(src1.size(), CV_8UC1);
	
	
	//get Ychannel wavelet and also note pixel souce
	Mat_<float> waveleteImg[4];
	Mat_<float> Vreign[4];
	Mat_<float> diff[4];
	for (int i = 0; i < 4; i++) {
		waveleteImg[i] = DWT.swavelett(RGBimg[i][0]);
		Vreign[i]= Mat(waveleteImg[i], Rect(0, 0, waveleteImg[i].cols / 8, waveleteImg[i].rows / 8));
		diff[i] = method.SWT(Vreign[i]);
	}
	for (int i = 0; i < src1.rows; i++) {
		for (int j = 0; j < src1.cols; j++) {
			int16_t diffP = 0;
			uchar num = 0;
			for (int t = 0; t < 4; t++) {
				if (diffP < pow(waveleteImg[t].at<float>(i,j), 2)) {
					diffP = pow(waveleteImg[t].at<float>(i, j), 2);
					num = t;
				}
			}
			sourcecount.at<uchar>(i, j) = num;
			
		}
	}
	Mat vcount = Mat(sourcecount, Rect(0, 0, Vreign[0].cols, Vreign[0].rows));
	for (int i = 0; i < Vreign[0].rows; i++) {
		for (int j = 0; j < Vreign[0].cols; j++) {
			float diffP = 0;
			uchar num = 0;
			for (int t = 0; t < 4; t++) {
				if (diffP < diff[t].at<float>(i, j)) {
					diffP = diff[t].at<float>(i, j);
					num = t;
				}
			}
			vcount.at<uchar>(i, j) = num;

		}
	}
		Mat plate = sourcecount.clone();
		Mat plateV = Mat::zeros(Size(3, 3), CV_8U);
		Mat vcontV = Mat::zeros(Size(3, 3), CV_8U);
		for (int i = 1; i < vcount.rows-1; i++) {
			for (int j = 1; j < vcount.cols-1; j++) {
				plateV = Mat(plate, Rect(j-1, i-1 , 3, 3));
				vcontV = Mat(sourcecount, Rect(j-1, i-1, 3, 3));
				plateV.copyTo(vcontV);
				selfcert(vcontV, 4);
			}
		}
		result[0] = DWT.deswt(waveleteImg[0]);
		//base source table refuse the wavelet,and deDWT
		for (int channel = 0; channel < 3; channel++) {
			for (int n = 0; n < 4; n++) {
				waveleteImg[n] = DWT.swavelett(RGBimg[n][channel]);
			}
			for (int i = 0; i < sourcecount.rows; i++) {
				for (int j = 0; j < sourcecount.cols; j++) {
					waveleteImg[0].at <float>(i, j) = waveleteImg[sourcecount.at<uchar>(i, j)].at<float>(i, j);
				}
			}
			result[channel] = DWT.deswt(waveleteImg[0]);
		}

		Mat dst;
		cv::merge(result, 3, dst);
		cvtColor(dst, dst, CV_HSV2RGB);
		imwrite("Mulity_wavelete2(HSV,selfcert).jpg", dst);




		return 0;
	}


