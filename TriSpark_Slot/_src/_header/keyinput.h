#pragma once
#include "ISingleton.h"
class CKeyInput_S : public ISingleton<CKeyInput_S>{
// [act]�L�[���͊Ď��A���ʂ�CKeyExport_S�N���X�֓`�d
	friend ISingleton<CKeyInput_S>;
	CKeyInput_S(){};
	~CKeyInput_S(){};
public:
	bool GetKeyState();
};