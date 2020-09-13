#pragma once
#include "IReadcsv.h"
#include "SSlotEffectData.hpp"
#include <vector>

class CEffectVariableManager;
class CSlotTimerManager;
class CReelManager;

class CReadEffectDataFromCSV : IReadCSVBase {
	enum class EReadStatus {
		eInitial, eSource, eColorMap, eDestination
	} mReadStatus;
	enum class ENowReadingHead {
		eNone, eImgSrc, eNumSrc, eReelSrc, eImgCM, eReelCM, eImgDst, eReelDst
	} mHeading;
	int mOrderCounter;

	void PushImgData(SSlotEffectData& pData, std::unique_ptr<IImageSourceManager>& sourcePtr, std::unique_ptr<IImageDestinationManager>& destPtr);

public:
	CReadEffectDataFromCSV() {
		mReadStatus = EReadStatus::eInitial;
		mHeading = ENowReadingHead::eNone;
		mOrderCounter = 0;
	}
	bool FileInit(int pFileID);
	bool MakeData(SSlotEffectData& pData, CEffectVariableManager& pVar, CSlotTimerManager& pTimer, CReelManager& pReel);
};