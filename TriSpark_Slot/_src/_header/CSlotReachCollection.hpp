#pragma once
#include "SReachCollectionData.hpp"
class CGameDataManage;
class CRestoreManagerRead;
class CRestoreManagerWrite;
class CReelManager;
class CSlotInternalDataManager;

class CSlotReachCollectionData {
	SReachCollectionData mCollectionData;
	int mListImageID;
	int mSecretImageID;
	std::vector<SReachCollectionElement*> mLatch;

public:
	bool Init(const CGameDataManage& pDataManager, int pListFileID, int reelNum);
	bool JudgeCollection(const CReelManager& pReelData, const CSlotInternalDataManager& pInternal);
	bool Latch(bool isRefresh);

	int GetCollectionNum() const { return mCollectionData.elem.size(); }
	int GetCompCount(int pIndex) const;
	std::string GetFirstCompDate(int pIndex) const;
	std::string GetLastCompDate(int pIndex) const;
	bool Draw(int pBeginPos, int pOffsetX, int pOffsetY, int pFontHandle) const;

	bool ReadRestore(CRestoreManagerRead& pReader);
	bool WriteRestore(CRestoreManagerWrite& pWriter) const;
};