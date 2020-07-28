#pragma once
#include "SDrawImageSourceData.hpp"
#include <string>
#include <vector>

class IImageSourceManager {
protected:
	int		mImageID;
	int		mPosX, mPosY, mPosW, mPosH;
	int		mCountX, mCountY;
	bool	mDirectionFlag;

	typedef std::vector<std::string> StringArr;

	template<class T> void StrToNum(T& InputFor, std::string& p_Data) {
		// [act]テンプレート型、1つ目の引数に2つ目の文字列から抽出した数字を代入します
		//		bool型の場合は整数で判別し、0がfalse、それ以外がtrueとなる模様です
		std::istringstream Data(p_Data);
		Data >> InputFor;
	};

public:
	IImageSourceManager();
	typedef unsigned int UINT;
	virtual bool					Init(StringArr pReadData);
	virtual UINT					GetSourceIndexMax() = 0;
	virtual SDrawImageSourceData	GetImageSource(int pIndex) = 0;
};

class CImageSourceDefault : public IImageSourceManager {
public:
	CImageSourceDefault() {}
	bool					Init(StringArr pReadData) override;
	UINT					GetSourceIndexMax() override;
	SDrawImageSourceData	GetImageSource(int pIndex) override;
};

class CImageSourceNumber : public IImageSourceManager {
	int mDigit, mAlign;
	bool mDrawMinusFlag;
	std::string mNumValName;
	int (* const mVariableGetter)(std::string);		// 変数値取得用関数ポインタ(int限定,引数はstring:変数名)
public:
	CImageSourceNumber(int (*const pGetVarFunc)(std::string));
	bool					Init(StringArr pReadData) override;
	UINT					GetSourceIndexMax() override;
	SDrawImageSourceData	GetImageSource(int pIndex) override;
};