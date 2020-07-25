#include "_header\CSlotTimerManager.hpp"
#include "DxLib.h"

bool CSlotTimerManager::Init(int pReelNum){
	m_reelNumMax = pReelNum;
	/* initialize timerData */{
		STimerData data;
		data.enable = false; data.originVal = 0; data.lastGetVal = 0;
		m_timerData.resize(eTimerSystemTimerMax + m_reelNumMax*eTimerReelTimerMax, data);
	}
	m_lastCount = DxLib::GetNowCount();
	m_resetCount = 0;

	SetTimer(eTimerGameStart);
	return true;
}

bool CSlotTimerManager::Process(){
	int nowCount = DxLib::GetNowCount();
	if (m_lastCount % ((long long)INT_MAX+1) > nowCount) ++m_resetCount;
	m_lastCount = nowCount + m_resetCount * ((long long)INT_MAX+1);
	return true;
}

bool CSlotTimerManager::SetTimer(ESystemTimerID pID, int offset){
	if (pID == eTimerSystemTimerMax) return false;
	m_timerData[pID].enable		= true;
	m_timerData[pID].originVal	= m_lastCount - offset;
	m_timerData[pID].lastGetVal = m_lastCount - offset;
	return true;
}

bool CSlotTimerManager::SetTimer(EReelTimerID pID, int pReelID, int offset){
	if (pID == eTimerReelTimerMax) return false;
	if (pReelID < 0 || pReelID >= m_reelNumMax) return false;
	const int index = eTimerSystemTimerMax + eTimerReelTimerMax * pReelID + pID;
	m_timerData[index].enable		= true;
	m_timerData[index].originVal	= m_lastCount - offset;
	m_timerData[index].lastGetVal	= m_lastCount - offset;
	return true;
}

bool CSlotTimerManager::DisableTimer(ESystemTimerID pID){
	if (pID == eTimerSystemTimerMax) return false;
	m_timerData[pID].enable		= false;
	m_timerData[pID].originVal	= 0;
	m_timerData[pID].lastGetVal	= 0;
	return true;
}

bool CSlotTimerManager::DisableTimer(EReelTimerID pID, int pReelID){
	if (pID == eTimerReelTimerMax) return false;
	if (pReelID < 0 || pReelID >= m_reelNumMax) return false;
	const int index = eTimerSystemTimerMax + eTimerReelTimerMax * pReelID + pID;
	m_timerData[index].enable		= false;
	m_timerData[index].originVal	= 0;
	m_timerData[index].lastGetVal	= 0;
	return true;
}

bool CSlotTimerManager::GetTime(long long& pInputFor, ESystemTimerID pID) const {
	if (pID == eTimerSystemTimerMax) return false;
	if (!m_timerData[pID].enable) return false;
	pInputFor = m_lastCount - m_timerData[pID].originVal;
	return true;
}

bool CSlotTimerManager::GetTime(long long& pInputFor, EReelTimerID pID, int pReelID) const{
	if (pID == eTimerReelTimerMax) return false;
	if (pReelID < 0 || pReelID >= m_reelNumMax) return false;
	const int index = eTimerSystemTimerMax + eTimerReelTimerMax * pReelID + pID;
	if (!m_timerData[index].originVal) return false;
	pInputFor = m_lastCount - m_timerData[index].originVal;
	return true;
}

bool CSlotTimerManager::GetTimeDiff(long long& pInputFor, ESystemTimerID pID, bool pRefreshFlag){
	if (pID == eTimerSystemTimerMax) return false;
	if (!m_timerData[pID].enable) return false;
	pInputFor = m_lastCount - m_timerData[pID].lastGetVal;
	if (pRefreshFlag) m_timerData[pID].lastGetVal = m_lastCount;
	return true;
}

bool CSlotTimerManager::GetTimeDiff(long long& pInputFor, EReelTimerID pID, int pReelID, bool pRefreshFlag){
	if (pID == eTimerReelTimerMax) return false;
	if (pReelID < 0 || pReelID >= m_reelNumMax) return false;
	const int index = eTimerSystemTimerMax + eTimerReelTimerMax * pReelID + pID;
	if (!m_timerData[index].originVal) return false;
	pInputFor = m_lastCount - m_timerData[index].lastGetVal;
	if (pRefreshFlag) m_timerData[index].lastGetVal = m_lastCount;
	return true;
}