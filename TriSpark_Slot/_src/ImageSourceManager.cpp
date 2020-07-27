#include "_header/ImageSourceManager.hpp"

IImageSourceManager::IImageSourceManager() {
	mPosX = -1; mPosY = -1; mPosW = -1; mPosH = -1;
	mCountX = 0; mCountY = 0;
	mDirectionFlag = false;
}

bool IImageSourceManager::Init(StringArr pReadData) {
	StrToNum(mImageID,			pReadData[1]);
	StrToNum(mPosX,				pReadData[2]);
	StrToNum(mPosY,				pReadData[3]);
	StrToNum(mPosW,				pReadData[4]);
	StrToNum(mPosH,				pReadData[5]);
	StrToNum(mCountX,			pReadData[6]);
	StrToNum(mCountY,			pReadData[7]);
	StrToNum(mDirectionFlag,	pReadData[8]);
	return true;
}

bool CImageSourceDefault::Init(StringArr pReadData) {
	if (pReadData.size() <= 8) return false;
	return IImageSourceManager::Init(pReadData);
}

unsigned int CImageSourceDefault::GetSourceIndexMax() {
	return mCountX * mCountY;
}

SDrawImageSourceData CImageSourceDefault::GetImageSource(int pIndex) {
	SDrawImageSourceData ans;
	ans.x = 0; ans.y = 0; ans.w = 0; ans.h = 0;
	ans.r = 0; ans.g = 0; ans.b = 0; ans.imageID = -1;
	if (pIndex < 0 || pIndex >= GetSourceIndexMax()) return ans;

	ans.r = 255; ans.g = 255; ans.b = 255;
	ans.x = mPosX; ans.y = mPosY;
	ans.w = mPosW; ans.h = mPosH;
	ans.imageID = mImageID;
	return ans;
}

CImageSourceNumber::CImageSourceNumber(int (* const pGetVarFunc)(std::string)) : mVariableGetter(pGetVarFunc) {
	mDigit = 0; mAlign = 0;
}

bool CImageSourceNumber::Init(StringArr pReadData) {
	if (pReadData.size() <= 11) return false;
	if (!IImageSourceManager::Init(pReadData)) return false;

	mNumValName = pReadData[9];
	StrToNum(mDigit,	pReadData[10]);
	StrToNum(mAlign,	pReadData[11]);

	return true;
}

