#pragma once
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class IImageDestinationManager {
private:
	EDrawModeForDST  GetDrawEnum(std::string pIndex);
	EBlendModeForDST GetBlendEnum(std::string pIndex);

protected:
	std::vector<SImageDestCSVCommonData> mCommonData;		// csvから読みだしたdstデータ、複数定義可能
	std::string mTimerID;									// 読み出しアニメーション・時分割を扱うタイマID
	const int* m_pLoopTime;									// タイマIDのループ点 (-1でループ無効)
	const long long* (* const mTimerReader)(std::string);	// タイマ値呼び出し用関数ポインタ
	int (* const mScreenManager)(std::string);				// 描画先画面呼び出し用関数ポインタ

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
	IImageDestinationManager(const long long* (* const pTimerReader)(std::string), int (* const pScreenManager)(std::string));
	// [act]文字列配列"pReadData"からsrcデータを取得する
	virtual bool	Init(StringArr pReadData, int* (* const pVariableManager)(std::string));
	// [act]
	virtual void	Draw(SDrawImageSourceData(* const pSourceGetter)(int, int)) = 0;
};

class CImageDestinationDefault : public IImageDestinationManager {
	int* m_pDrawNum;
	int  *m_pDiffX, *m_pDiffY;

	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	CImageDestinationDefault(const long long* (* const pTimerReader)(std::string), int (* const pScreenManager)(std::string));
	// [act]文字列配列"pReadData"からsrcデータを取得する
	bool	Init(StringArr pReadData, int* (* const pVariableManager)(std::string)) override;
	// [act]描画を行う
	void	Draw(SDrawImageSourceData (*const pSourceGetter)(int, int)) override;
};