#include "DxLib.h"
#include "_header\gameloop.h"
#include "_header\keyexport.h"
#include "_header\selectmode.h"

bool CGameLoop_S::StartGameLoop(){
// [act]�Q�[���̃��C�����[�v�����B�L�[���͊Ď�������B
// [ret]�Q�[�����[�v������ɓ��삵������Ԃ��B
	CKeyInput_S& KeyIn_Ins = CKeyInput_S::GetInstance();
	CSelectMode_S& GameIns = CSelectMode_S::GetInstance();
	CKeyExport_S& KeyOutIns = CKeyExport_S::GetInstance();
	bool Check=true,Ans=true;
	while( Check && !DxLib::ProcessMessage() && !DxLib::ClearDrawScreen() ){
		clsDx();
		Check =  KeyIn_Ins.GetKeyState();
		Check &= GameIns.MainLoopProcess(Ans);
		ScreenFlip();
	}
	return Ans;
}