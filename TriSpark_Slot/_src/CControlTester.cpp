#include "_header\CControlTester.hpp"
#include "_header\CGameState_ControlTestMain.hpp"
#include "_header\CGameState_ReadingData.hpp"
#include "_header\keyexport.h"
#include "DxLib.h"

bool CControlTester::Init(){
	if(!StartReadFile("launchData/controlTest.csv")) return false;
	m_pGameStateManage	= new CGameState_ReadingData;
	return m_pGameStateManage->Init(*m_pGameDataManage);
}
EChangeModeFlag CControlTester::Process(){
	EChangeStateFlag state = m_pGameStateManage->Process(*m_pGameDataManage);
	switch (state){
	case eStateContinue:
	case eStateEnd:
		break;
	case eStateMainStart:
		delete m_pGameStateManage;
		m_pGameStateManage = nullptr;
		m_pGameStateManage = new CGameState_ControlTestMain;
		if (!m_pGameStateManage->Init(*m_pGameDataManage)){
			DxLib::ErrorLogAdd("����e�X�g�{�̂̏��������ɃG���[���������܂����B");
			return eModeErrEnd;
		}
		break;
	case eStateErrEnd:
		DxLib::ErrorLogAdd("����e�X�g�̏������ɃG���[���������܂����B");
		return eModeErrEnd;
	default:
		DxLib::ErrorLogAdd("�s����State�n���h�����擾���܂����B");
		return eModeErrEnd;
		break;
	}
	if (!m_pGameStateManage->Draw(*m_pGameDataManage)){
		DxLib::ErrorLogAdd("����e�X�g��ʕ`�撆�ɃG���[���������܂����B");
		return eModeErrEnd;
	}

	// �e�X�g�ڍs
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	if (key.ExportKeyState(KEY_INPUT_F5)) return eModeGameMain;

	return state == eStateEnd ? eModeEnd : eModeContinue;
}
CControlTester::~CControlTester(){
	// delete�̓x�[�X�N���X��
}