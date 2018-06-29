#include "stdafx.h"
#include "wavelet.h"
#include"jingshenyantuo.h"





cv::Mat wavelet::swavelett(Mat &src)
{
	depth = 0;
	int w = src.cols;
	int h = src.rows;
	Mat dst;
	if(iscolor){
		dst = Mat::zeros(src.size(), CV_16SC3);
		Mat temp = dst.clone();
		while (w > min_half_size&&h > min_half_size) {
			w /= 2;
			h /= 2;
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < 2*w; j++) {
					temp.at<Vec3s>(i, j) = (src.at<Vec3b>(2 * i, j) + src.at<Vec3b>(2 * i + 1, j)) / 2;
					temp.at<Vec3s>(i+h,j)= (src.at<Vec3b>(2 * i, j)-src.at<Vec3b>(2 * i + 1, j)) / 2;
				}
			}
			for (int i = 0; i < 2 * h; i++) {
				for (int j = 0; j < w; j++) {
					dst.at <Vec3s>(i,j)= (temp.at<Vec3s>(i, 2 * j) + temp.at<Vec3s>(i, 2 * j + 1)) / 2;
					dst.at<Vec3s>(i,j+w)= (temp.at<Vec3s>(i, 2 * j) -temp.at<Vec3s>(i, 2 * j + 1)) / 2;
				}
			}
			depth++;
		}
	}
	else {
		dst = Mat::zeros(src.size(), CV_16SC1);
		Mat temp = dst.clone();
		while (w > min_half_size&&h > min_half_size) {
			w /= 2;
			h /= 2;
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < 2 * w; j++) {
					temp.at<signed>(i, j) = (src.at<uchar>(2 * i, j) + src.at<uchar>(2 * i + 1, j)) / 2;
					temp.at<signed>(i + h, j) = (src.at<uchar>(2 * i, j) - src.at<uchar>(2 * i + 1, j)) / 2;
				}
			}
			for (int i = 0; i < 2 * h; i++) {
				for (int j = 0; j < w; j++) {
					dst.at <signed>(i, j) = (temp.at<signed>(i, 2 * j) + temp.at<signed>(i, 2 * j + 1)) / 2;
					dst.at<signed>(i, j + w) = (temp.at<signed>(i, 2 * j) - temp.at<signed>(i, 2 * j + 1)) / 2;
				}
			}
			depth++;
		}

	}
	std::cout << "src is resolved to wavelet" << std::endl;
	return dst;


	


}

cv::Mat wavelet::deswt(Mat & swtimg)
{
	Mat dst = Mat::zeros(swtimg.size(), CV_32FC1);
	int Width = swtimg.cols / pow(2, depth - 1);
	int Height = swtimg.rows/pow(2,depth-1);
	Mat tmp = Mat::zeros(dst.size(), CV_32FC1);
	Mat imgtmp = swtimg.clone();
	imgtmp.convertTo(imgtmp, CV_32FC1);
	int depthcount = depth;
	while (depthcount > 0) {
		for (int i = 0; i < Height/ 2; i++) {
			for (int j = 0; j < Width; j++) {
				tmp.at<float>(2 * i, j) = (imgtmp.at<float>(i, j) + imgtmp.at<float>(i + Height / 2, j));
				tmp.at<float>(2 * i + 1, j) = (imgtmp.at<float>(i, j) - imgtmp.at<float>(i + Height/ 2, j));
			}
		}
		for (int i = 0; i < Height; i++) {
			for (int j = 0; j < Width / 2; j++) {
				dst.at<float>(i, 2 * j) = (tmp.at<float>(i, j) + tmp.at<float>(i, j + Width / 2));
				dst.at<float>(i, 2 * j + 1) = (tmp.at<float>(i, j) - tmp.at<float>(i, j + Width/ 2));
			}
		}
		Height *= 2;
		Width *= 2;
		depthcount--;
		imgtmp = dst;
	}
	convertScaleAbs(dst, dst);
	dst.convertTo(dst, CV_8UC1);
	return dst;
}

cv::Mat wavelet::wavelet_refusion(Mat src1,Mat src2)
{
	if (src1.depth() == CV_8U)
		src1 = swavelett(src1);
	if (src2.depth() == CV_8U)
		src2 = swavelett(src2);
	Mat DWT;
	Mat HH1, HH2;
	Mat DH1, DH2;
	Mat HD1, HD2;
	Mat DD1, DD2;
	
	DWT = Mat(src1.size(), CV_32FC1, Scalar::all(0));
	HH1 = Mat(src1, Rect(0, 0, src1.cols / 2, src1.rows / 2));
	HD1 = Mat(src1, Rect(src1.cols / 2, 0, src1.cols / 2, src1.rows / 2));
	DH1 = Mat(src1, Rect(0, src1.rows / 2, src1.cols / 2, src1.rows / 2));
	DD1 = Mat(src1, Rect(src1.cols / 2, src1.rows / 2, src1.cols / 2, src1.rows / 2));
	Mat HH(DWT, Rect(0, 0, src1.cols / 2, src1.rows / 2));
	Mat HD(DWT, Rect(src1.cols / 2, 0, src1.cols / 2, src1.rows / 2));
	Mat DH(DWT, Rect(0, src1.rows / 2, src1.cols / 2, src1.rows / 2));
	Mat DD(DWT, Rect(src1.cols / 2, src1.rows / 2, src1.cols / 2, src1.rows / 2));
	HH2 = Mat(src2, Rect(0, 0, src1.cols / 2, src1.rows / 2));
	HD2 = Mat(src2, Rect(src1.cols / 2, 0, src1.cols / 2, src1.rows / 2));
	DH2 = Mat(src2, Rect(0, src1.rows / 2, src1.cols / 2, src1.rows / 2));
	DD2 = Mat(src2, Rect(src1.cols / 2, src1.rows / 2, src1.cols / 2, src1.rows / 2));
	Mat L_wave1[3] = { DH1,HD1,DD1 };
	Mat L_wave2[3] = { DH2,HD2,DD2 };
	Mat Lut_1 = Mat::zeros(DH1.size(), CV_32F);
	Mat lut[3] = { Lut_1,Lut_1.clone(),Lut_1.clone() };
	for (int n = 0; n < 3; n++) {
		for (int i = 0; i < DH1.rows; i++) {
			for (int j = 0; j < DH1.cols; j++)
				lut[n].at<float>(i, j) = abs(L_wave1[n].at<float>(i, j)) > abs(L_wave2[n].at<float>(i, j)) ? 0 : 1;
		}

	}
	lut[0] = lut[0] * 0.4 + lut[1] * 0.4 + lut[2] * 0.2;
	Mat Lut(lut[0].size(), CV_8U);
	for (int i = 0; i < lut[0].rows; i++) {
		for (int j = 0; j < lut[0].cols; j++) {
			Lut.at<uchar>(i, j) = lut[0].at<float>(i, j) > 0.5 ? 1 : 0;
		}
	}
	Mat plate = Lut.clone();
	Mat plateV = Mat::zeros(Size(3, 3), CV_8U);
	Mat vcontV = Mat::zeros(Size(3, 3), CV_8U);
	for (int i = 3; i < Lut.rows - 2; i++) {
		for (int j = 3; j < Lut.cols - 2; j++) {
			plateV = Mat(plate, Rect(j - 2, i - 2, 5, 5));
			vcontV = Mat(Lut, Rect(j - 2, i - 2, 5, 5));
			plateV.copyTo(vcontV);
			selfcert(vcontV, 2);
		}
	}
	refuse(HH1, HH2, Lut, HH);

	refuse(HD1, HD2, Lut, HD);
	refuse(DH1, DH2, Lut, DH);
	refuse(DD1, DD2, Lut, DD);
	return(DWT);
}

void wavelet::selfcert(Mat & tmp, int nos)
{
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

cv::Mat wavelet::refuse(Mat & src1, Mat & src2, Mat & lut, Mat dst)
{
	CV_Assert(src1.size() == src2.size() && src1.size() == lut.size());

	for (int i = 0; i < src1.rows; i++) {
		for (int j = 0; j < src1.cols; j++) {
			dst.at<float>(i, j) = lut.at<uchar>(i, j) > 0 ? src2.at<float>(i, j) : src1.at<float>(i, j);
		}
	}
	return dst;
}

cv::Mat wavelet::wavelet_refusion(std::vector<Mat>& input,int size)
{
	
	std::vector<Mat_<float>> wavelet(size);
	
	std::vector<Mat_<float>> contentregion(size);
	int Width= input[0].cols;
	int Height = input[0].rows;
	Mat fusion = Mat::zeros(Height, Width, CV_32FC1);
	Mat Vfusion(fusion, Rect(0, 0, Width / pow(2, depth), Height / pow(2, depth)));
	std::vector<Mat_<uint16_t>> tmp(size);
	for (int i = 0; i < size; i++) {
		wavelet[i] = swavelett(input[i]);
		
		Mat Vregion(wavelet[i], Rect(0, 0, Width / pow(2, depth), Height / pow(2, depth)));
		Vregion.copyTo(contentregion[i]);
		
		
		tmp[i] = Mat::zeros(wavelet[i].size(), CV_16UC1);
		for (int x = 0; x < Height; x++) {
			for (int y= 0; y< Width; y++) {
				tmp[i].at<uint16_t>(x, y) = pow(wavelet[i].at<float>(x, y), 2);
			}
		}
		medianBlur(tmp[i], tmp[i], 3);
		
	}
	
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			uint16_t maxWavelet = tmp[0].at<uint16_t>(i,j);
			int count = 0;
			for (int x = 0; x < size; x++) {
				if (maxWavelet <tmp[x].at<uint16_t>(i,j)) {
					maxWavelet = tmp[x].at<uint16_t>(i,j);
					count = x;
				}
			}
			fusion.at<float>(i, j) = wavelet[count].at<float>(i, j);
		}
	}
	for (int i = 1; i < size; i++)
		Vfusion += contentregion[i];
	Vfusion /= size;
	Mat dst = deswt(fusion);
	return dst;





}



wavelet::~wavelet()
{
}
