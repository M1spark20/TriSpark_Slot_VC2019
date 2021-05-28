#pragma once
#include <string>
#include <vector>
#include <deque>
#include "SReelHistoryData.hpp"
#include "CMenuReadBonusTypeFromCSV.hpp"
#include "SSlotDataCounterComponent.hpp"
class CGameDataManage;
class CMenuReadHowtoFromCSV;
class CReelManager;
class CSlotDataCounter;
class CSlotReachCollectionData;

enum class EMenuList {
	eContinue,
	eLicense,
	eReelHistory,
	eHowTo,
	eBonusHistory,
	eReachPatternCollection,
	eMenuMax
};

class IMenuElements {
	const std::string	cElementName;
	const int			mBaseImgID;
	const int			mTitleFontHandle;

protected:
	bool DrawBase(const int pOpacity);
public:
	IMenuElements(const std::string pName, const int pBaseImgID, const int pTitleFontHandle);
	virtual bool		Init() = 0;
	virtual bool		Process() = 0;
	virtual bool		Draw(const int pOpacity) = 0;
	virtual EMenuList	PushButton(int pKeyHandleDX) = 0;

	virtual ~IMenuElements(){}
};

class CMenuLicenses : public IMenuElements {
	const static int BUFSIZE = 2048;
	const static int MOVE_AMOUNT = 20;

	const int				mLicenseFileID;
	const int				mFontHandle;
	int						mCurrentStartCol;
	std::vector<long long>	mSeekPosList;

public:
	CMenuLicenses(const int pLicenseFileID, const int pDataFontHandle, const int pBaseImgID, const int pTitleFontHandle);
	bool Init() override;											// ライセンスファイル読み込み
	bool Process() override;										// none
	bool Draw(const int pOpacity) override;							// ライセンスファイル内容表示(DrawText)
	EMenuList PushButton(int pKeyHandleDX) override;				// モード遷移/スクロール
};

class CMenuReelHistory : public IMenuElements {
	const int						mFontHandle;
	const int						mFontHandleMid;
	int								mHistBaseImgID;
	int								mReelImgID;
	std::vector<SReelHistoryData>	mHistoryData;
	size_t							mNowPage;

public:
	CMenuReelHistory(CGameDataManage& pGameData, const int pDataFontHandle, const int pDataFontHandleMid, const int pBaseImgID, const CReelManager& pReelData, const int pTitleFontHandle);	// : IMenuElements("Stop History") {}
	bool Init() override;						// ライセンスファイル読み込み
	bool Process() override;					// none
	bool Draw(const int pOpacity) override;		// ライセンスファイル内容表示(DrawText)
	EMenuList PushButton(int pKeyHandleDX) override;	// モード遷移/スクロール
};

class CMenuHowTo : public IMenuElements {
	std::vector<int>	mImages;
	int					mPageNum;
public:
	CMenuHowTo(CGameDataManage& pGameData, const int pBaseImgID, const int pTitleFontHandle);	// : IMenuElements("Stop History") {}
	bool Init() override;						// ライセンスファイル読み込み
	bool Process() override;					// none
	bool Draw(const int pOpacity) override;		// ライセンスファイル内容表示(DrawText)
	EMenuList PushButton(int pKeyHandleDX) override;	// モード遷移/スクロール
};

class CMenuBonusHistory : public IMenuElements {
	static const int								GRAPH_WIDTH = 420;
	const int										mFontHandle;
	const int										mFontHandleMid;
	int												mHistBaseImgID;
	int												mReelImgID;
	std::vector<SSlotDataCounterBonusHistoryData>	mHistoryData;
	std::vector<SMenuReadBonusTypePos>				mTypeData;
	std::deque<int>									mGraphData;
	int												mSelecting;
	float											mGraphDrawRate;
	int												mGraphRange;

	SMenuReadBonusTypePos GetBonusType(int bonusType);

public:
	CMenuBonusHistory(CGameDataManage& pGameData, const int pDataFontHandle, const int pDataFontHandleMid, const int pBaseImgID, const CSlotDataCounter& pSlotData, const int pTitleFontHandle);	// : IMenuElements("Stop History") {}
	bool Init() override;						// ライセンスファイル読み込み
	bool Process() override;					// none
	bool Draw(const int pOpacity) override;		// ライセンスファイル内容表示(DrawText)
	EMenuList PushButton(int pKeyHandleDX) override;	// モード遷移/スクロール
};

class CMenuReachCollection : public IMenuElements {
	static const int				PAGE_MOVE = 12;
	const int						mFontHandle;
	size_t							mNowPage;
	const CSlotReachCollectionData& mCollectionData;

public:
	CMenuReachCollection(CGameDataManage& pGameData, const int pDataFontHandle, const int pBaseImgID, const CSlotReachCollectionData& pColleData, const int pTitleFontHandle);	// : IMenuElements("Stop History") {}
	bool Init() override;						// ライセンスファイル読み込み
	bool Process() override;					// none
	bool Draw(const int pOpacity) override;		// ライセンスファイル内容表示(DrawText)
	EMenuList PushButton(int pKeyHandleDX) override;	// モード遷移/スクロール
};
