#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<iostream>
using namespace cv;
static Mat src1 = imread("3.jpg", 0);
static Mat src2 = imread("4.jpg", 0);
bool isclolor = 0;
#define MIN_HALF_SIZE 32
struct move_para {
	int index, step;
	int w, h;
	Mat src;
}Move_para[32];
struct tss_para {
	__int64 result;
	bool iscolor;
	int starx, endx, xstep;
	int stary, endy, ystep;
	int w, h;
	int xoffset, yoffset;
	int cx, cy, r2;
	float scale;
	Mat sample, master;
}Tss_para[32];
int tss(tss_para &tt) {
	_int64 result = tt.result;
	int starx = tt.starx;
	int stary = tt.stary;
	int endx = tt.endx;
	int endy = tt.endy;
	int xstep = tt.xstep;
	int ystep = tt.ystep;
	bool iscolor = tt.iscolor;
	Mat sample = tt.sample;
	Mat master = tt.master;

	int xoffset = tt.xoffset;
	int yoffset = tt.yoffset;
	double scale = tt.scale;
	int w = tt.w;
	int h = tt.h;


	if (xoffset > sample.cols / 8 || xoffset < -sample.cols / 8) { tt.result = 000; return 1; }
	if (yoffset > sample.rows / 8 || xoffset < -sample.rows / 8) { tt.result = 0; return 1; }

	if (iscolor) {
		for (int y = stary; y < endy; y += ystep) {
			int yy = (int)(((float)(y - yoffset) - (float)(h / 2))*scale + (float)(h / 2) + 0.5);
			if (yy < h / 16)yy = h / 16;
			if (yy > 15 * h / 16) yy = 15 * h / 16;
			for (int x = starx; x < endx; x += xstep) {
				int xx = (int)(((float)(x - xoffset) - (float)(w / 2))*scale + (float)(w / 2) + 0.5);
				if (xx < w / 16)xx = w / 16;
				if (xx > 15 * w / 16)xx = 15 * w / 16;
				Vec3b s = sample.at<Vec3b>(yy, xx);
				Vec3b m = master.at<Vec3b>(y, x);
				int t = s[0] - m[0]; result += (t*t) / 16;
				t = s[1] - m[1]; result += (t*t) / 16;
				t = s[2] - m[2]; result += (t*t) / 16;

			}
		}
	}
	else {
		for (int y = stary; y < endy; y += ystep) {
			int yy = (int)(((float)(y - yoffset) - (float)(h / 2))*scale + (float)(h / 2) + 0.5);
			if (yy < h / 16)yy = h / 16;
			if (yy > 15 * h / 16) yy = 15 * h / 16;
			for (int x = starx; x < endx; x += xstep) {
				int xx = (int)(((float)(x - xoffset) - (float)(w / 2))*scale + (float)(w / 2) + 0.5);
				if (xx < w / 16)xx = w / 16;
				if (xx > 15 * w / 16)xx = 15 * w / 16;
				uchar s = sample.at<uchar>(yy,xx);
				uchar m = master.at<uchar>(y,x);
				int t = s - m; result += (t*t) / 16;
			}
		}
	}
	tt.result = result;
	return 0;
}
_int64 testshiftnscale(Mat sample, Mat master, int xoffset, int yoffset, float scale) {
	_int64 result = 0;
	int starx, endx, stary, endy;
	int xstep, ystep;
	int h = sample.rows;
	int w = sample.cols;
	starx = w / 32;
	endx = w - starx;
	stary = h / 32;
	endy = h - stary;
	xstep = w / 1000; if (xstep < 2) xstep = 2;
	ystep = h / 1000; if (ystep < 2)ystep = 2;
	scale = 1.0f / scale;
	
	
	int object = 1;
	for (int obj = 0; obj < object; obj++) {
		Tss_para[obj].iscolor = isclolor;
		Tss_para[obj].stary = stary + ystep * obj;
		Tss_para[obj].endy = endy;
		Tss_para[obj].ystep = ystep * object;
		Tss_para[obj].starx = starx;
		Tss_para[obj].endx = endx;
		Tss_para[obj].xstep = xstep;
		Tss_para[obj].w = w;
		Tss_para[obj].h = h;
		Tss_para[obj].xoffset = xoffset;
		Tss_para[obj].yoffset = yoffset;
		Tss_para[obj].scale = scale;
		Tss_para[obj].sample = sample;
		Tss_para[obj].master = master;
		Tss_para[obj].result = 0;
		tss(Tss_para[obj]);
		result += Tss_para[obj].result;
	
	}
	return result;
}
struct split_para {
	int step, index;
	bool iscolor;
	int w, h;
	Mat in, out;
}Split_para[32];

uint split(split_para pp) {
	int index = pp.index;
	int step = pp.step;
	bool iscolor = pp.iscolor;
	Mat in = pp.in;
	Mat out = pp.out;
	int h = pp.h;
	int w = pp.w;

	int  x, y;
	if (iscolor) {
		for(y=index;y<h;y+=step)
			for(x=0;x<w;x++)
				out.at<Vec3b>(y,x)=(Vec3b)(in.at<Vec3b>(2*y,2*x)+in.at<Vec3b>(2*y+1,2*x)+
				in.at<Vec3b>(2 * y, 2 * x+ 1) + in.at<Vec3b>(2 * y + 1, 2 * x + 1)) / 4;
	}
	else{
		for (y = index; y<h; y += step)
			for (x = 0; x<w; x++)
				out.at<uchar>(y, x) = (uchar)(in.at<uchar>(2 * y, 2 * x) + in.at<uchar>(2 * y + 1, 2 * x) +
					in.at<uchar>(2 * y, 2 * x + 1) + in.at<uchar>(2 * y + 1, 2 * x+ 1)) / 4;
	}
	return 0;
		
}
struct RECT {
	int rect_left, rect_top, rect_bottom, rect_right;
};
struct sax_para{
	uint index,step;
	int w, h;
	int yoffset;
	bool iscolor;
	int frame_border;
	int smallw, smallh;
	
	Mat in, out;
	RECT goodRect;
	float scale;
}Sax_para[32];

uint sax(sax_para pp) {
	int index = pp.index;
	int h = pp.h;
	int w = pp.w;
	int frame_border = pp.frame_border;
	int step = pp.step;
	int yoffset = pp.yoffset;
	int yy;
	int dyy;
	int y;
	int x, xx;
	bool iscolor = pp.iscolor;
	int smallh = pp.smallh;
	int smallw = pp.smallw;
	
	Mat in = pp.in;
	Mat out = pp.out;
	RECT goodRect = pp.goodRect;
	float scale = pp.scale;

	for (y = frame_border + index; y< frame_border + smallh; y += step) {
		float yf = ((float)(y - yoffset) - (float)(h / 2))*scale + (float)(h / 2);
		yy = (int)yf;
		if (yy<frame_border) {
			if (y > goodRect.rect_top)goodRect.rect_top = y;
			yy = frame_border;
			if (yy > goodRect.rect_top)goodRect.rect_top= yy;
		}
		if (yy >frame_border + smallh - 2) {
			yy = frame_border + smallh-2;
			if (goodRect.rect_bottom > y || goodRect.rect_bottom > yy)
				goodRect.rect_bottom = y < yy ? y : yy;

		}
		for (x = frame_border; x < frame_border + smallw; x++) {
			 xx = (int)(((float)x - (float)(h / 2))*scale + float(h / 2));
			if (xx < frame_border) {
				xx = frame_border;
				if (goodRect.rect_left < xx || goodRect.rect_left < x)
					goodRect.rect_left = xx > x ? xx : x;
			}
			if (xx > frame_border + smallw - 2) {
				xx = frame_border + smallw - 2;
				if (goodRect.rect_right > xx || goodRect.rect_right > x)
					goodRect.rect_right = xx < x ? xx : x;
			}
		}
		if (isclolor) {
			out.at<Vec3b>(y, x) = in.at<Vec3b>(yy, xx);
		}
		else
			out.at<uchar>(y, x) = in.at<uchar>(yy, xx);
	}
	return 0;
}
int ironclaw() {
	int min_half_size = MIN_HALF_SIZE;
	
	Mat msubframe[32], ssubframe[32];
	int level;
	int width[32], height[32];
	bool change = false;
	msubframe[0] = src1;
	ssubframe[0] = src2;
	int w = src1.cols;
	int h = src1.rows;
	width[0] = w;
	height[0] = h;
	Mat temp;
	int minw = w, minh = h;
	level = 1;
	split_para pp;
	//split these image into pyrmid
	while (minh > min_half_size&&minw > min_half_size) {
		minh /= 2;
		minw /= 2;
		width[level] = minw;
		height[level] = minh;
		temp = Mat::zeros(Size(minw, minh), CV_8U);
		pp.w = width[level];
		pp.h = height[level];
		pp.iscolor = isclolor;
		pp.index = 0;
		pp.step = 1;
		pp.in = msubframe[level - 1];
		pp.out = temp;
		split(pp);
		
			
		msubframe[level] = pp.out;
		level++;
	}
	for (int levels = 1; levels <level; levels++) {
		temp = Mat::zeros(Size(width[levels], height[levels]), CV_8U);
		pp.w = width[levels];
		pp.h = height[levels];
		pp.iscolor = isclolor;
		pp.index = 0;
		pp.step = 1;
		pp.in = ssubframe[levels-1];
		pp.out = temp;
		split(pp);
		ssubframe[levels] = temp;
		
	}
	int xoff=0,yoff = 0;
	int newxoff=xoff, newyoff=yoff;
	

	float scale = 1.0f;
	float newscale=scale;
	level -= 1;
	for (level; level >= 0; level--) {
	retry:	change = false;
		_int64 besttest = testshiftnscale(ssubframe[level], msubframe[level], xoff, yoff, scale);
		_int64 test;
		int maxwh = max(width[level], height[level]);
		float scalestep = (float)(maxwh + 2) /(float) maxwh - 1.0f;
		besttest = testshiftnscale(ssubframe[level], msubframe[level], xoff, yoff, scale);
	
		for (float testscale = scale; testscale < 1.1f; testscale += scalestep) {
			test= testshiftnscale(ssubframe[level], msubframe[level], xoff, yoff, testscale);
			if (test < besttest) {
				besttest = test;
				newscale = testscale;
				change = true;
			}
		}
		for (float testscale = scale; testscale > 0.9f; testscale -= scalestep) {
			test = testshiftnscale(ssubframe[level], msubframe[level], xoff, yoff, testscale);
			if (test < besttest) {
				besttest = test;
				newscale = testscale;
				change = true;
			}

		}
		scale = newscale;
		for (int i = -1; i < 2; i++) {
			if (i == 0)continue;
			test = testshiftnscale(ssubframe[level], msubframe[level], xoff + i, yoff, scale);
			if (test < besttest) {
				besttest = test;
				newxoff = xoff + i;
				change = true;
			}
			

		}
		xoff = newxoff;
		for (int i = -1; i < 2; i++) {
			if (i ==0)continue;
			test = testshiftnscale(ssubframe[level], msubframe[level], xoff, yoff + i, scale);
			if (test < besttest) {
				besttest = test;
				newyoff = yoff + i;
				change = true;
			}
		}
		
		yoff = newyoff;
		if (change) goto retry;
		if (level != 0) {
			xoff *= width[level - 1] / width[level];
			yoff *= 2;
		}//改变初值，迭代
	}
	std::cout << "Best peizhun" <<"xoff="<< xoff << ",yoff=" << yoff << ",scale=" << scale;
	system("pause");
	return 0;

}