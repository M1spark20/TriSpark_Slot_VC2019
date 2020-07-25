#include "_header\CGameMode_SlotGameMain.hpp"
#include "_header\CGameState_ReadingData.hpp"
#include "_header\CGameState_SlotGameMain.hpp"
#include "_header\keyexport.h"
#include "DxLib.h"

bool CGameMode_SlotGameMain::Init(){
	if (!StartReadFile("data/readList/gameMain.csv")) return false;
	m_pGameStateManage = new CGameState_ReadingData;
	return m_pGameStateManage->Init(*m_pGameDataManage);
}

EChangeModeFlag CGameMode_SlotGameMain::Process(){
	EChangeStateFlag state = m_pGameStateManage->Process(*m_pGameDataManage);
	switch (state){
	case eStateContinue:
	case eStateEnd:
		break;
	case eStateMainStart:
		delete m_pGameStateManage;
		m_pGameStateManage = nullptr;
		m_pGameStateManage = new CGameState_SlotGameMain;
		if (!m_pGameStateManage->Init(*m_pGameDataManage)){
			DxLib::ErrorLogAdd("�Q�[���{�̂̏��������ɃG���[���������܂����B");
			return eModeErrEnd;
		}
		break;
	case eStateErrEnd:
		DxLib::ErrorLogAdd("�Q�[���{�̂̏������ɃG���[���������܂����B");
		return eModeErrEnd;
	default:
		DxLib::ErrorLogAdd("�s����State�n���h�����擾���܂����B");
		return eModeErrEnd;
		break;
	}
	if (!m_pGameStateManage->Draw(*m_pGameDataManage)){
		DxLib::ErrorLogAdd("�Q�[����ʕ`�撆�ɃG���[���������܂����B");
		return eModeErrEnd;
	}

	// �e�X�g�ڍs
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	if (key.ExportKeyState(KEY_INPUT_F1)) return eModeControlTest;

	return state == eStateEnd ? eModeEnd : eModeContinue;
}
CGameMode_SlotGameMain::~CGameMode_SlotGameMain(){
	// delete�̓x�[�X�N���X��
}