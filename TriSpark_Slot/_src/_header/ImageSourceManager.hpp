#pragma once
#include "CEffectImageCommonComponent.hpp"
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CEffectVariableManager;
class CSlotTimerManager;

// [act]imgSrc�f�[�^�Ǘ����s�����N���X�B���̌^�𓮓I�m�ۂ��Ăяo�����ꌳ������
//		�R���X�g���N�^��Init()�ŕK�v�ȃf�[�^���e�h���N���X�Ɏ�荞��
//		GetComaNum()�����GetImageSource()�ŉ摜�̐؂�o�����𒲐�����
class IImageSourceManager : public CEffectImageCommonComponent {
protected:
	std::vector<SImageSourceCSVCommonData> mCommonData;		// csv����ǂ݂�����src�f�[�^�A������`�\
	std::string mEffectDataName;

	typedef std::vector<std::string> StringArr;

	// [act]���[�v�_���l����������Ɏg�p���鎞�Ԃ�����o��
	long long GetCheckTime(const long long pNowCount);
	// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜��`�����肷��
	int GetDefinitionIndex();
	// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜�R�}�����肷��
	int GetImageIndex(int pDefinitionIndex);
	// [act]�A�j���[�V�����Ɏg�p�ł���R�}�����擾����(�K�v�ɉ���override����)
	virtual int GetComaNum(int pDefinitionIndex);
	// [act]definitionIndex��imageIndex����摜�͈͂����o��
	SDrawImageSourceData GetSourceDataFromIndex(int pDefinitionIndex, int pImageIndex);

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	IImageSourceManager(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	virtual bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager);
	// [act]�摜�ǂݍ��ݎQ�Ɛ��Ԃ�
	virtual SDrawImageSourceData	GetImageSource(int pWriteIndex, int pWriteNum) = 0;
	// [act]�G�t�F�N�g�ϐ���Ԃ�
	std::string						GetEffectDataName() const { return mEffectDataName; }
};

class CImageSourceDefault : public IImageSourceManager {
public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageSourceDefault(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager) override;
	// [act]�摜�ǂݍ��ݎQ�Ɛ��Ԃ�
	SDrawImageSourceData	GetImageSource(int pWriteIndex = 0, int pWriteNum = 0) override;
};

class CImageSourceNumber : public IImageSourceManager {
	enum class EAlign{ eLeft, eCenter, eRight };	// ���t���@�w��penum; eCenter�̓���͌��ݖ���`
	bool mPaddingFlag;								// 0�p�f�B���O���s�������w��; �ݒ莞�͕K�v�R�}+1
	bool mDrawMinusFlag;							// ���l���}�C�i�X�̎��ɕ`����s�������w��; �ݒ莞�͕K�v�R�}+1��2�Z�b�g(+-)�K�v
	int mNumSource;									// �`��Ώەϐ��i�[�p�|�C���^
	int mDigitCount;								// �`��ɕK�v�ȃR�}�����i�[����(10, 11, 12, 22 or 24)
	EAlign mNumAlign;								// �������t���@���w��

	// [act]�A�j���[�V�����Ɏg�p�ł���R�}�����擾����B�l�̓R�}������mDigitCount�������ċ��߂���
	int GetComaNum(int pDefinitionIndex) override;

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageSourceNumber(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager) override;
	SDrawImageSourceData	GetImageSource(int pWriteIndex, int pWriteNum) override;
};

class CImageSourceReel : public IImageSourceManager {
public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageSourceReel(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager) override;
	// [act]�摜�ǂݍ��ݎQ�Ɛ��Ԃ�
	SDrawImageSourceData	GetImageSource(int pWriteIndex = 0, int pWriteNum = 0) override;
};

