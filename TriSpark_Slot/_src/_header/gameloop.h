#pragma once
#include "ISingleton.h"
class CGameLoop_S : public ISingleton<CGameLoop_S>{
// [act]�Q�[���̃��C�����[�v�����B�L�[���͊Ď�������B
//		�V���O���g���ŊǗ�����Ă���B
	friend ISingleton<CGameLoop_S>;
	CGameLoop_S(){};
	~CGameLoop_S(){};
public:
	bool StartGameLoop();
};