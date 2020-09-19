#include "_header/ImageDestinationManager.hpp"
#include "_header/ErrClass.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CGameDataManage.h"
#include "_header/ImageSourceManager.hpp"
#include "_header/CImageColorManager.hpp"
#include "DxLib.h"

// [act]変数の初期化と関数ポインタの設定を行う
// [prm]pTimerReader	: タイマー値呼び出し用関数ポインタ
//		pScreenManager	: 描画先画面呼び出し用関数ポインタ
IImageDestinationManager::IImageDestinationManager(CEffectVariableManager& pVarManager)
	: CEffectImageCommonComponent(pVarManager) {
	mCommonData.clear();
}

// [act]文字列配列"pReadData"からsrcデータを取得する
// [prm]pReadData			: 初期化用csv分割データ
// [ret]データ取得に成功したかどうか
bool IImageDestinationManager::Init(StringArr pReadData, CSlotTimerManager& pTimerData) {
	try {
		SImageDestCSVCommonData data;
		data.screenID = mVarManager.GetScreenID(pReadData[1]);
		data.startTime = mVarManager.MakeValID(pReadData[2]);
		data.x = mVarManager.MakeValID(pReadData[3]);
		data.y = mVarManager.MakeValID(pReadData[4]);
		data.w = mVarManager.MakeValID(pReadData[5]);
		data.h = mVarManager.MakeValID(pReadData[6]);
		data.a = mVarManager.MakeValID(pReadData[7]);
		data.extend = GetDrawEnum(pReadData[8]);
		data.blend = GetBlendEnum(pReadData[9]);

		if (mCommonData.empty()) {
			mTimerID = pTimerData.GetTimerHandle(pReadData[10]);
			mLoopTime = mVarManager.MakeValID(pReadData[11]);
		}

		mCommonData.push_back(data);
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	
}

EDrawModeForDST IImageDestinationManager::GetDrawEnum(std::string pIndex) {
	if (pIndex == "Nearest")	return EDrawModeForDST::eNearest;
	if (pIndex == "Bilinear")	return EDrawModeForDST::eBilinear;
	return EDrawModeForDST::eNearest;
}

int IImageDestinationManager::GetDxDrawModeByEnum(EDrawModeForDST pData) {
	if (pData == EDrawModeForDST::eNearest)  return DX_DRAWMODE_NEAREST;
	if (pData == EDrawModeForDST::eBilinear) return DX_DRAWMODE_BILINEAR;
	return DX_DRAWMODE_NEAREST;
}

EBlendModeForDST IImageDestinationManager::GetBlendEnum(std::string pIndex) {
	if (pIndex == "None")		return EBlendModeForDST::eNone;
	if (pIndex == "Alpha")		return EBlendModeForDST::eAlpha;
	if (pIndex == "Add")		return EBlendModeForDST::eAdd;
	if (pIndex == "Sub")		return EBlendModeForDST::eSub;
	if (pIndex == "Mula")		return EBlendModeForDST::eMula;
	if (pIndex == "PMAAlpha")	return EBlendModeForDST::ePMAAlpha;
	if (pIndex == "PMAAdd")		return EBlendModeForDST::ePMAAdd;
	if (pIndex == "PMASub")		return EBlendModeForDST::ePMASub;
	if (pIndex == "Screen")		return EBlendModeForDST::eScreen;
	if (pIndex == "Overlay")	return EBlendModeForDST::eOverlay;
	if (pIndex == "Dodge")		return EBlendModeForDST::eDodge;
	if (pIndex == "Burn")		return EBlendModeForDST::eBurn;
	if (pIndex == "Darken")		return EBlendModeForDST::eDarken;
	if (pIndex == "Lighten")	return EBlendModeForDST::eLighten;
	if (pIndex == "Softlight")	return EBlendModeForDST::eSoftlight;
	if (pIndex == "Hardlight")	return EBlendModeForDST::eHardlight;
	if (pIndex == "Exclusion")	return EBlendModeForDST::eExclusion;
	return EBlendModeForDST::eNone;
}

int IImageDestinationManager::GetDxBlendModeByEnum(EBlendModeForDST pData) {
	if (pData == EBlendModeForDST::eNone)		return DX_BLENDMODE_NOBLEND;
	if (pData == EBlendModeForDST::eAlpha)		return DX_BLENDMODE_ALPHA;
	if (pData == EBlendModeForDST::eAdd)		return DX_BLENDMODE_ADD;
	if (pData == EBlendModeForDST::eSub)		return DX_BLENDMODE_SUB;
	if (pData == EBlendModeForDST::eMula)		return DX_BLENDMODE_MULA;
	if (pData == EBlendModeForDST::ePMAAlpha)	return DX_BLENDMODE_PMA_ALPHA;
	if (pData == EBlendModeForDST::ePMAAdd)		return DX_BLENDMODE_PMA_ADD;
	if (pData == EBlendModeForDST::ePMASub)		return DX_BLENDMODE_PMA_SUB;
	if (pData == EBlendModeForDST::eScreen)		return DX_GRAPH_BLEND_SCREEN;
	if (pData == EBlendModeForDST::eOverlay)	return DX_GRAPH_BLEND_OVERLAY;
	if (pData == EBlendModeForDST::eDodge)		return DX_GRAPH_BLEND_DODGE;
	if (pData == EBlendModeForDST::eBurn)		return DX_GRAPH_BLEND_BURN;
	if (pData == EBlendModeForDST::eDarken)		return DX_GRAPH_BLEND_DARKEN;
	if (pData == EBlendModeForDST::eLighten)	return DX_GRAPH_BLEND_LIGHTEN;
	if (pData == EBlendModeForDST::eSoftlight)	return DX_GRAPH_BLEND_SOFTLIGHT;
	if (pData == EBlendModeForDST::eHardlight)	return DX_GRAPH_BLEND_HARDLIGHT;
	if (pData == EBlendModeForDST::eExclusion)	return DX_GRAPH_BLEND_EXCLUSION;
	return DX_BLENDMODE_NOBLEND;
}

bool IImageDestinationManager::GetCanDrawDirectly(EBlendModeForDST pData) {
	if (pData == EBlendModeForDST::eNone)		return true;
	if (pData == EBlendModeForDST::eAlpha)		return true;
	if (pData == EBlendModeForDST::eAdd)		return true;
	if (pData == EBlendModeForDST::eSub)		return true;
	if (pData == EBlendModeForDST::eMula)		return true;
	if (pData == EBlendModeForDST::ePMAAlpha)	return true;
	if (pData == EBlendModeForDST::ePMAAdd)		return true;
	if (pData == EBlendModeForDST::ePMASub)		return true;
	return false;
}

// [act]ループ点を考慮した操作に使用する時間を割り出す
//		[throwable]ErrInternalVarUndeclaredに対するcatchを呼び出し時に適用してください
// [prm]pNowCount	: 現在のタイマカウント
// [ret]描画に使用するタイマカウント
long long IImageDestinationManager::GetCheckTime(const long long pNowCount) {
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
int IImageDestinationManager::GetDefinitionIndex() {
	if (mCommonData.empty()) return -1;
	if (!GetIsTimerSet() || !GetIsTimerEnable()) return -1;
	const auto definitionNum = mCommonData.size();

	try {
		const long long nowTime = GetTimer();
		const long long checkTime = GetCheckTime(nowTime);

		// 見ている要素のbeginTimeに未達ならその前のデータを使用する。第1要素に未達なら描画を行わない
		int ans = -1;
		for (auto it = mCommonData.begin(); it != mCommonData.end(); ++it, ++ans)
			if (checkTime < (long long)mVarManager.GetVal(it->startTime)) return ans;

		// ループ点があれば最後の要素を描画、なければ描画を行わない
		return mVarManager.GetVal(mLoopTime) >= 0 ? ans : -1;
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


// [act]変数の初期化と関数ポインタの設定を行う
// [prm]pTimerReader	: タイマー値呼び出し用関数ポインタ
//		pScreenManager	: 描画先画面呼び出し用関数ポインタ
CImageDestinationDefault::CImageDestinationDefault(CEffectVariableManager& pVarManager)
	: IImageDestinationManager(pVarManager) {
	mDrawNum = -1;
	mDiffX = -1;
	mDiffY = -1;
}

// [act]文字列配列"pReadData"からsrcデータを取得する
// [prm]pReadData			: 初期化用csv分割データ
// [ret]データ取得に成功したかどうか
bool CImageDestinationDefault::Init(StringArr pReadData, CSlotTimerManager& pTimerData) {
	try {
		if (pReadData.size() < 10) throw ErrLessCSVDefinition(pReadData, 10);
		if (pReadData.size() < 15 && mCommonData.empty()) throw ErrLessCSVDefinition(pReadData, 15);
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}

	if (mCommonData.empty()) {
		try {
			mDrawNum	= mVarManager.MakeValID(pReadData[12]);
			mDiffX		= mVarManager.MakeValID(pReadData[13]);
			mDiffY		= mVarManager.MakeValID(pReadData[14]);
		}
		catch (ErrUndeclaredVar e) {
			e.WriteErrLog();
			return false;
		}
	}
	return IImageDestinationManager::Init(pReadData, pTimerData);
}

// [act]描画を行う
//		アニメーション実装は後で
void CImageDestinationDefault::Draw(IImageSourceManager *const pSourceData, CImageColorManager* pColorData, CGameDataManage& pDataManager) {
	const auto dataIndex = GetDefinitionIndex();
	if (dataIndex < 0) return;

	const auto& destData = mCommonData[dataIndex];
	const int screenID = destData.screenID;

	for (int i = 0; i < mVarManager.GetVal(mDrawNum); ++i) {
		auto source = pSourceData->GetImageSource(i, mVarManager.GetVal(mDrawNum));
		if (pColorData != nullptr)
			if(!pColorData->GetColorData(pDataManager, source, i)) return;
		if (source.imageID == -1) continue;
		const int blendID = GetDxBlendModeByEnum(destData.blend);
		const int drawPos[]  = {
			mVarManager.GetVal(destData.x) + i * mVarManager.GetVal(mDiffX),
			mVarManager.GetVal(destData.y) + i * mVarManager.GetVal(mDiffY)
		};
		const int imageHandle = pDataManager.GetDataHandle(source.imageID);
		if (imageHandle == -1) return;

		if (GetCanDrawDirectly(destData.blend)) {
			const int drawID  = GetDxDrawModeByEnum(destData.extend);
			DxLib::SetDrawScreen(screenID);
			DxLib::SetDrawMode(drawID);
			DxLib::SetDrawBlendMode(blendID, mVarManager.GetVal(destData.a));
			DxLib::SetDrawBright(source.r, source.g, source.b);
			DxLib::DrawRectExtendGraph(
				drawPos[0], drawPos[1],
				drawPos[0] + mVarManager.GetVal(destData.w), drawPos[1] + mVarManager.GetVal(destData.h),
				source.x, source.y, source.w, source.h, imageHandle, TRUE);
		}
		else {
			DxLib::GraphBlendRectBlt(
				screenID, imageHandle, screenID,
				drawPos[0], drawPos[1], drawPos[0] + source.w, drawPos[1] + source.h,
				source.x, source.y, drawPos[0], drawPos[1],
				mVarManager.GetVal(destData.a), blendID
			);
		}
	}
}