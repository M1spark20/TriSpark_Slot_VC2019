#pragma once

class CSlotTimerManager;
class CEffectVariableManager;

class CEffectImageCommonComponent {
	long long mNowTime;
	bool mIsTimerSet;
	bool mIsTimerEnable;

protected:
	int mTimerID;											// 読み出しアニメーション・時分割を扱うタイマID
	int mLoopTime;											// タイマIDのループ点 (-1でループ無効)
	CEffectVariableManager& mVarManager;					// 変数管理クラスの参照

	long long GetTimer() const { return mNowTime; }
	bool GetIsTimerSet() const { return mIsTimerSet; }
	bool GetIsTimerEnable() const { return mIsTimerEnable; }

public:
	CEffectImageCommonComponent(CEffectVariableManager& pVar);

	// [act]描画に使用するタイマをセットする
	bool SetTimer(CSlotTimerManager& pTimerManager);
	void ResetTimer();

	int CalcAnimationPos(long long pNowTime, int pBeginTime, int pEndTime, int pBeginVal, int pEndVal);
};