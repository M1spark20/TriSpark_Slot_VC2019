#pragma once
#include "SDrawImageSourceData.hpp"

struct SReelDrawData{
//[act] ���[���`��p�\����
	unsigned int reelID;
	float x;
	float y;
	float comaW;
	float comaH;
	float offsetUpper;
	float offsetLower;
	unsigned int comaNum;
};

struct SReelDrawDataFromCSV {
	struct { int x, y, w, h; } srcPos, dstPos;
	int a;
	int extendModeID, blendModeID;
	int destScr, preDrawScr;		// csv�f�[�^����DxLib���̃X�N���[���n���h���ɕϊ��������̂����
	int blew, blewTime;
	int reelID, comaNum, originComa, comaBegin, comaIndexMax;
};