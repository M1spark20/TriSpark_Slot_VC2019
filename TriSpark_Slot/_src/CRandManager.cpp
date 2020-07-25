#include "_header\CRandManager.hpp"
#include "_header\CRandomizerReaderFromCSV.hpp"
#include "_header\CGameDataManage.h"
#include "_header\CSlotInternalDataManager.hpp"

bool CRandManager::Init(const CGameDataManage& pDataManager, int pFileID, long pRandSeed){
	CRandomizerReaderFromCSV reader;
	if (!reader.FileInit(pDataManager.GetDataHandle(pFileID))) return false;
	if (!reader.MakeData(m_flagData)) return false;
	SetRandSeed(pRandSeed);
	return true;
}

int CRandManager::GetFlagData(int pFlagID, EFlagType pType) const{
	if (pFlagID < 0 || pFlagID >= GetFlagNum()) return -1;
	if (pType < 0 || pType >= eFlagTypeMax) return -1;
	return m_flagData.flagType[pFlagID][pType];
}

std::string	CRandManager::GetFlagName(int pFlagID){
	if (pFlagID < 0 || pFlagID >= GetFlagNum()) return "";
	return m_flagData.flagName[pFlagID];
}

void CRandManager::SetRandSeed(long pRandSeed){
	if (pRandSeed == -1){
		std::random_device rd;
		m_randomizer.seed(rd());
	} else {
		m_randomizer.seed(pRandSeed);
	}
}

// [act]�t���O�𒊑I�ɂ�茈�肷��B���ʂ�pData�̒l�ύX�ɂ��B
//		�Ȃ������t���O�͖{�N���X��ʂ����ɒ��ڎw�肪�\�B
// [ret]���I�ɐ���������
// [prm]pData	: �����f�[�^�Ǘ��N���X(�f�[�^�擾&���ʊi�[)
bool CRandManager::Role(CSlotInternalDataManager& pData) {
	const auto data = pData.GetData();
	for (auto it = m_flagData.randomizerData.cbegin(); it != m_flagData.randomizerData.cend(); ++it){
		if (it->set != data.set && it->set != -1) continue;
		if (it->bet != data.betNum && it->bet != -1) continue;
		if (it->gameMode != data.gameMode && it->gameMode != -1) continue;
		if (it->rtMode != data.rtMode.first && it->rtMode != -1) continue;

		// ��������
		int randVal = 0;
		if (it->randMax >= 0){
			std::uniform_int_distribution<int> generator(0, it->randMax - 1);
			randVal = generator(m_randomizer);
		}

		const int temp = randVal;
		// �t���O����
		for (unsigned int i = 0; i < it->randSeed.size(); ++i){
			randVal -= it->randSeed[i];
			if (randVal >= 0) continue;
			// �t���O��RT���[�h(�{�[�i�X����G�̂�/��`�����݂���ꍇ�̂�)��pData�ɃZ�b�g
			pData.SetFlag(m_flagData.flagType[i][eFlagID], m_flagData.flagType[i][eBonusID]);
			if (data.flag.second == 0 && m_flagData.flagType[i][eBonusID] > 0){
				for (auto rtIt = m_flagData.rtModeAtBonusHit.cbegin(); rtIt != m_flagData.rtModeAtBonusHit.cend(); ++rtIt){
					if (rtIt->first != m_flagData.flagType[i][eBonusID]) continue;
					pData.SetRTMode(rtIt->second, false, true);
					break;
				}
			}
			return true;
		}
		return false;
	}
	return false;
}

bool CRandManager::GetBetAvailable(int pGameMode, int pBetNum) const {
	if (pGameMode < 0 || pBetNum <= 0) return false;
	for (auto it = m_flagData.betAvailable.cbegin(); it != m_flagData.betAvailable.cend(); ++it){
		if (it->first == pGameMode) return (it->second & 1 << (pBetNum - 1)) != 0;
	}
	return false;
}

int CRandManager::GetMaxBetNum(int pGameMode) const {
	if (pGameMode < 0) return -1;
	for (auto it = m_flagData.betAvailable.cbegin(); it != m_flagData.betAvailable.cend(); ++it){
		if (it->first == pGameMode){
			int buf = it->second & INT_MAX;	// �Z�p�V�t�g��1���V�t�g�����Ȃ��΍�
			for (int ans = 0; ; ++ans){
				if (buf == 0) return ans;
				buf >>= 1;
			}
		}
	}
	return -1;
}