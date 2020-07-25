#pragma once
// State Pattern & Singleton
#include "ISingleton.h"
#include "EChangeModeFlag.h"
// �N���X�̐�s�錾
class IGameModeBase;
class CSelectMode_S : public ISingleton<CSelectMode_S>{
// [act]�Q�[���̌��݂̃��[�h���Q�Ƃ��A���ꂼ��̃X�^�[�g�N���X�֏�����n���B
//		�X�e�[�g�p�^�[���E�V���O���g���ŊǗ�����Ă���B
	friend ISingleton<CSelectMode_S>;
	CSelectMode_S();
	~CSelectMode_S();
// ���݂̃��[�h�̃C���X�^���X�̃|�C���^(m1)
	IGameModeBase* m_pNowMode; 
public:
	bool MainLoopProcess(bool& Ans);
};