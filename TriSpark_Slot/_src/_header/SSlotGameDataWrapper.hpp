#pragma once
#include "CReelManager.hpp"
#include "CRandManager.hpp"
#include "CSlotEffectManager.hpp"
#include "CSlotTimerManager.hpp"
#include "CSlotInternalDataManager.hpp"
#include "CSlotCastChecker.hpp"
#include "CSlotDataCounter.hpp"

struct SSlotGameDataWrapper{
	CReelManager				reelManager;
	CRandManager				randManager;
	CSlotEffectManager			effectManager;
	CSlotTimerManager			timeManager;
	CSlotCastChecker			castChecker;
	CSlotInternalDataManager	internalDataManager;
	CSlotDataCounter			dataCounter;
	//CSlotExternalDataManager	m_externalDataManager;
	//CSlotStopHistoryManager		m_reelHistory;
};