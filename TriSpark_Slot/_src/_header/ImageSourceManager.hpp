#pragma once
#include "CEffectImageCommonComponent.hpp"
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CEffectVariableManager;
class CSlotTimerManager;

// [act]imgSrcデータ管理を行う基底クラス。この型を動的確保し呼び出しを一元化する
//		コンストラクタとInit()で必要なデータを各派生クラスに取り込む
//		GetComaNum()およびGetImageSource()で画像の切り出し方を調整する
class IImageSourceManager : public CEffectImageCommonComponent {
protected:
	std::vector<SImageSourceCSVCommonData> mCommonData;		// csvから読みだしたsrcデータ、複数定義可能
	std::string mEffectDataName;

	typedef std::vector<std::string> StringArr;

	// [act]ループ点を考慮した操作に使用する時間を割り出す
	long long GetCheckTime(const long long pNowCount);
	// [act]タイマー状況から読み出しタイミングで使用する画像定義を決定する
	int GetDefinitionIndex();
	// [act]タイマー状況から読み出しタイミングで使用する画像コマを決定する
	int GetImageIndex(int pDefinitionIndex);
	// [act]アニメーションに使用できるコマ数を取得する(必要に応じoverrideする)
	virtual int GetComaNum(int pDefinitionIndex);
	// [act]definitionIndexとimageIndexから画像範囲を取り出す
	SDrawImageSourceData GetSourceDataFromIndex(int pDefinitionIndex, int pImageIndex);

public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	IImageSourceManager(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	virtual bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager);
	// [act]画像読み込み参照先を返す
	virtual SDrawImageSourceData	GetImageSource(int pWriteIndex, int pWriteNum) = 0;
	// [act]エフェクト変数を返す
	std::string						GetEffectDataName() const { return mEffectDataName; }
};

class CImageSourceDefault : public IImageSourceManager {
public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	CImageSourceDefault(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager) override;
	// [act]画像読み込み参照先を返す
	SDrawImageSourceData	GetImageSource(int pWriteIndex = 0, int pWriteNum = 0) override;
};

class CImageSourceNumber : public IImageSourceManager {
	enum class EAlign{ eLeft, eCenter, eRight };	// 割付方法指定用enum; eCenterの動作は現在未定義
	bool mPaddingFlag;								// 0パディングを行うかを指定; 設定時は必要コマ+1
	bool mDrawMinusFlag;							// 数値がマイナスの時に描画を行うかを指定; 設定時は必要コマ+1が2セット(+-)必要
	int mNumSource;									// 描画対象変数格納用ポインタ
	int mDigitCount;								// 描画に必要なコマ数を格納する(10, 11, 12, 22 or 24)
	EAlign mNumAlign;								// 数字割付方法を指定

	// [act]アニメーションに使用できるコマ数を取得する。値はコマ数からmDigitCountを除して求められる
	int GetComaNum(int pDefinitionIndex) override;

public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	CImageSourceNumber(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager) override;
	SDrawImageSourceData	GetImageSource(int pWriteIndex, int pWriteNum) override;
};

class CImageSourceReel : public IImageSourceManager {
public:
	// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
	CImageSourceReel(CEffectVariableManager& pVarManager);
	// [act]文字列配列"pReadData"からsrcデータを取得する
	bool					Init(StringArr pReadData, CSlotTimerManager& pTimerManager) override;
	// [act]画像読み込み参照先を返す
	SDrawImageSourceData	GetImageSource(int pWriteIndex = 0, int pWriteNum = 0) override;
};

