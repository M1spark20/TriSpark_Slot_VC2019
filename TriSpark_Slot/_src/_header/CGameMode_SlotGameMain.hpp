﻿#pragma once
#include "IGameModeBase.h"
class CGameDataManage;
class IGameStateBase;

class CGameMode_SlotGameMain : public IGameModeBase {
public:
	bool Init() override;
	EChangeModeFlag Process(bool pExtendResolution) override;
	~CGameMode_SlotGameMain();
};
