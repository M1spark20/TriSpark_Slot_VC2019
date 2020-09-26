#pragma once
#include <string>

// [act]dstでの画像描画方法指定
enum class EDrawModeForDST { eNearest, eBilinear };

// [act]dstでの画像ブレンド方法指定
enum class EBlendModeForDST {
	eNone, eAlpha, eAdd, eSub, eMula,	// 全画面に描画可
	ePMAAlpha, ePMAAdd, ePMASub,		// MakeScreenのみ適用可
	eScreen, eOverlay, eDodge, eBurn, eDarken, eLighten, eSoftlight, eHardlight, eExclusion	// MakeScreenのみ適用可
};

// [act]src,flashの共通データかつ分割可能な部分を定義する
struct SImageSourceCSVCommonData {
	int startTime;
	int imageID;
	int x, y, w, h;
	int numX, numY;
	bool directionY;
};

// [act]dstの共通データかつ分割可能な部分を定義する
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