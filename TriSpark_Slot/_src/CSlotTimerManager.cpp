#include "_header\CSlotTimerManager.hpp"
#include "DxLib.h"
#include "_header/ErrClass.hpp"

bool CSlotTimerManager::Init(int pReelNum){
	m_reelNumMax = pReelNum;
	/* initialize timerData */{
		STimerData data;
		data.enable = false; data.originVal = 0; data.lastGetVal = 0;
		m_timerData.resize(eTimerSystemTimerMax + m_reelNumMax*eTimerReelTimerMax, data);
	}

	/* initialize keyData */ {
		mTimerNameList.push_back(std::pair<std::string, int>("general", eTimerGameStart));
		mTimerNameList.push_back(std::pair<std::string, int>("betWait",eTimerBetWaitStart));
		mTimerNameList.push_back(std::pair<std::string, int>("betInput",eTimerBetInput));
		mTimerNameList.push_back(std::pair<std::string, int>("leverAvailable",eTimerLeverAvailable));
		mTimerNameList.push_back(std::pair<std::string, int>("waitStart",eTimerWaitStart));
		mTimerNameList.push_back(std::pair<std::string, int>("waitEnd",eTimerWaitEnd));
		mTimerNameList.push_back(std::pair<std::string, int>("anyReelStart",eTimerAnyReelStart));
		mTimerNameList.push_back(std::pair<std::string, int>("anyReelStop",eTimerAnyReelStopAvailable));
		mTimerNameList.push_back(std::pair<std::string, int>("anyReelPush",eTimerAnyReelPush));
		mTimerNameList.push_back(std::pair<std::string, int>("anyReelStop",eTimerAnyReelStop));
		mTimerNameList.push_back(std::pair<std::string, int>("allReelStart",eTimerAllReelStart));
		mTimerNameList.push_back(std::pair<std::string, int>("allReelStop",eTimerAllReelStop));
		mTimerNameList.push_back(std::pair<std::string, int>("payout",eTimerPayout));

		for (auto i = 0; i < m_reelNumMax; ++i) {
			const int index = eTimerSystemTimerMax + eTimerReelTimerMax * i;
			mTimerNameList.push_back(std::pair<std::string, int>("reelStart[" + std::to_string(i) + "]"			, index + 0));
			mTimerNameList.push_back(std::pair<std::string, int>("reelStopAvailable[" + std::to_string(i) + "]"	, index + 1));
			mTimerNameList.push_back(std::pair<std::string, int>("reelPush[" + std::to_string(i) + "]"			, index + 2));
			mTimerNameList.push_back(std::pair<std::string, int>("reelStop[" + std::to_string(i) + "]"			, index + 3));
		}
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
	/* 各リールの設定 */{
		const int index = eTimerSystemTimerMax + eTimerReelTimerMax * pReelID + pID;
		m_timerData[index].enable = true;
		m_timerData[index].originVal = m_lastCount - offset;
		m_timerData[index].lastGetVal = m_lastCount - offset;
	}
	/* 共通リールの設定 */ {
		const int index = eTimerAnyReelStart + pID;
		m_timerData[index].enable = true;
		m_timerData[index].originVal = m_lastCount - offset;
		m_timerData[index].lastGetVal = m_lastCount - offset;
	}
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

// effect用自由タイマー
int CSlotTimerManager::GetTimerHandle(std::string pID) const{
	if(pID.empty()) throw ErrUndeclaredVar("Time: <Empty>");
	const std::string id = pID[0] == '@' ? pID.substr(1) : pID;
	for (auto it = mTimerNameList.cbegin(); it != mTimerNameList.cend(); ++it) {
		if (it->first == id) return it->second;
	}

	throw ErrUndeclaredVar("Time: " + pID);
	return -1;
}

bool CSlotTimerManager::CreateNewTimer(std::string pID) {
	if (pID.empty()) return false;
	const std::string id = pID[0] == '@' ? pID.substr(1) : pID;
	if (id.empty()) return false;

	// 重複したキーを登録しようとしていたらfalseを返す
	const auto dupSize = mDuplicateFinder.size();
	mDuplicateFinder.insert(id);
	if (dupSize == mDuplicateFinder.size()) return false;

	// 登録作業
	mTimerNameList.push_back(std::pair<std::string, int>(id, m_timerData.size()));
	STimerData data;
	data.enable = false; data.originVal = 0; data.lastGetVal = 0;
	m_timerData.push_back(data);
	
	return true;
}

bool CSlotTimerManager::GetTimeFromTimerHandle(long long& pInputFor, int pHandle) const{
	if (pHandle < 0 || pHandle >= m_timerData.size()) return false;
	if (!m_timerData[pHandle].enable) return false;
	pInputFor = m_lastCount - m_timerData[pHandle].originVal;
	return true;
}