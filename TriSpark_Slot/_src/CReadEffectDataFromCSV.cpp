#include "_header/CReadEffectDataFromCSV.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CReelManager.hpp"
#include "DxLib.h"

bool CReadEffectDataFromCSV::FileInit(int pFileID) {
	// [act]DxLib側で開いたファイルからデータを読み出す
	// [ret]ファイルオープンに成功したかどうか
	while (!DxLib::FileRead_eof(pFileID)) {
		TCHAR str[1024];
		DxLib::FileRead_gets(str, 1024, pFileID);
		m_ReadDataAll.append(str).append("\n");
	}
	return !m_ReadDataAll.empty();
}

void CReadEffectDataFromCSV::PushImgData(SSlotEffectData& pData, std::unique_ptr<IImageSourceManager>& sourcePtr, std::unique_ptr<IImageDestinationManager>& destPtr) {
	pData.imgData.push_back(std::pair<int, SImageEffectData>());
	auto pushData = pData.imgData.end() - 1;
	pushData->first = mOrderCounter++;
	pushData->second.pSource.swap(sourcePtr);
	pushData->second.pDest.swap(destPtr);
	sourcePtr.reset(nullptr);
	destPtr.reset(nullptr);
}

bool CReadEffectDataFromCSV::MakeData(SSlotEffectData& pData, CEffectVariableManager& pVar, CSlotTimerManager& pTimer, CReelManager& pReel) {
	typedef unsigned int UINT;
	std::unique_ptr<IImageSourceManager> sourcePtr(nullptr);
	std::unique_ptr<IImageDestinationManager> destPtr(nullptr);

	while (!DataEOF()) {
		StringArr NowGetStr;
		GetStrSplitByComma(NowGetStr);
		if (NowGetStr.at(0)[0] == ';') continue;
		if (NowGetStr.at(0) == "#imgSrc") {
			if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eImgSrc) return false;
			if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);
			if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceDefault(pVar));

			mReadStatus = EReadStatus::eSource;
			mHeading = ENowReadingHead::eImgSrc;
			sourcePtr->Init(NowGetStr, pTimer);
		}
		if (NowGetStr.at(0) == "#numSrc") {
			if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eNumSrc) return false;
			if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);
			if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceNumber(pVar));

			mReadStatus = EReadStatus::eSource;
			mHeading = ENowReadingHead::eNumSrc;
			sourcePtr->Init(NowGetStr, pTimer);
		}
		if (NowGetStr.at(0) == "#imgDst") {
			if (mReadStatus == EReadStatus::eDestination && mHeading != ENowReadingHead::eImgDst) return false;
			if (mReadStatus == EReadStatus::eSource) {
				if (mHeading != ENowReadingHead::eImgSrc && mHeading != ENowReadingHead::eNumSrc) return false;
			}
			if (destPtr == nullptr) destPtr.reset(new CImageDestinationDefault(pVar));

			mReadStatus = EReadStatus::eDestination;
			mHeading = ENowReadingHead::eImgDst;
			destPtr->Init(NowGetStr, pTimer);
		}
	}

	if(sourcePtr != nullptr && destPtr != nullptr) PushImgData(pData, sourcePtr, destPtr);
	return true;
}