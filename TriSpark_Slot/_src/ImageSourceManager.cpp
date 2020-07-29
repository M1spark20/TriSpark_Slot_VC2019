#include "_header/ImageSourceManager.hpp"
#include "_header/ErrClass.hpp"

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
	mDigit = 0; mDrawMinusFlag = true;
}

bool CImageSourceNumber::Init(StringArr pReadData) {
	if (pReadData.size() <= 11) return false;
	if (!IImageSourceManager::Init(pReadData)) return false;
	
	mNumValName = pReadData[9];
	StrToNum(mDigit,	pReadData[10]);
	mDrawMinusFlag = pReadData[11] == "T";
	return true;
}

unsigned int CImageSourceNumber::GetSourceIndexMax() {
	try {
		int ans = mVariableGetter(mNumValName);
		if (ans < 0) {
			if (!mDrawMinusFlag) return 0;
			return abs(ans) / 10 + 2;
		} else {
			return ans / 10 + 1;
		}
	}
	catch (ErrUndeclaredVar err) {	// エラークラスを後で定義する
		err.WriteErrLog();
	}
	return 0;
}

SDrawImageSourceData CImageSourceNumber::GetImageSource(int pIndex) {
	SDrawImageSourceData dummy;
	dummy.x = 0; dummy.y = 0; dummy.w = 0; dummy.h = 0;
	dummy.r = 0; dummy.g = 0; dummy.b = 0; dummy.imageID = -1;
	if (pIndex < 0 || pIndex >= GetSourceIndexMax()) return dummy;

	try {
		SDrawImageSourceData ans(dummy);
		int numIndex = mVariableGetter(mNumValName);
		for (int i = 0; i < pIndex; ++i) numIndex /= 10;
		numIndex %= 10;
		const int diffX = mPosW / mCountX, diffY = mPosH / mCountY;
		const int indexX = !mDirectionFlag ? numIndex % mCountX : numIndex / mCountY;
		const int indexY =  mDirectionFlag ? numIndex / mCountX : numIndex % mCountY;
		ans.x = mPosX + diffX * indexX; ans.y = mPosY + diffY * indexY;
		ans.w = mPosW / mCountX; ans.h = mPosH / mCountY;
		dummy.r = 255; dummy.g = 255; dummy.b = 255; dummy.imageID = mImageID;
		return ans;
	}
	catch (ErrUndeclaredVar err) {
		err.WriteErrLog();
	}
	return dummy;
}