#include"stdafx.h"
#include"jingshenyantuo2.h"


using namespace std;
int main() {
	jsyt JSYT;
	int size=4;
	vector<Mat> matIn(size);
	vector<Mat> lap(size);
	vector<Mat> splitMat(32);
	int level;
	for (int i = 0; i < size; i++) {
		string Matname = "E:\\works\\edf\\experiment\\pyrmid_laplace\\align" + to_string(i) + ".jpg";
		matIn[i] = imread(Matname, 1);
		matIn[i].convertTo(matIn[i], CV_32F, 1.0f / 255.0f);
		level = JSYT.split(matIn[i], splitMat);
		lap[i] = JSYT.lapPyramin(splitMat, level);

	}
	Mat lut = JSYT.getlut(lap);
	Mat dst = JSYT.fuseWithLut(matIn, lut, lap);
	imshow("dst", dst);
	dst.convertTo(dst, CV_8U, 255);
	imwrite("resy,with_lap_energy.bmp", dst);


	
	
}