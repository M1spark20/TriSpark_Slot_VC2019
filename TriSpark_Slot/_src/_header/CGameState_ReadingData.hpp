#pragma once
#include "IGameStateBase.hpp"
class CGameDataManage;

class CGameState_ReadingData : public IGameStateBase{
// [act]�Q�[����ʂ̃O���b�t�B�b�N�ǂݍ��݊֐�
public:
	bool Init(CGameDataManage& pDataManageIns) override;
	EChangeStateFlag Process(CGameDataManage& p_pDataManageIns)override;
	bool Draw(CGameDataManage& p_DataManageIns) override;
};