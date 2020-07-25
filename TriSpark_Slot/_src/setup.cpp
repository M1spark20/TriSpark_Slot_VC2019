#include "DxLib.h"
#include "_header\setup.h"
#include "_header\gameloop.h"
#include <new>

bool CSetup_S::StartSetup(){
// [act]ライブラリ初期化(ウィンドウモード)
//		ランタイムエラーor容量確保エラー時のログ書き込み
// [ret]プログラムの正常/異常終了フラグ
	DxLib::ChangeWindowMode(1);					// ウィンドウモードに
	DxLib::SetFullSceneAntiAliasingMode(3,2);	// 3D描画のクオリティを指定
	DxLib::SetAlwaysRunFlag(TRUE);				// アクティブじゃないときも処理を行う
	if(DxLib::DxLib_Init()) return false;		// DxLib初期化処理
	DxLib::SetDrawScreen(DX_SCREEN_BACK);		// 裏画面書き込み
	DxLib::SetUseZBuffer3D(TRUE);				// Ｚバッファを有効にする
	DxLib::SetWriteZBuffer3D(TRUE);				// Ｚバッファへの書き込みを有効にする
	DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);	// バイリニア法による拡大縮小を行う

	bool Result = true;
	try {
		CGameLoop_S& LoopIns=CGameLoop_S::GetInstance();
		Result = LoopIns.StartGameLoop();
	} catch(std::bad_alloc){
		DxLib::ErrorLogAdd("メモリ確保時に問題が発生しました。\n");
		Result = false;
	}
	if(!Result)
		DxLib::ErrorLogAdd("ソフトが強制終了しました。\n");
	DxLib_End();
	return Result;
}