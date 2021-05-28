#include "_header/CSlotReachCollection.hpp"
#include "_header/CReadReachCollectionFromCSV.hpp"
#include "_header/CGameDataManage.h"
#include "_header/CRestoreManager.hpp"
#include "_header/CReelManager.hpp"
#include "_header/CSlotInternalDataManager.hpp"
#include <iomanip>
#include <sstream>
#include "DxLib.h"

bool CSlotReachCollectionData::Init(const CGameDataManage& pDataManager, int pListFileID, int reelNum) {
	CReadReachCollectionFromCSV reader;
	if (!reader.FileInit(pDataManager.GetDataHandle(pListFileID))) return false;
	if (!reader.MakeData(mCollectionData)) return false;
	if (mCollectionData.reelNum != reelNum) return false;

	mListImageID = pDataManager.GetDataHandle(mCollectionData.listImgID);
	mSecretImageID = pDataManager.GetDataHandle(mCollectionData.secretImgID);
	if (mListImageID == -1 || mSecretImageID == -1) return false;
	return true;
}

bool CSlotReachCollectionData::JudgeCollection(const CReelManager& pReelData, const CSlotInternalDataManager& pInternal) {
	const auto inData = pInternal.GetData();
	const bool isHazureFlag = 
		std::find(mCollectionData.hazureNGFlags.begin(), mCollectionData.hazureNGFlags.end(), inData.flag.first) != mCollectionData.hazureNGFlags.end();
	for (auto& data : mCollectionData.elem) {
		if (data.isOnlyHazure && isHazureFlag) continue;
		if (data.betNum != inData.betNum) continue;
		if (data.gameMode != inData.gameMode) continue;
		if (pReelData.GetReelNum() != (int)data.reelElement.size()) return false;
		bool isComplete = true;
		for (int i = 0; i < pReelData.GetReelNum(); ++i) {
			const int comaData = data.reelElement[i];
			const auto histData = pReelData.GetHistoryData();
			if (data.isLeft1st && histData.firstStop != 0) { isComplete = false; break; }
			if (comaData == 0x40000000) {	// rotating
				if (histData.reelPos[i] != -1) { isComplete = false; break; }
			} else if (comaData & 0x80000000 || comaData == 0x0) {	// 位置指定
				if (histData.reelPos[i] != abs(comaData)) { isComplete = false; break; }
			} else {	// 各位置コマ判定
				int checkVal = 0;
				for (int comaPos = 0; comaPos < 3; ++comaPos) {
					if (histData.reelPos[i] == -1) { isComplete = false; break; }
					const int comaID =
						pReelData.GetCharaIDByFixedPos(i, (histData.reelPos[i] + comaPos) % pReelData.GetCharaNum());
					checkVal = checkVal | (1 << (10 * (2 - comaPos) + comaID));
				}
				if ((comaData & checkVal) != checkVal || !isComplete) { isComplete = false; break; }
			}
		}
		if (!isComplete) continue;
		mLatch.push_back(&data);
	}
	return true;
}

bool CSlotReachCollectionData::Latch(bool isRefresh) {
	if (isRefresh) {
		for (auto& data : mLatch) {
			// 達成時記録更新
			std::string compDate;
			/* 時間文字列取得 */ {
				time_t t = time(nullptr);
				tm lt; localtime_s(&lt, &t);
				std::stringstream s;
				s << std::setw(2) << std::setfill('0') << (lt.tm_year - 100) % 100; s << u8"-";
				s << std::setw(2) << std::setfill('0') << lt.tm_mon + 1; s << u8"-";
				s << std::setw(2) << std::setfill('0') << lt.tm_mday; s << u8" ";
				s << std::setw(2) << std::setfill('0') << lt.tm_hour; s << u8":";
				s << std::setw(2) << std::setfill('0') << lt.tm_min;
				compDate = s.str();
			}
			data->compCount = data->compCount + 1;
			if (data->compCount == 1) data->firstComp = compDate;
			data->lastComp = compDate;
		}
	}
	mLatch.clear();
	return true;
}

int CSlotReachCollectionData::GetCompCount(int pIndex) const {
	if (pIndex < 0 || pIndex >= GetCollectionNum()) return -1;
	return mCollectionData.elem[pIndex].compCount;
}

std::string CSlotReachCollectionData::GetFirstCompDate(int pIndex) const {
	if (pIndex < 0 || pIndex >= GetCollectionNum()) return "";
	return mCollectionData.elem[pIndex].firstComp;
}

std::string CSlotReachCollectionData::GetLastCompDate(int pIndex) const {
	if (pIndex < 0 || pIndex >= GetCollectionNum()) return "";
	return mCollectionData.elem[pIndex].lastComp;

}

bool CSlotReachCollectionData::ReadRestore(CRestoreManagerRead& pReader) {
	size_t dataCount;
	if (!pReader.ReadNum(dataCount)) return false;
	if (dataCount != mCollectionData.elem.size()) return false;
	for (size_t i = 0; i < dataCount; ++i) {
		auto& data = mCollectionData.elem[i];
		if (!pReader.ReadNum(data.compCount)) return false;
		if (!pReader.ReadStr(data.firstComp)) return false;
		if (!pReader.ReadStr(data.lastComp))  return false;
	}
	return true;
}

bool CSlotReachCollectionData::WriteRestore(CRestoreManagerWrite& pWriter) const {
	if (!pWriter.WriteNum((size_t)mCollectionData.elem.size())) return false;
	for (size_t i = 0; i < mCollectionData.elem.size(); ++i) {
		const auto& data = mCollectionData.elem[i];
		if (!pWriter.WriteNum(data.compCount)) return false;
		if (!pWriter.WriteStr(data.firstComp)) return false;
		if (!pWriter.WriteStr(data.lastComp))  return false;
	}
	return true;
}

bool CSlotReachCollectionData::Draw(int pBeginPos, int pOffsetX, int pOffsetY, int pFontHandle) const {
	const int w = 240, h = 180;
	for (int pos = 0; pos < 12; ++pos) {
		const int index = pos + pBeginPos;
		if (index < 0 || index >= GetCollectionNum()) break;
		std::string str = u8"No." + std::to_string(index+1);
		DxLib::DrawStringToHandle(pOffsetX + 12 + w * (pos % 4), pOffsetY + 109 + h * (pos / 4),
			str.c_str(), 0xFFFFFF, pFontHandle);

		const auto& data = mCollectionData.elem[index];
		if (data.compCount > 0) {
			str = u8"達成: " + std::to_string(data.compCount) + u8"回";
			int width = DxLib::GetDrawStringWidthToHandle(str.c_str(), str.length(), pFontHandle);
			DxLib::DrawStringToHandle(pOffsetX + 227 - width + w * (pos % 4), pOffsetY + 109 + h * (pos / 4),
				str.c_str(), 0xFFFFFF, pFontHandle);

			str = u8"初回: " + data.firstComp;
			width = DxLib::GetDrawStringWidthToHandle(str.c_str(), str.length(), pFontHandle);
			DxLib::DrawStringToHandle(pOffsetX + 120 + w * (pos % 4) - width / 2, pOffsetY + 131 + h * (pos / 4),
				str.c_str(), 0xFFFFFF, pFontHandle);

			str = u8"最近: " + data.lastComp;
			width = DxLib::GetDrawStringWidthToHandle(str.c_str(), str.length(), pFontHandle);
			DxLib::DrawStringToHandle(pOffsetX + 120 + w * (pos % 4) - width / 2, pOffsetY + 153 + h * (pos / 4),
				str.c_str(), 0xFFFFFF, pFontHandle);

			DxLib::DrawRectGraph(pOffsetX + (w - data.imgW) / 2 + w * (pos % 4), pOffsetY + h * (pos / 4),
				data.imgX, data.imgY, data.imgW, data.imgH, mListImageID, TRUE);
		} else {
			str = u8"未達成";
			int width = DxLib::GetDrawStringWidthToHandle(str.c_str(), str.length(), pFontHandle);
			DxLib::DrawStringToHandle(pOffsetX + 227 - width + w * (pos % 4), pOffsetY + 109 + h * (pos / 4),
				str.c_str(), 0xFF0000, pFontHandle);

			bool isDark = (index-1 < 0 || index-1 >= GetCollectionNum());
			if (!isDark) isDark = mCollectionData.elem[index - 1].compCount > 0;
			if (isDark) {
				DxLib::SetDrawBright(128, 128, 128);
				DxLib::DrawRectGraph(pOffsetX + (w - data.imgW) / 2 + w * (pos % 4), pOffsetY  + h * (pos / 4),
					data.imgX, data.imgY, data.imgW, data.imgH, mListImageID, TRUE);
				DxLib::SetDrawBright(255, 255, 255);
			} else {
				const auto& sec = mCollectionData.secretImgPos;
				DxLib::DrawRectGraph(pOffsetX + (w - sec.imgW) / 2 + w * (pos % 4), pOffsetY  + h * (pos / 4),
					sec.imgX, sec.imgY, sec.imgW, sec.imgH, mSecretImageID, TRUE);
			}
		}
	}
	return true;
}