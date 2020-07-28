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
		// [act]�e���v���[�g�^�A1�ڂ̈�����2�ڂ̕����񂩂璊�o���������������܂�
		//		bool�^�̏ꍇ�͐����Ŕ��ʂ��A0��false�A����ȊO��true�ƂȂ�͗l�ł�
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
	int (* const mVariableGetter)(std::string);		// �ϐ��l�擾�p�֐��|�C���^(int����,������string:�ϐ���)
public:
	CImageSourceNumber(int (*const pGetVarFunc)(std::string));
	bool					Init(StringArr pReadData) override;
	UINT					GetSourceIndexMax() override;
	SDrawImageSourceData	GetImageSource(int pIndex) override;
};