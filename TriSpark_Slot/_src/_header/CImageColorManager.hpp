#pragma once
#include "CEffectImageCommonComponent.hpp"
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CGameDataManage;

class CImageColorManager : public CEffectImageCommonComponent {
	std::vector<SImageSourceCSVCommonData> mCommonData;		// csvから読みだしたsrcデータ、複数定義可能
	bool mIsColorIndexDirY;

	typedef std::vector<std::string> StringArr;

	// [act]ループ点を考慮した操作に使用する時間を割り出す
	long long GetCheckTime(const long long pNowCount);
	// [act]タイマー状況から読み出しタイミングで使用する画像定義を決定する
	int GetDefinitionIndex();
	// [act]タイマー状況から読み出しタイミングで使用する画像コマを決定する
	double GetImageIndex(int pDefinitionIndex);
	// [act]アニメーションに使用できるコマ数を取得する(必要に応じoverrideする)
	virtual int GetComaNum(int pDefinitionIndex);
	// [act]definitionIndexとimageIndexから画像範囲を取り出す
	bool GetColorDataFromIndex(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pDefinitionIndex, int pImageIndex, int pColorIndex);

	void GetAnimationNext(int pNowDef, int pNowImg, int& pNextDef, int& pNextImg);

	int CalcColorAnimation(int pBeginVal, int pEndVal, double pProgress);

public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	CImageColorManager(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	bool	Init(StringArr pReadData, CSlotTimerManager& pTimerManager);
	// [act]画像読み込み参照先を返す
	bool	GetColorData(const CGameDataManage& pGameData, SDrawImageSourceData& pData, int pWriteIndex);
};