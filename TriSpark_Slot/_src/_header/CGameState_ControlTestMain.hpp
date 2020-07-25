#pragma once
#include "IGameStateBase.hpp"
#include "CReelManager.hpp"
#include "CReelController.hpp"
#include "SCastData.hpp"
#include "CRandManager.hpp"
#include "SReelControlData.hpp"
class CGameDataManage;

class CGameState_ControlTestMain : public IGameStateBase{
// [act]制御テスト・リーチ目確認モードメイン処理
	typedef std::vector<std::vector<std::vector<std::vector<char>>>> tStopMatrix;

	CReelManager		m_reelManager;
	CRandManager		m_randManager;

	static const int	STOP_LIST_NUM_PER_PAGE = 20;

	// 内部メンバ
	std::vector<int>	m_pushPos;			// データ表示時のボタン押下位置
	std::vector<int>	m_pushOrder;		// データ解析・押下時の押し順定義
	std::vector<int>	m_checkedFlagID;	// 解析を行うフラグID一覧
	std::vector<short>	m_ctrlResult;		// 前押し順・全停止形の停止位置メモ
	std::vector<int>	m_stopPos;			// データ表示時のリール停止位置書き出し先
	std::vector<int>	m_reachCount;		// 各リール1st時のリーチ目数
	int					m_selectingReel;	// データ表示時の選択中リール定義
	int					m_selectingFlag;	// データ表示時の選択中フラグ
	int					m_orderID;			// データ解析時の押し順フラグ
	int					m_orderIDMax;		// 押し順の数(リール数の階乗)
	tStopMatrix			m_stopMatrix;		// 全停止形の停止状況保存(リーチ目等) [order][1st][col][row]
	std::vector<int>	m_stopList;			// 特定停止形を引き込む押し位置を記録する変数(要求時のみ作成)
	unsigned int		m_stopListPage;		// 停止形表示時のページ番号
	bool				m_stopListPushStop;	// 停止形表示時の位置種別(true:stop,false:push)

	// internal method
	void ReelMove(bool isUpper);
	void SelectReelChange(bool isRight);
	unsigned int PowInteger(unsigned int v1, unsigned int v2){
		if (v2 == 0) return 1;
		unsigned int defVal = v1;
		for (unsigned int i = 0; i < v2-1; ++i) v1 *= defVal;
		return v1;
	}

public:
	bool Init(CGameDataManage& pDataManageIns) override;
	EChangeStateFlag Process(CGameDataManage& pDataManageIns) override;
	bool Draw(CGameDataManage& pDataManageIns) override;
};