#include "DxLib.h"
#include "crtdbg.h"
#include "_src\_header\setup.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	// [prm]DxLib�w��̕�
	// [act]CRTDEBUG�Ń��������[�N�`�F�b�N�̐錾���s��
	//		CSetup_S�N���X(setup.h)���Ăяo��
	// [ret]0:����I�� / -1:�ُ�I��
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	CSetup_S& SetupIns = CSetup_S::GetInstance();
	if (SetupIns.StartSetup()) return 0;
	else return -1;
}
