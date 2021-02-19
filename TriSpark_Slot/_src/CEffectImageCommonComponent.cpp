#include "_header/CEffectImageCommonComponent.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/ErrClass.hpp"

CEffectImageCommonComponent::CEffectImageCommonComponent(CEffectVariableManager& pVar)
	: mVarManager(pVar) {
	mLoopTime = -1;
	mTimerID = -1;
	mNowTime = -1;
	mLoopTime = -1;
	mIsTimerSet = false;
	mIsTimerEnable = false;
}

// [act]描画に使用するタイマをセットする
// [prm]pTimerManager	: 使用するタイマのマネージャー
// [ret]タイマ設定に設定したかどうか(タイマが存在するが無効の場合はtrueを返す)
bool CEffectImageCommonComponent::SetTimer(CSlotTimerManager& pTimerManager) {
	try {
		mIsTimerEnable = pTimerManager.GetTimeFromTimerHandle(mNowTime, mTimerID);
		mIsTimerSet = true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	return true;
}

// [act]描画に使用するタイマをリセット(無効化)する
void CEffectImageCommonComponent::ResetTimer() {
	mIsTimerSet = false;
	mIsTimerEnable = false;
}

int CEffectImageCommonComponent::CalcAnimationPos(int pBeginVal, int pEndVal, double pProgress) const{
	const double rate = pProgress - floor(pProgress);
	const int diff = pEndVal - pBeginVal;
	return pBeginVal + static_cast<int>(diff * rate);
}
