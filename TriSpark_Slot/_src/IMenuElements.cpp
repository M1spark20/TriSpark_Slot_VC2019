#include "_header/IMenuElements.hpp"
#include "_header/CGameDataManage.h"
#include "_header/CGetSysDataFromCSV.hpp"
#include "_header/CMenuReadHowtoFromCSV.hpp"
#include "_header/CReelManager.hpp"
#include "_header/CSlotDataCounter.hpp"
#include "DxLib.h"
#include <string>
#include <cstring>
#include <algorithm>

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
		return EMenuList::eBonusHistory;
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
		DxLib::DrawStringToHandle(relX + 198, relY + 119, u8"NO DATA", 0xFF0000, mFontHandleMid);
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
		DxLib::DrawStringToHandle(relX + 454, relY + 393, u8"NO DATA", 0xFF0000, mFontHandleMid);
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
		return EMenuList::eBonusHistory;
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


CMenuBonusHistory::CMenuBonusHistory(CGameDataManage& pGameData, const int pDataFontHandle, const int pDataFontHandleMid, const int pBaseImgID, const CSlotDataCounter& pSlotData, const int pTitleFontHandle)
		: IMenuElements(u8"Bonus History", pBaseImgID, pTitleFontHandle), mFontHandle(pDataFontHandle), mFontHandleMid(pDataFontHandleMid), mHistBaseImgID(pBaseImgID) {
	CGetSysDataFromCSV reader;
	reader.FileInit(pGameData.GetDataHandle(0));

	mHistBaseImgID = pGameData.GetDataHandle(reader.GetSysDataID("BonusHistoryBase"));
	mReelImgID = pGameData.GetDataHandle(reader.GetSysDataID("ReelForMenu"));

	CMenuReadBonusTypeFromCSV csv;
	csv.FileInit(pGameData.GetDataHandle(reader.GetSysDataID("MenuBonusType")));
	csv.GetImagePos(mTypeData);
	// resID付け替え
	for (auto& data : mTypeData) data.resID = pGameData.GetDataHandle(data.resID);

	mHistoryData = pSlotData.GetBonusHistory();
	mGraphData = pSlotData.GetCoinGraphData();

	mSelecting = mHistoryData.size() - 1;
	mGraphRange = 500;
	if (mHistoryData.empty()) return;
	if (!mHistoryData.rbegin()->isActivate) --mSelecting;
}

bool CMenuBonusHistory::Init() {
	if (!mGraphData.empty()) {
		mGraphDrawRate = ((float)mGraphData.size() - 1.f) / (GRAPH_WIDTH - 1);
		if (mGraphDrawRate < 1.f) mGraphDrawRate = 1.f;
	} else {
		mGraphDrawRate = 1.f;
	}

	if (mGraphData.empty()) return true;
	int graphMax = *std::max_element(mGraphData.begin(), mGraphData.end());
	graphMax = abs(graphMax < 0 ? 0 : graphMax) / 500 + 2;
	int graphMin = *std::min_element(mGraphData.begin(), mGraphData.end());
	graphMin = abs(graphMin > 0 ? 0 : graphMin) / 500 + 1;
	mGraphRange = (graphMax / 2 > graphMin ? graphMax / 2 : graphMin) * 500;
	return true;
}

bool CMenuBonusHistory::Process() {
	return true;
}

bool CMenuBonusHistory::Draw(const int pOpacity) {
	DrawBase(pOpacity);

	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, pOpacity);
	DxLib::DrawGraph(231, 320, mHistBaseImgID, TRUE);
	const int relX = 231, relY = 320;

	/* グラフ描画 */ {
		const int graphBegX = relX + 541, graphBegY = relY + 458, rangeHeight = 80;
		if (!mGraphData.empty()) {
			int y1 = 0, y2 = 0;
			for (int xPos = 0; xPos < GRAPH_WIDTH; ++xPos) {
				const int index = std::floorf(mGraphDrawRate * xPos);
				if (index >= (int)mGraphData.size()) break;
				y2 = (float)mGraphData[index] / mGraphRange * rangeHeight * -1;	// yは下向き正のため-1倍
				DxLib::DrawLine(graphBegX + xPos, graphBegY + y1, graphBegX + xPos + 1, graphBegY + y2, 0xFFFFFF, 2);
				y1 = y2;
			}
		}
		for (int i = 2; i >= -1; --i) {
			std::string num = std::to_string(mGraphRange * i);
			int len = DxLib::GetDrawStringWidthToHandle(num.c_str(), num.length(), mFontHandle);
			DxLib::DrawStringToHandle(graphBegX - 5 - len, graphBegY - 11 - i * rangeHeight, num.c_str(), 0xB0B0B0, mFontHandle);
		}
	}

	// ボーナス履歴描画
	if (mHistoryData.empty()) {
		std::string writeData = u8"NO DATA";
		int numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
		DxLib::DrawStringToHandle(relX + 217 - numWidth / 2, relY +  50, writeData.c_str(), 0xFF0000, mFontHandleMid);
		DxLib::DrawStringToHandle(relX + 695 - numWidth / 2, relY + 137, writeData.c_str(), 0xFF0000, mFontHandleMid);
		return true;
	}

	const int maxSize = mHistoryData.rbegin()->isActivate ? mHistoryData.size() : mHistoryData.size() - 1;
	int beginPos = mSelecting + 4;
	if (beginPos >= maxSize) beginPos = maxSize - 1;
	int endPos = beginPos - 9;
	if (endPos < 0) {
		endPos = 0;	beginPos = endPos + 9;
		if (beginPos >= maxSize) beginPos = maxSize - 1;
	}

	const int begX = relX, begY = relY + 50;
	for (int i = beginPos, y = 0; i >= endPos; --i, ++y) {
		std::string writeData = std::to_string(i + 1);
		const int yPos = begY + 50 * y;
		int numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
		DxLib::DrawStringToHandle(begX + 57 - numWidth / 2, yPos, writeData.c_str(), 0xFFFFFF, mFontHandleMid);

		const auto& data = mHistoryData[i];
		writeData = std::to_string(data.startGame);
		numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
		DxLib::DrawStringToHandle(begX + 177 - numWidth / 2, yPos, writeData.c_str(), 0xFFFFFF, mFontHandleMid);

		const auto& nowType = GetBonusType(data.getPayoutEffect);
		if (nowType.resID == -1) return false;
		DxLib::DrawRectGraph(begX + 284 - nowType.w / 2, yPos + 16 - nowType.h / 2,
			nowType.x, nowType.y, nowType.w, nowType.h, nowType.resID, TRUE);

		if (data.isSetGet) {
			writeData = std::to_string(data.medalAfter - data.medalBefore);
			numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
			DxLib::DrawStringToHandle(begX + 374 - numWidth / 2, yPos, writeData.c_str(), 0xFFFFFF, mFontHandleMid);
		}

		if (i == mSelecting) DxLib::DrawLine(begX, yPos + 34, begX + 408, yPos + 34, 0xFFFF00, 4);
	}

	// 成立時出目描画
	const int index = mSelecting;
	const auto& pattern = mHistoryData[index].flagMadeGameReel;
	std::string writeData = std::to_string(mHistoryData[index].flagLossGame);
	int numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 460, relY + 25, writeData.c_str(), 0xFFFFFF, mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 460 + numWidth, relY + 32, u8"G前", 0xFFFFFF, mFontHandle);

	writeData = std::to_string(pattern.betNum);
	numWidth = DxLib::GetDrawStringWidthToHandle(writeData.c_str(), writeData.length(), mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 898 - numWidth, relY + 25, writeData.c_str(), 0xFFFFFF, mFontHandleMid);
	DxLib::DrawStringToHandle(relX + 898, relY + 32, u8"BET", 0xFFFFFF, mFontHandle);
	for (size_t i = 0; i < pattern.reelPos.size(); ++i)
		DxLib::DrawRectGraph(relX + 463 + i * 156, relY + 55, i * 154, pattern.reelPos[i] * 70, 154, 210, mReelImgID, TRUE);
	DxLib::DrawStringToHandle(relX + 514 + pattern.firstStop * 156, relY + 267, u8"<1st>", 0xFFFFFF, mFontHandle);

	return true;
}

EMenuList CMenuBonusHistory::PushButton(int pKeyHandleDX) {
	switch (pKeyHandleDX)
	{
	case KEY_INPUT_LEFT:
		return EMenuList::eReelHistory;
		break;
	case KEY_INPUT_RIGHT:
		return EMenuList::eLicense;
		break;
	default:
		break;
	}

	if (mSelecting < 0) return EMenuList::eContinue;

	switch (pKeyHandleDX)
	{
	case KEY_INPUT_DOWN:
		mSelecting = mSelecting == 0 ? mHistoryData.size() - 1 : --mSelecting;
		mSelecting = mHistoryData[mSelecting].isActivate ? mSelecting : mSelecting - 1;
		return EMenuList::eContinue;
		break;
	case KEY_INPUT_UP:
		mSelecting = mSelecting + 1 >= mHistoryData.size() ? 0 : ++mSelecting;
		mSelecting = mHistoryData[mSelecting].isActivate ? mSelecting : 0;
		return EMenuList::eContinue;
		break;
	default:
		break;
	}
	return EMenuList::eContinue;
}

SMenuReadBonusTypePos CMenuBonusHistory::GetBonusType(int bonusType) {
	for (const auto& data : mTypeData) {
		if (data.payoutID == bonusType) return data;
	}
	return SMenuReadBonusTypePos();
}