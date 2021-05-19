#include "_header/IMenuElements.hpp"
#include "_header/CGameDataManage.h"
#include "_header/CGetSysDataFromCSV.hpp"
#include "_header/CMenuReadHowtoFromCSV.hpp"
#include "_header/CReelManager.hpp"
#include "DxLib.h"
#include <string>
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
		return EMenuList::eHowTo;
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


CMenuReelHistory::CMenuReelHistory(CGameDataManage& pGameData, const int pDataFontHandle, const int pDataFontHandleMid, const int pBaseImgID, const CReelManager& pReelData, const int pTitleFontHandle)
	 :	IMenuElements(u8"Stop History", pBaseImgID, pTitleFontHandle), mFontHandle(pDataFontHandle), mFontHandleMid(pDataFontHandleMid), mHistBaseImgID(pBaseImgID) {
	mNowPage = 0;

	CGetSysDataFromCSV reader;
	reader.FileInit(pGameData.GetDataHandle(0));

	mHistBaseImgID = pGameData.GetDataHandle(reader.GetSysDataID("ReelHistoryBase"));
	mReelImgID = pGameData.GetDataHandle(reader.GetSysDataID("ReelForMenu"));

	for (size_t i = 0; i < pReelData.GetHistoryNum(); ++i) {
		mHistoryData.push_back(pReelData.GetHistoryData(i));
	}
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
	DxLib::DrawGraph(231, 320, mHistBaseImgID, TRUE);
	const int relX = 231, relY = 320;

	// 1段目
	std::string writeData = std::to_string(mNowPage * 2);
	int numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 24, relY - 1, writeData.c_str(), 0xFFFFFF, mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 24 + numWidth, relY + 6, u8"G前", 0xFFFFFF, mFontHandle);
	if (mHistoryData.size() <= mNowPage * 2) {
		DxLib::DrawStringToHandle(relX + 220, relY + 125, u8"NO DATA", 0xFF0000, mFontHandleMid);
	} else {
		const auto& histData = mHistoryData[mNowPage * 2];
		writeData = std::to_string(histData.betNum);
		numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
		DxLib::DrawStringToHandle(relX + 460 - numWidth, relY - 1, writeData.c_str(), 0xFFFFFF, mFontHandleMid);
		DxLib::DrawStringToHandle(relX + 460, relY + 6, u8"BET", 0xFFFFFF, mFontHandle);
		for (size_t i = 0; i < histData.reelPos.size(); ++i)
			DxLib::DrawRectGraph(relX + 26 + i * 156, relY + 30, i * 154, histData.reelPos[i] * 70, 154, 210, mReelImgID, TRUE);
		DxLib::DrawStringToHandle(relX + 77 + histData.firstStop*156, relY + 240, u8"<1st>", 0xFFFFFF, mFontHandle);
	}

	// 2段目
	writeData = std::to_string(mNowPage * 2 + 1);
	numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 279, relY + 274, writeData.c_str(), 0xFFFFFF, mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 279 + numWidth, relY + 281, u8"G前", 0xFFFFFF, mFontHandle);
	if (mHistoryData.size() <= mNowPage * 2 + 1) {
		DxLib::DrawStringToHandle(relX + 476, relY + 399, u8"NO DATA", 0xFF0000, mFontHandleMid);
	} else {
		const auto& histData = mHistoryData[mNowPage * 2 + 1];
		writeData = std::to_string(histData.betNum);
		numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
		DxLib::DrawStringToHandle(relX + 717 - numWidth, relY + 274, writeData.c_str(), 0xFFFFFF, mFontHandleMid);
		DxLib::DrawStringToHandle(relX + 717, relY + 281, u8"BET", 0xFFFFFF, mFontHandle);
		for (size_t i = 0; i < histData.reelPos.size(); ++i)
			DxLib::DrawRectGraph(relX + 282 + i * 156, relY + 304, i * 154, histData.reelPos[i] * 70, 154, 210, mReelImgID, TRUE);
		DxLib::DrawStringToHandle(relX + 333 + histData.firstStop*156, relY + 516, u8"<1st>", 0xFFFFFF, mFontHandle);
	}

	return true;
}

EMenuList CMenuReelHistory::PushButton(int pKeyHandleDX) {
	switch (pKeyHandleDX)
	{
	case KEY_INPUT_LEFT:
		return EMenuList::eHowTo;
		break;
	case KEY_INPUT_RIGHT:
		return EMenuList::eLicense;
		break;
	case KEY_INPUT_UP:
		mNowPage = mNowPage == 0 ? (mHistoryData.size()-1) / 2 : --mNowPage;
		mNowPage = mHistoryData.size() == 0 ? 0 : mNowPage;
		return EMenuList::eContinue;
		break;
	case KEY_INPUT_DOWN:
		mNowPage = mNowPage >= (mHistoryData.size()-1) / 2 ? 0 : ++mNowPage;
		mNowPage = mHistoryData.size() == 0 ? 0 : mNowPage;
		return EMenuList::eContinue;
		break;
	default:
		break;
	}
	return EMenuList::eContinue;
}

CMenuHowTo::CMenuHowTo(CGameDataManage& pGameData, const int pBaseImgID, const int pTitleFontHandle)
	: IMenuElements(u8"How to play", pBaseImgID, pTitleFontHandle) {
	CGetSysDataFromCSV reader;
	reader.FileInit(pGameData.GetDataHandle(0));

	CMenuReadHowtoFromCSV howto;
	howto.FileInit(pGameData.GetDataHandle(reader.GetSysDataID("howtoList")));
	for (auto i = 0; ; ++i) {
		const int fileHandle = howto.GetImageID(i);
		if (fileHandle == -1) break;
		mImages.push_back(pGameData.GetDataHandle(fileHandle));
	}

	mPageNum = 0;
}

bool CMenuHowTo::Init() {
	return true;
}

bool CMenuHowTo::Process() {
	return true;
}

bool CMenuHowTo::Draw(const int pOpacity) {
	DrawBase(pOpacity);

	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, pOpacity);
	DxLib::DrawGraph(231, 320, mImages[mPageNum], TRUE);
	return true;
}

EMenuList CMenuHowTo::PushButton(int pKeyHandleDX) {
	switch (pKeyHandleDX)
	{
	case KEY_INPUT_LEFT:
		return EMenuList::eLicense;
		break;
	case KEY_INPUT_RIGHT:
		return EMenuList::eReelHistory;
		break;
	case KEY_INPUT_UP:
		if (mPageNum > 0) --mPageNum;
		return EMenuList::eContinue;
		break;
	case KEY_INPUT_DOWN:
		if ((unsigned int)mPageNum < mImages.size() - 1) ++mPageNum;
		return EMenuList::eContinue;
		break;
	default:
		break;
	}
	return EMenuList::eContinue;
}
