#pragma once
#include <algorithm>
class CSlotTimerManager;

class CSlotInternalDataManager {
	struct SCreditData {
		int			originVal;
		int			offset;
		int			nowShownOffset;
		bool		isValChanged;
		long long	lastValChangeTime;
	};
public:
	struct SSlotInternalData {
		unsigned int			set;
		std::pair<int, int>		flag;				// flagID, bonusID
		int						gameMode;
		std::pair<int, int>		gameModeLimit;		// Play/GetLimit, PayLimit[or]Play/GetLimit
		std::pair<bool, bool>	isGameLimit;
		bool					isGetCountOnlyJac;	// 4���@�炵���{�[�i�X����p
		std::pair<int, int>		gameModeAtEnd;		// PayLim�쓮����GameLim���쎞�̂ݗL��(JacGame�p)
		std::pair<int, int>		rtMode;				// rtID, rtLimitGame
		std::pair<int, int>		rtModeAtModeEnd;	// rtID, rtLimitGame
		bool					isRtOverrideEnable;
		unsigned int			betNum;
		unsigned int			inCount;
		unsigned int			outCount;
		bool					isReplay;
		unsigned int			credit;
		unsigned int			payout;
	};

private:
	SSlotInternalData		m_data;
	bool					m_IsCreditResetAdd;
	std::pair<bool, bool>	m_isGameNoCount;

	void	ChangeCreditVal(int margin);

public:
	void	Init();
	void	SetFlag(int pFlagID, int pBonusID);
	void	ResetBonusFlag();
	void	SetRTMode(int pRTID, bool pCanOverride, bool pIsForceOverride);
	void	SetRTMode(int pRTID, int pRTGame, bool pCanOverride, bool pIsForceOverride);
	void	SetRTModeAtEnd(int pRTID, int pRTGame);
	void	SetGameMode(int pNextMode, int pPayLim);
	void	SetGameMode(int pNextMode, int pGameLim, int pGetLim);
	void	SetSlotSetting(unsigned int pData) { m_data.set = pData; }
	void	AddBetCount(bool pIsReplay);
	void	ResetBetCount(bool pIsReBet);
	void	LatchBet();
	void	AddPayout();
	void	SetReplayStatus(bool pRepFlag);
	void	CheckGameModeEnd(bool pGameCount, bool pGetCount);

	SSlotInternalData	GetData() const { return m_data; }
};