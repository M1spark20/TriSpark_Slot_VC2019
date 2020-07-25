#include "DxLib.h"
#include "_header\setup.h"
#include "_header\gameloop.h"
#include <new>

bool CSetup_S::StartSetup(){
// [act]���C�u����������(�E�B���h�E���[�h)
//		�����^�C���G���[or�e�ʊm�ۃG���[���̃��O��������
// [ret]�v���O�����̐���/�ُ�I���t���O
	DxLib::ChangeWindowMode(1);					// �E�B���h�E���[�h��
	DxLib::SetFullSceneAntiAliasingMode(3,2);	// 3D�`��̃N�I���e�B���w��
	DxLib::SetAlwaysRunFlag(TRUE);				// �A�N�e�B�u����Ȃ��Ƃ����������s��
	if(DxLib::DxLib_Init()) return false;		// DxLib����������
	DxLib::SetDrawScreen(DX_SCREEN_BACK);		// ����ʏ�������
	DxLib::SetUseZBuffer3D(TRUE);				// �y�o�b�t�@��L���ɂ���
	DxLib::SetWriteZBuffer3D(TRUE);				// �y�o�b�t�@�ւ̏������݂�L���ɂ���
	DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);	// �o�C���j�A�@�ɂ��g��k�����s��

	bool Result = true;
	try {
		CGameLoop_S& LoopIns=CGameLoop_S::GetInstance();
		Result = LoopIns.StartGameLoop();
	} catch(std::bad_alloc){
		DxLib::ErrorLogAdd("�������m�ێ��ɖ�肪�������܂����B\n");
		Result = false;
	}
	if(!Result)
		DxLib::ErrorLogAdd("�\�t�g�������I�����܂����B\n");
	DxLib_End();
	return Result;
}