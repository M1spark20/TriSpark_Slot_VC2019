#include "_header/CReelControlReaderFromCSV.hpp"
#include "_header/SReelControlData.hpp"
#include "_header/SMainReadFileIndex.h"
#include "DxLib.h"
#include <fstream>

CReelControlReaderFromCSV::CReelControlReaderFromCSV(){
	m_binaryData = nullptr;
	m_binarySize = 0;
}

bool CReelControlReaderFromCSV::FileInit(){
// [act]リール演出抽選データが格納されたCSVを開く
// [ret]ファイルオープンに成功したかどうか
	const char* FileName = "data/reel_table.csv";
	return false;
}

bool CReelControlReaderFromCSV::FileInit(const SMainReadFileIndex* p_pFileData){
	// [act]DxLib側で開いたファイルからデータを読み出す
	// [ret]ファイルオープンに成功したかどうか
	int fileID = p_pFileData->DataHandle;
	int fileSize = p_pFileData->FileSize & 0xFFFFFFFF;
	m_binaryData = new char[fileSize];
	m_binarySize = DxLib::FileRead_read(m_binaryData, fileSize, fileID);
	return m_binarySize == fileSize;
}

bool CReelControlReaderFromCSV::MakeData(SReelControlData& p_Data, int pReelNum){
	typedef unsigned int UINT;
	std::string::size_type readPos = 0;
	std::string::size_type readMax = 0;

	while (readPos < m_binarySize){
		SReelControlHeader newHeader;
		//ErrorLogFmtAdd("start: %d / %x : %d / %x", readPos, readPos, readMax, readMax);
		newHeader.flagID	=m_binaryData[readPos + 0];
		newHeader.bonusID	= m_binaryData[readPos + 1];
		newHeader.ctrlType	= m_binaryData[readPos + 2];
		ConvertBinaryNum(newHeader.dataSize, readPos + 3, 2);
		readMax = readPos + (std::string::size_type)newHeader.dataSize;

		//ErrorLogFmtAdd("Data flag: %d, bonus: %d, type: %d, size:%d", newHeader.flagID, newHeader.bonusID, newHeader.ctrlType, newHeader.dataSize);
		if (readMax > m_binarySize)
			return false;
		readPos += 5;

		int loopCount;
		SReelControlCommand newCommand;
		while (readPos < readMax){
			loopCount = newCommand.slipData.size();
			// ctrlType:1 のspotData読み込み
			if (loopCount >= 3 + 21*6 && newHeader.ctrlType == 1){
				SReelControlSpot newSpot;
				ConvertBinaryNum(newSpot.spot, readPos, 2);
				newSpot.slipStart = loopCount;
				newCommand.spotData.push_back(newSpot);
				readPos += 2;
			}
			// ctrlType:2 のspotData読み込み
			if (loopCount >= 3 && newHeader.ctrlType == 2){
				SReelControlSpot newSpot;
				ConvertBinaryNum(newSpot.spot,readPos, 2);
				newSpot.slipStart = loopCount;
				newCommand.spotData.push_back(newSpot);
				readPos += 2;
			}
			// ctrlType:3 のspotData読み込み
			if (loopCount >= 3 && newHeader.ctrlType == 3 && loopCount%2 == 1){
				SReelControlSpot newSpot;
				ConvertBinaryNum(newSpot.spot, readPos, 1);
				newSpot.slipStart = loopCount;
				newCommand.spotData.push_back(newSpot);
				++readPos;
			}
			unsigned long long slipVal;
			ConvertBinaryNum(slipVal, readPos, 8);
			newCommand.slipData.push_back(slipVal);
			readPos += 8;
		}
		p_Data.command.push_back(newCommand);
		p_Data.header.push_back(newHeader);
	}
	return true;
}

CReelControlReaderFromCSV::~CReelControlReaderFromCSV(){
	if (m_binaryData != nullptr) delete m_binaryData;
}
