#include <numeric>
#include "_header/CGameState_ControlTestMain.hpp"
#include "_header/CGameDataManage.h"
#include "_header/CGetSysDataFromCSV.hpp"
#include "_header/CReelDataReaderFromCSV.hpp"
#include "_header/CCastReaderFromCSV.hpp"
#include "_header/CRandomizerReaderFromCSV.hpp"
#include "_header/keyexport.h"
#include "DxLib.h"

bool CGameState_ControlTestMain::Init(CGameDataManage& pDataManageIns){
	CGetSysDataFromCSV sysReader;

	// データ初期化
	sysReader.FileInit(pDataManageIns.GetDataHandle(0));
	m_reelManager.Init(pDataManageIns, sysReader);
	m_randManager.Init(pDataManageIns, sysReader.GetSysDataID("flags"), 0);

	// 停止形解析のための初期設定1
	const unsigned int comaNum = m_reelManager.GetCharaNum();
	const unsigned int reelNum = m_reelManager.GetReelNum();
	m_pushOrder.resize(m_reelManager.GetReelNum());
	std::iota(m_pushOrder.begin(), m_pushOrder.end(), 0);
	m_orderIDMax = 1; for (auto x : m_pushOrder) m_orderIDMax *= (x+1);
	// その2
	m_checkedFlagID.reserve(m_randManager.GetFlagNum());
	for (int i = 0; i < m_randManager.GetFlagNum(); ++i)
		if (m_randManager.GetFlagData(i, eIsReachCheck) >= 1) m_checkedFlagID.push_back(i);
	const unsigned int checkMax = m_orderIDMax * PowInteger(comaNum, reelNum) * m_checkedFlagID.size();
	m_ctrlResult.resize(checkMax, 0);

	// 停止形解析
	for (unsigned int checkC = 0; checkC < checkMax; ++checkC){
		unsigned int currentPos[3], flagID;
		for (unsigned int i = 0; i < reelNum; ++i) currentPos[i] = (checkC / PowInteger(comaNum, reelNum-1-i)) % comaNum;
		if (checkC % (PowInteger(comaNum, reelNum)*m_checkedFlagID.size()) == 0 && checkC>0)
			std::next_permutation(m_pushOrder.begin(), m_pushOrder.end());
		flagID = (checkC / PowInteger(comaNum, reelNum)) % m_checkedFlagID.size();
		m_reelManager.StartReel(
			m_randManager.GetFlagData(m_checkedFlagID[flagID], eFlagID),
			m_randManager.GetFlagData(m_checkedFlagID[flagID], eBonusID), 0);

		for (unsigned int orderC = 0; orderC < reelNum; ++orderC){
			if (!m_reelManager.StopReel(m_pushOrder[orderC], currentPos[m_pushOrder[orderC]]))
				return false;
		}
		auto result = m_reelManager.GetReelPos();
		for (unsigned int orderC = 0; orderC < result.size(); ++orderC){
			m_ctrlResult[checkC] |= (result[orderC] & 0x1F) << (5*(reelNum-1-orderC));
		}
	}
	while (std::next_permutation(m_pushOrder.begin(), m_pushOrder.end()));

	/* matrix & reachNum initialize */{
		std::vector<char> t1(comaNum, 0);
		std::vector<std::vector<char>> t2(comaNum, t1);
		std::vector<std::vector<std::vector<char>>> t3(comaNum, t2);
		m_stopMatrix.resize(reelNum, t3);
		m_reachCount.resize(reelNum, PowInteger(comaNum, reelNum));
	}

	// マトリクスデータ作成
	for (unsigned int checkC = 0; checkC < checkMax; ++checkC){
		unsigned int orderID, flagID, currentPos[] = {
			(unsigned int)m_ctrlResult[checkC] >> 10 & 0x1F,
			(unsigned int)m_ctrlResult[checkC] >>  5 & 0x1F,
			(unsigned int)m_ctrlResult[checkC] >>  0 & 0x1F
		};
		orderID = checkC / PowInteger(comaNum, reelNum) / m_checkedFlagID.size();
		flagID = (checkC / PowInteger(comaNum, reelNum)) % m_checkedFlagID.size();
		unsigned int matrixDim[3];
		/* matrixDim決定 */	{
			int cnt = 1;
			for (unsigned int i = 0; i<reelNum; ++i){
				if (i == m_pushOrder[0]) { matrixDim[0] = i; continue; }
				matrixDim[cnt++] = i;
			}
		}
		if (checkC % (PowInteger(comaNum, reelNum)*m_checkedFlagID.size()) == 0 && checkC>0){
			std::next_permutation(m_pushOrder.begin(), m_pushOrder.end());
		}
		auto& inputFor = m_stopMatrix[matrixDim[0]][currentPos[matrixDim[0]]][currentPos[matrixDim[1]]][currentPos[matrixDim[2]]];
		auto  bonusID = m_randManager.GetFlagData(flagID, eIsReachCheck) & 0xFF;
		inputFor |= bonusID;
	}
	while (std::next_permutation(m_pushOrder.begin(), m_pushOrder.end()));

	// リーチ目数カウント
	for(unsigned int firstC=0; firstC<reelNum; ++firstC)
	for(auto t1=m_stopMatrix[firstC].begin(); t1 != m_stopMatrix[firstC].end(); ++t1)
	for(auto t2=t1->begin(); t2 != t1->end(); ++t2)
	for(auto t3 = t2->begin(); t3 != t2->end(); ++t3){
		if (*t3 & 0x3 || *t3 == 0x0) --m_reachCount[firstC];
	}
	
	// UI用変数の初期化
	m_pushPos.resize(m_reelManager.GetReelNum(), 0);
	m_stopPos.resize(m_reelManager.GetReelNum(), 0);
	m_selectingReel = -1;
	m_selectingFlag = 0;
	m_orderID = 0;
	m_stopListPage = 0;
	m_stopListPushStop = false;

#ifdef __ANDROID__
#elif __APPLE__
#else
	DxLib::SetMainWindowText("Reel control test mode");
#endif
	return true;
}

EChangeStateFlag CGameState_ControlTestMain::Process(CGameDataManage& pDataManageIns, bool pExtendResolution){
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	bool isMoveReel = false;
	if (m_selectingReel == -1){
		m_selectingReel = 0;
		isMoveReel = true;
	}

	if (key.ExportKeyState(KEY_INPUT_UP)){
		ReelMove(true);
		isMoveReel = true;
	}
	if (key.ExportKeyState(KEY_INPUT_DOWN)){
		ReelMove(false);
		isMoveReel = true;
	}
	if (key.ExportKeyState(KEY_INPUT_RIGHT)){
		SelectReelChange(true);
	}
	if (key.ExportKeyState(KEY_INPUT_LEFT)){
		SelectReelChange(false);
	}
	if (key.ExportKeyState(KEY_INPUT_Q)){
		std::next_permutation(m_pushOrder.begin(), m_pushOrder.end());
		if (++m_orderID >= m_orderIDMax) m_orderID -= m_orderIDMax;
		isMoveReel = true;
	}
	if (key.ExportKeyState(KEY_INPUT_A)){
		std::prev_permutation(m_pushOrder.begin(), m_pushOrder.end());
		if (--m_orderID < 0) m_orderID += m_orderIDMax;
		isMoveReel = true;
	}
	if (key.ExportKeyState(KEY_INPUT_W)){
		if (++m_selectingFlag >= m_checkedFlagID.size())
			m_selectingFlag -= m_checkedFlagID.size();
		isMoveReel = true;
	}
	if (key.ExportKeyState(KEY_INPUT_S)){
		if (--m_selectingFlag < 0)
			m_selectingFlag += m_checkedFlagID.size();
		isMoveReel = true;
	}
	if (key.ExportKeyState(KEY_INPUT_E)){
		++m_stopListPage;
		if (m_stopListPage * STOP_LIST_NUM_PER_PAGE > (m_stopList.size()-1)) m_stopListPage = 0;
	}
	if (key.ExportKeyState(KEY_INPUT_D)){
		if (m_stopListPage == 0) m_stopListPage = (m_stopList.size()-1) / STOP_LIST_NUM_PER_PAGE + 1;
		--m_stopListPage;
	}
	// 押し位置解析(選択中の押し順で押し位置指定の位置で止まる制御を検索)
	if (key.ExportKeyState(KEY_INPUT_P) && m_stopList.empty()){
		m_stopListPage = 0;
		const unsigned int loopMax = m_ctrlResult.size() / m_orderIDMax;
		const unsigned int charaCombin = PowInteger(m_reelManager.GetCharaNum(), m_reelManager.GetReelNum());
		m_stopListPushStop = key.ExportKeyStateFrame(KEY_INPUT_LSHIFT) >= 1 || key.ExportKeyStateFrame(KEY_INPUT_RSHIFT) >= 1;
		const unsigned short verify = m_stopListPushStop ? 
			m_stopPos[0] << 10 | m_stopPos[1] << 5 | m_stopPos[2] : 
			m_pushPos[0] << 10 | m_pushPos[1] << 5 | m_pushPos[2];
		for (unsigned int i = 0; i < loopMax; ++i) {
			const unsigned int checkID = m_orderID * loopMax + i;
			if (m_ctrlResult[checkID] == verify) m_stopList.push_back(i);
		}
	}

	if (key.ExportKeyState(KEY_INPUT_ESCAPE)) return eStateEnd;

	if (isMoveReel){
		const unsigned int hash =
			m_orderID * PowInteger(m_reelManager.GetCharaNum(), m_reelManager.GetReelNum()) * m_checkedFlagID.size() +
			m_selectingFlag * PowInteger(m_reelManager.GetCharaNum(), m_reelManager.GetReelNum()) +
			m_pushPos[0] * PowInteger(m_reelManager.GetCharaNum(),2) +
			m_pushPos[1] * PowInteger(m_reelManager.GetCharaNum(),1) +
			m_pushPos[2];
		const short stopData = m_ctrlResult[hash];
		m_stopPos[0] = (stopData >> 10) & 0x1F;
		m_stopPos[1] = (stopData >>  5) & 0x1F;
		m_stopPos[2] = (stopData >>  0) & 0x1F;
		m_stopList.clear();
		m_stopListPage = 0;
		//ErrorLogFmtAdd("%d %d %d %d %d %d %d", hash, m_pushPos[0], m_pushPos[1], m_pushPos[2],
		//	m_stopPos[0], m_stopPos[1], m_stopPos[2]);
	}

	return eStateContinue;
}

bool CGameState_ControlTestMain::Draw(CGameDataManage& pDataManageIns){
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	for (int x = 0; x < m_reelManager.GetCharaNum(); ++x){
		char str[2];	str[0] = '0' + x;	str[1] = '\0';
		if (x >= 10) str[0] += 7;
		DxLib::DrawString(176 + x*16, 119, str, 0xA0A0A0);
		DxLib::DrawString(160, 135 + x*16, str, 0xA0A0A0);
		DxLib::DrawString(5, 12 + x*22, str, 0xFFFFFF);
	}

	/* Matrix描画 */{
		int firstPos = -1;
		const int size = m_reelManager.GetCharaNum();
		for (int x = 0; x < size; ++x)
		for (int y = 0; y < size; ++y){
			const char data = m_stopMatrix[m_pushOrder[0]][m_stopPos[m_pushOrder[0]]][x][y];
			int color;
			if (data == 0x0) color = 0x0000000;
			else if (data == 0x1)		color = 0x1E0E0E0;
			else if (data & 0x1)		color = 0x10000C0;
			else if (data & 0x2)		color = 0x100C000;
			else if ((data ^ 0x4) == 0)	color = 0x1AA00FF;
			else if ((data ^ 0x8) == 0)	color = 0x1C0C000;
			else if (data & 0xB)		color = 0x1C00000;
			DxLib::DrawBox(172 + x * 16, 135 + y * 16, 172 + (x + 1) * 16, 135 + (y + 1) * 16, color & 0xFFFFFF, color & 0x1000000 ? TRUE : FALSE);
			DxLib::DrawBox(172 + x * 16, 135 + y * 16, 172 + (x + 1) * 16, 135 + (y + 1) * 16, 0x808080, FALSE);
		}
		DxLib::DrawBox(171, 134, 173 + size * 16, 136 + size * 16, 0x808080, FALSE);
	}

	for (int i = 0; i < m_reelManager.GetReelNum(); ++i) {
		SReelDrawData data;
		data.reelID = i; data.x = 19+44*i; data.y = 9;
		data.comaW = 44; data.comaH = 22;
		data.offsetLower = 0; data.offsetUpper = 0; data.comaNum = 3;
		for (int j = 0; j < m_reelManager.GetCharaNum() / 3; ++j) {
			m_reelManager.DrawReel(pDataManageIns, data, 3 * j);
			data.y += data.comaH * data.comaNum;
		}
	}

	DxLib::DrawString(324, 56, u8"→", 0xFFFF00);
	bool reverseFlag = true;
	unsigned int selectingMatrixColor[2];	// あとで解析するときに使用
	for (int n = 0; n < m_reelManager.GetReelNum(); ++n){
		char str[2];	str[1] = '\0';
		str[0] = '0' + m_pushPos[n];	if (m_pushPos[n] >= 10) str[0] += 7;
		DxLib::DrawString(206 + n * 45, 3, str, 0xB1E2FF);
		str[0] = '0' + m_stopPos[n];	if (m_stopPos[n] >= 10) str[0] += 7;
		DxLib::DrawString(360 + n * 45, 3, str, 0xFFFF00);
		str[0] = '0' + n + 1;
		DxLib::DrawString(206 + m_pushOrder[n] * 45, 98, str, m_selectingReel==m_pushOrder[n] ? 0xFF0000 : 0xFFFFFF);
		int slip = m_pushPos[n] - m_stopPos[n];
		while (slip < 0) slip += m_reelManager.GetCharaNum();
		while (slip >= m_reelManager.GetCharaNum()) slip += m_reelManager.GetCharaNum();
		str[0] = '0' + slip;	if (slip >= 10) str[0] += 7;
		DxLib::DrawString(360 + n * 45, 98, str, 0xFFFFFF);

		SReelDrawData data;
		data.reelID = n; data.x = 188 + n * 45; data.y = 21;
		data.comaW = 44; data.comaH = 22;
		data.offsetLower = 5; data.offsetUpper = 5; data.comaNum = 3;

		m_reelManager.DrawReel(pDataManageIns, data, m_pushPos[n]);
		data.x = 342 + n * 45;
		m_reelManager.DrawReel(pDataManageIns, data, m_stopPos[n]);

		if (m_pushOrder[0] == n) continue;
		
		if (!reverseFlag){
			int y = m_pushPos[n], w=16*m_reelManager.GetCharaNum();
			DxLib::DrawBox(157, 135 + y*16, 172+w, 135 + (y+1)*16, 0xB1E2FF, FALSE);
			DxLib::DrawBox(156, 134 + y*16, 173+w, 136 + (y+1)*16, 0xB1E2FF, FALSE);
			y = m_stopPos[n];
			DxLib::DrawBox(157, 135 + y*16, 172+w, 135 + (y+1)*16, 0xFFFF00, FALSE);
			DxLib::DrawBox(156, 134 + y*16, 173+w, 136 + (y+1)*16, 0xFFFF00, FALSE);
			selectingMatrixColor[0] = y;
		} else {
			int x = m_pushPos[n], h = 16 * m_reelManager.GetCharaNum();
			DxLib::DrawBox(172 + x*16, 118, 172 + (x+1)*16, 135+h, 0xB1E2FF, FALSE);
			DxLib::DrawBox(171 + x*16, 117, 173 + (x+1)*16, 136+h, 0xB1E2FF, FALSE);
			x = m_stopPos[n];
			DxLib::DrawBox(172 + x*16, 118, 172 + (x+1)*16, 135+h, 0xFFFF00, FALSE);
			DxLib::DrawBox(171 + x*16, 117, 173 + (x+1)*16, 136+h, 0xFFFF00, FALSE);
			selectingMatrixColor[1] = x;
		}
		reverseFlag = !reverseFlag;
 	}

	DxLib::DrawFormatString(500, 0, 0xFFFFFF, "Flag: %s", m_randManager.GetFlagName(m_checkedFlagID[m_selectingFlag]).c_str());
	DxLib::DrawFormatString(540, 460, 0xFFFFFF, "reach:%5d", m_reachCount[m_pushOrder[0]]);

	const char matrixData = m_stopMatrix[m_pushOrder[0]][m_stopPos[m_pushOrder[0]]][selectingMatrixColor[1]][selectingMatrixColor[0]];
	DxLib::DrawFormatString(566, 440, 0xFFFFFF, "Info: %02x", matrixData & 0xF);

	// 停止形検索結果表示
	if (m_stopList.size() > 0){
		const unsigned int charaConbin = PowInteger(m_reelManager.GetCharaNum(), m_reelManager.GetReelNum());
		unsigned int i;
		for (i = 0; i < STOP_LIST_NUM_PER_PAGE; ++i){
			const unsigned int ID = i + m_stopListPage * STOP_LIST_NUM_PER_PAGE;
			if (ID >= m_stopList.size()) break;
			DxLib::DrawFormatString(512, 135 + 15 * i, 0xFFFFFF, "%2d-%2d-%2d:%s",
				(m_stopList[ID] / PowInteger(m_reelManager.GetCharaNum(), 2)) % m_reelManager.GetCharaNum(),
				(m_stopList[ID] / PowInteger(m_reelManager.GetCharaNum(), 1)) % m_reelManager.GetCharaNum(),
				(m_stopList[ID] / PowInteger(m_reelManager.GetCharaNum(), 0)) % m_reelManager.GetCharaNum(),
				m_randManager.GetFlagName(m_checkedFlagID[(m_stopList[ID] / charaConbin) % m_checkedFlagID.size()]).c_str());
		}
		const auto& drawPos = m_stopListPushStop ? m_stopPos : m_pushPos;
		DxLib::DrawFormatString(512, 105, 0xFFFFFF, u8"Stop→%2d-%2d-%2d", drawPos[0], drawPos[1], drawPos[2]);
		DxLib::DrawFormatString(512, 120, 0xFFFFFF, "%4d-%4d/%4d",
			m_stopListPage*STOP_LIST_NUM_PER_PAGE+1, m_stopListPage*STOP_LIST_NUM_PER_PAGE + i, m_stopList.size());
	}
	return true;
}

void CGameState_ControlTestMain::ReelMove(bool isUpper){
	auto& data = m_pushPos[m_selectingReel];
	data += isUpper ? -1 : 1;
	while (data < 0) data += m_reelManager.GetCharaNum();
	while (data >= m_reelManager.GetCharaNum()) data -= m_reelManager.GetCharaNum();
}

void CGameState_ControlTestMain::SelectReelChange(bool isRight){
	auto& data = m_selectingReel;
	data += isRight ? 1 : -1;
	while (data < 0) data += m_reelManager.GetReelNum();
	while (data >= m_reelManager.GetReelNum()) data -= m_reelManager.GetReelNum();
}
