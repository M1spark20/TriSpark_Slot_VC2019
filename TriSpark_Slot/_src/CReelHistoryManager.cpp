#include "_header/CReelHistoryManager.hpp"
#include "_header/CRestoreManager.hpp"

bool CReelHistoryManager::Init() {
	return true;
}

void CReelHistoryManager::AddData(const SReelHistoryData pData) {
	while (m_historyData.size() >= RECORD_NUM) m_historyData.pop_back();
	m_historyData.push_front(pData);
}

SReelHistoryData CReelHistoryManager::GetData(int pIndex) const {
	if (pIndex < 0 || (size_t)pIndex >= GetHistorySize()) return SReelHistoryData();
	return m_historyData[pIndex];
}

bool CReelHistoryManager::ReadRestore(CRestoreManagerRead& pReader) {
	size_t records = 0;
	if (!pReader.ReadNum(records)) return false;
	for (size_t i = 0; i < records; ++i) {
		int reelNum = 0;
		SReelHistoryData addData;
		if (!pReader.ReadNum(reelNum)) return false;
		if (reelNum < 0) return false;
		addData.reelPos.resize(reelNum, -1);
		addData.slipCount.resize(reelNum, 0);
		for (size_t j = 0; j < (size_t)reelNum; ++j) {
			if (!pReader.ReadNum(addData.reelPos[j])) return false;
			if(pReader.GetReadVer() >= 2) if (!pReader.ReadNum(addData.slipCount[j])) return false;
		}
		if (!pReader.ReadNum(addData.betNum)) return false;
		if (!pReader.ReadNum(addData.firstStop)) return false;
		m_historyData.push_back(addData);
	}
	return true;
}

bool CReelHistoryManager::WriteRestore(CRestoreManagerWrite& pWriter) const {
	if (!pWriter.WriteNum(m_historyData.size())) return false;
	for (size_t i = 0; i < m_historyData.size(); ++i) {
		const auto& data = m_historyData[i];
		if (!pWriter.WriteNum((int)data.reelPos.size())) return false;
		for (size_t j = 0; j < data.reelPos.size(); ++j) {
			if (!pWriter.WriteNum(data.reelPos[j])) return false;
			if (!pWriter.WriteNum(data.slipCount[j])) return false;
		}
		if (!pWriter.WriteNum(data.betNum)) return false;
		if (!pWriter.WriteNum(data.firstStop)) return false;
	}
	return true;
}
