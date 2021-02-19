#pragma once
#include "CEffectImageCommonComponent.hpp"
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CGameDataManage;

class CImageColorManager : public CEffectImageCommonComponent {
	std::vector<SImageColorCSVCommonData> mCommonData;		// csvから読みだしたsrcデータ、複数定義可能
	std::string mDataSetName;

	typedef std::vector<std::string> StringArr;

	// [act]ループ点を考慮した操作に使用する時間を割り出す
	long long GetCheckTime(const long long pNowCount) const;
	// [act]タイマー状況から読み出しタイミングで使用する画像定義を決定する
	int GetDefinitionIndex() const;
	// [act]タイマー状況から読み出しタイミングで使用する画像コマを決定する
	double GetImageIndex(int pDefinitionIndex) const;
	// [act]アニメーションに使用できるコマ数を取得する(必要に応じoverrideする)
	virtual int GetComaNum(int pDefinitionIndex) const;
	// [act]definitionIndexとimageIndexから画像範囲を取り出す
	bool GetColorDataFromIndex(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pDefinitionIndex, int pImageIndex, int pColorIndex) const;
	// [act]次のアニメーションコマを取得する
	void GetAnimationNext(int pNowDef, int pNowImg, int& pNextDef, int& pNextImg) const;

public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	CImageColorManager(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	bool	Init(StringArr pReadData, CSlotTimerManager& pTimerManager);
	// [act]画像読み込み参照先を返す
	bool	GetColorData(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pWriteIndex) const;

	// [act]エフェクト照合関連関数
	void	SetDataName(std::string pName) { mDataSetName = pName; }
	bool	MatchDataName(std::string pKey) { return pKey == mDataSetName; }
};
