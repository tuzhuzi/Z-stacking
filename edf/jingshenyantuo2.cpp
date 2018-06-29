#include "stdafx.h"
#include "jingshenyantuo2.h"

bool jsyt::split(Mat &in, Mat &out)
{
	int w = in.cols;
	int h = in.rows;
	int min_half_size = MIN_HALF_SIZE;
	if (w <min_half_size|| h<min_half_size) {
		std::cout << "get minsize" << std::endl;
		return 0;
	}
		
	int newh = h / 2;
	int neww = w / 2;
	if (newh % 2)
		newh--;
	if(neww % 2)
		neww--;
	out = Mat::zeros(Size(neww,newh), CV_8U);
	for (int i = 0; i < newh; i++) {
		for (int j = 0; j < neww; j++) {
			out.at<uchar>(i, j) = (in.at<uchar>(2 * i, 2 * j) + in.at<uchar>(2 * i + 1, 2 * j + 1) + in.at<uchar>(2 * i + 1, 2 * j) + in.at<uchar>(2 * i, 2 * j + 1)) / 4;
		}
	}
	return 1;

}

_int64 jsyt::tss(tss_para & pp)
{
	CV_Assert(pp.src1.size() == pp.src2.size()&&pp.src1.type()==pp.src2.type());
	int xoff = pp.xoffset;
	int xstep = pp.xstep;
	
	int yoff = pp.yoffset;
	
	int ystep = pp.ystep;
	float scale = pp.scale;
	_int64 result = pp.result;
	result = 0;
	bool iscolor = pp.iscolor;
	int xx, yy;
	int w = pp.src1.cols;
	int h = pp.src1.rows;
	if (abs(xoff) > w / 8 && abs(yoff) > h > 8 && abs(scale - 1) > 0.1) {
		result = 0;
		return -1;
	}
	//平移放缩图象，计算像素差值
	if (iscolor == 0) {
		for (int x =0; x <w-xstep; x += xstep) {
			for (int y =0; y < h-ystep; y += ystep) {
				xx = (int)((float(x - xoff) - w / 2)*scale + w / 2);
				yy = (int)((float(y - yoff) - h / 2)*scale + h / 2);
				if (xx < 0 || yy < 0)continue;
				if (xx >= w|| yy >= h)continue;

				result += pow((pp.src1.at<uchar>(y, x) - pp.src2.at<uchar>(yy, xx)), 2);
			}
		}
	}
	else
		std::cout << "this method is used for graymap" << std::endl;
	return result;


}

__int64 jsyt::tss(Mat &src1, Mat &src2, int xoff, int yoff, float scale)
{
	CV_Assert(src1.size() == src2.size()&&src1.type() == src2.type());
	int w = src1.cols;
	int h = src2.rows;
	int xstep = w / 1000;
	if (xstep < 2)xstep = 2;
	int ystep = h / 1000;
	if (ystep < 2)ystep = 2;
	tss_para tt;
	tt.iscolor = 0;
	tt.result = 0;
	tt.scale = scale;
	tt.src1 = src1;
	tt.src2 = src2;
	tt.xoffset = xoff;
	tt.yoffset = yoff;
	tt.xstep = xstep;
	tt.ystep = ystep;
	return tss(tt);
	

	
}

void jsyt::scaleAdjust(Mat &input, Size &target)
{
	Size in_size = input.size();
	resize(input, input, target);
}
jsyt::move_para jsyt::findMovePara(Mat &src1, Mat &src2,Rect &mat_in)//这个程序就和名字写的一样
{ 
	move_para mm;
	tss_para tt;
	bool change;
	int xoff = 0;
	int yoff = 0;
	float scale = 1.0f;
	int newxoff = 0;
	int newyoff = 0;
	float newscale = 1.0f;
	int w = src1.cols;
	int h = src1.rows;
	Point pt1, pt2;
	mat_in = Rect(Point(0, 0), Point(src1.cols, src1.rows));

	//split image
	Mat temp;
	Mat *msubframe = new Mat[32];
	Mat *ssubframe = new Mat[32];
	msubframe[0] = src1;
	ssubframe[0] = src2;
	int level = 0;
	int anolevel = 0;
	int min_half_size= MIN_HALF_SIZE;
	while (msubframe[level].size().width > min_half_size&&msubframe[level].size().height > min_half_size) {
		split(msubframe[level], temp);
		level += 1;
		msubframe[level] = temp;
	}
	while (ssubframe[anolevel].size().width > min_half_size&&ssubframe[anolevel].size().height > min_half_size) {
		split(ssubframe[anolevel], temp);
		anolevel += 1;
		ssubframe[anolevel] = temp;
	}
	if (!anolevel == level) {
		std::cout << "Initial size doesn't match" << std::endl;
	}
	//自下而上迭代以获得操作参数
	_int64 bestresult;
	_int64 test;
	for (level; level >= 0; level--) {
  retry:change = false;
		bestresult = tss(msubframe[level], ssubframe[level], xoff, yoff, scale);
		for (int i = -1; i <= 1; i++) {
			if (i == 0) continue;
			test = tss(msubframe[level], ssubframe[level], xoff + i, yoff, scale);
			if (test < bestresult) {
				newxoff = xoff + i;
				bestresult = test;
				change = true;
			}

		}
		xoff = newxoff;
		for (int i = -1; i <= 1; i++) {
			if (i == 0) continue;
			test = tss(msubframe[level], ssubframe[level], xoff , yoff+i, scale);
			if (test < bestresult) {
				newyoff = yoff + i;
				bestresult = test;
				change = true;
			}

		}
		yoff = newyoff;
		int maxhw = max(msubframe[level].rows, msubframe[level].cols);
		float scalestep = max(0.01f, 1.0f / float(maxhw));
		float temp = scale;
		for (newscale=temp; newscale < 1.1f; newscale += scalestep) {
			test = tss(msubframe[level], ssubframe[level], xoff, yoff, newscale);
			if (test < bestresult) {
				change = true;
				bestresult = test;
				scale = newscale;
			}
		}
		for (newscale=temp; newscale >0.9f; newscale -= scalestep) {
			test = tss(msubframe[level], ssubframe[level], xoff, yoff, newscale);
			if (test < bestresult) {
				change = true;
				bestresult = test;
				scale = newscale;
			}
		}
		if (change)
			goto retry;
		if (!level == 0) {
			xoff *= 2;
			yoff *= 2;
		}


	}
	//get public range
	pt1.x = max(0, w / 2 - int(w / (2 * scale)) + xoff);
	pt1.y = max(0, h / 2 - int(h / (2 * scale)) + yoff);
	pt2.x = min(w, w / 2 + int(w / (2 * scale)) + xoff);
	pt2.y = min(h, h / 2 +int(h / (2 * scale))+ yoff);
	mat_in = Rect(pt1, pt2);
	mm.xoff = xoff;
	mm.yoff = yoff;
	mm.scale = scale;
	return mm;
 


}

void jsyt::allignment(std::vector<Mat> &matIn)
{
	int length = matIn.size();
	int w = matIn[0].cols;
	int h = matIn[0].rows;
	std::vector<Mat> matgray(length);
	for (int i = 0; i < length;i++) {
		if (matIn[i].type() == CV_8UC1)
			matgray[i] = matIn[i].clone();
		else {
			cvtColor(matIn[i], matgray[i], COLOR_RGB2GRAY);
		}
	}
	std::vector<move_para> mp(length);
	int max_size = MAX_SIZE;
	std::vector<Rect> rect(length);
	rect[0] = Rect(Point(0, 0), Point(max_size, max_size));//rect[0]stores the all_in_cover reign
	mp[0] = { 0,0,1.0f };
	Point p1, p2, p3, p4;
	for (int i = 1; i < length; i++) {
		mp[i] = findMovePara(matgray[0], matgray[i], rect[i]);
	std::cout << "fingd public reign[" << i << "]successfully!" << std::endl;
	p1 = Point(rect[0].x, rect[0].y);
	p2 = Point(rect[0].x + rect[0].width, rect[0].y + rect[0].height);
	p3 = Point(rect[i].x, rect[i].y);
	p4 = Point(rect[i].x + rect[i].width, rect[i].y + rect[i].height);
	p1.x = max(p1.x, p3.x);
	p1.y = max(p1.y, p3.y);
	p2.x = min(p2.x, p4.x);
	p2.y = min(p2.y, p4.y);
	rect[0] = Rect(p1, p2);
	if (rect[0].width > w&&rect[0].height > h) 
		std::cout << "rect fail" << std::endl;
	
	}
	std::cout << "find the public reigns successfully" << std::endl;
	//calculate public reign on current mat and resize it
	Mat temp;
	for (int i = 1; i < length; i++) {
		p3.x = (int)((p1.x - mp[i].xoff - w / 2)*mp[i].scale + w / 2 );
		p3.y = (int)((p1.y - mp[i].yoff - h / 2)*mp[i].scale + h / 2);
		p4.x = (int)((p2.x - mp[i].xoff - w / 2)*mp[i].scale + w / 2);
		p4.y = (int)((p2.y - mp[i].yoff - h / 2)*mp[i].scale + h / 2);
		rect[i] = Rect(p3, p4);
		if (p3.x<0 && p3.y<0 && p4.x>w&&p4.y>h) {
			std::cout << "error size" << std::endl;
		}
			
		temp = Mat(matIn[i], rect[i]).clone();
		resize(temp, matIn[i], Size(w, h));
	std::cout << "resize and allign Main_in successfilly" << std::endl;
	std::cout << "mp:" << mp[i].xoff << "," << mp[i].yoff << ",scale:" << mp[i].scale << "\n";
	}
	temp = Mat(matIn[0], rect[0]).clone();
	resize(temp, matIn[0], Size(w, h));
	std::cout << "allignment finish" << std::endl;
}

void jsyt::laplace_method(std::vector<Mat>& matIn,Mat &dst)
{
	
	int msize = matIn.size();
	int w0 = matIn[0].cols;
	int h0 = matIn[0].rows;
	int w = getopticsize(w0);
	int h = getopticsize(h0);
	if (msize > 1) {
		allignment(matIn);
	}
	/*for (int i = 0; i < msize; i++) {
		std::string imgname = "E:\\works\\edf\\experiment\\pyrmid_laplace" + std::string("align") + std::to_string(i) + ".jpg";
		imwrite(imgname, matIn[i]);
		std::cout << imgname << "saved" << std::endl;
	}*/
	int min_half_size = MIN_HALF_SIZE;
	std::vector<Mat> matgray(msize);
	for (int i = 0; i <msize; i++) {
		resize(matIn[i], matIn[i], Size(w, h));
		if (matIn[i].type() == CV_8UC3)
			cvtColor(matIn[i], matgray[i], COLOR_RGB2GRAY);
		else
			matgray[i] = matIn[i].clone();
	}
	/*struct splitimg {
		std::vector<Mat> split;
	};
	std::vector<splitimg> Simg(32);//存储不同等级的压缩图像
	Simg[0].split = matgray;
	int w = matgray[0].cols;
	int h = matgray[0].rows;
	while (w > min_half_size&&h > min_half_size) {
		w /= 2;
		h /= 2;
		std::vector<Mat> temp(msize);
		for (int i = 0; i < msize; i++)
			split(Simg[level-1].split[i], temp[i]);
		Simg[level].split = temp;
		level++;
	}
	std::vector<Mat> lut(level);
	std::vector<Mat> lap(msize);
	for (int i = 0; i < msize; i++)
		Laplacian(Simg[level - 1].split[i], lap[i],CV_8U,3);
	if (lap[0].cols != w || lap[0].rows != h)
		std::cout << "w and h not suit" << std::endl;
	lut[level - 1] = Mat::zeros(Size(w, h), CV_8U);
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int num = 0;
			int max = 0;
			for (int n = 0; n < msize; n++) {
				if (max < lap[n].at<uchar>(i, j)) {
					max = lap[n].at<uchar>(i, j);
					num = n;
				}

			}
			lut[level - 1].at<uchar>(i, j) = num ;
		}
	}
	level -= 1;
	for (level; level > 0; level--) {
		lut[level - 1] = Mat::zeros(Size(w * 2, h * 2), CV_8U);
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				lut[level - 1].at<uchar>(2 * i, 2 * j) = lut[level - 1].at<uchar>(2 * i, 2 * j + 1) = lut[level - 1].at<uchar>(2 * i + 1, 2 * j) = lut[level - 1].at<uchar>(2 * i + 1, 2 * j + 1) =
					lut[level].at<uchar>(i, j);
			}
		}
		w *= 2; h *= 2;
		self_assert(lut[level - 1], msize);
	}*/
	std::vector<Mat> lap(msize);//store final laplace 
	std::vector<Mat> smat(32);
	std::vector<Mat> temp(32);
	for (int n = 0; n < msize; n++) {
		lap[n] = Mat::zeros(Size(w, h), CV_16SC1);
		w = matgray[n].cols;
		h = matgray[n].rows;
		smat[0] = matgray[n].clone();
		int level = 1;
		while (w > min_half_size&&h > min_half_size) {
			w /= 2;
			h /= 2;
			split(smat[level - 1], smat[level]);
			level++;
		}
		for (int levels = level; levels > 0; levels--) {
			Laplacian(smat[levels - 1], temp[levels - 1], 0,3);
			cv::resize(smat[levels - 1], smat[levels-1],Size(w0, h0));
			lap[n] += smat[levels - 1];
			//threshold(temp[levels- 1], temp[levels- 1], findthreshold(temp[levels-1]),0, CV_THRESH_TOZERO);
		}
		
		//if (n == 0)
			//imwrite("lap with adp thres.jpg", lap[n]);
		
		std::cout << "lap" << n << "get" << std::endl;
	}
	//get lookuptable by comparing laplace
	Mat lut = Mat::zeros(lap[0].size(), CV_16SC1);
	/*for (int i = 0; i < msize; i++) {
		std::string matname = "E:\\works\\edf\\experiment\\pyrmid_laplace" + std::string("lap") + std::to_string(i) + ".jpg";
		imwrite(matname, lap[i]);
		std::cout << matname << "saved\n";
	}*/
	lut = getlut(lap);
				
	
		

	std::cout << "lut get" << std::endl;
	//imwrite("lut.jpg", lut);
	//fuse image with lookuotable
	dst = fuseWithLut(matIn, lut,lap);
	return dst;
}
int jsyt::findmax(Mat &kernel, int size) {
	CV_Assert(kernel.size() == Size(3, 3));
	int num, maxnum = 0;
	int mm = -1;
	for (int n = 0; n < size; n++) {
		num = 0;
		for (int i = 0; i < 3; i++) {
			for (int j =0 ; j < 3; j++) {
				if (kernel.at<uchar>(i, j) == n)
					num++;
			}
		}
		if (num > maxnum) {
			maxnum = num;
			mm = n;
		}
	}
	if (mm < 0)
		std::cout << "find max num error\a" << std::endl;
	return mm;

}

void jsyt::highpass(Mat & in,int Threshold)
{
	if (in.type() != CV_8UC1) {
		throw("滤波器只用于灰度图像");
	}
	Mat temp = Mat::zeros(in.size(), in.type());

	GaussianBlur(in, temp, Size(3, 3),0);
	absdiff(temp, in, temp);
	for (int i = 0; i < in.rows; i++) {
		for (int j = 0; j < in.cols; j++) {
			if (temp.at<uchar>(i, j) <=Threshold)
				in.at<uchar>(i, j) = 0;
		}
	}
}

double jsyt::findthreshold(Mat & in)
{
	CV_Assert(in.type() == CV_8UC1);
	int max = 0;
	for (int i = 0; i < in.rows; i++) {
		for (int j = 0; j < in.cols; j++) {
			if (max < in.at<uchar>(i, j))
				max = in.at<uchar>(i, j);
		}
	}
	double t = (double)max / (double)20;
	return t;
}

int jsyt::getopticsize(int w)
{
	int min_half_size = MIN_HALF_SIZE;
	while (min_half_size < w) {
		min_half_size *= 2;
	}
	return min_half_size;


}

void jsyt::island(Mat & in) {
	int *PS;
	int w = in.cols;
	int h = in.rows;
	PS = new int(w*h);
	Mat visited = Mat::zeros(in.size(), CV_8UC1);
	Mat PN = Mat(in.size(),CV_16SC1,Scalar::all(-1));
	int pn, ps, mm, clc;
	int x, y, xx, yy;
	uchar cp, np;
	int X[4], Y[4];
	pn = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (visited.at<uchar>(i, j))
				continue;
			PN.at<int>(i, j) = pn;
			x = j;
			y = i;
			X[0] = x;
			Y[0] = y;
			mm = 1;
			clc = 0;
			for (;;) {
				
				if (mm == 0)
					break;
				mm--;
				cp = in.at<uchar>(Y[mm], X[mm]);
				
				xx = x - 1;
				if (xx > -1) {
					np = in.at<short>(y, xx);
					if (!visited.at<uchar>(y, xx) && cp == np && PN.at<int>(y, xx) == -1) {
						X[mm] = xx;
						Y[mm] = y;
						mm++;
						visited.at<uchar>(y, xx) = 1;
						PN.at<int>(y, xx) = pn;
						clc++;
					}
				}
				yy = y - 1;
				if (yy > -1) {
					np = in.at<short>(yy, x);
					if (!visited.at<uchar>(yy, x) && cp == np && PN.at<int>(yy, x) == -1) {
						X[mm] =x;
						Y[mm] = yy;
						mm++;
						visited.at<uchar>(yy, x) = 1;
						PN.at<int>(yy, x) = pn;
						clc++;
					}
				}
				yy = y + 1;
				if (yy<h) {
					np = in.at<short>(yy, x);
					if (!visited.at<uchar>(yy, x) && cp == np && PN.at<int>(yy, x) == -1) {
						X[mm] = x;
						Y[mm] = yy;
						mm++;
						visited.at<uchar>(yy, x) = 1;
						PN.at<int>(yy, x) = pn;
						clc++;
					}
				}
				xx = x + 1;
				if (xx <w) {
					np = in.at<short>(y, xx);
					if (!visited.at<uchar>(y, xx) && cp == np && PN.at<int>(y, xx) == -1) {
						X[mm] = xx;
						Y[mm] = y;
						mm++;
						visited.at<uchar>(y, xx) = 1;
						PN.at<int>(y, xx) = pn;
						clc++;
					}
				}
				
			}
			PS[pn] = clc;
			pn++;
		}
	}
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (!visited.at<uchar>(i, j))
				continue;
			if (PN.at<int>(i, j) == -1)
				throw("存在未迭代点");
			if (PS[PN.at<int>(i, j)] < (int)(w / 100 * h / 100))
				in.at <uchar>(i,j)=0;
		}
	}
	
}

Mat jsyt::fuseWithLut(std::vector<Mat>& in, Mat &lut,std::vector<Mat>&lap)
{
	Mat dst = in[0].clone();
	Mat clc = Mat::zeros(in[0].size(), CV_32FC1);
	Mat lutT;
	int size = in.size();
	for (int i = 0; i<size; i++) {
		cv::boxFilter(lap[i], lutT,CV_32FC1,Size(5,5));
		abs(lutT);
		clc += in[i];
	}
	int w = lut.cols;
	int h = lut.rows;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (lut.at<short>(i, j) == 0)
				dst.at<float>(i, j) = clc.at<float>(i, j) / size;
			else
				dst.at<float>)(i, j) = in[lut.at<short>(i, j) - 1].at<float>(i, j);
		}
	}
	return dst;
}

void jsyt::removeStep(Mat & lut, std::vector<Mat>& matIn)
{
	short abs1, abs2, abs3, abs4, abs5;
	uchar r1, r2, r3, g1, g2, g3, b1, b2, b3;
	Mat visited = Mat::zeros(lut.size(), CV_8UC1);
	for (int i = 2; i < lut.rows-2; i++) {
		for (int j = 2; j < lut.cols - 2; j++) {
			abs1 = 0, abs2 = 0, abs3 = 0, abs4 = 0, abs5 = 0;
			short m = lut.at<short>(i, j);
			if (m != 0||visited.at<uchar>(i,j))
				continue;
			//x direction
			if (lut.at<short>(i + 1, j) > 0&&!visited.at<uchar>(i+1,j)) {
				r1 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j)[0];
				g1 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j)[1];
				b1 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j)[2];
				r2 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j + 1)[0];
				g2 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j + 1)[1];
				b2 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j + 1)[2];
				r3 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j - 1)[0];
				g3 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j - 1)[1];
				b3 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j - 1)[2];
				abs2 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs2 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i + 1, j);
					abs1 = abs2;
				}
			}

			if (lut.at<short>(i, j + 1) > 0 &&!visited.at<uchar>(i, j+1)) {
				r1 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j + 1)[0];
				g1 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j + 1)[1];
				b1 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j + 1)[2];
				r2 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j + 2)[0];
				g2 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j + 2)[1];
				b2 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j + 2)[2];
				r3 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j)[0];
				g3 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j)[1];
				b3 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j)[2];
				abs3 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs3 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i, j + 1);
					abs1 = abs3;
				}
			}


			if (lut.at<short>(i - 1, j) > 0 && !visited.at<uchar>(i-1,j)) {
				r1 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j)[0];
				g1 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j)[1];
				b1 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j)[2];
				r2 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j + 1)[0];
				g2 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j + 1)[1];
				b2 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j + 1)[2];
				r3 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j - 1)[0];
				g3 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j - 1)[1];
				b3 = matIn[lut.at<short>(i - 1, j) - 1].at<Vec3b>(i - 1, j - 1)[2];
				abs4 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs4 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i - 1, j);
					abs1 = abs4;
				}

			}
			if (lut.at<short>(i, j - 1) > 0 &&!visited.at<uchar>(i, j-1)) {
				r1 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 1)[0];
				g1 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 1)[1];
				b1 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 1)[2];
				r2 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 2)[0];
				g2 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 2)[1];
				b2 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 2)[2];
				r3 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j)[0];
				g3 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j)[1];
				b3 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j)[2];
				abs5 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs5 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i, j - 1);
					abs1 = abs5;
				}
			}
			visited.at<uchar>(i, j) = 1;
			//y direction
		   /* if (lut.at<short>(i + 1, j)>0) {
				r1 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j)[0];
				g1 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j)[1];
				b1 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 1, j)[2];
				r2 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 2, j)[0];
				g2 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 2, j)[1];
				b2 = matIn[lut.at<short>(i + 1, j) - 1].at<Vec3b>(i + 2, j)[2];
				r3 = matIn[lut.at<short>(i+1, j)-1].at<Vec3b>(i, j )[0];
				g3 = matIn[lut.at<short>(i+1, j)-1].at<Vec3b>(i, j )[1];
				b3 = matIn[lut.at<short>(i+1, j)-1].at<Vec3b>(i, j )[2];
				abs2 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs2 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i + 1, j);
					abs1 = abs2;
				}
			}

			if (lut.at<short>(i, j + 1) > 0) {
				r1 = matIn[lut.at<short>(i, j+1) - 1].at<Vec3b>(i, j+1)[0];
				g1 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j+1)[1];
				b1 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i, j+1)[2];
				r2 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i+1, j + 1)[0];
				g2 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i+1, j + 1)[1];
				b2 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i+1, j + 1)[2];
				r3 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i-1, j +1)[0];
				g3 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i-1, j +1)[1];
				b3 = matIn[lut.at<short>(i, j + 1) - 1].at<Vec3b>(i-1, j +1)[2];
				abs3 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs3 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i, j + 1);
					abs1 = abs3;
				}
			}


			if (lut.at<short>(i - 1, j) > 0) {
				r1 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i-1, j )[0];
				g1 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i-1, j )[1];
				b1 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i-1, j)[2];
				r2 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i, j)[0];
				g2 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i, j )[1];
				b2 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i, j )[2];
				r3 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i+1, j)[0];
				g3 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i+1, j)[1];
				b3 = matIn[lut.at<short>(i-1, j) - 1].at<Vec3b>(i+1, j)[2];
				abs4 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs4 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i-1, j);
					abs1 = abs4;
				}

			}
			if (lut.at<short>(i, j - 1) > 0) {
				r1 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 1)[0];
				g1 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 1)[1];
				b1 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i, j - 1)[2];
				r2 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i + 1, j - 1)[0];
				g2 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i + 1, j - 1)[1];
				b2 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i + 1, j - 1)[2];
				r3 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i - 1, j - 1)[0];
				g3 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i - 1, j - 1)[1];
				b3 = matIn[lut.at<short>(i, j - 1) - 1].at<Vec3b>(i - 1, j - 1)[2];
				abs5 = abs(r2 + r3 - 2 * r1) + abs(g2 + g3 - 2 * g1) + abs(b2 + b3 - 2 * b2);
				if (abs5 > abs1) {
					lut.at<short>(i, j) = lut.at<short>(i, j - 1);
					abs1 = abs5;
				}*/
		
		}
	}
}

Mat jsyt::getlut(std::vector<Mat> &lap)
{
	std::vector<Mat> lapT(lap.size());
	if (lap[0].type() != CV_32FC1) {
		std::cout << "lap will be convertTo CV_32FC1";
		for (int i = 0; i < lap.size(); i++) {
			lap[i].convertTo(lap[i], CV_32FC1, 1.0f / 255.0f);
		}
	}
	Mat clc = Mat::zeros(lapT[0].size(), CV_32FC1);
	for (int i = 0; i < lap.size(); i++) {
		boxFilter(lap[i], lapT[i], CV_32FC1, Size(5, 5));
		abs(lapT[i]);
		threshold(lapT[i], lapT[i], 0.01f, 0, THRESH_TOZERO);
		clc += lapT[i];
	}
	int w = clc.cols;
	int h = clc.rows;
	Mat dst(clc.size(), CV_16SC1);
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (clc.at<float>(i, j) == 0)
				dst.at<short>(i, j) = 0;//means using average method
			else {
				float max=0;
				uchar num = 0;
				for (int n = 0; n < lap.size(); n++) {
					if (lapT[n].at<float>(i, j) > max) {
						num = n + 1;
						max = lapT[n].at<float>(i, j);
					}
				}
				dst.at<short>(i, j) = num;
			}

		}
	}
	return dst;
	
}

Mat jsyt::lapPyramin(std::vector<Mat> splitMat, int level)
{
	int w0 = splitMat[0].cols;
	int h0 = splitMat[0].rows;
	Mat dst = Mat::zeros(Size(w0, h0), CV_32FC1);
	for (int i = level; i >= 0; i--) {
		Laplacian(splitMat[i ], splitMat[i ], 0, 3);
		if (i != 0)
			resize(splitMat[i ], splitMat[i],Size(w0,h0));
		dst += splitMat[i];
	}
	return dst;
		
}

int jsyt::split(Mat & in, std::vector<Mat>& splitM)
{
	int level = 0;
	splitM[0] = in;
	int w = in.cols;
	int h = in.rows;
	w /= 2, h /= 2;

	while (jsyt::split(splitM[level], splitM[level + 1]))
	{
		level++;
	}
	return level;
}

	

void jsyt::self_assert(Mat & in, int size)
{
	
		CV_Assert(in.type() == CV_8UC1);
		int w = in.cols;
		int h = in.rows;
		Mat temp = in.clone();
		int maxnum = 0;
		int num = 0;

		Mat kernel = Mat::zeros(Size(3, 3), CV_8UC1);
		//return the most elements of every 3*3 kernel
		for (int i = 1; i < h - 1; i += 2) {
			for (int j = 1; j < w - 1; j += 2) {
				kernel = Mat(temp, Rect(Point(j - 1, i - 1), Size(3, 3)));
				in.at<uchar>(i, j) = findmax(kernel, size);
			}
		}
}




