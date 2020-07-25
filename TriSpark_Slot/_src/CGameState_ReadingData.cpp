#include "_header\CGameState_ReadingData.hpp"
#include "_header\CGameDataManage.h"
#include "DxLib.h"

bool CGameState_ReadingData::Init(CGameDataManage& p_pDataManageIns){
// [act]�������Ȃ��B�ǂݍ��݃f�[�^�ꗗ��CGameDataManage�̏��������ɋK��B
//		���̃N���X�ł͂��̓ǂݍ��݊�����DxLib�֐��ɂ���ĊĎ����邾���B
	return true;
}
EChangeStateFlag CGameState_ReadingData::Process(CGameDataManage& p_pDataManageIns){
// [act]�t�@�C���̓ǂݍ��݂��I��������m�F���A
//		�I����Ă���΃V�[�����v���C��ʂɈڂ��B
// [ret]����̃Q�[�����[�h(�I�����Ƀv���C��ʂցB)
	if (DxLib::GetASyncLoadNum() == 0){
		if (!p_pDataManageIns.CheckReadFile()) return eStateErrEnd;
		return eStateMainStart;
	}
	return eStateContinue;
}
bool CGameState_ReadingData::Draw(CGameDataManage& p_DataManageIns){
// [act]���[�h���ł��邱�Ƃ�m�点���ʂ̕`����s���܂��B
// [ret]�֐��������������ǂ����B
	DxLib::printfDx("NowLoading...");
	return true;
}
