#pragma once

// [act]src,flashの共通データかつ分割可能な部分を定義する
struct SImageSourceCSVCommonData {
	int* startTime;
	int* imageID;
	int  *x, *y, *w, *h;
	int  *numX, *numY;
	bool directionY;
};

// [act]dstの共通データかつ分割可能な部分を定義する
struct SImageDestCSVCommonData {
	int*  screenID;
	int*  startTime;
	int   *x, *y, *w, *h, *a;
	int   *extendMode, *blendMode;
};