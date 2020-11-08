#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include <set>

enum ESystemTimerID{
	eTimerGameStart,
	eTimerBetWaitStart,
	eTimerBetInput,
	eTimerLeverAvailable,
	eTimerWaitStart,
	eTimerWaitEnd,
	// �e���[���̉���,��~�^�C�}�͕ʒ�` ���L��`�͂����ꂩ�̃��[�����������Ƃ��Ɏ����K�p
	eTimerAnyReelStart,
	eTimerAnyReelStopAvailable,
	eTimerAnyReelPush,
	eTimerAnyReelStop,

	eTimerAllReelStart,
	eTimerAllReelStop,
	eTimerPayout,
	eTimerSystemTimerMax
};

enum EReelTimerID {
	eTimerReelStart,
	eTimerReelStopAvailable,
	eTimerReelPush,
	eTimerReelStop,
	eTimerReelTimerMax
};

class CSlotTimerManager{
	struct STimerData{
		bool enable;
		long long originVal;
		long long lastGetVal;
	};

	std::vector<STimerData>	m_timerData;	// enableFlag, originVal
	long long				m_lastCount;
	int						m_resetCount;
	int						m_reelNumMax;

	std::vector<std::pair<std::string, int>> mTimerNameList;
	std::set<std::string> mDuplicateFinder;

public:
	bool	Init(int pReelNum);
	bool	Process();
	bool	SetTimer(ESystemTimerID pID, int offset = 0);
	bool	SetTimer(EReelTimerID pID, int pReelID, int offset = 0);
	bool	DisableTimer(ESystemTimerID pID);
	bool	DisableTimer(EReelTimerID pID, int pReelID);
	bool	GetTime(long long& pInputFor, ESystemTimerID pID) const;
	bool	GetTime(long long& pInputFor, EReelTimerID pID, int pReelID) const;
	bool	GetTimeDiff(long long& pInputFor, ESystemTimerID pID, bool pRefreshFlag = true);
	bool	GetTimeDiff(long long& pInputFor, EReelTimerID pID, int pReelID, bool pRefreshFlag = true);

	int		GetTimerHandle(std::string pID) const;
	bool	CreateNewTimer(std::string pID);
	bool	GetTimeFromTimerHandle(long long& pInputFor, int pHandle) const;
};