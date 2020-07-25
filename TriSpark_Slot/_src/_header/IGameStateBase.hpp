#pragma once
#include "EChangeStateFlag.h"
// �N���X�̐�s�錾
class CGameDataManage;

class IGameStateBase {
	// [act]�Q�[���{�̂̃��[�h�I��p�̊��N���X
public:
	virtual bool Init(CGameDataManage& pDataManageIns) = 0;
	virtual EChangeStateFlag Process(CGameDataManage& pDataManageIns) = 0;
	virtual bool Draw(CGameDataManage& pDataManageIns) = 0;
	virtual ~IGameStateBase(){};
};