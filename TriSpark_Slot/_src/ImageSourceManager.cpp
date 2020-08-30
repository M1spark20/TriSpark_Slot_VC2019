#include "_header/ImageSourceManager.hpp"
#include "_header/ErrClass.hpp"
#include "_header/CEffectVariableManager.hpp"

// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
// [prm]pTimerReader	: �^�C�}�[�l�Ăяo���p�֐��|�C���^
IImageSourceManager::IImageSourceManager(const long long* (* const pTimerReader)(std::string), CEffectVariableManager& pVarManager)
	: mTimerReader(pTimerReader), mVarManager(pVarManager){
	mCommonData.clear();
	mLoopTime -1;
}

// [act]������z��"pReadData"����src�f�[�^���擾����
// [prm]pReadData			: �������pcsv�����f�[�^
//		pVariableManager	: �ϐ��Ǘ��p�֐����w�聨�l�̓|�C���^�ŊǗ�����
// [ret]�f�[�^�擾�ɐ����������ǂ���
bool IImageSourceManager::Init(StringArr pReadData) {
	try {
		SImageSourceCSVCommonData data;
		data.startTime	= mVarManager.MakeValID(pReadData[1]);
		data.imageID	= mVarManager.MakeValID(pReadData[2]);
		data.x			= mVarManager.MakeValID(pReadData[3]);
		data.y			= mVarManager.MakeValID(pReadData[4]);
		data.w			= mVarManager.MakeValID(pReadData[5]);
		data.h			= mVarManager.MakeValID(pReadData[6]);
		data.numX		= mVarManager.MakeValID(pReadData[7]);
		data.numY		= mVarManager.MakeValID(pReadData[8]);
		data.directionY = pReadData[9] == "Y";

		if (mCommonData.empty()) {
			mTimerID	= pReadData[10];
			mLoopTime = mVarManager.MakeValID(pReadData[11]);
		}

		mCommonData.push_back(data);
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}

// [act]���[�v�_���l����������Ɏg�p���鎞�Ԃ�����o��
// [prm]pNowCount	: ���݂̃^�C�}�J�E���g
// [ret]�`��Ɏg�p����^�C�}�J�E���g
long long IImageSourceManager::GetCheckTime(const long long pNowCount) {
	const long long loopTime = mVarManager.GetVal(mLoopTime);
	if (pNowCount < loopTime || loopTime < 0) return pNowCount;

	if (mCommonData.empty()) throw ErrInternalVarUndeclared("mCommonData");
	const int offset = mVarManager.GetVal(mCommonData.begin()->startTime);
	const int diffCount = loopTime - offset;
	if (diffCount == 0) return offset;
	long long ans = pNowCount - offset;
	return ans - (diffCount * (ans / diffCount)) + offset;
}

// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜��`�����肷��
// [ret]-1	:����͕`����s���^�C�~���O�ł͂Ȃ��ꍇ
//		else:�`�悷���`ID @mCommonData
int IImageSourceManager::GetDefinitionIndex() {
	if (mCommonData.empty()) return -1;
	const auto definitionNum = mCommonData.size();
	const int loopTime = mVarManager.GetVal(mLoopTime);

	try {
		const long long* const nowTime = mTimerReader(mTimerID);
		if (nowTime == nullptr) return -1;
		const long long checkTime = GetCheckTime(*nowTime);

		// ���Ă���v�f��beginTime�ɖ��B�Ȃ炻�̑O�̃f�[�^���g�p����B��1�v�f�ɖ��B�Ȃ�`����s��Ȃ�
		int ans = -1;
		for (auto it = mCommonData.begin(); it != mCommonData.end(); ++it, ++ans)
			if (checkTime < (long long)it->startTime) return ans;

		// ���[�v�_������΍Ō�̗v�f��`��A�Ȃ���Ε`����s��Ȃ�
		return loopTime >= 0 ? ans : -1;
	}
	catch (ErrInternalVarUndeclared e) {
		e.WriteErrLog();
		return -1;
	}
}

// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜�R�}�����肷��
// [prm]pDefinitionIndex	: �`�悷���`ID @mCommonData
// [ret]-1	: �G���[
//		else: �摜�R�}ID
int IImageSourceManager::GetImageIndex(int pDefinitionIndex) {
	if (pDefinitionIndex < 0 || (size_t)pDefinitionIndex >= mCommonData.size()) return -1;

	const int comaNum = GetComaNum(pDefinitionIndex);
	if (comaNum == -1) return -1;
	const long long offset = mVarManager.GetVal(mCommonData[pDefinitionIndex].startTime);
	const long long interval = -offset + ((size_t)pDefinitionIndex + 1 == mCommonData.size() ?
		mVarManager.GetVal(mLoopTime) : mVarManager.GetVal(mCommonData[(size_t)pDefinitionIndex+1].startTime));
	const double division = interval / (double)comaNum;

	const long long *const nowTime =  mTimerReader(mTimerID);
	if (nowTime == nullptr) return -1;

	try {
		const long long operateTime = GetCheckTime(pDefinitionIndex) - offset;
		return static_cast<int>(operateTime / division);
	}
	catch (ErrInternalVarUndeclared e) {
		e.WriteErrLog();
		return -1;
	}
}

// [act]�A�j���[�V�����Ɏg�p�ł���R�}�����擾����(�K�v�ɉ���override����)
// [prm]pDefinitionIndex	: �`�悷���`ID @mCommonData
// [ret]-1	: �G���[
//		else: ���p�\�R�}��
int IImageSourceManager::GetComaNum(int pDefinitionIndex) {
	if (pDefinitionIndex < 0 || (size_t)pDefinitionIndex >= mCommonData.size()) return -1;
	return abs(mVarManager.GetVal(mCommonData[pDefinitionIndex].numX)) * abs(mVarManager.GetVal(mCommonData[pDefinitionIndex].numY));
}

// [act]definitionIndex��imageIndex����摜�͈͂����o��
SDrawImageSourceData IImageSourceManager::GetSourceDataFromIndex(int pDefinitionIndex, int pImageIndex) {
	// index�ɉ����ĉ摜��؂�o��
	SDrawImageSourceData ans;
	const auto& nowData = mCommonData[pDefinitionIndex];
	unsigned int posX = nowData.directionY ? pImageIndex / abs(mVarManager.GetVal(nowData.numX)) : pImageIndex % abs(mVarManager.GetVal(nowData.numY));
	unsigned int posY = nowData.directionY ? pImageIndex % abs(mVarManager.GetVal(nowData.numX)) : pImageIndex / abs(mVarManager.GetVal(nowData.numY));
	if (mVarManager.GetVal(nowData.numX) < 0) posX = abs(mVarManager.GetVal(nowData.numX)) - posX - 1;
	if (mVarManager.GetVal(nowData.numY) < 0) posY = abs(mVarManager.GetVal(nowData.numY)) - posY - 1;

	ans.r = 255; ans.g = 255; ans.b = 255;
	ans.w = mVarManager.GetVal(nowData.w) / abs(mVarManager.GetVal(nowData.numX));
	ans.h = mVarManager.GetVal(nowData.h) / abs(mVarManager.GetVal(nowData.numY));
	ans.x = mVarManager.GetVal(nowData.x) + ans.w * posX;
	ans.y = mVarManager.GetVal(nowData.y) + ans.h * posY;
	ans.imageID = mVarManager.GetVal(nowData.imageID);
	return ans;
}


// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
// [prm]pTimerReader	: �^�C�}�[�l�Ăяo���p�֐��|�C���^
CImageSourceDefault::CImageSourceDefault(const long long* (* const pTimerReader)(std::string), CEffectVariableManager& pVarManager)
	: IImageSourceManager(pTimerReader, pVarManager) {
}

// [act]������z��"pReadData"����src�f�[�^���擾����
// [prm]pReadData			: �������pcsv�����f�[�^
//		pVariableManager	: �ϐ��Ǘ��p�֐����w�聨�l�̓|�C���^�ŊǗ�����
// [ret]�f�[�^�擾�ɐ����������ǂ���
bool CImageSourceDefault::Init(StringArr pReadData) {
	try {
		if (pReadData.size() < 10) throw ErrLessCSVDefinition(pReadData, 10);
		if (pReadData.size() < 12 && mCommonData.empty()) throw ErrLessCSVDefinition(pReadData, 12);
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}
	return IImageSourceManager::Init(pReadData);
}

// [act]�摜�ǂݍ��ݎQ�Ɛ��Ԃ�
// [prm]pWriteIndex	: �����ڂ̕`��摜�̎��o�����s�������w��(������default�ł͕s�g�p)
//		pWriteNum	: ���s���̕`��ŉ���摜���J��Ԃ��`�悷�邩�w��(������default�ł͕s�g�p)
SDrawImageSourceData CImageSourceDefault::GetImageSource(int pWriteIndex, int pWriteNum) {
	const auto dataIndex = GetDefinitionIndex();
	if (dataIndex < 0) return SDrawImageSourceData();
	const auto imageIndex = GetImageIndex(dataIndex);
	if (imageIndex < 0) return SDrawImageSourceData();

	return GetSourceDataFromIndex(dataIndex, imageIndex);
}


CImageSourceNumber::CImageSourceNumber(const long long* (* const pTimerReader)(std::string), CEffectVariableManager& pVarManager)
	: IImageSourceManager(pTimerReader, pVarManager) {
	mPaddingFlag = false; mDrawMinusFlag = false;
	mNumSource = -1;
	mNumAlign = EAlign::eRight;
	mDigitCount = 10;
}

bool CImageSourceNumber::Init(StringArr pReadData) {
	try {
		if (pReadData.size() < 10) throw ErrLessCSVDefinition(pReadData, 10);
		if (pReadData.size() < 16 && mCommonData.empty()) throw ErrLessCSVDefinition(pReadData, 16);
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}

	if (mCommonData.empty()) {
		try {
			mNumSource		= mVarManager.MakeValID(pReadData[12]);
			mNumAlign		= pReadData[13] == "L" ? EAlign::eLeft : EAlign::eRight;
			mPaddingFlag	= pReadData[14] == "T";
			mDrawMinusFlag	= pReadData[15] == "T";
		}
		catch (ErrUndeclaredVar e) {
			e.WriteErrLog();
			return false;
		}

		if (mPaddingFlag) ++mDigitCount;
		if (mDrawMinusFlag) mDigitCount = (mDigitCount + 1) * 2;
	}
	return IImageSourceManager::Init(pReadData);
}

int CImageSourceNumber::GetComaNum(int pDefinitionIndex) {
	const int comaNum = IImageSourceManager::GetComaNum(pDefinitionIndex);
	if (comaNum < 0) return -1;
	return comaNum / mDigitCount;
}

SDrawImageSourceData CImageSourceNumber::GetImageSource(int pWriteIndex, int pWriteNum) {
	if (pWriteIndex < 0 || pWriteIndex >= pWriteNum) return SDrawImageSourceData();
	const auto dataIndex = GetDefinitionIndex();
	if (dataIndex < 0) return SDrawImageSourceData();
	const auto imageIndex = GetImageIndex(dataIndex);
	if (imageIndex < 0) return SDrawImageSourceData();
	// ���l���}�C�i�X�����̒l�������Ȃ��ꍇ�͕`�悵�Ȃ�
	if (!mDrawMinusFlag && mVarManager.GetVal(mNumSource) < 0) return SDrawImageSourceData();
	int numIndex;

	/* ���l���擾���ĕ`�悷��R�}�����肷�� */ {
		int nowVal = abs(mVarManager.GetVal(mNumSource));
		int digitNum;
		if (nowVal == 0) digitNum = 1;
		else for (digitNum = 0; nowVal > 0; ++digitNum) nowVal /= 10;

		// pWriteIndex���E�[��ɕϊ�����
		pWriteIndex = pWriteNum - pWriteIndex - 1;

		/* Align�����̏ꍇ��pWriteIndex�𒲐����� */
		if(mNumAlign == EAlign::eLeft && !mPaddingFlag) {
			pWriteIndex = digitNum - pWriteIndex + mDrawMinusFlag ? 1 : 0;
			if (pWriteIndex < 0 || pWriteIndex >= pWriteNum) return SDrawImageSourceData();
		}

		bool drawSignFlag = mPaddingFlag && mDrawMinusFlag && pWriteIndex + 1 == pWriteNum;
		drawSignFlag |= !mPaddingFlag && mDrawMinusFlag && pWriteIndex == digitNum;
		drawSignFlag |= digitNum >= pWriteNum && pWriteIndex + 1 == pWriteNum;
		bool drawPaddingFlag = mPaddingFlag && pWriteIndex >= digitNum && !drawSignFlag;

		// �`�悵�Ȃ��ꍇ
		if (pWriteIndex >= digitNum && !drawSignFlag && !drawPaddingFlag) return SDrawImageSourceData();
		// ������`�悷��ꍇ
		else if (drawSignFlag) numIndex = (mPaddingFlag ? 10 : 11);
		// �p�f�B���O��`�悷��ꍇ
		else if (drawPaddingFlag) numIndex = 10;
		// else:���l��`�悷��ꍇ
		else {
			nowVal = abs(mVarManager.GetVal(mNumSource));
			for (auto i = 0; i < pWriteIndex; ++i) nowVal /= 10;
			numIndex = nowVal % 10;
		}
		numIndex += (mVarManager.GetVal(mNumSource) < 0 ? mDigitCount / 2 : 0);
	}

	return GetSourceDataFromIndex(dataIndex, imageIndex * mDigitCount + numIndex);
}