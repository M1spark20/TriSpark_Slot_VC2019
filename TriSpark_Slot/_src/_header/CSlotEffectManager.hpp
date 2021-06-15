#pragma once
#include "CEffectVariableManager.hpp"
#include "CSlotSoundManager.hpp"
#include "SSlotEffectData.hpp"
#include <vector>

class CGameDataManage;
struct SSlotGameDataWrapper;
class CRestoreManagerRead;
class CRestoreManagerWrite;

class CSlotEffectManager {
	SSlotEffectData			mEffectData;
	CEffectVariableManager	mVariableManager;
	CSlotSoundManager		mSoundManager;

public:
	bool Init(CGameDataManage& pGameData, int pFileID, CSlotTimerManager& pTimer, CReelManager& pReel);
	bool Process(CSlotTimerManager& pTimer, CSlotInternalDataManager& pInternal, CSlotReachCollectionData& pColle, const SSlotGameDataWrapper& pWrapper, bool pExtendResolution);
	bool Draw(CGameDataManage& pGameData, CSlotTimerManager& pTimer, int pBasicScr);
	bool RingSound(CSlotTimerManager& pTimer, CGameDataManage& pGameData);

	bool ReadRestore(CRestoreManagerRead& pReader);
	bool WriteRestore(CRestoreManagerWrite& pWriter) const;
};
