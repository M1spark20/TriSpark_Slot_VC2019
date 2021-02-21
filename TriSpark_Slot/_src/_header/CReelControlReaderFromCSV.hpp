#pragma once
struct SReelControlData;
struct SMainReadFileIndex;

class CReelControlReaderFromCSV {

	signed char* m_binaryData;
	unsigned int m_binarySize;
	template<class T> void ConvertBinaryNum(T& pResFor, unsigned int start, unsigned int size){
		pResFor = 0;
		for (unsigned int i = 0; i < size; ++i){
			T temp = m_binaryData[i+start] & 0xFF;
			pResFor |= temp << (i * 8);
		}
	}

public:
	CReelControlReaderFromCSV();
	bool FileInit();
	bool FileInit(const SMainReadFileIndex* p_pFileData);
	bool MakeData(SReelControlData& p_Data, int pReelNum);
	~CReelControlReaderFromCSV();

};
