#include"stdafx.h"
#include"jingshenyantuo.h"
#include"wavelet.h"
#include<iostream>
using namespace std;
int flamethrow() {
	Mat src1 = imread("1.jpg", 1);
	Mat src2 = imread("2.jpg", 1);
	Mat src3= imread("10.jpg", 1);
	Mat src4= imread("11.jpg", 1);
	
	jingshenyantuo method ;	
	Mat diff1 = method.SWT(src1);
	Mat diff2 = method.SWT(src2);
	Mat diff3 = method.SWT(src3);
	Mat diff4 = method.SWT(src4);
	Mat diff[] = { diff1,diff2,diff3,diff4 };
	Mat src[] = { src1,src2,src3,src4 };
	Mat dst = method.imgfusion(diff, src, 4,1);
	
 	imshow("jieguo", dst);
	imwrite("Scharr_diffarr_jiaquan.jpg", dst);
	
	



	waitKey();
	return 0;


}