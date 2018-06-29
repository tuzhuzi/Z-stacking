#include "stdafx.h"
#include "jingshenyantuo.h"


jingshenyantuo::jingshenyantuo() 
{
	sigma = 0;


}

jingshenyantuo::jingshenyantuo(int a) {
	sigma = a;

}

cv::Mat jingshenyantuo::getYchannel(cv::Mat & img)
{
	cv::Mat Ychannel;
	cvtColor(img, Ychannel, COLOR_BGR2YCrCb);
	std::vector<cv::Mat> channels;
	split(Ychannel, channels);
	Ychannel = channels[0];
	return Ychannel;

	
}

cv::Mat jingshenyantuo::getGaussblur(int row,int col)
{
	cv::Mat gaussBlur(row, col, CV_32FC2);
		for (int i = 0; i < row; i++) {
			float *pr= gaussBlur.ptr<float>(i);
			for (int j = 0; j < col; j++) {
				float d = pow(i - row / 2, 2) + pow(j - col / 2, 2);
				pr[2*j] = expf(-d / sigma);
				pr[2*j + 1] = expf(-d / sigma);
			}

		}

	return gaussBlur;
}

cv::Mat jingshenyantuo::freqgauss(cv::Mat & img)
{
	Mat ychanel = img.clone();
	cvtColor(ychanel, ychanel, COLOR_BGR2GRAY);
	
	int m = getOptimalDFTSize(ychanel.rows);
	int n = getOptimalDFTSize(ychanel.cols);
	Mat padded;
	copyMakeBorder(ychanel, padded, m - ychanel.rows, 0, n - ychanel.cols, 0, BORDER_CONSTANT, Scalar::all(0));//扩充矩阵
	Mat freqGauss = getGaussblur(m, n);
	padded.convertTo(padded, CV_32F);
	for (int i = 0; i<padded.rows; i++)//中心化操作
	{
		float *ptr = padded.ptr<float>(i);
		for (int j = 0; j<padded.cols; j++)
			ptr[j] *= pow(-1, i + j);
	}
	//准备傅里叶变换
	Mat plane[] = { padded,Mat::zeros(padded.size(),CV_32F) };
	Mat complix;
	merge(plane, 2, complix);
	dft(complix, complix);
	//高斯频域滤波
	multiply(complix, freqGauss, complix);
	//反变换
	idft(complix, complix);
	split(complix, plane);
	magnitude(plane[0], plane[1], plane[0]);
	normalize(plane[0], plane[0], 1, 0,CV_MINMAX);
	
	//转换成8位图，并裁去扩充的部分
	plane[0].convertTo(plane[0],CV_8U,255,0);
	Mat bulred(plane[0], Rect(n-ychanel.cols,m-ychanel.rows,ychanel.cols,ychanel.rows));
	//得到差异矩阵
	absdiff(bulred, ychanel, plane[0]);
	
	

	return plane[0];

	
}

cv::Mat jingshenyantuo::imgfusion(cv::Mat diff[], cv::Mat src[], int n,int size)
{
	CV_Assert(diff[0].type() == CV_8UC1 && src[0].type() == CV_8UC3);
	
	Mat dst(src[0].size(), src[0].type());
	if (size==0) {
		for (int i = 0; i < src[0].rows; i++) {
			for (int j = 0; j < src[0].cols; j++) {
				double quanzhong[4];
				int total = diff[0].at<uchar>(i, j) + diff[1].at<uchar>(i, j) + diff[2].at<uchar>(i, j) + diff[3].at<uchar>(i, j);
				if (total == 0) {
					quanzhong[0] = quanzhong[1] = quanzhong[2] = quanzhong[3] = 0.25;
				}
				else {
					for (int x = 0; x < 4; x++) {
						quanzhong[x] = (double)diff[x].at<uchar>(i, j) / total;
					}

				}
				dst.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(src[0].at<Vec3b>(i, j)[0] * quanzhong[0] + src[1].at<Vec3b>(i, j)[0] * quanzhong[1] +
					src[2].at<Vec3b>(i, j)[0] * quanzhong[2] + src[3].at<Vec3b>(i, j)[0] * quanzhong[3]);
				dst.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(src[0].at<Vec3b>(i, j)[1] * quanzhong[0] + src[1].at<Vec3b>(i, j)[1] * quanzhong[1] +
					src[2].at<Vec3b>(i, j)[1] * quanzhong[2] + src[3].at<Vec3b>(i, j)[1] * quanzhong[3]);
				dst.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(src[0].at<Vec3b>(i, j)[2] * quanzhong[0] + src[1].at<Vec3b>(i, j)[2] * quanzhong[1] +
					src[2].at<Vec3b>(i, j)[2] * quanzhong[2] + src[3].at<Vec3b>(i, j)[2] * quanzhong[3]);
			}
		}
	}
	//下面的是最值策略，删除//解除封印
	else if (size== 1) {
		for (int i = 0; i < diff[0].rows; i++) {
			for (int j = 0; j < diff[0].cols; j++) {
				int count = 0;
				uchar Maxpixel = diff[0].at<uchar>(i, j);
				for (int x = 0; x < n; x++) {
					if (Maxpixel < diff[x].at<uchar>(i, j)) {
						Maxpixel = diff[x].at<uchar>(i, j);
						count = x;
					}
				}
				dst.at<Vec3b>(i, j)[0] = src[count].at<Vec3b>(i, j)[0];
				dst.at<Vec3b>(i, j)[1] = src[count].at<Vec3b>(i, j)[1];
				dst.at<Vec3b>(i, j)[2] = src[count].at<Vec3b>(i, j)[2];
			}
		}
	}
	
	return dst;
}

cv::Mat jingshenyantuo::laplacialfilter(cv::Mat &img)
{
	Mat img_gray;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	Mat dst;
	Mat laplace = (Mat_<int>(3, 3) << 1, 1, 1,
		1, -8, 1,
		1, 1, 1);//比较苛刻的那种拉普拉斯算子，要求不高可以用[0,1,0;1,-4,1;0,1,0]
	filter2D(img_gray, dst, CV_16S, laplace);
	convertScaleAbs(dst, dst);
	return dst;
}

cv::Mat jingshenyantuo::gaussblur(cv::Mat & img)
{
	Mat dst;
	Mat img_gray;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	GaussianBlur(img_gray, dst, Size(251, 251), 0, 0);//Size没啥道理，窝自己试的，越大效果越好，但计算量越大
	Mat diff;
	absdiff(img_gray, dst, diff);
 	return diff;
}

cv::Mat jingshenyantuo::sobleDev(cv::Mat & img, int ksize)
{
	Mat dst_gray;
	cvtColor(img, dst_gray, COLOR_BGR2GRAY);
	Mat dst_x(dst_gray.size(), CV_16S);
	Mat dst_y(dst_gray.size(), CV_16S);
	Scharr(dst_gray, dst_x, CV_16S, 1, 0);
	Scharr(dst_gray, dst_y, CV_16S, 0, 1);
	addWeighted(dst_x,0.5,dst_y,0.5,1.0,dst_x);
	convertScaleAbs(dst_x, dst_x);
	return dst_x;

}

cv::Mat jingshenyantuo::SWT(cv::Mat & img)
{
	
	Mat h0 = (Mat_<float>(1, 2) << 0.5, 0.5);
	Mat h1 = (Mat_<float>(1, 2) << 0.5, -0.5);
	Mat g0 = (Mat_<float>(2, 1) << 0.5, 0.5);
	Mat g1 = (Mat_<float>(2, 1) << 0.5, -0.5);
	Mat vsrc(img.size(), CV_32FC1);
	img.convertTo(vsrc, CV_32FC1);
	int depth = 3;
	std::vector<Mat_<float>> wavelet(3);
	std::vector<std::vector<Mat_<float>>> Kwavelet(depth);
	for (int i = 0; i < depth; i++) {
		Mat tmp1, tmp2;
		filter2D(vsrc, tmp1, CV_32FC1, h0, Point(0, 0));
		filter2D(vsrc, tmp2, CV_32FC1, h1, Point(0, 0));
		filter2D(tmp1, vsrc, CV_32FC1, g0, Point(0, 0));
		filter2D(tmp1, wavelet[0], CV_32FC1, g1, Point(0, 0));
		filter2D(tmp2, wavelet[1], CV_32FC1, g0, Point(0, 0));
		filter2D(tmp2, wavelet[2], CV_32FC1, g1, Point(0, 0));

		Kwavelet[i] = wavelet;


	}
	Mat M = (Mat_<float>(1, 3) << 3.0 / 24.0, (double)3 / 24, (double)2 / 24);
	Mat dst = Mat::zeros(img.size(), CV_32FC1);
	for (int i = 0; i < depth; i++) {
		for (int j = 0; j < 3; j++) {
			dst = Kwavelet[i][j] * M.at<float>(0, j) + dst;
		}
	}
	dst = abs(dst);
	
	
	
	return dst; 

}



jingshenyantuo::~jingshenyantuo()
{
	
}

	

