#include "_header/IMenuElements.hpp"
#include "DxLib.h"
#include <cstring>

IMenuElements::IMenuElements(const std::string pName, const int pBaseImgID, const int pTitleFontHandle)
	 :	cElementName(pName), mBaseImgID(pBaseImgID), mTitleFontHandle(pTitleFontHandle) {
}

bool IMenuElements::DrawBase(const int pOpacity) {
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, pOpacity);
	DxLib::DrawGraph(161, 180, mBaseImgID, TRUE);

	const int titleWidth = 
		DxLib::GetDrawStringWidthToHandle(cElementName.c_str(), strlen(cElementName.c_str()), mTitleFontHandle);
	if (titleWidth == -1) return false;
	DxLib::DrawStringToHandle(255 + (792 - titleWidth) / 2, 206, cElementName.c_str(), 0xFFFFFF, mTitleFontHandle);
	return true;
}


CMenuLicenses::CMenuLicenses(const int pLicenseFileID, const int pDataFontHandle, const int pBaseImgID, const int pTitleFontHandle)
	 :	IMenuElements(u8"Credits / Licenses", pBaseImgID, pTitleFontHandle),
		mLicenseFileID(pLicenseFileID),
		mFontHandle(pDataFontHandle) {
	mCurrentStartCol = 0;
}

bool CMenuLicenses::Init() {
	int hoge = DxLib::FileRead_seek(mLicenseFileID, 0, SEEK_SET);

	for (auto i = 0; DxLib::FileRead_eof(mLicenseFileID) == 0; ++i) {
		if (i % MOVE_AMOUNT == 0) mSeekPosList.push_back(DxLib::FileRead_tell(mLicenseFileID));
		char buf[BUFSIZE];
		DxLib::FileRead_gets(buf, 2048, mLicenseFileID);
	}
	return true;
}

bool CMenuLicenses::Process() {
	return true;
}

bool CMenuLicenses::Draw(const int pOpacity) {
	DrawBase(pOpacity);

	DxLib::FileRead_seek(mLicenseFileID, mSeekPosList[mCurrentStartCol], SEEK_SET);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, pOpacity);
	for (auto i = 0; i < 28; ++i) {
		char buf[BUFSIZE];
		DxLib::FileRead_gets(buf, BUFSIZE, mLicenseFileID);
		DxLib::DrawStringToHandle(171,290+21*i, buf, 0xFFFFFF, mFontHandle);
	}
	return true;
}

EMenuList CMenuLicenses::PushButton(int pKeyHandleDX) {
	switch (pKeyHandleDX)
	{
	case KEY_INPUT_LEFT:
		return EMenuList::eReelHistory;
		break;
	case KEY_INPUT_RIGHT:
		return EMenuList::eReelHistory;
		break;
	case KEY_INPUT_UP:
		if (mCurrentStartCol > 0) --mCurrentStartCol;
		return EMenuList::eContinue;
		break;
	case KEY_INPUT_DOWN:
		if ((unsigned int)mCurrentStartCol < mSeekPosList.size()-1) ++mCurrentStartCol;
		return EMenuList::eContinue;
		break;
	default:
		break;
	}
	return EMenuList::eContinue;
}


CMenuReelHistory::CMenuReelHistory(const int pDataFontHandle,const int pBaseImgID, const int pTitleFontHandle)
	 :	IMenuElements(u8"Stop History", pBaseImgID, pTitleFontHandle),
		mFontHandle(pDataFontHandle) {
}

bool CMenuReelHistory::Init() {
	return true;
}

bool CMenuReelHistory::Process() {
	return true;
}

bool CMenuReelHistory::Draw(const int pOpacity) {
	DrawBase(pOpacity);

	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, pOpacity);
	DxLib::DrawStringToHandle(171, 290, u8"Coming Soon...", 0xFFFFFF, mFontHandle);
	return true;
}

EMenuList CMenuReelHistory::PushButton(int pKeyHandleDX) {
	switch (pKeyHandleDX)
	{
	case KEY_INPUT_LEFT:
		return EMenuList::eLicense;
		break;
	case KEY_INPUT_RIGHT:
		return EMenuList::eLicense;
		break;
	case KEY_INPUT_UP:
		return EMenuList::eContinue;
		break;
	case KEY_INPUT_DOWN:
		return EMenuList::eContinue;
		break;
	default:
		break;
	}
	return EMenuList::eContinue;
}
