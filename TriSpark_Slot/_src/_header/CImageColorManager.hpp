#pragma once
#include "CEffectImageCommonComponent.hpp"
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CGameDataManage;

class CImageColorManager : public CEffectImageCommonComponent {
	std::vector<SImageSourceCSVCommonData> mCommonData;		// csv����ǂ݂�����src�f�[�^�A������`�\
	bool mIsColorIndexDirY;

	typedef std::vector<std::string> StringArr;

	// [act]���[�v�_���l����������Ɏg�p���鎞�Ԃ�����o��
	long long GetCheckTime(const long long pNowCount);
	// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜��`�����肷��
	int GetDefinitionIndex();
	// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜�R�}�����肷��
	double GetImageIndex(int pDefinitionIndex);
	// [act]�A�j���[�V�����Ɏg�p�ł���R�}�����擾����(�K�v�ɉ���override����)
	virtual int GetComaNum(int pDefinitionIndex);
	// [act]definitionIndex��imageIndex����摜�͈͂����o��
	bool GetColorDataFromIndex(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pDefinitionIndex, int pImageIndex, int pColorIndex);

	void GetAnimationNext(int pNowDef, int pNowImg, int& pNextDef, int& pNextImg);

	int CalcColorAnimation(int pBeginVal, int pEndVal, double pProgress);

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageColorManager(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool	Init(StringArr pReadData, CSlotTimerManager& pTimerManager);
	// [act]�摜�ǂݍ��ݎQ�Ɛ��Ԃ�
	bool	GetColorData(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pWriteIndex);
};