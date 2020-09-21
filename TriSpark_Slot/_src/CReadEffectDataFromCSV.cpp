#include "_header/CReadEffectDataFromCSV.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CReelManager.hpp"
#include "DxLib.h"

bool CReadEffectDataFromCSV::FileInit(int pFileID) {
	// [act]DxLib���ŊJ�����t�@�C������f�[�^��ǂݏo��
	// [ret]�t�@�C���I�[�v���ɐ����������ǂ���
	while (!DxLib::FileRead_eof(pFileID)) {
		TCHAR str[1024];
		DxLib::FileRead_gets(str, 1024, pFileID);
		m_ReadDataAll.append(str).append("\n");
	}
	return !m_ReadDataAll.empty();
}

void CReadEffectDataFromCSV::PushImgData(SSlotEffectData& pData, std::unique_ptr<IImageSourceManager>& sourcePtr, std::unique_ptr<CImageColorManager>& colorPtr, std::unique_ptr<IImageDestinationManager>& destPtr) {
	pData.imgData.push_back(std::pair<int, SImageEffectData>());
	auto pushData = pData.imgData.end() - 1;
	pushData->first = mOrderCounter++;
	pushData->second.pSource.swap(sourcePtr);
	pushData->second.pColor.swap(colorPtr);
	pushData->second.pDest.swap(destPtr);
	sourcePtr.reset(nullptr);
	colorPtr.reset(nullptr);
	destPtr.reset(nullptr);
}

bool CReadEffectDataFromCSV::MakeData(SSlotEffectData& pData, CEffectVariableManager& pVar, CSlotTimerManager& pTimer, CReelManager& pReel) {
	typedef unsigned int UINT;
	std::unique_ptr<IImageSourceManager> sourcePtr(nullptr);
	std::unique_ptr<CImageColorManager> colorPtr(nullptr);
	std::unique_ptr<IImageDestinationManager> destPtr(nullptr);

	while (!DataEOF()) {
		StringArr NowGetStr;
		GetStrSplitByComma(NowGetStr);
		if (NowGetStr.at(0)[0] == ';') continue;
		if (NowGetStr.at(0) == "#imgSrc") {
			if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eImgSrc) return false;
			if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, colorPtr, destPtr);
			if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceDefault(pVar));

			mReadStatus = EReadStatus::eSource;
			mHeading = ENowReadingHead::eImgSrc;
			if(!sourcePtr->Init(NowGetStr, pTimer)) return false;
		}
		if (NowGetStr.at(0) == "#numSrc") {
			if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eNumSrc) return false;
			if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, colorPtr, destPtr);
			if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceNumber(pVar));

			mReadStatus = EReadStatus::eSource;
			mHeading = ENowReadingHead::eNumSrc;
			if (!sourcePtr->Init(NowGetStr, pTimer)) return false;
		}
		if (NowGetStr.at(0) == "#reelSrc") {
			if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eReelSrc) return false;
			if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, colorPtr, destPtr);
			if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceReel(pVar));

			mReadStatus = EReadStatus::eSource;
			mHeading = ENowReadingHead::eReelSrc;
			if(!sourcePtr->Init(NowGetStr, pTimer)) return false;
		}
		if (NowGetStr.at(0) == "#imgColorMap") {
			if (mReadStatus == EReadStatus::eDestination) return false;
			if (colorPtr == nullptr) colorPtr.reset(new CImageColorManager(pVar));

			//mReadStatus = EReadStatus::eColorMap;
			//mHeading = ENowReadingHead::eImgCM;
			if(!colorPtr->Init(NowGetStr, pTimer)) return false;
		}
		if (NowGetStr.at(0) == "#imgDst") {
			if (mReadStatus == EReadStatus::eDestination && mHeading != ENowReadingHead::eImgDst) return false;
			if (mReadStatus == EReadStatus::eSource) {
				if (mHeading != ENowReadingHead::eImgSrc && mHeading != ENowReadingHead::eNumSrc) return false;
			}
			if (destPtr == nullptr) destPtr.reset(new CImageDestinationDefault(pVar));

			mReadStatus = EReadStatus::eDestination;
			mHeading = ENowReadingHead::eImgDst;
			if(!destPtr->Init(NowGetStr, pTimer)) return false;
		}
		if (NowGetStr.at(0) == "#reelDst") {
			if (mReadStatus == EReadStatus::eDestination && mHeading != ENowReadingHead::eReelDst) return false;
			if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eReelSrc) return false;
			if (destPtr == nullptr) destPtr.reset(new CImageDestinationReel(pVar, pReel));

			mReadStatus = EReadStatus::eDestination;
			mHeading = ENowReadingHead::eReelDst;
			if(!destPtr->Init(NowGetStr, pTimer)) return false;
		}
		if (NowGetStr.at(0) == "#makeScr") {
			pVar.MakeScreenID(NowGetStr);
		}
		if (NowGetStr.at(0) == "#clearScr") {
			pData.clearScreenData.push_back(std::pair<int, std::string>(mOrderCounter++, NowGetStr.at(1)));
		}
	}

	if(sourcePtr != nullptr && destPtr != nullptr) PushImgData(pData, sourcePtr, colorPtr, destPtr);
	return true;
}