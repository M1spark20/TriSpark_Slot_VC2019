#include "_header/CSlotInternalDataManager.hpp"

void CSlotInternalDataManager::Init(){
	m_data.set					= 0;
	m_data.flag					= std::pair<int, int>(0, 0);
	m_data.gameMode				= 0;
	m_data.gameModeLimit		= std::pair<int, int>(-1, -1);
	m_data.isGameLimit			= std::pair<bool, bool>(false, false);
	m_data.isGetCountOnlyJac	= false;
	m_data.gameModeAtEnd		= std::pair<int, int>(-1, -1);
	m_data.rtMode				= std::pair<int, int>(0, -1);
	m_data.rtModeAtModeEnd		= std::pair<int, int>(-1, -1);
	m_data.isRtOverrideEnable	= true;
	m_data.betNum				= 0;
	m_data.inCount				= 0;
	m_data.outCount				= 0;
	m_data.isReplay				= false;
	m_data.credit				= 0;
	m_data.payout				= 0;
	m_IsCreditResetAdd			= false;
	m_isGameNoCount				= std::pair<bool, bool>(false, false);
}

void CSlotInternalDataManager::SetFlag(int pFlagID, int pBonusID){
	m_data.flag.first = pFlagID;
	if (m_data.flag.second == 0) m_data.flag.second = pBonusID;
}

void CSlotInternalDataManager::ResetBonusFlag(){
	m_data.flag.second = 0;
}

void CSlotInternalDataManager::SetRTMode(int pRTID, bool pCanOverride, bool pIsForceOverride){
	if (!m_data.isRtOverrideEnable & !pIsForceOverride) return;
	m_data.rtMode.first  = pRTID;
	m_data.rtMode.second = -1;
	m_data.isRtOverrideEnable = pCanOverride;
}

void CSlotInternalDataManager::SetRTMode(int pRTID, int pRTGame, bool pCanOverride, bool pIsForceOverride){
	if (!m_data.isRtOverrideEnable & !pIsForceOverride) return;
	m_data.rtMode.first  = pRTID;
	m_data.rtMode.second = pRTGame;
	m_data.isRtOverrideEnable = pCanOverride;
}

void CSlotInternalDataManager::SetRTModeAtEnd(int pRTID, int pRTGame){
	m_data.rtModeAtModeEnd.first = pRTID;
	m_data.rtModeAtModeEnd.second = pRTGame;
}

void CSlotInternalDataManager::SetGameMode(int pNextMode, int pPayLim){
	m_data.gameModeAtEnd.first = 0;
	m_data.isGameLimit.first = false;
	m_data.gameMode = pNextMode;
	m_data.gameModeLimit.first = pPayLim;
	ResetBonusFlag();
}

void CSlotInternalDataManager::SetGameMode(int pNextMode, int pGameLim, int pGetLim){
	if (m_data.gameModeLimit.first >= 0){
		m_data.isGameLimit.second = true;
		m_data.gameModeAtEnd.second = m_data.gameMode;
		m_data.gameMode = pNextMode;
		m_data.gameModeLimit.second = 100 * pGetLim + pGameLim;
		m_isGameNoCount.second = true;
	}
	else {
		m_data.isGameLimit.first = true;
		m_data.gameModeAtEnd.first = 0;
		m_data.gameMode = pNextMode;
		if (pGetLim < 0){
			m_data.isGetCountOnlyJac = true;
			m_data.gameModeLimit.first = 100 * -pGetLim + pGameLim;
		} else {
			m_data.isGetCountOnlyJac = false;
			m_data.gameModeLimit.first = 100 * pGetLim + pGameLim;
		}
		m_isGameNoCount.first = true;
	}
	ResetBonusFlag();
}

void CSlotInternalDataManager::ChangeCreditVal(int margin){
	int ans = m_data.credit + margin;
	if (ans <=  0) m_data.credit =  0;
	else if (ans >= 50) m_data.credit = 50;
	else m_data.credit = ans;
}

void CSlotInternalDataManager::AddBetCount(bool pIsReplay){
	++m_data.betNum;
	m_IsCreditResetAdd = !(m_data.credit <= 0);
	if(!pIsReplay) ChangeCreditVal(-1);
}

void CSlotInternalDataManager::ResetBetCount(bool pIsReBet){
	if (m_IsCreditResetAdd && pIsReBet) ChangeCreditVal(m_data.betNum);
	m_data.betNum = 0;
}

void CSlotInternalDataManager::LatchBet(){
	m_data.inCount += m_data.betNum;
	m_data.payout = 0;
}

void CSlotInternalDataManager::AddPayout(){
	if (m_data.isReplay) return;
		ChangeCreditVal(1);
		++m_data.outCount;
		++m_data.payout;
		if (m_data.gameModeLimit.first > 0 && !m_data.isGameLimit.first)
			--m_data.gameModeLimit.first;
}

void CSlotInternalDataManager::SetReplayStatus(bool pRepFlag){
	m_data.isReplay = pRepFlag;
	if(pRepFlag) m_data.outCount += m_data.betNum;
}

void CSlotInternalDataManager::CheckGameModeEnd(bool pGameCount, bool pGetCount){
	bool is2ndModeEnd = false, is1stModeEnd = false;
	if (m_data.gameModeLimit.second >= 0 && m_data.isGameLimit.second){
		if (!m_isGameNoCount.second){
			if (pGameCount) m_data.gameModeLimit.second -= 1;
			if (pGetCount) m_data.gameModeLimit.second -= 100;
		}
		if (m_data.gameModeLimit.second < 100 || m_data.gameModeLimit.second % 100 == 0){
			// secondに指定されたモードの終了
			is2ndModeEnd = true;
			m_data.gameMode = m_data.gameModeAtEnd.second;
			m_data.gameModeAtEnd.second = -1;
			m_data.gameModeLimit.second = -1;
			m_data.isGameLimit.second = false;
		}
	}

	if (m_data.gameModeLimit.first >= 0){
		if (m_data.isGameLimit.first && m_data.gameModeLimit.second == -1){
			if (!m_isGameNoCount.first){
				if (pGameCount) m_data.gameModeLimit.first -= 1;
				// 通常の入賞減算
				if (pGetCount && !m_data.isGetCountOnlyJac) m_data.gameModeLimit.first -= 100;
				// JAC終了による減算
				if (is2ndModeEnd && m_data.isGetCountOnlyJac) m_data.gameModeLimit.first -= 100;
			}
			is1stModeEnd = (m_data.gameModeLimit.first < 100 || m_data.gameModeLimit.first % 100 == 0);
		} else if (!m_data.isGameLimit.first){
			is1stModeEnd = m_data.gameModeLimit.first == 0;
		}
	}
	if (is1stModeEnd){
		// 1st,2ndともモードを強制終了させる
		m_data.gameMode = m_data.gameModeAtEnd.first;
		m_data.gameModeAtEnd = std::pair<int, int>(-1, -1);
		m_data.gameModeLimit = std::pair<int, int>(-1, -1);
		m_data.isGameLimit = std::pair<bool, bool>(false, false);
		m_data.isGetCountOnlyJac = false;
	}
	if(pGameCount) m_isGameNoCount = std::pair<bool, bool>(false, false);
}
