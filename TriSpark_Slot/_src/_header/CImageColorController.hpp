#pragma once
#include "CImageColorManager.hpp"
#include <vector>
class CSlotTimerManager;

class CImageColorController {
	std::vector<CImageColorManager>											mColorData;		// colorデータ一覧
	std::vector<std::pair<std::string,std::vector<CImageColorManager*>>>	mActionData;	// 動作データをcolorImageよりコピー

	CImageColorManager&					GetColorDataByID(std::string pColorDataName);
	std::vector<CImageColorManager*>&	GetActionDataByID(std::string pUseEffectName);
	bool								CheckAvailableName(std::string pData);

public:
	// [act]csv読み込み時におけるcolorData定義追加
	void							AddColorData(CImageColorManager& pData) { mColorData.push_back(pData); }
	// [act]アクションデータID新規作成
	void							CreateActionData(std::string pUseEffectName);
	// [act]IDからアクションデータを追加
	void							AddActionData(std::string pUseEffectName, std::string pColorDataName);
	// [act]指定IDのアクションデータをクリア
	void							ResetActionData(std::string pUseEffectName);
	// [act]描画に使用するcolorデータを呼び出す
	const CImageColorManager *const	GetColorData(std::string pUseEffectName, int pIndex);
	// [act]指定したIDの全データに対してタイマーをセットする
	bool							SetTimerAll(CSlotTimerManager& pTimer);
	// [act]指定したIDの全データに対してタイマーをリセットする
	void							ResetTimerAll();
};