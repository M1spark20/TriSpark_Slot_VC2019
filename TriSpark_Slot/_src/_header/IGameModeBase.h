#pragma once
#include "EChangeModeFlag.h"
#include "CGameDataManage.h"
#include "IGameStateBase.hpp"

class IGameModeBase{
// [act]�Q�[���S�̂ł̃��[�h(���j���[�Ȃ�)�̃X�e�[�g�p�^�[���p���N���X
protected:
	CGameDataManage*	m_pGameDataManage;
	IGameStateBase*		m_pGameStateManage;

	bool StartReadFile(const std::string pReadListFileName);

public:
	IGameModeBase()
		: m_pGameDataManage(nullptr),
		  m_pGameStateManage(nullptr){};
	virtual bool Init()=0;
	virtual EChangeModeFlag Process()=0;
	virtual ~IGameModeBase(){
		delete m_pGameDataManage;
		delete m_pGameStateManage;
	}
};