#pragma once
#include "CEffectVariableManager.hpp"
#include "SSlotEffectData.hpp"
#include <vector>

class CGameDataManage;
class CSlotInternalDataManager;
class CSlotTimerManager;
class CReelManager;
class CSlotInternalDataManager;

class CSlotEffectManager {
	SSlotEffectData			mEffectData;
	CEffectVariableManager	mVariableManager;

public:
	bool Init(CGameDataManage& pGameData, int pFileID, CSlotTimerManager& pTimer, CReelManager& pReel);
	bool Process(CSlotTimerManager& pTimer, CSlotInternalDataManager& pInternal);
	bool Draw(CGameDataManage& pGameData);
};