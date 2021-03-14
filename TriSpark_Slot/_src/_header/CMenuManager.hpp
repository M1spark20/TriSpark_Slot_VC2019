#pragma once
#include "IMenuElements.hpp"
class CGameDataManage;

class CMenuManager {
	IMenuElements*	mMenuElement;
	int				mMenuStartTime;
	int				mMenuFinishTime;

	int				mBaseImgID;
	int				mTitleFontHandle;
	int				mDataFontHandle;
	int				mLicenseTXT;

public:
	CMenuManager();
	bool Init(CGameDataManage& pDataManageIns, const int pLicenseFileID, const int pDataFontHandle, const int pBaseImgID, const int pTitleFontHandle);
	bool Process(CGameDataManage& pDataManageIns);
	bool Draw();
	~CMenuManager();
};