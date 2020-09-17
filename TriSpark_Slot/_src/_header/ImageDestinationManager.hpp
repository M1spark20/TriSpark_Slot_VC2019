#pragma once
#include "CEffectImageCommonComponent.hpp"
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CEffectVariableManager;
class CSlotTimerManager;
class IImageSourceManager;
class CGameDataManage;

class IImageDestinationManager : public CEffectImageCommonComponent {
private:
	EDrawModeForDST  GetDrawEnum(std::string pIndex);
	EBlendModeForDST GetBlendEnum(std::string pIndex);

protected:
	std::vector<SImageDestCSVCommonData> mCommonData;		// csvから読みだしたdstデータ、複数定義可能

	typedef std::vector<std::string> StringArr;

	// [act]ループ点を考慮した操作に使用する時間を割り出す
	long long GetCheckTime(const long long pNowCount);
	// [act]タイマー状況から読み出しタイミングで使用する描画定義を決定する
	int GetDefinitionIndex();

	int GetDxDrawModeByEnum(EDrawModeForDST pData);
	int GetDxBlendModeByEnum(EBlendModeForDST pData);
	bool GetCanDrawDirectly(EBlendModeForDST pData);

public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	IImageDestinationManager(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	virtual bool	Init(StringArr pReadData, CSlotTimerManager& pTimerData);
	// [act]
	virtual void	Draw(IImageSourceManager *const pSourceData, CGameDataManage& pDataManager) = 0;
};

class CImageDestinationDefault : public IImageDestinationManager {
	int mDrawNum;
	int mDiffX, mDiffY;

public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	CImageDestinationDefault(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	bool	Init(StringArr pReadData, CSlotTimerManager& pTimerData) override;
	// [act]描画を行う
	void	Draw(IImageSourceManager *const pSourceData, CGameDataManage& pDataManager) override;
};