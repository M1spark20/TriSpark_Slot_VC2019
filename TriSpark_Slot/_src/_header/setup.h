#pragma once
#include "ISingleton.h"
class CSetup_S : public ISingleton<CSetup_S>{
// [act]DX���C�u���������������B�ُ�I�����̓��b�Z�[�W�����O�Ɏc��
//		�V���O���g���ŊǗ�����Ă���
	friend ISingleton<CSetup_S>;
	CSetup_S(){};
	~CSetup_S(){};
public:
	bool StartSetup();
};