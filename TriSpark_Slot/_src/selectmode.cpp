#include "_header/selectmode.h"
#include "_header/CControlTester.hpp"
#include "_header/CGameMode_SlotGameMain.hpp"

CSelectMode_S::CSelectMode_S(){
// [act]起動時のモードのインスタンスを生成し、初期化処理を行う
	m_pNowMode = new CGameMode_SlotGameMain;	// 現在の初期モードはControlTester
	m_pNowMode->Init();
}
bool CSelectMode_S::MainLoopProcess(bool& Ans){
// [prm]p1;呼び出し元で終了時に返す値の参照
// [act]モードごとの処理を行い、必要ならモード変更、再度初期化処理を行う。
//		終了の場合はAns変数に正常か異常かをセット
// [ret]ゲームループが継続するか
	EChangeModeFlag Next=m_pNowMode->Process();
	if(Next!=eModeContinue){
		delete m_pNowMode;	m_pNowMode=nullptr;
		switch(Next){
		case eModeControlTest: m_pNowMode=new CControlTester; break;
		case eModeGameMain: m_pNowMode=new CGameMode_SlotGameMain; break;
		case eModeEnd: Ans=true; return false; break;
		case eModeErrEnd: Ans=false; return false; break;
		}
		m_pNowMode->Init();
	}
	return true;
}
CSelectMode_S::~CSelectMode_S(){
// [act]m_pNowModeのインスタンスをdeleteする
	delete m_pNowMode;
}
