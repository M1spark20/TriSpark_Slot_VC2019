#include "_header/CGameState_SlotGameMain.hpp"
#include "_header/CGameDataManage.h"
#include "_header/CGetSysDataFromCSV.hpp"
#include "_header/CSlotFlowList.hpp"
#include "_header/keyexport.h"
#include "_header/CRestoreManager.hpp"
#include "DxLib.h"

bool CGameState_SlotGameMain::Init(CGameDataManage& pDataManageIns){
	CGetSysDataFromCSV sysReader;
	sysReader.FileInit(pDataManageIns.GetDataHandle(0));
	if(!m_data.reelManager.Init(pDataManageIns, sysReader))							return false;
	if(!m_data.randManager.Init(pDataManageIns, sysReader.GetSysDataID("flags")))	return false;
	if(!m_data.castChecker.Init(pDataManageIns, sysReader.GetSysDataID("cast")))	return false;
	if(!m_data.timeManager.Init(m_data.reelManager.GetReelNum()))					return false;
	m_data.internalDataManager.Init();
	m_data.internalDataManager.SetSlotSetting(3);

	if (
		!m_data.effectManager.Init(pDataManageIns, sysReader.GetSysDataID("effect"),
			m_data.timeManager, m_data.reelManager)
	) return false;
	if (!m_data.reachCollection.Init(
		pDataManageIns, sysReader.GetSysDataID("collection"), m_data.reelManager.GetReelNum()
	)) return false;

	if (!m_menuManager.Init(pDataManageIns,
		sysReader.GetSysDataID("license"),
		sysReader.GetSysDataID("menuDataFont"),
		sysReader.GetSysDataID("menuDataFontMid"),
		sysReader.GetSysDataID("menuBase"),
		sysReader.GetSysDataID("menuTitleFont")
	)) return false;
	
	CRestoreManagerRead reader;
	if (reader.StartRead()) {
		if (!m_data.internalDataManager.ReadRestore(reader)) return false;
		if (!m_data.dataCounter.ReadRestore(reader)) return false;
		if (!m_data.reelManager.ReadRestore(reader)) return false;
		if (!m_data.timeManager.ReadRestore(reader)) return false;
		if (!m_data.effectManager.ReadRestore(reader)) return false;
		if (!m_data.reachCollection.ReadRestore(reader)) return false;
	}

	m_data.timeManager.Process();
	m_pFlowManager = new CSlotFlowBet;
	return m_pFlowManager->Init(m_data);
}

EChangeStateFlag CGameState_SlotGameMain::Process(CGameDataManage& pDataManageIns){
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	if (key.GetExportStatus() == EKeyExportStatus::eGameMain && key.ExportKeyState(KEY_INPUT_ESCAPE))
		return eStateEnd;
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
	m_data.effectManager.Process(m_data.timeManager, m_data.internalDataManager, m_data.reachCollection, m_data);
	m_menuManager.Process(pDataManageIns, m_data);
	
	// データ保存
	if (m_data.restoreManager.IsActivate()) {
		if(!m_data.restoreManager.StartWrite()) return eStateErrEnd;
		if(!m_data.internalDataManager.WriteRestore(m_data.restoreManager)) return eStateErrEnd;
		if(!m_data.dataCounter.WriteRestore(m_data.restoreManager)) return eStateErrEnd;
		if(!m_data.reelManager.WriteRestore(m_data.restoreManager)) return eStateErrEnd;
		if(!m_data.timeManager.WriteRestore(m_data.restoreManager)) return eStateErrEnd;
		if(!m_data.effectManager.WriteRestore(m_data.restoreManager)) return eStateErrEnd;
		if(!m_data.reachCollection.WriteRestore(m_data.restoreManager)) return eStateErrEnd;
		if(!m_data.restoreManager.Flush()) return eStateErrEnd;
	}
	
	return eStateContinue;
}

bool CGameState_SlotGameMain::Draw(CGameDataManage& pDataManageIns){
	m_data.effectManager.Draw(pDataManageIns, m_data.timeManager);
	m_data.effectManager.RingSound(m_data.timeManager, pDataManageIns);
	m_menuManager.Draw();
	return true;
}

CGameState_SlotGameMain::~CGameState_SlotGameMain(){
	delete m_pFlowManager; 
}
