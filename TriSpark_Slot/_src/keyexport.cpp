#include "_header\keyexport.h"
CKeyExport_S::CKeyExport_S(){
	for(int i=0;i<256;i++)
		m_NowKeyState[i]=0;
}
void CKeyExport_S::KeyStateUpdate(char* pInput){
// [prm]p1;キー入力情報が入った変数のポインタ(m1で指定したサイズの配列であること)
// [act]CKeyInput_Sからのアクセス専用。m2を更新する。
	for(int i=0;i<GetInputArrayMax_C;i++)
		if(pInput[i]) m_NowKeyState[i]++;
		else m_NowKeyState[i]=0;
}
bool CKeyExport_S::ExportKeyState(int KeyHandle,int LessFrame, int MaxFrame) const{
// [prm]p2;下限キーフレーム数(これ以上ならtrue)
// [act]指定キーがp2フレーム以上押されているか返す
//		p1が異常値( 0～m1-1 以外)ならfalseを返す。
	if(KeyHandle<0||KeyHandle>=GetInputArrayMax_C) return false;
	bool Ans = (m_NowKeyState[KeyHandle]>=LessFrame);
	Ans &= (m_NowKeyState[KeyHandle]<=MaxFrame);
	return Ans;
}
int CKeyExport_S::ExportKeyStateFrame(int KeyHandle) const{
// [act]指定のキーが連続で押されているフレーム数を返す。
//		p1が異常値なら-1を返す。
	if(KeyHandle<0||KeyHandle>=GetInputArrayMax_C) return -1;
	return m_NowKeyState[KeyHandle];
}
