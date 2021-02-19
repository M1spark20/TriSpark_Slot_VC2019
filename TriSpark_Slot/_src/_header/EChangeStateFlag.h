#pragma once
enum EChangeStateFlag {
// [act]ゲーム本体の現在のモード(ゲーム中、メニュー)などの変更フラグ
	eStateContinue, eStateMainStart, eStateReadStart, eStateEnd, eStateErrEnd,
};
