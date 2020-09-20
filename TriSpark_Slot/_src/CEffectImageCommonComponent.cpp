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

// [act]�`��Ɏg�p����^�C�}���Z�b�g����
// [prm]pTimerManager	: �g�p����^�C�}�̃}�l�[�W���[
// [ret]�^�C�}�ݒ�ɐݒ肵�����ǂ���(�^�C�}�����݂��邪�����̏ꍇ��true��Ԃ�)
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

// [act]�`��Ɏg�p����^�C�}�����Z�b�g(������)����
void CEffectImageCommonComponent::ResetTimer() {
	mIsTimerSet = false;
	mIsTimerEnable = false;
}

int CEffectImageCommonComponent::CalcAnimationPos(long long pNowTime, int pBeginTime, int pEndTime, int pBeginVal, int pEndVal) {
	const int diffVal = pEndVal - pBeginVal;
	const double diffTime = pEndTime - (long long)pBeginTime;
	const double progress = (pNowTime - pBeginTime) / diffTime;
	return pBeginVal + static_cast<int>(diffVal * progress);
}