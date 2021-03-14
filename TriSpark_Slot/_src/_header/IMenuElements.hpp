#pragma once
#include <string>
#include <vector>
class CGameDataManage;
class CMenuReadHowtoFromCSV;

enum class EMenuList {
	eContinue,
	eLicense,
	eReelHistory,
	eHowTo,
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
	const int				mFontHandle;
public:
	CMenuReelHistory(const int pDataFontHandle, const int pBaseImgID, const int pTitleFontHandle);	// : IMenuElements("Stop History") {}
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