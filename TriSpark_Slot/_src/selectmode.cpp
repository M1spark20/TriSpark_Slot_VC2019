#include "_header\selectmode.h"
#include "_header\CControlTester.hpp"
#include "_header\CGameMode_SlotGameMain.hpp"

CSelectMode_S::CSelectMode_S(){
// [act]�N�����̃��[�h�̃C���X�^���X�𐶐����A�������������s��
	m_pNowMode = new CGameMode_SlotGameMain;	// ���݂̏������[�h��ControlTester
	m_pNowMode->Init();
}
bool CSelectMode_S::MainLoopProcess(bool& Ans){
// [prm]p1;�Ăяo�����ŏI�����ɕԂ��l�̎Q��
// [act]���[�h���Ƃ̏������s���A�K�v�Ȃ烂�[�h�ύX�A�ēx�������������s���B
//		�I���̏ꍇ��Ans�ϐ��ɐ��킩�ُ킩���Z�b�g
// [ret]�Q�[�����[�v���p�����邩
	EChangeModeFlag Next=m_pNowMode->Process();
	if(Next!=eModeContinue){
		delete m_pNowMode;	m_pNowMode=nullptr;
		switch(Next){
		case eModeControlTest: m_pNowMode=new CControlTester; break;
		case eModeGameMain: m_pNowMode=new CGameMode_SlotGameMain; break;
		case eModeEnd: Ans=true; return false; break;
		case eModeErrEnd: Ans=false; return false; break;
		}
		m_pNowMode->Init();
	}
	return true;
}
CSelectMode_S::~CSelectMode_S(){
// [act]m_pNowMode�̃C���X�^���X��delete����
	delete m_pNowMode;
}