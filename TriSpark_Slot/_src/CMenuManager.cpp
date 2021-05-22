#include "_header/CMenuManager.hpp"
#include "_header/keyexport.h"
#include "_header/CGameDataManage.h"
#include "DxLib.h"
#include <cmath>

CMenuManager::CMenuManager() {
	mMenuElement = nullptr;
	mMenuStartTime = -1;
	mMenuFinishTime = -1;
	mBaseImgID = -1;
	mTitleFontHandle = -1;
	mDataFontHandle = -1;
	mDataFontHandleMid = -1;
	mLicenseTXT = -1;
}

bool CMenuManager::Init(CGameDataManage& pDataManageIns, const int pLicenseFileID, const int pDataFontHandle, const int pDataFontHandleMid, const int pBaseImgID, const int pTitleFontHandle) {
	mBaseImgID = pDataManageIns.GetDataHandle(pBaseImgID);
	mTitleFontHandle = pDataManageIns.GetDataHandle(pTitleFontHandle);
	mDataFontHandle = pDataManageIns.GetDataHandle(pDataFontHandle);
	mDataFontHandleMid = pDataManageIns.GetDataHandle(pDataFontHandleMid);
	mLicenseTXT = pDataManageIns.GetDataHandle(pLicenseFileID);
	return true;
}

bool CMenuManager::Process(CGameDataManage& pDataManageIns, SSlotGameDataWrapper& pSlotData) {
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	if (key.GetExportStatus() == EKeyExportStatus::eGameMain) {
		if (key.ExportKeyState(KEY_INPUT_M)) {
			// initialize
			mMenuElement = new CMenuHowTo(pDataManageIns, mBaseImgID, mTitleFontHandle);
			mMenuElement->Init();
			key.SetExportStatus(EKeyExportStatus::eMenu);
			mMenuStartTime = DxLib::GetNowCount();
		}
	}
	else if (key.GetExportStatus() == EKeyExportStatus::eMenu)
	if (mMenuElement != nullptr) {
		for (int a = 0; a < 256; ++a) {
			if (key.ExportKeyState(a)) {
				const auto nextMode = mMenuElement->PushButton(a);
				switch (nextMode) {
				case EMenuList::eLicense:
					delete mMenuElement;
					mMenuElement = nullptr;
					mMenuElement = new CMenuLicenses(mLicenseTXT, mDataFontHandle, mBaseImgID, mTitleFontHandle);
					mMenuElement->Init();
					break;
				case EMenuList::eReelHistory:
					delete mMenuElement;
					mMenuElement = nullptr;
					mMenuElement = new CMenuReelHistory(pDataManageIns, mDataFontHandle, mDataFontHandleMid, mBaseImgID, pSlotData.reelManager, mTitleFontHandle);
					mMenuElement->Init();
					break;
				case EMenuList::eBonusHistory:
					delete mMenuElement;
					mMenuElement = nullptr;
					mMenuElement = new CMenuBonusHistory(pDataManageIns, mDataFontHandle, mDataFontHandleMid, mBaseImgID, pSlotData.dataCounter, mTitleFontHandle);
					mMenuElement->Init();
					break;
				case EMenuList::eHowTo:
					delete mMenuElement;
					mMenuElement = nullptr;
					mMenuElement = new CMenuHowTo(pDataManageIns, mBaseImgID, mTitleFontHandle);
					mMenuElement->Init();
					break;
				}
			}
		}
		if (mMenuFinishTime < 0)
		if (key.ExportKeyState(KEY_INPUT_M) || key.ExportKeyState(KEY_INPUT_ESCAPE)){
			mMenuFinishTime = DxLib::GetNowCount();
		}

		const int nowCount = DxLib::GetNowCount();
		if (mMenuStartTime > 0 && nowCount - mMenuStartTime >= 250) mMenuStartTime = -1;
		if (mMenuFinishTime > 0 && nowCount - mMenuFinishTime >= 250) {
			delete mMenuElement;	
			mMenuElement = nullptr;
			mMenuFinishTime = -1;
			key.SetExportStatus(EKeyExportStatus::eGameMain);
		}
	}

	return true;
}

bool CMenuManager::Draw() {
	if (mMenuElement == nullptr) return true;

	int opacity = 255;
	const int nowCount = DxLib::GetNowCount();
	if (mMenuStartTime >= 0) opacity = floorf(255.f * ((nowCount - mMenuStartTime) / 250.f));
	if (mMenuFinishTime >= 0) opacity = 255 - floorf(255.f * ((nowCount - mMenuFinishTime) / 250.f));

	mMenuElement->Draw(opacity);
	return true;
}

CMenuManager::~CMenuManager() {
	delete mMenuElement;
}