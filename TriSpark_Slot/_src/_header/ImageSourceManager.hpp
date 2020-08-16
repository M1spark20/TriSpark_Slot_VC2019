#pragma once
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

// [act]imgSrc�f�[�^�Ǘ����s�����N���X�B���̌^�𓮓I�m�ۂ��Ăяo�����ꌳ������
//		�R���X�g���N�^��Init()�ŕK�v�ȃf�[�^���e�h���N���X�Ɏ�荞��
//		GetComaNum()�����GetImageSource()�ŉ摜�̐؂�o�����𒲐�����
class IImageSourceManager {
protected:
	std::vector<SImageSourceCSVCommonData> mCommonData;		// csv����ǂ݂�����src�f�[�^�A������`�\
	std::string mTimerID;									// �ǂݏo���A�j���[�V�����E�������������^�C�}ID
	const int* m_pLoopTime;									// �^�C�}ID�̃��[�v�_ (-1�Ń��[�v����)
	const long long* (* const mTimerReader)(std::string);	// �^�C�}�l�Ăяo���p�֐��|�C���^

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
	IImageSourceManager(const long long* (* const pTimerReader)(std::string));
	// [act]������z��"pReadData"����src�f�[�^���擾����
	virtual bool					Init(StringArr pReadData, int* (*const pVariableManager)(std::string));
	// [act]�摜�ǂݍ��ݎQ�Ɛ��Ԃ�
	virtual SDrawImageSourceData	GetImageSource(int pWriteIndex, int pWriteNum) = 0;
};

class CImageSourceDefault : public IImageSourceManager {
public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageSourceDefault(const long long* (* const pTimerReader)(std::string));
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool					Init(StringArr pReadData, int* (*const pVariableManager)(std::string)) override;
	// [act]�摜�ǂݍ��ݎQ�Ɛ��Ԃ�
	SDrawImageSourceData	GetImageSource(int pWriteIndex = 0, int pWriteNum = 0) override;
};

class CImageSourceNumber : public IImageSourceManager {
	enum class EAlign{ eLeft, eCenter, eRight };	// ���t���@�w��penum; eCenter�̓���͌��ݖ���`
	bool mPaddingFlag;								// 0�p�f�B���O���s�������w��; �ݒ莞�͕K�v�R�}+1
	bool mDrawMinusFlag;							// ���l���}�C�i�X�̎��ɕ`����s�������w��; �ݒ莞�͕K�v�R�}+1��2�Z�b�g(+-)�K�v
	int* m_pNumSource;								// �`��Ώەϐ��i�[�p�|�C���^
	int mDigitCount;								// �`��ɕK�v�ȃR�}�����i�[����(10, 11, 12, 22 or 24)
	EAlign mNumAlign;								// �������t���@���w��

	// [act]�A�j���[�V�����Ɏg�p�ł���R�}�����擾����B�l�̓R�}������mDigitCount�������ċ��߂���
	int GetComaNum(int pDefinitionIndex) override;

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageSourceNumber(const long long* (* const pTimerReader)(std::string));
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool					Init(StringArr pReadData, int* (*const pVariableManager)(std::string)) override;
	SDrawImageSourceData	GetImageSource(int pWriteIndex, int pWriteNum) override;
};