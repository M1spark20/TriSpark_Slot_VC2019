#pragma once
#include "CReelManager.hpp"
#include "CRandManager.hpp"
#include "CSlotEffectManager.hpp"
#include "CSlotTimerManager.hpp"
#include "CSlotInternalDataManager.hpp"
#include "CSlotCastChecker.hpp"
#include "CSlotDataCounter.hpp"
#include "CRestoreManager.hpp"
#include "CSlotReachCollection.hpp"

struct SSlotGameDataWrapper{
	CReelManager				reelManager;
	CRandManager				randManager;
	CSlotEffectManager			effectManager;
	CSlotTimerManager			timeManager;
	CSlotCastChecker			castChecker;
	CSlotInternalDataManager	internalDataManager;
	CSlotDataCounter			dataCounter;
	CRestoreManagerWrite		restoreManager;
	CSlotReachCollectionData	reachCollection;
};
