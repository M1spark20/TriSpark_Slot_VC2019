#pragma once
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CEffectVariableManager;
class CSlotTimerManager;
class IImageSourceManager;
class CGameDataManage;

class IImageDestinationManager {
private:
	long long mNowTime;
	bool mIsTimerSet;
	bool mIsTimerEnable;

	EDrawModeForDST  GetDrawEnum(std::string pIndex);
	EBlendModeForDST GetBlendEnum(std::string pIndex);

protected:
	std::vector<SImageDestCSVCommonData> mCommonData;		// csv����ǂ݂�����dst�f�[�^�A������`�\
	int mTimerID;											// �ǂݏo���A�j���[�V�����E�������������^�C�}ID
	int mLoopTime;											// �^�C�}ID�̃��[�v�_ (-1�Ń��[�v����)
	CEffectVariableManager& mVarManager;					// �ϐ��Ǘ��N���X�̎Q��

	typedef std::vector<std::string> StringArr;

	// [act]���[�v�_���l����������Ɏg�p���鎞�Ԃ�����o��
	long long GetCheckTime(const long long pNowCount);
	// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����`���`�����肷��
	int GetDefinitionIndex();

	int GetDxDrawModeByEnum(EDrawModeForDST pData);
	int GetDxBlendModeByEnum(EBlendModeForDST pData);
	bool GetCanDrawDirectly(EBlendModeForDST pData);

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	IImageDestinationManager(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	virtual bool	Init(StringArr pReadData, CSlotTimerManager& pTimerData);
	// [act]�`��Ɏg�p����^�C�}���Z�b�g����
	bool			SetTimer(CSlotTimerManager& pTimerManager);
	void							ResetTimer();
	// [act]
	virtual void	Draw(IImageSourceManager *const pSourceData, CGameDataManage& pDataManager) = 0;
};

class CImageDestinationDefault : public IImageDestinationManager {
	int mDrawNum;
	int mDiffX, mDiffY;

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageDestinationDefault(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool	Init(StringArr pReadData, CSlotTimerManager& pTimerData) override;
	// [act]�`����s��
	void	Draw(IImageSourceManager *const pSourceData, CGameDataManage& pDataManager) override;
};