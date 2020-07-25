#pragma once
#include "IGameStateBase.hpp"
class CGameDataManage;

class CGameState_ReadingData : public IGameStateBase{
// [act]ゲーム画面のグラッフィック読み込み関数
public:
	bool Init(CGameDataManage& pDataManageIns) override;
	EChangeStateFlag Process(CGameDataManage& p_pDataManageIns)override;
	bool Draw(CGameDataManage& p_DataManageIns) override;
};