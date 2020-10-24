#include "_header\CGameState_SlotGameMain.hpp"
#include "_header\CGameDataManage.h"
#include "_header\CGetSysDataFromCSV.hpp"
#include "_header\CSlotFlowList.hpp"
#include "_header\keyexport.h"
#include "DxLib.h"

bool CGameState_SlotGameMain::Init(CGameDataManage& pDataManageIns){
	CGetSysDataFromCSV sysReader;
	sysReader.FileInit(pDataManageIns.GetDataHandle(0));
	if(!m_data.reelManager.Init(pDataManageIns, sysReader))							return false;
	if(!m_data.randManager.Init(pDataManageIns, sysReader.GetSysDataID("flags")))	return false;
	if(!m_data.castChecker.Init(pDataManageIns, sysReader.GetSysDataID("cast")))	return false;
	if(!m_data.timeManager.Init(m_data.reelManager.GetReelNum()))					return false;
	m_data.internalDataManager.Init();
	m_data.internalDataManager.SetSlotSetting(5);

	if (
		!m_data.effectManager.Init(pDataManageIns, sysReader.GetSysDataID("effect"),
			m_data.timeManager, m_data.reelManager)
	) return false;
	

	m_pFlowManager = new CSlotFlowBet;
	return m_pFlowManager->Init(m_data);
}

EChangeStateFlag CGameState_SlotGameMain::Process(CGameDataManage& pDataManageIns){
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	if (key.ExportKeyState(KEY_INPUT_ESCAPE)) return eStateEnd;
	m_data.timeManager.Process();
	ESlotFlowFlag flow = m_pFlowManager->Process(m_data);
	if (flow != eFlowContinue){
		delete m_pFlowManager;	m_pFlowManager = nullptr;
		switch (flow){
		case eFlowBetting:			m_pFlowManager = new CSlotFlowBet;		break;
		case eFlowWaiting:			m_pFlowManager = new CSlotFlowWaiting;	break;
		case eFlowReelAcceration:	m_pFlowManager = new CSlotFlowReelAcc;	break;
		case eFlowReelMoving:		m_pFlowManager = new CSlotFlowReelMove;	break;
		case eFlowPayout:			m_pFlowManager = new CSlotFlowPayout;	break;
		case eFlowEnd:
			return eStateEnd;
			break;
		case eFlowErrEnd:
			return eStateErrEnd;
			break;
		default:
			return eStateErrEnd;
			break;
		}
		if (!m_pFlowManager->Init(m_data)) return eStateErrEnd;
	}

	m_data.reelManager.Process(m_data.timeManager);
	m_data.effectManager.Process(m_data.timeManager, m_data.internalDataManager);
	return eStateContinue;
}

bool CGameState_SlotGameMain::Draw(CGameDataManage& pDataManageIns){
	m_data.effectManager.Draw(pDataManageIns);

	// ƒŠ[ƒ‹b’è
	/*SReelDrawData data;
	for (int i = 0; i < 3; ++i){
		data.reelID = i; data.x = 290.f + i * 346.f; data.y = 352.f;
		data.comaW = 330.f; data.comaH = 150.f;
		data.offsetLower = 8; data.offsetUpper = 20; data.comaNum = 3;
		m_data.reelManager.DrawReel(pDataManageIns, data);
	}*/

	/*DxLib::DrawFormatString(0, 0, 0xFFFFFF, "cred: %d, %d, %d",
		m_data.internalDataManager.GetData().betNum,
		m_data.internalDataManager.GetData().credit,
		m_data.internalDataManager.GetData().payout);
	DxLib::DrawFormatString(0, 20, 0xFFFFFF, "flag: %d, %d",
		m_data.internalDataManager.GetData().flag.first,
		m_data.internalDataManager.GetData().flag.second);
	DxLib::DrawFormatString(0, 40, 0xFFFFFF, "mode: %d, %d, %d",
		m_data.internalDataManager.GetData().gameMode,
		m_data.internalDataManager.GetData().gameModeLimit.first,
		m_data.internalDataManager.GetData().gameModeLimit.second);*/

	return true;
}

CGameState_SlotGameMain::~CGameState_SlotGameMain(){
	delete m_pFlowManager; 
}