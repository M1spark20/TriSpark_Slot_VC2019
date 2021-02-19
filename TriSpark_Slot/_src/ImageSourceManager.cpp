﻿#include "_header/ImageSourceManager.hpp"
#include "_header/ErrClass.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotTimerManager.hpp"

// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
// [prm]pTimerReader	: タイマー値呼び出し用関数ポインタ
IImageSourceManager::IImageSourceManager(CEffectVariableManager& pVarManager)
	: CEffectImageCommonComponent(pVarManager){
	mCommonData.clear();
	mEffectDataName = "";
}

// [act]文字列配列"pReadData"からsrcデータを取得する
// [prm]pReadData			: 初期化用csv分割データ
//		pVariableManager	: 変数管理用関数を指定→値はポインタで管理する
// [ret]データ取得に成功したかどうか
bool IImageSourceManager::Init(StringArr pReadData, CSlotTimerManager& pTimerManager) {
	try {
		SImageSourceCSVCommonData data;
		data.startTime	= mVarManager.MakeValID(pReadData[1]);
		data.isImageFromScreen = pReadData[2][0] == '*';
		if (data.isImageFromScreen)		data.imageID = mVarManager.GetScreenID(pReadData[2]);
		else							data.imageID = mVarManager.MakeValID(pReadData[2]);
		data.x			= mVarManager.MakeValID(pReadData[3]);
		data.y			= mVarManager.MakeValID(pReadData[4]);
		data.w			= mVarManager.MakeValID(pReadData[5]);
		data.h			= mVarManager.MakeValID(pReadData[6]);
		data.numX		= mVarManager.MakeValID(pReadData[7]);
		data.numY		= mVarManager.MakeValID(pReadData[8]);
		data.directionY = pReadData[9] == "Y";

		if (mCommonData.empty()) {
			mTimerID = pTimerManager.GetTimerHandle(pReadData[10]);
			mLoopTime = mVarManager.MakeValID(pReadData[11]);
			mEffectDataName = pReadData[12];
		}

		mCommonData.push_back(data);
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}

// [act]ループ点を考慮した操作に使用する時間を割り出す
// [prm]pNowCount	: 現在のタイマカウント
// [ret]描画に使用するタイマカウント
long long IImageSourceManager::GetCheckTime(const long long pNowCount) {
	const long long loopTime = mVarManager.GetVal(mLoopTime);
	if (pNowCount < loopTime || loopTime < 0) return pNowCount;

	if (mCommonData.empty()) throw ErrInternalVarUndeclared("mCommonData");
	const int loopPoint = mVarManager.GetVal((mCommonData.end() - 1)->startTime);
	const int diffCount = loopPoint - loopTime;
	if (diffCount == 0) return loopTime;
	long long ans = pNowCount - loopTime;
	return ans - (diffCount * (ans / diffCount)) + loopTime;
}

// [act]タイマー状況から読み出しタイミングで使用する画像定義を決定する
// [ret]-1	:今回は描画を行うタイミングではない場合
//		else:描画する定義ID @mCommonData
int IImageSourceManager::GetDefinitionIndex() {
	if (mCommonData.empty()) return -1;
	if (!GetIsTimerSet() || !GetIsTimerEnable()) return -1;
	const auto definitionNum = mCommonData.size();
	const int loopTime = mVarManager.GetVal(mLoopTime);

	try {
		const long long nowTime = GetTimer();
		const long long checkTime = GetCheckTime(nowTime);

		// 見ている要素のbeginTimeに未達ならその前のデータを使用する。第1要素に未達なら描画を行わない
		int ans = -1;
		for (auto it = mCommonData.begin(); it != mCommonData.end(); ++it, ++ans)
			if (checkTime < (long long)mVarManager.GetVal(it->startTime)) return ans;

		// ループ点があれば最後の要素を描画、なければ描画を行わない
		return loopTime >= 0 ? ans : -1;
	}
	catch (ErrInternalVarUndeclared e) {
		e.WriteErrLog();
		return -1;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return -1;
	}
}

// [act]タイマー状況から読み出しタイミングで使用する画像コマを決定する
// [prm]pDefinitionIndex	: 描画する定義ID @mCommonData
// [ret]-1	: エラー
//		else: 画像コマID
int IImageSourceManager::GetImageIndex(int pDefinitionIndex) {
	if (pDefinitionIndex < 0 || (size_t)pDefinitionIndex >= mCommonData.size()) return -1;

	const int comaNum = GetComaNum(pDefinitionIndex);
	if (comaNum == -1) return -1;
	const long long offset = mVarManager.GetVal(mCommonData[pDefinitionIndex].startTime);
	const long long interval = -offset + ((size_t)pDefinitionIndex + 1 == mCommonData.size() ?
		mVarManager.GetVal(mLoopTime) : mVarManager.GetVal(mCommonData[(size_t)pDefinitionIndex+1].startTime));
	if (interval == 0) return 0;	// srcが変化しない場合
	const double division = interval / (double)comaNum;

	try {
		const long long operateTime = GetCheckTime(GetTimer()) - offset;
		return static_cast<int>(operateTime / division);
	}
	catch (ErrInternalVarUndeclared e) {
		e.WriteErrLog();
		return -1;
	}
}

// [act]アニメーションに使用できるコマ数を取得する(必要に応じoverrideする)
// [prm]pDefinitionIndex	: 描画する定義ID @mCommonData
// [ret]-1	: エラー
//		else: 利用可能コマ数
int IImageSourceManager::GetComaNum(int pDefinitionIndex) {
	if (pDefinitionIndex < 0 || (size_t)pDefinitionIndex >= mCommonData.size()) return -1;
	return abs(mVarManager.GetVal(mCommonData[pDefinitionIndex].numX)) * abs(mVarManager.GetVal(mCommonData[pDefinitionIndex].numY));
}

// [act]definitionIndexとimageIndexから画像範囲を取り出す
SDrawImageSourceData IImageSourceManager::GetSourceDataFromIndex(int pDefinitionIndex, int pImageIndex) {
	// indexに応じて画像を切り出す
	SDrawImageSourceData ans;
	const auto& nowData = mCommonData[pDefinitionIndex];
	unsigned int posX = nowData.directionY ? pImageIndex / abs(mVarManager.GetVal(nowData.numY)) : pImageIndex % abs(mVarManager.GetVal(nowData.numX));
	unsigned int posY = nowData.directionY ? pImageIndex % abs(mVarManager.GetVal(nowData.numY)) : pImageIndex / abs(mVarManager.GetVal(nowData.numX));
	if (mVarManager.GetVal(nowData.numX) < 0) posX = abs(mVarManager.GetVal(nowData.numX)) - posX - 1;
	if (mVarManager.GetVal(nowData.numY) < 0) posY = abs(mVarManager.GetVal(nowData.numY)) - posY - 1;

	ans.r = 255; ans.g = 255; ans.b = 255;
	ans.w = mVarManager.GetVal(nowData.w) / abs(mVarManager.GetVal(nowData.numX));
	ans.h = mVarManager.GetVal(nowData.h) / abs(mVarManager.GetVal(nowData.numY));
	ans.x = mVarManager.GetVal(nowData.x) + ans.w * posX;
	ans.y = mVarManager.GetVal(nowData.y) + ans.h * posY;
	ans.isImageFromScreen = nowData.isImageFromScreen;
	ans.imageID = ans.isImageFromScreen ? nowData.imageID : mVarManager.GetVal(nowData.imageID);	// true:screenData
	return ans;
}


// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
// [prm]pTimerReader	: タイマー値呼び出し用関数ポインタ
CImageSourceDefault::CImageSourceDefault(CEffectVariableManager& pVarManager)
	: IImageSourceManager(pVarManager) {
}

// [act]文字列配列"pReadData"からsrcデータを取得する
// [prm]pReadData			: 初期化用csv分割データ
//		pVariableManager	: 変数管理用関数を指定→値はポインタで管理する
// [ret]データ取得に成功したかどうか
bool CImageSourceDefault::Init(StringArr pReadData, CSlotTimerManager& pTimerManager) {
	try {
		if (pReadData.size() < 10) throw ErrLessCSVDefinition(pReadData, 10);
		if (pReadData.size() < 13 && mCommonData.empty()) throw ErrLessCSVDefinition(pReadData, 13);
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}
	return IImageSourceManager::Init(pReadData, pTimerManager);
}


// [act]画像読み込み参照先を返す
// [prm]pWriteIndex	: 何枚目の描画画像の取り出しを行うかを指定(ただしdefaultでは不使用)
//		pWriteNum	: 実行中の描画で何回画像を繰り返し描画するか指定(ただしdefaultでは不使用)
SDrawImageSourceData CImageSourceDefault::GetImageSource(int pWriteIndex, int pWriteNum) {
	const auto dataIndex = GetDefinitionIndex();
	if (dataIndex < 0) return SDrawImageSourceData();
	const auto imageIndex = GetImageIndex(dataIndex);
	if (imageIndex < 0) return SDrawImageSourceData();

	return GetSourceDataFromIndex(dataIndex, imageIndex);
}


CImageSourceNumber::CImageSourceNumber(CEffectVariableManager& pVarManager)
	: IImageSourceManager(pVarManager) {
	mPaddingFlag = false; mDrawMinusFlag = false;
	mNumSource = -1;
	mNumAlign = EAlign::eRight;
	mDigitCount = 10;
}

bool CImageSourceNumber::Init(StringArr pReadData, CSlotTimerManager& pTimerManager) {
	try {
		if (pReadData.size() < 10) throw ErrLessCSVDefinition(pReadData, 10);
		if (pReadData.size() < 16 && mCommonData.empty()) throw ErrLessCSVDefinition(pReadData, 17);
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}

	if (mCommonData.empty()) {
		try {
			mNumSource		= mVarManager.MakeValID(pReadData[13]);
			mNumAlign		= pReadData[14] == "L" ? EAlign::eLeft : EAlign::eRight;
			mPaddingFlag	= pReadData[15] == "T";
			mDrawMinusFlag	= pReadData[16] == "T";
		}
		catch (ErrUndeclaredVar e) {
			e.WriteErrLog();
			return false;
		}

		if (mPaddingFlag) ++mDigitCount;
		if (mDrawMinusFlag) mDigitCount = (mDigitCount + 1) * 2;
	}
	return IImageSourceManager::Init(pReadData, pTimerManager);
}

int CImageSourceNumber::GetComaNum(int pDefinitionIndex) {
	const int comaNum = IImageSourceManager::GetComaNum(pDefinitionIndex);
	if (comaNum < 0) return -1;
	return comaNum / mDigitCount;
}

SDrawImageSourceData CImageSourceNumber::GetImageSource(int pWriteIndex, int pWriteNum) {
	if (pWriteIndex < 0 || pWriteIndex >= pWriteNum) return SDrawImageSourceData();
	const auto dataIndex = GetDefinitionIndex();
	if (dataIndex < 0) return SDrawImageSourceData();
	const auto imageIndex = GetImageIndex(dataIndex);
	if (imageIndex < 0) return SDrawImageSourceData();
	// 数値がマイナスかつ負の値を書かない場合は描画しない
	if (!mDrawMinusFlag && mVarManager.GetVal(mNumSource) < 0) return SDrawImageSourceData();
	int numIndex;

	/* 数値を取得して描画するコマを決定する */ {
		int nowVal = abs(mVarManager.GetVal(mNumSource));
		int digitNum;
		if (nowVal == 0) digitNum = 1;
		else for (digitNum = 0; nowVal > 0; ++digitNum) nowVal /= 10;

		// pWriteIndexを右端基準に変換する
		pWriteIndex = pWriteNum - pWriteIndex - 1;

		/* Alignが左の場合はpWriteIndexを調整する */
		if(mNumAlign == EAlign::eLeft && !mPaddingFlag) {
			pWriteIndex = digitNum - pWriteIndex + mDrawMinusFlag ? 1 : 0;
			if (pWriteIndex < 0 || pWriteIndex >= pWriteNum) return SDrawImageSourceData();
		}

		bool drawSignFlag = mPaddingFlag && mDrawMinusFlag && pWriteIndex + 1 == pWriteNum;
		drawSignFlag |= !mPaddingFlag && mDrawMinusFlag && pWriteIndex == digitNum;
		drawSignFlag |= mDrawMinusFlag &&digitNum >= pWriteNum && pWriteIndex + 1 == pWriteNum;
		bool drawPaddingFlag = mPaddingFlag && pWriteIndex >= digitNum && !drawSignFlag;

		// 描画しない場合
		if (pWriteIndex >= digitNum && !drawSignFlag && !drawPaddingFlag) return SDrawImageSourceData();
		// 符号を描画する場合
		else if (drawSignFlag) numIndex = (mPaddingFlag ? 11 : 10);
		// パディングを描画する場合
		else if (drawPaddingFlag) numIndex = 10;
		// else:数値を描画する場合
		else {
			nowVal = abs(mVarManager.GetVal(mNumSource));
			for (auto i = 0; i < pWriteIndex; ++i) nowVal /= 10;
			numIndex = nowVal % 10;
		}
		numIndex += (mVarManager.GetVal(mNumSource) < 0 ? mDigitCount / 2 : 0);
	}

	return GetSourceDataFromIndex(dataIndex, imageIndex * mDigitCount + numIndex);
}

// [act]変数の初期化とタイマ値呼び出し用関数ポインタの設定を行う
// [prm]pTimerReader	: タイマー値呼び出し用関数ポインタ
CImageSourceReel::CImageSourceReel(CEffectVariableManager& pVarManager)
	: IImageSourceManager(pVarManager) {
}

// [act]文字列配列"pReadData"からsrcデータを取得する
// [prm]pReadData			: 初期化用csv分割データ
//		pVariableManager	: 変数管理用関数を指定→値はポインタで管理する
// [ret]データ取得に成功したかどうか
bool CImageSourceReel::Init(StringArr pReadData, CSlotTimerManager& pTimerManager) {
	try {
		if (pReadData.size() < 10) throw ErrLessCSVDefinition(pReadData, 10);
		if (pReadData.size() < 13 && mCommonData.empty()) throw ErrLessCSVDefinition(pReadData, 13);
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}
	return IImageSourceManager::Init(pReadData, pTimerManager);
}


// [act]画像読み込み参照先を返す
// [prm]pWriteIndex	: 取り出す画像のコマを指定する
//		pWriteNum	: 実行中の描画で何回画像を繰り返し描画するか指定(ただしReelでは不使用)
SDrawImageSourceData CImageSourceReel::GetImageSource(int pWriteIndex, int pWriteNum) {
	const auto dataIndex = GetDefinitionIndex();
	if (dataIndex < 0) return SDrawImageSourceData();
	const auto imageIndex = pWriteIndex;
	if (imageIndex < 0) return SDrawImageSourceData();

	return GetSourceDataFromIndex(dataIndex, imageIndex);
}
