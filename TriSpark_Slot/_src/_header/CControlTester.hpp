#pragma once
#include "IGameModeBase.h"
class CGameDataManage;
class IGameStateBase;

class CControlTester : public IGameModeBase{
public:
	bool Init() override;
	EChangeModeFlag Process() override;
	~CControlTester();
};