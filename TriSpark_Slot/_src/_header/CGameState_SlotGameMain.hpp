#pragma once
#include "IGameStateBase.hpp"
#include "SSlotGameDataWrapper.hpp"
#include "CMenuManager.hpp"

class ISlotFlowManager;
class CGameDataManage;

class CGameState_SlotGameMain : public IGameStateBase{

	ISlotFlowManager*		m_pFlowManager;
	SSlotGameDataWrapper	m_data;
	CMenuManager			m_menuManager;

public:
	bool Init(CGameDataManage& pDataManageIns) override;
	EChangeStateFlag Process(CGameDataManage& pDataManageIns) override;
	 bool Draw(CGameDataManage& pDataManageIns) override;
	 ~CGameState_SlotGameMain();
};
