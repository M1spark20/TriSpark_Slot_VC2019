#pragma once
#include "SReelHistoryData.hpp"
#include <deque>
class CRestoreManagerRead;
class CRestoreManagerWrite;

class CReelHistoryManager {
	static const size_t				RECORD_NUM = 30;
	std::deque<SReelHistoryData>	m_historyData;

public:
	bool Init();
	void AddData(const SReelHistoryData pData);
	size_t GetHistorySize() const { return m_historyData.size(); }
	SReelHistoryData GetData(int pIndex) const;

	bool ReadRestore(CRestoreManagerRead& pReader);
	bool WriteRestore(CRestoreManagerWrite& pWriter) const;
};
