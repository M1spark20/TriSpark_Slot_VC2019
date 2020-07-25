#pragma once
#include "ISlotFlowManager.hpp"
#include <algorithm>

class CSlotFlowBet : public ISlotFlowManager {
	int							m_betFor;
	const std::pair<int, int>	BET_INTERVAL;	// 値はコンストラクタで定義

	void	SetBetFor(SSlotGameDataWrapper& pGameData, int pBetNum, bool pIsReplay);
public:
	CSlotFlowBet();
	bool Init(SSlotGameDataWrapper& pGameData) override;
	ESlotFlowFlag Process(SSlotGameDataWrapper& pGameData) override;
};

class CSlotFlowWaiting : public ISlotFlowManager {
public:
	bool Init(SSlotGameDataWrapper& pGameData) override;
	ESlotFlowFlag Process(SSlotGameDataWrapper& pGameData) override;
};

class CSlotFlowReelAcc : public ISlotFlowManager {
public:
	bool Init(SSlotGameDataWrapper& pGameData) override;
	ESlotFlowFlag Process(SSlotGameDataWrapper& pGameData) override;
};

class CSlotFlowReelMove : public ISlotFlowManager {
public:
	bool Init(SSlotGameDataWrapper& pGameData) override;
	ESlotFlowFlag Process(SSlotGameDataWrapper& pGameData) override;
};

class CSlotFlowPayout : public ISlotFlowManager {
	int		m_payoutFor;
	int		m_nowPayCount;
	bool	m_isDummyPay;

public:
	bool Init(SSlotGameDataWrapper& pGameData) override;
	ESlotFlowFlag Process(SSlotGameDataWrapper& pGameData) override;
};