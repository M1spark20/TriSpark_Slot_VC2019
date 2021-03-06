﻿#pragma once
#include "IGameStateBase.hpp"
#include "SSlotGameDataWrapper.hpp"
#include "CMenuManager.hpp"

class ISlotFlowManager;
class CGameDataManage;

class CGameState_SlotGameMain : public IGameStateBase{

	ISlotFlowManager*		m_pFlowManager;
	SSlotGameDataWrapper	m_data;
	CMenuManager			m_menuManager;

	int						mDisplayW, mDisplayH;
	int						mBGWindow;

public:
	bool Init(CGameDataManage& pDataManageIns) override;
	EChangeStateFlag Process(CGameDataManage& pDataManageIns, bool pExtendResolution) override;
	 bool Draw(CGameDataManage& pDataManageIns) override;
	 ~CGameState_SlotGameMain();
};
