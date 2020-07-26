#pragma once
#include "SDrawImageSourceData.hpp"
#include <string>

class IImageSourceManager {
	int		mPosX, mPosY, mPosW, mPosH;
	int		mCountX, mCountY;
	bool	mDirectionFlag;

public:
	IImageSourceManager();
	typedef unsigned int UINT;
	virtual bool					Init(std::string pReadData) = 0;
	virtual UINT					GetSourceIndexMax() = 0;
	virtual SDrawImageSourceData	ImageSource(int pIndex) = 0;
};