#pragma once
#include "CEffectVariableManager.hpp"
#include "CSlotSoundManager.hpp"
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
	CSlotSoundManager		mSoundManager;

public:
	bool Init(CGameDataManage& pGameData, int pFileID, CSlotTimerManager& pTimer, CReelManager& pReel);
	bool Process(CSlotTimerManager& pTimer, CSlotInternalDataManager& pInternal);
	bool Draw(CGameDataManage& pGameData, CSlotTimerManager& pTimer);
	bool RingSound(CSlotTimerManager& pTimer, CGameDataManage& pGameData);
};