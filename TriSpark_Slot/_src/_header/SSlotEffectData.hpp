#pragma once
#include <memory>
#include <algorithm>
#include <vector>
#include "ImageSourceManager.hpp"
#include "ImageDestinationManager.hpp"
#include "CImageColorController.hpp"
#include "CSlotEffectVariableCompare.hpp"
#include "CEffectVariableManager.hpp"
#include "CSlotSoundManager.hpp"
#include "CSlotTimerManager.hpp"

struct SImageEffectData {
	std::unique_ptr<IImageSourceManager> pSource;
	std::unique_ptr<IImageDestinationManager> pDest;
};

struct SSlotEffectData {
	std::vector<CSlotEffectVariableCompare>					conditionData;
	CImageColorController									colorController;

	std::vector<std::pair<int, SImageEffectData>>			imgData;
	std::vector<std::pair<int, std::string>>				clearScreenData;	// [order, screenName]
	std::vector<std::pair<int, SImageColorApplyData>>		colorApplyData;
	std::vector<std::pair<int, SEffectVariableInsertData>>	varInsertData;
	std::vector<std::pair<int, SEffectVariableRoleData>>	varRoleData;
	std::vector<std::pair<int, SSlotSoundRingData>>			soundRingData;
	std::vector<std::pair<int, SSlotSoundVolumeData>>		soundVolData;
	std::vector<std::pair<int, SSlotSoundStopData>>			soundStopData;
	std::vector<std::pair<int, SSlotTimerActionData>>		timerActionData;
	std::vector<std::pair<int, SSlotTimerStopData>>			timerStopData;
};
