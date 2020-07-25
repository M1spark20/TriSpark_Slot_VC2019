#include "_header\CGameState_ReadingData.hpp"
#include "_header\CGameDataManage.h"
#include "DxLib.h"

bool CGameState_ReadingData::Init(CGameDataManage& p_pDataManageIns){
// [act]何もしない。読み込みデータ一覧はCGameDataManageの初期化時に規定。
//		このクラスではその読み込み完了をDxLib関数によって監視するだけ。
	return true;
}
EChangeStateFlag CGameState_ReadingData::Process(CGameDataManage& p_pDataManageIns){
// [act]ファイルの読み込みが終わったか確認し、
//		終わっていればシーンをプレイ画面に移す。
// [ret]次回のゲームモード(終了時にプレイ画面へ。)
	if (DxLib::GetASyncLoadNum() == 0){
		if (!p_pDataManageIns.CheckReadFile()) return eStateErrEnd;
		return eStateMainStart;
	}
	return eStateContinue;
}
bool CGameState_ReadingData::Draw(CGameDataManage& p_DataManageIns){
// [act]ロード中であることを知らせる画面の描画を行います。
// [ret]関数が成功したかどうか。
	DxLib::printfDx("NowLoading...");
	return true;
}
