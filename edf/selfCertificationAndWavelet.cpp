#include"stdafx.h"
#include"wavelet.h"
#include<iostream>
using namespace cv;
static wavelet DWT(3);
static void selfCert(Mat &plate, int nos) {//将中心值返回为周围数量最多的元素数
	CV_Assert(plate.depth() == CV_8UC1);
	std::vector<uchar> num(nos);
	int most = 0;
	for (int n = 0; n < nos; n++) {
		num[n]=0;
		for (int i = 0; i < plate.rows; i++) {
			for (int j = 0; j < plate.cols; j++) {
				if (plate.at<uchar>(i, j) ==n)
					num[n]++;
			}
		}
		
	}
	for (int n = 0; n < nos; n++) {
		if (num[0] < num[n]) {
			num[0] = num[n];
			most = n;
		}
	}
	plate = most;
}
static void getDiv(Mat &src, Mat &dst, Size Ksize) {//获取图像方差
	Mat avr, avrq;
	blur(src, avr, Ksize);
	avr.convertTo(avr, CV_16U, 1, 0);
	avr = avr.mul(avr);
	src.convertTo(avrq, CV_16U, 1, 0);
	avrq = avrq.mul(avrq);
	blur(avrq, avrq, Ksize);
	absdiff(avr, avrq, dst);
	MatIterator_<int16_t> it = dst.begin<int16_t>(), itend = dst.end<int16_t>();
	for (; it != itend; it++) {
		*it = sqrt(*it );
	}
}
int trickroom() {
	Mat src1 = imread("1.jpg", 1);
	Mat src1gray = imread("1.jpg", 0);
	Mat src2 = imread("2.jpg", 1);
	Mat src2gray = imread("2.jpg", 0);
	Mat src3 = imread("10.jpg", 1);
	Mat src3gray = imread("10.jpg", 0);
	Mat src4 = imread("11.jpg", 1);
	Mat src4gray = imread("11.jpg", 0);
	Mat dst=Mat(src1.size(),src1.type());
	Mat count=Mat::zeros(src1.size(),CV_8UC1);
	Mat src[4] = { src1,src2,src3,src4 };
	Mat srcgray[4] = { src1gray,src2gray,src3gray,src4gray };
	Mat waveletimg[4];//小波变换后的图
	Mat vreign[4];//小波变换后的低频部分
	Mat diff[4];//方差矩阵
	for (int i = 0; i < 4; i++) {
		waveletimg[i] = DWT.swavelett(srcgray[i]);
		vreign[i] = Mat(waveletimg[i], Rect(0, 0, src1.cols/8, src1.rows/8));
		getDiv(vreign[i], diff[i], Size(3, 3));
	}
	for (int i = 0; i< src1.rows; i++) {
		for (int j = 0; j < src1.cols; j++) {
			double Maxp = 0;
			uchar sum = 0;
			for (int n = 0; n < 4; n++) {
				if (Maxp < pow(waveletimg[n].at<float>(i, j), 2)) {
					Maxp = pow(waveletimg[n].at<float>(i, j), 2);
					sum = n;
				}
			}
			count.at<uchar>(i, j) = sum;
		}
	}
	for (int i = 0; i < diff[0].rows; i++) {//以局部方差为依据获得查找矩阵部分的值
		for (int j = 0; j < diff[0].cols; j++) {
			int16_t Maxp = 0;
			uchar sum = 0;
			for (int n = 0; n < 4; n++) {
				if (Maxp < diff[n].at<int16_t>(i, j)) {
					Maxp = diff[n].at<int16_t>(i, j);
					sum = n;
				}
			}
			count.at<uchar>(i, j) = sum;
		}
	}
	//自我纠正参照矩阵
	Mat plate = count.clone();
	Mat plater=Mat::zeros(Size(3, 3), CV_8U);
	Mat countr=Mat::zeros(Size(3, 3), CV_8U);
	for (int i = 0; i < count.rows/3; i++) {
		for (int j = 0; j < count.cols/3; j++) {
			countr = Mat(count, Rect(3*j ,3*i , 3, 3));
			plater = Mat(plate, Rect(3*j , 3*i , 3, 3));
			plater.copyTo(countr);
			selfCert(countr,4);
			
		}
	}
//以参照矩阵为依据重新融合图像
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
	for (int channel = 0; channel < 3; channel++) {
		for (int n = 0; n < 4; n++) {
			waveletimg[n] = DWT.swavelett(RGBimg[n][channel]);
		}
		for (int i = 0; i < count.rows; i++) {
			for (int j = 0; j < count.cols; j++) {
				waveletimg[0].at <float>(i, j) = waveletimg[count.at<uchar>(i, j)].at<float>(i, j);
			}
		}
		result[channel] = DWT.deswt(waveletimg[0]);
	}
	merge(result, 3, dst);
	imwrite("divandselfcet.jpg", dst);
	return 0;   
}
