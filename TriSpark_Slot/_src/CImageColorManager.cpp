#include "_header/CImageColorManager.hpp"
#include "_header/ErrClass.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CGameDataManage.h"
#include "DxLib.h"

CImageColorManager::CImageColorManager(CEffectVariableManager& pVarManager)
	: CEffectImageCommonComponent(pVarManager) {
	mCommonData.clear();
}

// [act]������z��"pReadData"����src�f�[�^���擾����
// [prm]pReadData			: �������pcsv�����f�[�^
//		pVariableManager	: �ϐ��Ǘ��p�֐����w�聨�l�̓|�C���^�ŊǗ�����
// [ret]�f�[�^�擾�ɐ����������ǂ���
bool CImageColorManager::Init(StringArr pReadData, CSlotTimerManager& pTimerManager) {
	if (pReadData.size() < 11) throw ErrLessCSVDefinition(pReadData, 13);
	if (pReadData.size() < 13 && mCommonData.empty()) throw ErrLessCSVDefinition(pReadData, 16);

	try {
		SImageColorCSVCommonData data;
		data.startTime = mVarManager.MakeValID(pReadData[1]);
		data.imageID = mVarManager.MakeValID(pReadData[2]);
		data.x = mVarManager.MakeValID(pReadData[3]);
		data.y = mVarManager.MakeValID(pReadData[4]);
		data.w = mVarManager.MakeValID(pReadData[5]);
		data.h = mVarManager.MakeValID(pReadData[6]);
		data.numX = mVarManager.MakeValID(pReadData[7]);
		data.numY = mVarManager.MakeValID(pReadData[8]);
		data.directionY = pReadData[9] == "Y";
		data.isColorIndexDirY = pReadData[10] == "Y";
		data.useAnimation = pReadData[11] == "T";
		data.loopCount = mVarManager.MakeValID(pReadData[12]);

		if (mCommonData.empty()) {
			mTimerID = pTimerManager.GetTimerHandle(pReadData[13]);
			mLoopTime = mVarManager.MakeValID(pReadData[14]);
			mDataSetName = pReadData[15];
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
long long CImageColorManager::GetCheckTime(const long long pNowCount) const{
	const long long loopTime = mVarManager.GetVal(mLoopTime);
	if (pNowCount < loopTime || loopTime < 0) return pNowCount;

	if (mCommonData.empty()) throw ErrInternalVarUndeclared("mCommonData");
	const int loopPoint = mVarManager.GetVal((mCommonData.end() - 1)->startTime);
	const int diffCount = loopPoint - loopTime;
	if (diffCount == 0) return loopTime;
	long long ans = pNowCount - loopTime;
	return ans - (diffCount * (ans / diffCount)) + loopTime;
}

// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜��`�����肷��
// [ret]-1	:����͕`����s���^�C�~���O�ł͂Ȃ��ꍇ
//		else:�`�悷���`ID @mCommonData
int CImageColorManager::GetDefinitionIndex() const{
	if (mCommonData.empty()) return -1;
	if (!GetIsTimerSet() || !GetIsTimerEnable()) return -1;
	const auto definitionNum = mCommonData.size();
	const int loopTime = mVarManager.GetVal(mLoopTime);

	try {
		const long long nowTime = GetTimer();
		const long long checkTime = GetCheckTime(nowTime);

		// ���Ă���v�f��beginTime�ɖ��B�Ȃ炻�̑O�̃f�[�^���g�p����B��1�v�f�ɖ��B�Ȃ�`����s��Ȃ�
		int ans = -1;
		for (auto it = mCommonData.begin(); it != mCommonData.end(); ++it, ++ans)
			if (checkTime < (long long)mVarManager.GetVal(it->startTime)) return ans;

		// ���[�v�_������΍Ō�̗v�f��`��A�Ȃ���Ε`����s��Ȃ�
		return loopTime >= 0 ? ans : -1;
	}
	catch (ErrInternalVarUndeclared e) {
		e.WriteErrLog();
		return -1;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return -1;
	}
}

// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����摜�R�}�����肷��
// [prm]pDefinitionIndex	: �`�悷���`ID @mCommonData
// [ret]-1	: �G���[
//		else: �摜�R�}ID
double CImageColorManager::GetImageIndex(int pDefinitionIndex) const{
	if (pDefinitionIndex < 0 || (size_t)pDefinitionIndex >= mCommonData.size()) return -1.;

	const int comaNum = GetComaNum(pDefinitionIndex);
	if (comaNum == 0) return 0;
	if (comaNum == -1) return -1.;
	const long long offset = mVarManager.GetVal(mCommonData[pDefinitionIndex].startTime);
	const long long interval = -offset + ((size_t)pDefinitionIndex + 1 == mCommonData.size() ?
		mVarManager.GetVal(mLoopTime) : mVarManager.GetVal(mCommonData[(size_t)pDefinitionIndex + 1].startTime));
	if (interval == 0) return 0.;	// src���ω����Ȃ��ꍇ
	const double division = interval / (double)comaNum;

	try {
		const long long operateTime = GetCheckTime(GetTimer()) - offset;
		return operateTime / division;
	}
	catch (ErrInternalVarUndeclared e) {
		e.WriteErrLog();
		return -1.;
	}
}

// [act]�A�j���[�V�����Ɏg�p�ł���R�}�����擾����(�K�v�ɉ���override����)
// [prm]pDefinitionIndex	: �`�悷���`ID @mCommonData
// [ret]-1	: �G���[
//		else: ���p�\�R�}��
int CImageColorManager::GetComaNum(int pDefinitionIndex) const{
	if (pDefinitionIndex < 0 || (size_t)pDefinitionIndex >= mCommonData.size()) return -1;
	const int hoge = mVarManager.GetVal(mCommonData[pDefinitionIndex].loopCount);
	return abs(mVarManager.GetVal(mCommonData[pDefinitionIndex].numX)) * abs(mVarManager.GetVal(mCommonData[pDefinitionIndex].numY)) * mVarManager.GetVal(mCommonData[pDefinitionIndex].loopCount);
}


// [act]definitionIndex��imageIndex����F�������o��
bool CImageColorManager::GetColorDataFromIndex(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pDefinitionIndex, int pImageIndex, int pColorIndex) const{
	// index�ɉ����ĉ摜��؂�o��
	const auto& nowData = mCommonData[pDefinitionIndex];
	const int width = mVarManager.GetVal(nowData.w);
	const int height = mVarManager.GetVal(nowData.h);
	if (pColorIndex < 0 || pColorIndex >= abs(width) * abs(height)) {
		pData.r = 0;
		pData.g = 0;
		pData.b = 0;
		return false;
	}

	const unsigned int baseImageIndexNum = abs(mVarManager.GetVal(nowData.numX)) * abs(mVarManager.GetVal(nowData.numY));
	const unsigned int imageIndexForComa = pImageIndex % baseImageIndexNum;
	unsigned int posX = nowData.directionY ? imageIndexForComa / abs(mVarManager.GetVal(nowData.numY)) : imageIndexForComa % abs(mVarManager.GetVal(nowData.numX));
	unsigned int posY = nowData.directionY ? imageIndexForComa % abs(mVarManager.GetVal(nowData.numY)) : imageIndexForComa / abs(mVarManager.GetVal(nowData.numX));
	if (mVarManager.GetVal(nowData.numX) < 0) posX = abs(mVarManager.GetVal(nowData.numX)) - posX - 1;
	if (mVarManager.GetVal(nowData.numY) < 0) posY = abs(mVarManager.GetVal(nowData.numY)) - posY - 1;
	posX *= (mVarManager.GetVal(nowData.w) / abs(mVarManager.GetVal(nowData.numX)));
	posY *= (mVarManager.GetVal(nowData.h) / abs(mVarManager.GetVal(nowData.numY)));

	unsigned int indexX = nowData.isColorIndexDirY ? pColorIndex / abs(height) : pColorIndex % abs(width);
	unsigned int indexY = nowData.isColorIndexDirY ? pColorIndex % abs(height) : pColorIndex / abs(width);
	if (width < 0) indexX = abs(width) - indexX - 1;
	if (height < 0) indexY = abs(height) - indexY - 1;
	indexX += mVarManager.GetVal(nowData.x);
	indexY += mVarManager.GetVal(nowData.y);

	int grHandle = pGameData.GetDataHandle(mVarManager.GetVal(nowData.imageID));
	int ans = DxLib::GetPixelSoftImage(grHandle, posX + indexX, posY + indexY, &pData.r, &pData.g, &pData.b, NULL);
	return ans == 0;
}

// [act]���̃A�j���[�V�����R�}���擾����
// [prm]pNowDef		: ���݂�definitionID
//		pNowImg		: ���݂�imageID
//		pNextDef	: �����definitionID�i�[��(��`���Ȃ��ꍇ-1)
//		pNextImg	: �����imageID�i�[��(��`���Ȃ��ꍇ-1)
void CImageColorManager::GetAnimationNext(int pNowDef, int pNowImg, int& pNextDef, int& pNextImg) const{
	pNextDef = pNowDef; pNextImg = pNowImg + 1;
	if (pNextImg < GetComaNum(pNowDef)) return;

	pNextDef = pNowDef+1; pNextImg = 0;
	if ((size_t)pNextDef < mCommonData.size()) return;

	pNextDef = mCommonData.size() - 1;
	for (size_t i = 0; i < mCommonData.size(); ++i) {
		if (mVarManager.GetVal(mCommonData[i].startTime) <= mVarManager.GetVal(mLoopTime)) continue;
		pNextDef = i - 1;
		if (pNextDef < 0) pNextImg = -1;
		return;
	}
}

// [act]�摜�ǂݍ��ݎQ�Ɛ�ɐF����t�^����
// [prm]pWriteIndex	: �����ڂ̕`��摜�̎��o�����s�������w��
bool CImageColorManager::GetColorData(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pWriteIndex) const{
	const auto dataIndex = GetDefinitionIndex();
	if (dataIndex < 0) return false;
	const auto imageIndex = GetImageIndex(dataIndex);
	if (imageIndex < 0) return false;

	if (!GetColorDataFromIndex(pGameData, pData, dataIndex, static_cast<int>(imageIndex), pWriteIndex)) return false;

	// �A�j���[�V��������
	if (mCommonData[dataIndex].useAnimation) {
		int nextDef, nextImg;
		GetAnimationNext(dataIndex, static_cast<int>(imageIndex), nextDef, nextImg);
		if (nextDef < 0) {
			pData.r = CalcAnimationPos(pData.r, 0, imageIndex);
			pData.g = CalcAnimationPos(pData.g, 0, imageIndex);
			pData.b = CalcAnimationPos(pData.b, 0, imageIndex);
		}
		else {
			SDrawImageSourceData nextColor;
			if (!GetColorDataFromIndex(pGameData, nextColor, nextDef, nextImg, pWriteIndex)) return false;
			pData.r = CalcAnimationPos(pData.r, nextColor.r, imageIndex);
			pData.g = CalcAnimationPos(pData.g, nextColor.g, imageIndex);
			pData.b = CalcAnimationPos(pData.b, nextColor.b, imageIndex);
		}
	}

	return true;
}
