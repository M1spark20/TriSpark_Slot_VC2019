#pragma once
#include <string>

// [act]dst�ł̉摜�`����@�w��
enum class EDrawModeForDST { eNearest, eBilinear };

// [act]dst�ł̉摜�u�����h���@�w��
enum class EBlendModeForDST {
	eNone, eAlpha, eAdd, eSub, eMula,	// �S��ʂɕ`���
	ePMAAlpha, ePMAAdd, ePMASub,		// MakeScreen�̂ݓK�p��
	eScreen, eOverlay, eDodge, eBurn, eDarken, eLighten, eSoftlight, eHardlight, eExclusion	// MakeScreen�̂ݓK�p��
};

// [act]src,flash�̋��ʃf�[�^�������\�ȕ������`����
struct SImageSourceCSVCommonData {
	int startTime;
	int imageID;
	int x, y, w, h;
	int numX, numY;
	bool directionY;
};

// [act]dst�̋��ʃf�[�^�������\�ȕ������`����
struct SImageDestCSVCommonData {
	int screenID;
	int  startTime;
	int  x, y, w, h, a;
	EDrawModeForDST  extend;
	EBlendModeForDST blend;
};

struct SImageColorCSVCommonData {
	int startTime;
	int imageID;
	int x, y, w, h;
	int numX, numY;
	int loopCount;
	bool directionY;
	bool isColorIndexDirY;
	bool useAnimation;
};