#include "DxLib.h"
#include "_src/_header/setup.h"

#ifdef __ANDROID__
int android_main(void) {

#else
#include "crtdbg.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
#endif

	// [prm]DxLib指定の物
	// [act]CRTDEBUGでメモリリークチェックの宣言を行う
	//		CSetup_Sクラス(setup.h)を呼び出す
	// [ret]0:正常終了 / -1:異常終了
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CSetup_S& SetupIns = CSetup_S::GetInstance();
	if (SetupIns.StartSetup()) return 0;
	else return -1;
}
