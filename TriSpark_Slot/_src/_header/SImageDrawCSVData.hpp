#pragma once

// [act]src,flash�̋��ʃf�[�^�������\�ȕ������`����
struct SImageSourceCSVCommonData {
	int* startTime;
	int* imageID;
	int  *x, *y, *w, *h;
	int  *numX, *numY;
	bool directionY;
};

// [act]dst�̋��ʃf�[�^�������\�ȕ������`����
struct SImageDestCSVCommonData {
	int*  screenID;
	int*  startTime;
	int   *x, *y, *w, *h, *a;
	int   *extendMode, *blendMode;
};