#pragma once

class CSlotTimerManager;
class CEffectVariableManager;

class CEffectImageCommonComponent {
	long long mNowTime;
	bool mIsTimerSet;
	bool mIsTimerEnable;

protected:
	int mTimerID;											// �ǂݏo���A�j���[�V�����E�������������^�C�}ID
	int mLoopTime;											// �^�C�}ID�̃��[�v�_ (-1�Ń��[�v����)
	CEffectVariableManager& mVarManager;					// �ϐ��Ǘ��N���X�̎Q��

	long long GetTimer() const { return mNowTime; }
	bool GetIsTimerSet() const { return mIsTimerSet; }
	bool GetIsTimerEnable() const { return mIsTimerEnable; }

public:
	CEffectImageCommonComponent(CEffectVariableManager& pVar);

	// [act]�`��Ɏg�p����^�C�}���Z�b�g����
	bool SetTimer(CSlotTimerManager& pTimerManager);
	void ResetTimer();

	int CalcAnimationPos(long long pNowTime, int pBeginTime, int pEndTime, int pBeginVal, int pEndVal);
};