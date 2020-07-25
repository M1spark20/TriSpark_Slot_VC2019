#include "DxLib.h"
#include "_header\keyinput.h"
#include "_header\keyexport.h"

bool CKeyInput_S::GetKeyState(){
// [act]キーステート取得(DxLib::GetHitKeyStateAll)
//		結果をCKeyExport_Sクラスへ伝播(ExIns.KeyStateUpdate)
// [ret]関数が成功したか
// DxLibのキーボード取得時の配列マジックナンバー
	const int GetInputArrayMax_C = 256;
	char KeyState[GetInputArrayMax_C];
	if(DxLib::GetHitKeyStateAll(KeyState)) return false;
	CKeyExport_S& ExIns=CKeyExport_S::GetInstance();
	ExIns.KeyStateUpdate(KeyState);
	return true;
}