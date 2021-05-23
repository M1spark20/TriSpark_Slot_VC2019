#pragma once
#include "IMenuElements.hpp"
#include "SSlotGameDataWrapper.hpp"
class CGameDataManage;

class CMenuManager {
	IMenuElements*	mMenuElement;
	int				mMenuStartTime;
	int				mMenuFinishTime;
	EMenuList		mResumeMenu;

	int				mBaseImgID;
	int				mTitleFontHandle;
	int				mDataFontHandle;
	int				mDataFontHandleMid;
	int				mLicenseTXT;

	bool ResetMenuContents(EMenuList pNext, CGameDataManage& pDataManageIns, SSlotGameDataWrapper& pSlotData);

public:
	CMenuManager();
	bool Init(CGameDataManage& pDataManageIns, const int pLicenseFileID, const int pDataFontHandle, const int pDataFontHandleMid, const int pBaseImgID, const int pTitleFontHandle);
	bool Process(CGameDataManage& pDataManageIns, SSlotGameDataWrapper& pSlotData);
	bool Draw();
	~CMenuManager();
};