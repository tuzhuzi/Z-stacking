#include"stdafx.h"
#include"jingshenyantuo2.h"


using namespace std;
int main() {
	jsyt JSYT;
	int size=4;
	vector<Mat> matIn(size);
	vector<Mat> lap(size);
	for (int i = 0; i < size; i++) {
		string Matname = "E:\\works\\edf\\experiment\\pyrmid_laplace\\align" + to_string(i) + ".jpg";
		matIn[i] = imread(Matname, 1);
		matIn[i].convertTo(matIn[i], CV_32FC1, 1.0f / 255.0f);
		lap[i] = JSYT.lapPyramin(matIn[i], 7);
	}
	Mat lut = JSYT.getlut(lap);

}
