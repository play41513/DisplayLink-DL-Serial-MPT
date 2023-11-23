﻿// Module:
// Notices: Copyright(c) 2017 Leno.Lee.
// Purpose:
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 3.2.2.79__declspec(uuid("{2DD69FE9-6425-4DB2-A21F-7987EDF08B77}"))
// ---------------------------------------------------------------------------
#define     Define_Get_No_Key_Time  200
#define     Timer2Interval          100
#define     Timer2_Delay_Interval   1000
#define     DelayUs_Value           500

#define buffer_size 2048
#define sampledot 100000
#define sampletime 0.00001

#include <vcl.h>
#pragma hdrstop



#include <vfw.h>
#include <direct.h>
#include <stdio.h>


extern "C" {
#include "hidsdi.h"
#include "setupapi.h"
}
#include "DUT.h"
#include "Uint1.h"

#include <inifiles.hpp>
#include <systdate.h>
#include <mmsystem.h>
#include <time.h>
#include <tchar.h>
#include "ConstantString.h"

char autoERRC, autoSTEP;
char oneERRC;
bool VocI, VocO, AuL, AuR, oneSTEP;
bool AuLPass, AuRPass, autoClose;
USB_Hub_Tree *g_pUsb_Hub_Tree = NULL;
int showaudiotup;
bool G_bCheckDevInfo = false;

// char Read_ID,Read_Cmd;
// unsigned int Read_Addr,Read_Len,Read_Len_ck;
// ---------------------------------------------------------------------------
bool TUSBHIDForm::DisplayHub(tinyxml2::XMLElement *pHub, int tier){
	String tmp;

	for(int i=0; i<tier; i++){
    	tmp += _T("---");
	}

	tmp += pHub->GetText();
	memoMsg->Lines->Add(tmp);

	//子節點
	pHub = pHub->FirstChildElement();

	while(pHub != NULL){
        if(!DisplayHub(pHub, ++tier)){			return false;		}		pHub = pHub->NextSiblingElement();	};

	return true;
}
// ---------------------------------------------------------------------------
bool TUSBHIDForm::GetUsbHubTree(USB_Hub_Tree &hubTree){
	bool bl;	tinyxml2::XMLElement *pHub;	int tier;	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	memoMsg->Lines->Add(USB_HUB_FIRMWARE_VERSION);	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	bl = hubTree.Refresh();
	if(bl){		//根節點		tier = 0;		pHub = hubTree.RootElement()->FirstChildElement();		while(pHub != NULL){			if(!DisplayHub(pHub, tier)){				bl = false;				break;			}			pHub = pHub->NextSiblingElement();		};	}	return bl;}
// ---------------------------------------------------------------------------
bool TUSBHIDForm::CompareTreeNode(tinyxml2::XMLElement *pRef, tinyxml2::XMLElement *pTst){
	String refDisplayName, tstDisplayName;

	assert(pRef);

	refDisplayName = pRef->GetText();
	tstDisplayName = pTst->GetText();
	if(refDisplayName.Trim() != tstDisplayName.Trim()){
    	return false;
	}

	//子節點
	pRef = pRef->FirstChildElement();

	if(pRef != NULL && pTst != NULL){
		pTst = pTst->FirstChildElement();
	}

	while(pRef != NULL){
		if(!CompareTreeNode(pRef, pTst)){
			return false;
		}

		pRef = pRef->NextSiblingElement();

        if(pRef != NULL && pTst != NULL){
			pTst = pTst->NextSiblingElement();
		}
	};

	return true;
}
// ---------------------------------------------------------------------------
bool TUSBHIDForm::CompareUsbHubTree(USB_Hub_Tree &refTree, USB_Hub_Tree &tstTree){
	//根節點	XMLElement *pRef = refTree.RootElement();	XMLElement *pTst = tstTree.RootElement();				//	//根節點的值				//	pRefRoot->GetText();                //	//根節點的第一個屬性                //	const XMLAttribute *pAttribute = pRefRoot->FindAttribute();                //	//屬性的名和值                //	pAttribute->Name();                //	pAttribute->Value();                //	//查找節點的屬性值                //	float value = 0.1f;                //	pRefRoot->QueryFloatAttribute("version", &value);                //	//設罝節點屬性值                //	pRefRoot->SetAttribute("version", 1.4);                //	//根節點的第一個子節點                //	XMLElement *pDict = pRefRoot->FirstChildElement();                //	//節點的值                //	pDict->Name();				//	pDict->GetText();	//子節點	pRef = pRef->FirstChildElement();	if(pRef != NULL){		pTst = pTst->FirstChildElement();		if(pTst == NULL){			return false;		}	}	while(pRef != NULL){		if(!CompareTreeNode(pRef, pTst)){			return false;		}		pRef = pRef->NextSiblingElement();		if(pTst != NULL){			pTst = pTst->NextSiblingElement();		}	};	return true;}
// ---------------------------------------------------------------------------
bool TUSBHIDForm::VerifyUsbHubHardwareId(){	bool bl;	USB_Hub_Tree hubTree;	bl = hubTree.Refresh();	if(bl){		bl = CompareUsbHubTree(refUsbHubTree, hubTree);	}	return bl;}
//---------------------------------------------------------------------------bool TUSBHIDForm::Xml2Tree(tinyxml2::XMLDocument *doc){	bool bl;	tinyxml2::XMLElement *node = doc->RootElement();	trvReference->Items->Clear();	if(ProcessXmlItem(node, NULL)){		trvReference->FullExpand();		trvReference->Select(trvReference->Items->Item[0]);		bl = true;	}else{		trvReference->Items->Clear();		bl = false;	}	return bl;}//---------------------------------------------------------------------------bool TUSBHIDForm::ProcessXmlItem(tinyxml2::XMLElement *pParentNode, TTreeNode *pTreeNode){	tinyxml2::XMLElement *pNode;	if(pParentNode == NULL){		return false;	}	trvReference->Items->BeginUpdate();	pTreeNode = trvReference->Items->AddChild(pTreeNode, pParentNode->GetText());	trvReference->Items->EndUpdate();	pNode = pParentNode->FirstChildElement();	while(pNode){		if(!ProcessXmlItem(pNode, pTreeNode)){            trvReference->Items->Clear();			return false;		}		pNode = pNode->NextSiblingElement();	};	return true;}bool TUSBHIDForm::CompareInfoFlash(InfoFlash &di, String backupFirmware, String MinFwVersion, String &msg){	 if(backupFirmware != di.BackFirmware){		msg = _T("_Backup Firmware Error");
		return false;
	 }

	if(MinFwVersion != di.MinFwVersion){
		msg = _T("_Encrypted Version Error");
		return false;
	}

	return true;}
bool __fastcall TUSBHIDForm::VerifyInfoFlash() {
	String msg;

	if(!CompareInfoFlash(m_dut._If1, edtBackupFirmwareSample->Text, edtMinFwVersionSample->Text, msg)){
		memoMsg->Lines->Add(msg + String(_T(" on ") + m_dut._Dev1));
		return false;
	}

	if(m_dut._Kind == 2){
		if(!CompareInfoFlash(m_dut._If2, edtBackupFirmwareSample->Text, edtMinFwVersionSample->Text, msg)){
			memoMsg->Lines->Add(msg + String(_T(" on ") + m_dut._Dev2));
			return false;
		}
	}

	return true;
}

// 配置檔頭記憶體資源
bool TUSBHIDForm::WaveMakeHeader(DWORD ulSize, HGLOBAL &HData,
	HGLOBAL &HWaveHdr, LPSTR &lpData, LPWAVEHDR &lpWaveHdr) {
	HData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, ulSize);
	if (!HData)
		return false;

	lpData = (LPSTR)GlobalLock(HData);
	if (!lpData) {
		GlobalFree(HData);
		lpData = NULL;
		return false;
	}

	HWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(WAVEHDR));
	if (!HWaveHdr) {
		GlobalUnlock(HData);
		GlobalFree(HData);
		lpData = NULL;
		return false;
	}

	lpWaveHdr = (LPWAVEHDR)GlobalLock(HWaveHdr);
	if (!lpWaveHdr) {
		GlobalUnlock(HWaveHdr);
		GlobalFree(HWaveHdr);
		lpWaveHdr = NULL;
		GlobalUnlock(HData);
		GlobalFree(HData);
		lpData = NULL;
		return false;
	}

	// zero-out the WAVEHDR then assign the lpData member of the
	// header to the allocated data buffer and the dwBufferLength
	// member of the header to the size of data block
	ZeroMemory(lpWaveHdr, sizeof(WAVEHDR));
	ZeroMemory(lpData, ulSize);
	lpWaveHdr->lpData = lpData;
	lpWaveHdr->dwBufferLength = ulSize;

	return true;
}

// ---------------------------------------------------------------------------
// 釋放檔頭記憶體資源
void TUSBHIDForm::WaveFreeHeader(HGLOBAL &HData, HGLOBAL &HWaveHdr) {
	GlobalUnlock(HWaveHdr);
	GlobalFree(HWaveHdr);
	GlobalUnlock(HData);
	GlobalFree(HData);
}

// ---------------------------------------------------------------------------
// 開啟錄音設備
bool TUSBHIDForm::WaveRecordOpen(LPHWAVEIN lphwi, HWND Hwnd, int nChannels,
	long lFrequency, int nBits) {
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)nChannels;
	wfx.nSamplesPerSec = (DWORD)lFrequency;
	wfx.wBitsPerSample = (WORD)nBits;
	wfx.nBlockAlign = (WORD)((wfx.nChannels * wfx.wBitsPerSample) / 8);
	wfx.nAvgBytesPerSec = (wfx.nSamplesPerSec * wfx.nBlockAlign);
	wfx.cbSize = 0;

	MMRESULT result;// = MMSYSERR_NOERROR;

	result = waveInOpen(lphwi, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_QUERY);
	if (result != MMSYSERR_NOERROR) {
		ShowMessage("錄音設備不支援預設音訊格式!");
		return false;
	}

	result = waveInOpen(lphwi, WAVE_MAPPER, &wfx, (LONG)Hwnd, NULL,
		CALLBACK_WINDOW);

	if (result != MMSYSERR_NOERROR) {
		ShowMessage("無法開啟錄音設備!");
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------------
// 準備錄音
bool TUSBHIDForm::WaveRecordPrepare(HWAVEIN hwi, LPWAVEHDR &lpWaveHdr) {
	MMRESULT result = waveInPrepareHeader(hwi, lpWaveHdr, sizeof(WAVEHDR));
	if (result == MMSYSERR_NOERROR) {
		MMRESULT result = waveInAddBuffer(hwi, lpWaveHdr, sizeof(WAVEHDR));
		if (result == MMSYSERR_NOERROR)
			return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// 結束錄音
void TUSBHIDForm::WaveRecordEnd(HWAVEIN hwi, LPWAVEHDR &lpWaveHdr) {
	waveInStop(hwi);
	waveInReset(hwi);
	waveInUnprepareHeader(hwi, lpWaveHdr, sizeof(WAVEHDR));
}

// ---------------------------------------------------------------------------
// 關閉錄音設備
void TUSBHIDForm::WaveRecordClose(HWAVEIN hwi) {
	waveInReset(hwi);
	waveInClose(hwi);
}

// ---------------------------------------------------------------------------
// 開啟播音設備
bool TUSBHIDForm::WavePlayOpen(LPHWAVEOUT lphwo, HWND Hwnd, int nChannels,
	long lFrequency, int nBits) {
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)nChannels;
	wfx.nSamplesPerSec = (DWORD)lFrequency;
	wfx.wBitsPerSample = (WORD)nBits;
	wfx.nBlockAlign = (WORD)((wfx.nChannels * wfx.wBitsPerSample) / 8);
	wfx.nAvgBytesPerSec = (wfx.nSamplesPerSec * wfx.nBlockAlign);
	wfx.cbSize = 0;

	MMRESULT result;// = MMSYSERR_NOERROR;
	result = waveOutOpen(lphwo, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_QUERY);
	if (result != MMSYSERR_NOERROR) {
		ShowMessage("播音設備不支援預設音訊格式!");
		return false;
	}

	result = waveOutOpen(lphwo, WAVE_MAPPER, &wfx, (LONG)Hwnd, NULL,
		CALLBACK_WINDOW);
	if (result != MMSYSERR_NOERROR) {
		ShowMessage("無法開啟播音設備!");
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------------
// 準備播音
bool TUSBHIDForm::WavePlayPrepare(HWAVEOUT hwo, LPWAVEHDR &lpWaveHdr) {
	MMRESULT result = waveOutPrepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));
	if (result == MMSYSERR_NOERROR) {
		int x;
		// 填入波型資料
		for (int i = 0; i < len; i++) {
			x = i / 22;
			if (x % 2 == 0) {
				lpWaveHdr_O->lpData[i] = 0x01;
			}
			else {
				lpWaveHdr_O->lpData[i] = 0x7F;
			}
		}
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// 結束播音
void TUSBHIDForm::WavePlayEnd(HWAVEOUT hwo, LPWAVEHDR &lpWaveHdr) {
	waveOutReset(hwo);
	waveOutUnprepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));
}

// ---------------------------------------------------------------------------
// 關閉播音設備
void TUSBHIDForm::WavePlayClose(HWAVEOUT hwo) {
	waveOutReset(hwo);
	waveOutClose(hwo);
}

// ---------------------------------------------------------------------------
unsigned char buf[20000];

void __fastcall TUSBHIDForm::MMWimData(TMessage &Msg) {
	Memo1->Lines->Add("MMWimData");
	// char buf[20000];
	// if(FPlaying){
	// ShowMessage("未取得完整測試音訊資料!");
	// }

	// 取得錄音音訊資料
	// static unsigned char * buf = new unsigned char(len);
	memset(buf, 0, len);
	Memo1->Lines->Add(len);
	memcpy(buf, lpData_I, len);

	// #ifdef _DEBUG
	/*
	FILE  *fp = fopen("data.txt","w+");
	for(int i=0;i<len;i++)
	{
	//	buf[i]/=200;
	fprintf(fp,"i%d=%d\n",i,(unsigned int)buf[i]);
	}
	fclose(fp);
	 */
	// #endif
	// delete buf;

	// Caption = "DONE RECORDING 6" ;

	if (FRecording)
		WaveRecordEnd(hwi, lpWaveHdr_I);
	waveInUnprepareHeader(hwi, lpWaveHdr_I, sizeof(WAVEHDR));
	WaveRecordClose(hwi);
	FRecording = false;
	// Caption = "DONE TESTING 7" ;

	// 釋放資源
	// WaveFreeHeader
	WaveFreeHeader(hgData_I, hgWaveHdr_I);
	lpData_I = NULL;
	lpWaveHdr_I = NULL;
	/*
	if(!FPlaying){
	WaveFreeHeader(hgData_O, hgWaveHdr_O);
	lpData_O = NULL;
	lpWaveHdr_O = NULL;
	}
	 */
	// if(!FRecording){
	// Caption = "DONE TESTING 8" ;
	// }
	VocI = true;
	CheckAudio();
}

// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::MMWomDone(TMessage &Msg) {
	Memo1->Lines->Add("MMWomDone");
	// Caption = "DONE PLAYING 8" ;
	waveOutUnprepareHeader(hwo, lpWaveHdr_O, sizeof(WAVEHDR));
	WavePlayClose(hwo);
	FPlaying = false;
	// 釋放資源
	// WaveFreeHeader
	/*
	if(!FRecording){
	WaveFreeHeader(hgData_I, hgWaveHdr_I);
	lpData_I = NULL;
	lpWaveHdr_I = NULL;
	}
	 */
	WaveFreeHeader(hgData_O, hgWaveHdr_O);
	lpData_O = NULL;
	lpWaveHdr_O = NULL;
	VocO = true;
	CheckAudio();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
int __fastcall TUSBHIDForm::HexToInt(AnsiString HexStr) {
	int iIndex, iHex, totalChar;
	TCHAR HexChar[8];

	HexStr.UpperCase(); // 把字串轉成全大寫

	_tcscpy_s(HexChar, 8, HexStr.c_str());
//	strcpy(HexChar, HexStr.c_str()); // 把字串轉成字元陣列
	iHex = 0;
	totalChar = HexStr.Length(); // 取得字串的長度
	for (iIndex = 0; iIndex < totalChar; iIndex++) {
		iHex <<= 4;
		if ((HexChar[iIndex] >= 0x30) && (HexChar[iIndex] <= 0x39)) {
			iHex += (HexChar[iIndex] - 0x30); // 把 0 - 9 字元轉成數字
		}
		else if ((HexChar[iIndex] >= 0x41) && (HexChar[iIndex] <= 0x46)) {
			iHex += (HexChar[iIndex] - 0x37); // ­把 A - F 字元轉成數字
		}
		else {
			iHex = 0;
			break;
		}
	}
	return iHex;
}// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUSBHIDForm *USBHIDForm;
HANDLE hReadHIDThread;
// ---------------------------------------------------------------------------

__fastcall TUSBHIDForm::TUSBHIDForm(TComponent* Owner) : TForm(Owner) {;
}
// ---------------------------------------------------------------------------
AnsiString FileName;

void __fastcall TUSBHIDForm::FormCreate(TObject *Sender) {
	showaudiotup = 0;
    int windowsVer;
	OSVERSIONINFO osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	windowsVer = osvi.dwMajorVersion *10 + osvi.dwMinorVersion;
	if(windowsVer == 62){
		MessageBox(NULL, "Windows版本不符!", "警告訊息", MB_ICONWARNING);
	}
	Caption = APP_TITLE;
	ckbVerifyUsbHubVersion->Caption = VERIFY_USB_HUB_VERSION;
	ckbVerifyDisplayLinkVersion->Caption = VERIFY_DL_FIRMWARE_VERSION;	ckbDutMaxTimeGetId->ItemIndex = 11;	cmbNumOfChip->ItemIndex = 0;	getKindCnt = 0;    memoMsg->Height = 684;	// SYSTEMTIME   SystemTime;	// GetSystemTime(&SystemTime);     //获取格林威治时间	// AnsiString strMessage;	SYSTEMTIME myTime; // = GetSysTime();	GetSystemTime(&myTime);	// strMessage=myTime.wMonth, myTime.wDay, myTime.wYear, myTime.wHour, myTime.wMinute, myTime.wSecond );	FileName.printf("C:\\ASMP\\bin\\DisplayLink DL Serial MPT\\log\\%02d%02d%d%02d%02d%02d.LOG", myTime.wMonth, myTime.wDay,		myTime.wYear, myTime.wHour, myTime.wMinute, myTime.wSecond);	FAllocated_I = false;	FAllocated_O = false;	FRecording = false;	FPlaying = false;	hReadHIDThread = CreateEvent(NULL, FALSE, TRUE, NULL);	ReadHIDThread = new TReadHIDThread(TRUE);//	ReadHIDThread->FreeOnTerminate = true;	ReadHIDThread->Start();	CurrentUserHandle = NULL;	if(FileExists(REFERENCE_XML)){		refUsbHubTree.LoadFile(REFERENCE_XML);		Xml2Tree(&refUsbHubTree);	}	if (FileExists(ChangeFileExt(Application->ExeName, ".ini"))) {		TIniFile *ini;		ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));		//KIND		cmbNumOfChip->ItemIndex = ini->ReadInteger("KIND", "KIND", 0);		//TEST_ITEMS		ckbVerifyUsbHubVersion->Checked = ini->ReadBool("TEST_ITEMS", "U3_HUB_REV", false);		ckbVerifyDisplayLinkVersion->Checked = ini->ReadBool("TEST_ITEMS", "FIRMWARE_REV", false);		ckbDataBusTest->Checked = ini->ReadBool("TEST_ITEMS", "DDR_DATA", false);		ckbAddressBusTest->Checked = ini->ReadBool("TEST_ITEMS", "DDR_ADDRESS", false);		ckbCheckerboardTest->Checked = ini->ReadBool("TEST_ITEMS", "DDR_CHECKER", false);		ckbByteCountDataTest->Checked = ini->ReadBool("TEST_ITEMS", "DDR_BYTE", false);		ckbEthernetLoopbackTest->Checked = ini->ReadBool("TEST_ITEMS", "ETH_LOOP", false);		ckbVideo1080p->Checked = ini->ReadBool("TEST_ITEMS", "VIDEO_1080P", false);		ckbVideo4k->Checked = ini->ReadBool("TEST_ITEMS", "VIDEO_4K", false);		ckbVideo4k601->Checked = ini->ReadBool("TEST_ITEMS", "VIDEO_4K60", false);		ckbVideo4k602->Checked = ini->ReadBool("TEST_ITEMS", "VIDEO_4K60_2", false);		ckbAudio->Checked = ini->ReadBool("TEST_ITEMS", "AUDIO", false);        //BURN_SETTING		ckbBurn->Checked = ini->ReadBool("BURN_SETTING", "BURN", false);		ckbBurnVerify->Checked = ini->ReadBool("BURN_SETTING", "VERIFY", false);		ckbAutoPrint->Checked = ini->ReadBool("BURN_SETTING", "PRINT", false);	 	ckbAutoIncrease->Checked = ini->ReadBool("BURN_SETTING", "AUTO", false);		edtBurnCount->Text = ini->ReadString(_T("BURN_SETTING"), _T("BUTN_COUNT"), _T("1"));		mskedtBeginSn->Text = ini->ReadString(_T("BURN_SETTING"), _T("BEGIN_SN"), _T("0"));//		mskedtEndSn->Text = ini->ReadString(_T("BURN_SETTING"), _T("END_SN"), _T("0"));		mskedtMacPre->Text = ini->ReadString(_T("BURN_SETTING"), _T("MAC_PREFIX"), _T("00249B"));		mskedtBeginMac->Text = ini->ReadString(_T("BURN_SETTING"), _T("BEGIN_MAC_SN"), _T("000001"));		//BURN_CONTENT		mskedtBurnSn->Text = ini->ReadString(_T("BURN_CONTENT"), _T("BURN_SN"), _T("0"));		mskedBurntMacPre->Text = ini->ReadString(_T("BURN_CONTENT"), _T("MAC_PREFIX"), _T("0"));//		mskedtBurnMac->Text = ini->ReadString(_T("BURN_CONTENT"), _T("MAC_SN"), _T("0"));		//REWORK		ckbRework->Checked = ini->ReadBool("REWORK", "REWORK", false);		//SAMPLE		edtBackupFirmwareSample->Text = ini->ReadString(_T("SAMPLE"),			_T("BackupFirmware"), _T("0"));		edtMinFwVersionSample->Text = ini->ReadString(_T("SAMPLE"),			_T("MinFwVersion"), _T("0"));		edtBackupFirmwareSample2->Text = ini->ReadString(_T("SAMPLE"),			_T("BackupFirmware2"), _T("0"));		edtMinFwVersionSample2->Text = ini->ReadString(_T("SAMPLE"),			_T("MinFwVersion2"), _T("0"));
		//HID		Edit1->Text = ini->ReadString("HID", "E1", "0");		Edit10->Text = ini->ReadString("HID", "E10", "0");		Edit11->Text = ini->ReadString("HID", "E11", "0");		Edit12->Text = ini->ReadString("HID", "E12", "0");		Edit13->Text = ini->ReadString("HID", "E13", "0");		Edit14->Text = ini->ReadString("HID", "E14", "0");		Edit15->Text = ini->ReadString("HID", "E15", "0");		Edit16->Text = ini->ReadString("HID", "E16", "0");		Edit17->Text = ini->ReadString("HID", "E17", "0");		Edit18->Text = ini->ReadString("HID", "E18", "0");		Edit19->Text = ini->ReadString("HID", "E19", "0");		Edit20->Text = ini->ReadString("HID", "E20", "0");		Edit21->Text = ini->ReadString("HID", "E21", "0");		Edit22->Text = ini->ReadString("HID", "E22", "0");		Edit23->Text = ini->ReadString("HID", "E23", "0");		Edit24->Text = ini->ReadString("HID", "E24", "0");		Edit25->Text = ini->ReadString("HID", "E25", "0");		Edit26->Text = ini->ReadString("HID", "E26", "0");		Edit27->Text = ini->ReadString("HID", "E27", "0");		Edit28->Text = ini->ReadString("HID", "E28", "0");		Edit29->Text = ini->ReadString("HID", "E29", "0");		Edit30->Text = ini->ReadString("HID", "E30", "0");		Edit31->Text = ini->ReadString("HID", "E31", "0");		Edit32->Text = ini->ReadString("HID", "E32", "0");		Edit33->Text = ini->ReadString("HID", "E33", "0");		Edit34->Text = ini->ReadString("HID", "E34", "0");		Edit35->Text = ini->ReadString("HID", "E35", "0");		Edit36->Text = ini->ReadString("HID", "E36", "0");		Edit37->Text = ini->ReadString("HID", "E37", "0");		Edit38->Text = ini->ReadString("HID", "E38", "0");		Edit39->Text = ini->ReadString("HID", "E39", "0");		Edit40->Text = ini->ReadString("HID", "E40", "0");		Edit41->Text = ini->ReadString("HID", "E41", "0");		ckbDutMaxTimeGetId->ItemIndex = ini->ReadInteger("DELAY", "SwitchMode", 9);		sb_value_audio->Position      = ini->ReadInteger("AUDIO", "Voiume",20);		ckbDutMaxTimeGetId->Update();		//Save check items for enable MTS mode.		m_bMts[0] = ckbBurn->Checked;		m_bMts[1] = ckbBurnVerify->Checked;		m_bMts[2] = ckbAutoPrint->Checked;		m_bMts[3] = ckbAutoIncrease->Checked;		m_bMts[4] = ckbRework->Checked;		ckbMtsMode->Checked = ini->ReadBool("TEST_MODE", "MTS", false);		delete ini;	}	int i = ckbDutMaxTimeGetId->Items->Strings[ckbDutMaxTimeGetId->ItemIndex].ToInt();	m_dut.SetDutMaxTimeGetId(i);	Timer1->Enabled = true;

	if(FileExists("loging.log")){
		DeleteFile("loging.log");
	}
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::FormClose(TObject *Sender, TCloseAction &Action) {
	/*
	if (FAllocated_I){ WaveFreeHeader(hgData_I, hgWaveHdr_I); }
	if (FAllocated_O){ WaveFreeHeader(hgData_O, hgWaveHdr_O); }
	if (FRecording) WaveRecordClose(hwi);
	if (FPlaying) WavePlayClose(hwo);
	 */
	Timer1->Enabled = false;
    Timer2->Enabled = false;
	Timer3->Enabled = false;

	ReadHIDThread->Terminate();
	CloseHandle(hReadHIDThread);

    //delete ReadHIDThread;  //會導致無法正常關閉FORM




	TIniFile *ini;
	ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));

	//First step. Save the MTS mode option.
	ini->WriteBool("TEST_MODE", "MTS", ckbMtsMode->Checked);

	//Seconed step. 回復受引響的設定
	if(ckbMtsMode->Checked){
		ckbMtsMode->Checked = false;
	}

	//Thired step. 儲存所有設定
	ini->WriteString(_T("KIND"), _T("KIND"), cmbNumOfChip->Text);
	//TEST_ITEMS
	ini->WriteBool("TEST_ITEMS", "U3_HUB_REV", ckbVerifyUsbHubVersion->Checked);
	ini->WriteBool("TEST_ITEMS", "FIRMWARE_REV", ckbVerifyDisplayLinkVersion->Checked);
	ini->WriteBool("TEST_ITEMS", "DDR_DATA", ckbDataBusTest->Checked);
	ini->WriteBool("TEST_ITEMS", "DDR_ADDRESS", ckbAddressBusTest->Checked);
	ini->WriteBool("TEST_ITEMS", "DDR_CHECKER", ckbCheckerboardTest->Checked);
	ini->WriteBool("TEST_ITEMS", "DDR_BYTE", ckbByteCountDataTest->Checked);
	ini->WriteBool("TEST_ITEMS", "ETH_LOOP", ckbEthernetLoopbackTest->Checked);
	ini->WriteBool("TEST_ITEMS", "VIDEO_1080P", ckbVideo1080p->Checked);
	ini->WriteBool("TEST_ITEMS", "VIDEO_4K", ckbVideo4k->Checked);
	ini->WriteBool("TEST_ITEMS", "VIDEO_4K60", ckbVideo4k601->Checked);
	ini->WriteBool("TEST_ITEMS", "VIDEO_4K60_2", ckbVideo4k602->Checked);
	ini->WriteBool("TEST_ITEMS", "AUDIO", ckbAudio->Checked);
	//BURN_SETTING
	ini->WriteBool("BURN_SETTING", "BURN", ckbBurn->Checked);
	ini->WriteBool("BURN_SETTING", "VERIFY", ckbBurnVerify->Checked);
	ini->WriteBool("BURN_SETTING", "PRINT", ckbAutoPrint->Checked);
	ini->WriteBool("BURN_SETTING", "AUTO", ckbAutoIncrease->Checked);
	ini->WriteString(_T("BURN_SETTING"), _T("BUTN_COUNT"), edtBurnCount->Text);
	ini->WriteString(_T("BURN_SETTING"), _T("BEGIN_SN"), mskedtBeginSn->Text);
//	ini->WriteString(_T("BURN_SETTING"), _T("END_SN"), mskedtEndSn->Text);
	ini->WriteString(_T("BURN_SETTING"), _T("MAC_PREFIX"), mskedtMacPre->Text);
	ini->WriteString(_T("BURN_SETTING"), _T("BEGIN_MAC_SN"), mskedtBeginMac->Text);
//	ini->WriteString(_T("BURN_SETTING"), _T("END_MAC_SN"), mskedtEndMac->Text);
	//BURN_CONTENT
	ini->WriteString(_T("BURN_CONTENT"), _T("BURN_SN"), mskedtBurnSn->Text);
	ini->WriteString(_T("BURN_CONTENT"), _T("MAC_PREFIX"), mskedBurntMacPre->Text);
//	ini->WriteString(_T("BURN_CONTENT"), _T("MAC_SN"), mskedtBurnMac->Text);
   //REWORK
	ini->WriteBool("REWORK", "REWORK", ckbRework->Checked);
	//SAMPLE
	ini->WriteString("SAMPLE", "BackupFirmware",
		edtBackupFirmwareSample->Text.c_str());
	ini->WriteString("SAMPLE", "MinFwVersion",
		edtMinFwVersionSample->Text.c_str());
	ini->WriteString("SAMPLE", "BackupFirmware2",
		edtBackupFirmwareSample2->Text.c_str());
	ini->WriteString("SAMPLE", "MinFwVersion2",
		edtMinFwVersionSample2->Text.c_str());

	ini->WriteString("HID", "E1", Edit1->Text.c_str());
	//HID
	ini->WriteString("HID", "E10", Edit10->Text.c_str());
	ini->WriteString("HID", "E11", Edit11->Text.c_str());
	ini->WriteString("HID", "E12", Edit12->Text.c_str());
	ini->WriteString("HID", "E13", Edit13->Text.c_str());
	ini->WriteString("HID", "E14", Edit14->Text.c_str());
	ini->WriteString("HID", "E15", Edit15->Text.c_str());
	ini->WriteString("HID", "E16", Edit16->Text.c_str());
	ini->WriteString("HID", "E17", Edit17->Text.c_str());
	ini->WriteString("HID", "E18", Edit18->Text.c_str());
	ini->WriteString("HID", "E19", Edit19->Text.c_str());

	ini->WriteString("HID", "E20", Edit20->Text.c_str());
	ini->WriteString("HID", "E21", Edit21->Text.c_str());
	ini->WriteString("HID", "E22", Edit22->Text.c_str());
	ini->WriteString("HID", "E23", Edit23->Text.c_str());
	ini->WriteString("HID", "E24", Edit24->Text.c_str());
	ini->WriteString("HID", "E25", Edit25->Text.c_str());
	ini->WriteString("HID", "E26", Edit26->Text.c_str());
	ini->WriteString("HID", "E27", Edit27->Text.c_str());
	ini->WriteString("HID", "E28", Edit28->Text.c_str());
	ini->WriteString("HID", "E29", Edit29->Text.c_str());

	ini->WriteString("HID", "E30", Edit30->Text.c_str());
	ini->WriteString("HID", "E31", Edit31->Text.c_str());
	ini->WriteString("HID", "E32", Edit32->Text.c_str());
	ini->WriteString("HID", "E33", Edit33->Text.c_str());
	ini->WriteString("HID", "E34", Edit34->Text.c_str());
	ini->WriteString("HID", "E35", Edit35->Text.c_str());
	ini->WriteString("HID", "E36", Edit36->Text.c_str());
	ini->WriteString("HID", "E37", Edit37->Text.c_str());
	ini->WriteString("HID", "E38", Edit38->Text.c_str());
	ini->WriteString("HID", "E39", Edit39->Text.c_str());

	ini->WriteString("HID", "E40", Edit40->Text.c_str());
	ini->WriteString("HID", "E41", Edit41->Text.c_str());

	ini->WriteInteger("DELAY", "SwitchMode", ckbDutMaxTimeGetId->ItemIndex);
	ini->WriteInteger("AUDIO", "Voiume", sb_value_audio->Position);

	refUsbHubTree.SaveFile(REFERENCE_XML);

	delete ini;

}

void __fastcall TUSBHIDForm::Button2Click(TObject *Sender) {
	if (Button2->Caption == "Start") {
		Button2->Caption = "Stop";
		Timer1->Enabled = true;
	}
	else {
		Button2->Caption = "Start";
		Timer1->Enabled = false;
	}
}
// ***************************************************************************     Thread
__fastcall TReadHIDThread::TReadHIDThread(bool CreateThread) : TThread(TRUE)
{}
// ---------------------------------------------------------------------------
void __fastcall TReadHIDThread::Execute() {
	Result = 0;
	// ---- Place thread code here ----
	while (!Terminated) {
		Sleep(1000);
		if (Result) {
			InpR[0] = 0x00;
			InpR[1] = 0x00;
			InpR[2] = 0x00;
			InpR[3] = 0x00;
			InpR[4] = 0x00;
			InpR[5] = 0x00;
			InpR[6] = 0x00;
			InpR[7] = 0x00;
			Result = ReadFile(hDeviceHandleR, InpR, 5, &NOBR, 0);
			// if (!Result)
			// USBHIDForm->Timer1->Enabled=true;
			// else
			Synchronize(BackUI);
		}else if (USBHIDForm->PASS == 3){
			Result = 1;
        }
	};
}

// ---------------------------------------------------------------------------
void __fastcall TReadHIDThread::BackUI(void) {
	AnsiString SS, s0, s1, s2, s3, s4, s5, s6, s7, s8;
	s0 = InpR[0];
	s1 = InpR[1];
	s2 = InpR[2];
	s3 = InpR[3];
	s4 = InpR[4];
	s5 = InpR[5];
	s6 = InpR[6];
	s7 = InpR[7];
	if (!Result) {
		USBHIDForm->Timer1->Enabled = true;
	}
	else if (InpR[0]) {
		// ToDO
		SS.printf("接收 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", s0.ToInt(),
			s1.ToInt(), s2.ToInt(), s3.ToInt(), s4.ToInt());
		USBHIDForm->Memo1->Lines->Add(SS);
	}
}
// ***************************************************************************     Timer1
// ---------------------------------------------------------------------------






void __fastcall TUSBHIDForm::Button4Click(TObject *Sender) {
	Memo1->Clear();
//	Memo2->Clear();
	Memo3->Clear();
}
// ---------------------------------------------------------------------------
char __fastcall TUSBHIDForm::DUTT(void) {
	int i, ERRC = 0;
	String cmd;

	Memo3->Clear();

	// -----------------------------------------------------Compare informations of flash
	// Abin 指示重工測成品時，dock information會隨驅動程式更新，所以不驗證dock information
	// 但保留查詢功能
	//201409041624 加入CheckBox，預設為不檢查
	if (!ERRC) {
		if(ckbVerifyDisplayLinkVersion->Checked){
			memoMsg->Lines->Add(VERIFY_DL_FIRMWARE_VERSION);
			if(!VerifyInfoFlash()){
				ERRC = 0x02;
				memoMsg->Lines->Add("(!)DisplayLink 韌體版本不符");
			}

//			Sleep(1000);
		}
	}

	// -----------------------------------------------------DDR
	if (!ERRC) {
//		if (ckbDataBusTest->Checked || ckbAddressBusTest->Checked || ckbCheckerboardTest->Checked ||
//			ckbByteCountDataTest->Checked)
//			memoMsg->Lines->Add("測試DDR");
		i = 0x00;

		if (ckbDataBusTest->Checked) {
			memoMsg->Lines->Add("DDR::Data bus test");

			if(!m_dut.DataBusTest()){
            	i |= 0x81;
				memoMsg->Lines->Add("(!)DDR::Data bus test 異常");
			}

//			Sleep(1000);
		}

		if (ckbAddressBusTest->Checked) {
			memoMsg->Lines->Add("DDR::Address Bus test");

			if(!m_dut.AddressBusTest()) {
				i |= 0x82;
				memoMsg->Lines->Add("(!)DDR::Address Bus test 異常");
			}

//			Sleep(1000);
		}

		if (ckbCheckerboardTest->Checked) {
			memoMsg->Lines->Add("DDR::Checkerboard test");

			if (!m_dut.CheckerboardTest()) {
				i |= 0x84;
				memoMsg->Lines->Add("(!)DDR::Checkerboard test 異常");
			}

//			Sleep(1000);
		}

		if (ckbByteCountDataTest->Checked) {
			memoMsg->Lines->Add("DDR::Byte Count Data test");

			if (!m_dut.ByteCountDataTest()) {
				i |= 0x88;
				memoMsg->Lines->Add("(!)DDR::Byte Count Data test 異常");
			}

//			Sleep(1000);
		}

		if (i) {
			ERRC = i;
//			Memo3->Lines->Add("(測試)DDR TEST Fail!");

		}
	}
	// -----------------------------------------------------Ethernet
	if (!ERRC) {
		if (ckbEthernetLoopbackTest->Checked) {
			memoMsg->Lines->Add("Ethernet loopback test");

			if(!m_dut.EthernetLoopbackTest()) {
				ERRC = 0x03;
				Memo3->Lines->Add("(測試)Ethernet loopback test Fail!");
				memoMsg->Lines->Add("(!)Ethernet loopback test 異常");
			}

//			Sleep(1000);
		}
	}
//	--------------------------------------------------------Audio
	if (!ERRC) {
		if (ckbAudio->Checked) {
			//if(	MessageBox(NULL,"請插入音效再按確認鍵","MP Tools", MB_OK )==1)
			//{
				memoMsg->Lines->Add("測試音效");
				Button5Click(NULL);
			//}
		}
	}
	//-----------------------------------------------------------------  video
	if (!ERRC){
		if(ckbVideo1080p->Checked){
			memoMsg->Lines->Add("測試顯示 1080P");

			if(m_dut.ShowVideoBlack()){
				if(!m_dut.ShowVideo1080P()){
					ERRC=0x05;
					Memo3->Lines->Add("(測試) 顯示 1080P Fail!");
					memoMsg->Lines->Add("(!)顯示 1080P 異常");
				}else{
//					Sleep(1000);

					if(	MessageBox(NULL,"請檢查畫面是否正常","測試顯示 1080P", MB_YESNO )==7){
						memoMsg->Lines->Add("(!)測試顯示 1080P 異常");
						ERRC=0x06;
					}
				}
			}else{
				ERRC = 0x04;
				memoMsg->Lines->Add("(!)顯示黑屏失敗");
			}
		}
	}

	if (!ERRC){
		if(ckbVideo4k->Checked){
			memoMsg->Lines->Add("測試顯示 4K");

			//if(m_dut.ShowVideoBlack()){
				if(!m_dut.ShowVideo4K()){
					ERRC=0x05;
					Memo3->Lines->Add("(測試) 顯示 4K Fail!");
					memoMsg->Lines->Add("(!)顯示 4K 異常");
				}else{
//					Sleep(1000);

					if(	MessageBox(NULL,"請檢查畫面是否正常","MP Tools", MB_YESNO )==7)
					{
						memoMsg->Lines->Add("(!)測試顯示 4K");
						ERRC=0x06;
					}
				}
			//}else{
			//	ERRC = 0x04;
			//	memoMsg->Lines->Add("(!)顯示黑屏失敗");
			//}
		}
	}
 //jim 20161102 新增4K 60 1的測項
	if (!ERRC){
		if(ckbVideo4k601->Checked){
			memoMsg->Lines->Add("測試顯示 4K 60 1");

			//if(m_dut.ShowVideoBlack()){
				if(!m_dut.ShowVideo4K60()){
					ERRC=0x05;
					Memo3->Lines->Add("(測試) 顯示 4K 60 Fail!");
					memoMsg->Lines->Add("(!)顯示 4K 60 異常");
				}else{
//					Sleep(1000);

					if(	MessageBox(NULL,"請檢查畫面是否正常","MP Tools", MB_YESNO )==7)
					{
						memoMsg->Lines->Add("(!)測試顯示 4K 60");
						ERRC=0x06;
					}
				}
			//}else{
			//	ERRC = 0x04;
			//	memoMsg->Lines->Add("(!)顯示黑屏失敗");
			//}
		}
	}

	//jim 20161102 新增4K 60 2的測項
	if (!ERRC){
		if(ckbVideo4k602->Checked){
			m_dut.DisableVideo();
			if(	MessageBox(NULL,"請更換顯示裝置再按確認鍵","MP Tools", MB_OK )==1)
			{
				memoMsg->Lines->Add("測試顯示 4K 60 2");
				CallCommand_C("dl_mfg_test mtt");
				Delay(5000);

				//if(m_dut.ShowVideoBlack()){
					if(!m_dut.ShowVideo4K60()){
						ERRC=0x05;
						Memo3->Lines->Add("(測試) 顯示 4K 60 Fail!");
						memoMsg->Lines->Add("(!)顯示 4K 60 異常");
					}else{
	//					Sleep(1000);

						if(	MessageBox(NULL,"請檢查畫面是否正常","MP Tools", MB_YESNO )==7)
						{
							memoMsg->Lines->Add("(!)測試顯示 4K 60");
							ERRC=0x06;
						}
					}
				//}else{
				//	ERRC = 0x04;
				//	memoMsg->Lines->Add("(!)顯示黑屏失敗");
				//}
			}
		}
	}
	// -----------------------------------------------------------------  audio


	if (ERRC) {
		if (autoSTEP != 1) {
			btnGetInfo->Enabled = true;
		}
	}
	return(ERRC);
}
// ---------------------------------------------------------------------------
char __fastcall TUSBHIDForm::DUTB(void) {
	int ERRC = 0;
	AnsiString SN, MAC;
	AnsiString updcode, SS;
	char macbuf[18];

	memoMsg->Lines->Add(BURN_DUT);
    // -----------------------------------------------------upd flash
	if (!ERRC) {
		updcode = " upd -s ";
		SN = "\t" + AnsiString(mskedtBurnSn->Text);

		if(m_dut._Kind == 2 && m_dut._Dev1.Pos(_T("4301_")) != 0){
			updcode += "00249BAABBFF";
			//updcode += mskedtBurnSn->Text.c_str();
			MAC = AnsiString("\t") + _T("000000000000");
		}else{
			//updcode += mskedtBurnSn->Text.c_str();
			updcode += "00249BAABBFF";
//			SN = "\t" + mskedtBurnSn->Text;
			updcode += " -a ";
			SS = mskedtMacPre->Text;
			strncpy(&macbuf[0], SS.c_str(), 6);
			SS = mskedtBurnMac->Text;
			strncpy(&macbuf[6], SS.c_str(), 6);
			macbuf[17] = 0x00;
			macbuf[16] = macbuf[11];
			macbuf[15] = macbuf[10];
			macbuf[14] = ':';
			macbuf[13] = macbuf[9];
			macbuf[12] = macbuf[8];
			macbuf[11] = ':';
			macbuf[10] = macbuf[7];
			macbuf[9] = macbuf[6];
			macbuf[8] = ':';
			macbuf[7] = macbuf[5];
			macbuf[6] = macbuf[4];
			macbuf[5] = ':';
			macbuf[4] = macbuf[3];
			macbuf[3] = macbuf[2];
			macbuf[2] = ':';
			macbuf[1] = macbuf[1];
			macbuf[0] = macbuf[0];
			updcode += macbuf;
			MAC = AnsiString("\t") + macbuf;
		}

		if(!m_dut.ProgramUPD(updcode)){
			ERRC = 0x01;
		}else{
            ERRC = 0x00;
//			Sleep(1000);
		}
	}

	if (!ERRC) {
		AnsiString strMessage;
		SYSTEMTIME myTime; // = GetSysTime();
		GetSystemTime(&myTime);
		// strMessage=myTime.wMonth, myTime.wDay, myTime.wYear, myTime.wHour, myTime.wMinute, myTime.wSecond );
		strMessage.printf("%02d-%02d-%d %02d:%02d:%02d", myTime.wMonth,
			myTime.wDay, myTime.wYear, myTime.wHour, myTime.wMinute,
			myTime.wSecond);
		strMessage += SN + MAC + "\t";
		strMessage += edtBackupFirmwareSample->Text + "\t";
		strMessage += edtMinFwVersionSample->Text + "\t";

		if(oneSTEP)
			strMessage += "REWORK";
		else
			strMessage += "NORMAL";

		Memo5->Lines->Add(strMessage);
		Memo5->Lines->SaveToFile(FileName);
	}else{
		memoMsg->Lines->Add("(!)燒錄失敗");
	}

	return(ERRC);
}

// ---------------------------------------------------------------------------
char __fastcall TUSBHIDForm::DUTV(void) {
	int ERRC;
	UnicodeString mac1, sn1, mac2, sn2, msg;
	UnicodeString refMac = mskedBurntMacPre->Text + mskedtBurnMac->Text;

	//重新讀取晶片info flash
	if(m_dut.GetInfo()){
		//讀取成功
		ERRC = 0x00;

		sn1 = m_dut._If1.SN;
		mac1 = m_dut._If1.MAC;

		if(m_dut._Kind == 2){
			sn2 = m_dut._If2.SN;
			mac2 = m_dut._If2.MAC;
		}else{
			sn2 = L"";
			mac2 = L"";
		}
	}else{
		//讀取失敗
		ERRC = 0xFF;
	}

//	Sleep(1000);
	//---------------------------------------------------------Verify
	if (!ERRC) {
		memoMsg->Lines->Add(VERIFY_DUT_SERIAL);

		if(m_dut._Kind == 1){
			if(sn1 != mskedtBurnSn->Text){
				memoMsg->Lines->Add("(!)序號不符");
				ERRC = 0x40;
			}
		}else{
			if(sn1.Length() == 8){
				if(sn1 != mskedtBurnSn->Text){
					memoMsg->Lines->Add("(!)序號不符 on flash1");
					ERRC = 0x40;
				}

				if(sn2 != mskedtBurnSn->Text + L"B"){
					memoMsg->Lines->Add("(!)序號不符 on flash2");
					ERRC = 0x40;
				}
			}else{
				if(sn1 != mskedtBurnSn->Text + L"B"){
					memoMsg->Lines->Add("(!)序號不符 on flash1");
					ERRC = 0x40;
				}

				if(sn2 != mskedtBurnSn->Text){
					memoMsg->Lines->Add("(!)序號不符 on flash2");
					ERRC = 0x40;
				}
			}
		}

		memoMsg->Lines->Add(VERIFY_DUT_MAC);

		if(m_dut._Kind == 1){
			if(mac1 != refMac){
				memoMsg->Lines->Add("(!)MAC 不符");
				ERRC = 0x40;
			}
		}else{
			if(mac1 != mac2){
				if(mac1 == _T("000000000000")){
					if(mac2!= refMac){
						memoMsg->Lines->Add("(!)MAC 不符! on flash2");
						ERRC = 0x40;
					}
				}else if(mac2 == _T("000000000000")){
					if(mac1 != refMac){
						memoMsg->Lines->Add("(!)MAC 不符! on flash1");
						ERRC = 0x40;
					}
				}else{
					memoMsg->Lines->Add("(!)MAC 異常");
					ERRC = 0x40;
				}
			}else if(mac1 != _T("000000000000")){
                memoMsg->Lines->Add("(!)MAC 異常");
				ERRC = 0x40;
			}
		}
	}

//	if (ERRC){
//		memoMsg->Lines->Add("(!)驗證失敗.");
//	}

	return(ERRC);
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnAutoTestClick(TObject *Sender) {
	m_bAutoIncrease = false;
	autoClose = false;
	memoMsg->Clear();
	lblStatus->Color = clSkyBlue;
	if (btnAutoTest->Caption == "自動化量產") {
		autoSTEP = 0;
		autoERRC = 0;
		oneERRC = 0;
		Timer2->Enabled = true;
		btnPrint->Enabled = true;
		btnGetInfo->Enabled = false;
		btnSetting->Enabled = false;
		GroupBox1->Enabled = false;
		ckbDutMaxTimeGetId->Enabled = false;
		gbBurnSetting->Enabled = false;
		GroupBox6->Enabled = false;

		if(ckbAutoIncrease->Checked){
			mskedtBurnSn->Text = mskedtBeginSn->Text;
			mskedBurntMacPre->Text = mskedtMacPre->Text;
			mskedtBurnMac->Text = mskedtBeginMac->Text;
		}

		btnAutoTest->Caption = "結束 量產";
	}
	else {
		lblStatus->Caption = "結束 量產";
		Wave_View->Picture = NULL;
		btnSetting->Enabled = true;
		btnPrint->Enabled = false;
		btnGetInfo->Enabled = true;
		autoSTEP = 99;
		autoERRC = 0;
		oneERRC = 0;
		Timer2->Enabled = false;
		btnAutoTest->Enabled = false;
		GroupBox1->Enabled = false;
		ckbDutMaxTimeGetId->Enabled = false;
		gbBurnSetting->Enabled = false;
		GroupBox6->Enabled = false;
		btnAutoTest->Caption = "自動化量產";
	}
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnSettingClick(TObject *Sender) {
	Memo1->Clear();

	lblStatus->Color = clSkyBlue;

	if (btnSetting->Caption == "設定") {
		if (MaskEdit9->Text == "set") {
			cmbNumOfChip->Enabled = true;
			btnLoadDutVersion->Enabled = true;
			GroupBox1->Enabled = true;
			ckbDutMaxTimeGetId->Enabled = true;
			gbBurnSetting->Enabled = true;
			GroupBox6->Enabled = true;
			lblStatus->Caption = "設定作業";
			MaskEdit9->Clear();
			btnSetting->Caption = "結束";
			btnPrint->Enabled = false;
			btnGetInfo->Enabled = false;
			GroupBox1->Enabled = true;
			ckbDutMaxTimeGetId->Enabled = true;
			gbBurnSetting->Enabled = true;
			GroupBox6->Enabled = true;
			ckbMtsMode->Enabled = true;
			mskedtBurnSn->Text = mskedtBeginSn->Text;
//			mskedtMacPre->Text = "00249B";
			mskedtBurnMac->Text = mskedtBeginMac->Text;


			//重工控制
			ckbRework->Enabled = true;
			btnRework->Enabled = false;
//			btnBurn->Enabled = false;
//			btnVerify->Enabled = false;
//			btnTest->Enabled = false;
//			btnRework->Enabled = false;
		}
		else {
			// Button20->Enabled=true;
			btnGetInfo->Enabled = true;
			lblStatus->Caption = "密碼錯誤";
			GroupBox1->Enabled = false;
			ckbDutMaxTimeGetId->Enabled = false;
			gbBurnSetting->Enabled = false;
			GroupBox6->Enabled = false;
			MessageBox(Handle, _T("密碼錯誤"), _T("WARNING"), MB_ICONWARNING);
		}
	}else{
		if(cmbNumOfChip->ItemIndex == 0){
			MB_WARNING(Handle, DUT_STRING_NOT_SET_KIND);
			Label3->Font->Color = clRed;
			return;
		}else{
			Label3->Font->Color = clBlack;
		}

		cmbNumOfChip->Enabled = false;
		btnLoadDutVersion->Enabled = false;
		mskedtBurnSn->Text = mskedtBurnSn->Text.UpperCase();
		mskedtEndSn->Text = mskedtEndSn->Text.UpperCase();
		mskedtBurnMac->Text = mskedtBurnMac->Text.UpperCase();
		mskedtMacPre->Text = mskedtMacPre->Text.UpperCase();
		mskedtMacPre->Text = mskedtMacPre->Text.UpperCase();
		MaskEdit9->Text = MaskEdit9->Text.UpperCase();

		btnSetting->Caption = "設定";
		btnGetInfo->Enabled = true;
		GroupBox1->Enabled = false;
		ckbDutMaxTimeGetId->Enabled = false;
		gbBurnSetting->Enabled = false;
		GroupBox6->Enabled = false;
		ckbMtsMode->Enabled = false;

		//重工控制
		ckbRework->Enabled = false;
		if(ckbRework->Checked){
			btnAutoTest->Enabled = false;
			btnRework->Enabled = true;
			lblStatus->Caption = "重工作業";
		}else{
			btnAutoTest->Enabled = true;
			btnPrint->Enabled = true;
			lblStatus->Caption = "生產作業";
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Timer2Timer(TObject *Sender) {
	int kind;
	if(!G_bCheckDevInfo){
		Timer2->Enabled = false;

		switch(autoSTEP) {
		case 0:
			kind = m_dut.GetKind();
			m_dut.SetKind(kind);

			if(kind == 0){
				if(!oneSTEP){
					btnAutoTest->Enabled = true;
				}

				memoMsg->Clear();
				memoMsg->Refresh();
				Wave_View->Picture = NULL;
				Memo3->Lines->Add("等候裝置插入");
				lblStatus->Color = clSkyBlue;
				lblStatus->Caption = "等候DUT插入";
			}else if(kind != cmbNumOfChip->ItemIndex){
				if(getKindCnt < m_dut.GetDutMaxTimeGetId()){
					getKindCnt++;
	//				memoMsg->Lines->Clear();
					memoMsg->Lines->Add(AnsiString("裝置不符，嘗試重新讀取...次數 ") + IntToStr(getKindCnt));
					break;
				}

				getKindCnt = 0;
				autoERRC = 1;
				autoSTEP = 9;

				memoMsg->Lines->Add("裝置不符，請檢查DisplayLink類型!");
			}else if(!m_dut.GetInfo()){
				memoMsg->Lines->Add("讀取DisplayLink韌體失敗!");
				autoERRC = 1;
				autoSTEP = 9;
			}else{
				if(!oneSTEP){
					btnAutoTest->Enabled = false;
				}

				btnGetInfo->Enabled = false;
				btnGetInfo->Enabled = false;
				// Button20->Enabled=false;
				getKindCnt = 0;
				autoERRC = 0;
				autoSTEP = 1;
				memoMsg->Lines->Add("開始作業");
				AuL = false;
				AuR = false;
				AuLPass = false;
				AuRPass = false;
				lblStatus->Color = clSkyBlue;
				lblStatus->Caption = "生產作業";
			}

			break;
		case 1:
			if(ckbVerifyUsbHubVersion->Checked){
				//驗證USB HUB版本
				memoMsg->Lines->Add(VERIFY_USB_HUB_VERSION);

				if(!VerifyUsbHubHardwareId()){
					memoMsg->Lines->Add("(!)USB HUB 韌體版本不符");
					autoERRC = 1;
					autoSTEP = 9;
					break;
				}
			}

			autoSTEP = 2;

			break;
		case 2:
			if(ckbVerifyDisplayLinkVersion->Checked || ckbDataBusTest->Checked || ckbAddressBusTest->Checked || ckbCheckerboardTest->Checked || ckbByteCountDataTest->Checked || ckbEthernetLoopbackTest->Checked || ckbVideo1080p->Checked || ckbVideo4k->Checked || ckbAudio->Checked){
				autoERRC = DUTT();

				if (autoERRC) {
					autoSTEP = 9;
					break;
				}
			 }

			autoSTEP = 3;
			break;
		case 3:
			if (ckbAudio->Checked) {
				if (AuL && AuR) {
					if (!AuLPass || !AuRPass) {
						autoERRC = 5;
						autoSTEP = 9;
						break;
					}
				}
			}

			autoSTEP = 5;
			break;
		case 5:
			if(ckbBurn->Checked){
				lblStatus->Caption = "燒錄程序";
				autoERRC = DUTB();

				if (autoERRC) {
					autoSTEP = 9;
					break;
				}
			}

			autoSTEP = 7;
			break;
		case 7:
			if(ckbBurnVerify->Checked){
				lblStatus->Caption = "驗證程序";
				autoERRC = DUTV();

				if (!autoERRC) {
					m_bAutoIncrease = true;

					if (ckbAutoPrint->Checked) {
						Timer3->Enabled = true;
					}
				}
			}else{
				m_bAutoIncrease = true;
			}

			autoSTEP = 9;
			break;
		case 9:
			if (autoERRC) {
				Button19Click(NULL);
				lblStatus->Caption = "!!! 異常發生 !!!";
				lblStatus->Color = clRed;
			}
			else {
				memoMsg->Lines->Add("完成作業");
				Memo3->Lines->Add("等候裝置拔除");
				lblStatus->Caption = "PASS 0";
				lblStatus->Color = clGreen;
			}

			if(!oneSTEP){
				btnAutoTest->Enabled = true;
			}

			btnGetInfo->Enabled = true;
			autoSTEP = 10;

			break;
		case 10:
			if(oneSTEP){
				autoSTEP = 99;
				btnRework->Enabled = true;
				gbBurnContent->Enabled = true;
				break;
			}

			if(autoERRC){
				MessageBeep(MB_ICONERROR);
			}else{
				MessageBeep(MB_OK);
			}


			kind = m_dut.GetKind();
			if(kind == 0){
				m_dut.SetMtt(false);
				autoSTEP = 0;

				if(ckbBurn->Checked == false && ckbBurnVerify->Checked == false){
					m_bAutoIncrease = true;
				}

				AutoIncrease();

				if (autoClose) {
					autoSTEP = 99;
					lblStatus->Color = clYellow;
					lblStatus->Caption = "自動跳號已滿";
					MessageBox(NULL, _T("自動跳號已滿"), _T("INFORMATION"), MB_OK);
					btnAutoTestClick(NULL);
				}
			}else{
				autoSTEP = 10;
			}

			break;
		} //End switch case.

		if (autoSTEP != 99) {
			Timer2->Enabled = true;
		}else{
			m_dut.SetMtt(false);
		}
	}
}
// ---------------------------------------------------------------------------
void TUSBHIDForm::AutoIncrease(){
	AnsiString SS, SD, SN, MAC;
	int tmp;

	if(ckbAutoIncrease->Checked && m_bAutoIncrease) {
		m_bAutoIncrease = false;

		tmp = mskedtBurnSn->Text.ToInt();

		if (tmp < mskedtEndSn->Text.ToInt()) {
			tmp++;
			SS = IntToStr(tmp);
			SD = "";

			for (j = 0; j < (8 - SS.Length()); j++) {
				SD += "0";
			}

			SD += SS.c_str();
			mskedtBurnSn->Text = SD;
		}else {
			// close Auto ......
			autoClose = true;
		}

		tmp = HexToInt(mskedtBurnMac->Text);
		if (tmp < HexToInt(mskedtEndMac->Text)) {
			tmp++;
			SD = IntToHex(tmp, 6);
			mskedtBurnMac->Text = SD;
		}else{
			// close Auto ......
			autoClose = true;
		}
	}
}

void __fastcall TUSBHIDForm::Button5Click(TObject *Sender) {
//////////////////////////////2017 jim 修改音效測試
	int volnum = sb_value_audio->Position * 655;
	AnsiString cmd = "nircmdc.exe setsysvolume " + IntToStr(volnum);
	CallCommand_C(cmd);
	my_data=new short[buffer_size];
	bool Result = true;
	Wav_Name_Edit->Text = "L1kR1k.wav";
	PlayVoice(0);
	Delay(300);
	Result = CheckData(2);
	if(Result){
		AuL = true;
		AuR = true;
		AuLPass = true;
		AuRPass = true;
		memoMsg->Lines->Add("測試音效  PASS");
	}
	else{
		AuL = true;
		AuR = true;
		AuLPass = false;
		AuRPass = false;
		memoMsg->Lines->Add("測試音效  FAIL");
	}


//////原版音效盒測試   (勿刪)
	/*if(PASS != 3){
		AuL = true;
		AuR = true;
//		MessageBox(NULL, _T("(!)USB AUDIO 切換製具異常"), _T("WARNING"), MB_ICONWARNING);
		memoMsg->Lines->Add("(!)USB AUDIO 切換製具異常");
		return;
	}else{
		AuL = false;
		AuR = false;
	}

	// 設定左聲道輸出
	// DWORD new_volume =0x0000FFFF;
	// 設定右聲道輸出
	// DWORD new_volume =0xFFFF0000;
	DWORD new_volume = 0xFFFFFFFF;
	waveOutSetVolume((HWAVEOUT)WAVE_MAPPER, new_volume);
	TestAudio(2, 8000, 8, 1);
	// Caption = "TESTING OK" ;    */
}
// ---------------------------------------------------------------------------
void TUSBHIDForm::TestAudio(int nChannels, long lFrequency, int nBits, int sec)
{
	len = sec * lFrequency * nChannels * nBits / 8;
	VocI = false;
	VocO = false;

	// open recording driver...
	if (WaveRecordOpen(&hwi, Handle, nChannels, lFrequency, nBits)) {
		// allocate buffer / header
		if (WaveMakeHeader(len, hgData_I, hgWaveHdr_I, lpData_I, lpWaveHdr_I)) {
			FAllocated_I = true;
			// start recording...
			if (WaveRecordPrepare(hwi, lpWaveHdr_I)) {
				FRecording = true;
			}
			else {
				FRecording = false;
				WaveRecordClose(hwi);
				WaveFreeHeader(hgData_I, hgWaveHdr_I);
				lpData_I = NULL;
				lpWaveHdr_I = NULL;
				FAllocated_I = false;
				// Caption = "DONE TESTING 1" ;
				ShowMessage("錄音設備無法加入配置記憶體!");
			}
		}
		else {
			WaveRecordClose(hwi);
			// Caption = "DONE TESTING 2" ;
			ShowMessage("錄音設備無法配置記憶體!");
		}
	}

	if (FRecording)

	{
		if (WavePlayOpen(&hwo, Handle, nChannels, lFrequency, nBits)) {
			len = sec * lFrequency * nChannels * nBits / 8;

			if (WaveMakeHeader(len, hgData_O, hgWaveHdr_O, lpData_O,
					lpWaveHdr_O)) {
				FAllocated_O = true;

				if (WavePlayPrepare(hwo, lpWaveHdr_O)) {
					FPlaying = true;
				}
				else {
					FPlaying = false;
					WavePlayClose(hwo);
					WaveFreeHeader(hgData_O, hgWaveHdr_O);
					lpData_O = NULL;
					lpWaveHdr_O = NULL;
					FAllocated_O = false;
					ShowMessage("播音設備無法加入配置記憶體!");

				}
			}
			else {
				WavePlayClose(hwo);
				ShowMessage("播音設備無法配置記憶體!");
			}
		}
	}

	if (FPlaying) {
		// Caption = "Testing..." ;
		MMRESULT result = waveInStart(hwi);
		if (result == MMSYSERR_NOERROR) {
			Memo1->Lines->Add("waveInStart T");

			result = waveOutWrite(hwo, lpWaveHdr_O, sizeof(WAVEHDR));
			if (result != MMSYSERR_NOERROR) {
				Memo1->Lines->Add("waveInStart T2");
				if (FRecording)
					WaveRecordEnd(hwi, lpWaveHdr_I);
				waveInUnprepareHeader(hwi, lpWaveHdr_I, sizeof(WAVEHDR));
				WaveRecordClose(hwi);
				FRecording = false;
				WaveFreeHeader(hgData_I, hgWaveHdr_I);
				lpData_I = NULL;
				lpWaveHdr_I = NULL;
				FAllocated_I = false;

				waveOutUnprepareHeader(hwo, lpWaveHdr_O, sizeof(WAVEHDR));
				WavePlayClose(hwo);
				FPlaying = false;
				WaveFreeHeader(hgData_O, hgWaveHdr_O);
				lpData_O = NULL;
				lpWaveHdr_O = NULL;
				FAllocated_O = false;

				// Caption = "DONE TESTING 3" ;

				ShowMessage("播音失敗!");
			}

		}
		else {
			Memo1->Lines->Add("waveInStart F");
			waveInUnprepareHeader(hwi, lpWaveHdr_I, sizeof(WAVEHDR));
			WaveRecordClose(hwi);
			FRecording = false;
			WaveFreeHeader(hgData_I, hgWaveHdr_I);
			lpData_I = NULL;
			lpWaveHdr_I = NULL;
			FAllocated_I = false;

			waveOutUnprepareHeader(hwo, lpWaveHdr_O, sizeof(WAVEHDR));
			WavePlayClose(hwo);
			FPlaying = false;
			WaveFreeHeader(hgData_O, hgWaveHdr_O);
			lpData_O = NULL;
			lpWaveHdr_O = NULL;
			FAllocated_O = false;

			// Caption = "DONE TESTING 4" ;

			ShowMessage("錄音失敗!");
		}
	}
	else {

		// 釋放資源，重置旗標
		FRecording = false;
		WaveFreeHeader(hgData_I, hgWaveHdr_I);
		lpData_I = NULL;
		lpWaveHdr_I = NULL;
		FAllocated_I = false;

		FPlaying = false;
		WaveFreeHeader(hgData_O, hgWaveHdr_O);
		lpData_O = NULL;
		lpWaveHdr_O = NULL;
		FAllocated_O = false;
		ShowMessage("播音失敗!");
		// Caption = "DONE TESTING 5" ;
	}

}
// ---------------------------------------------------------------------------

bool __fastcall TUSBHIDForm::CheckVoc(void) {
	bool zo;
	int zc = 0;
	for (i = 1000; i < 15000; i++) {
		buf[i] /= 200;
		if (buf[i]) // H
		{
			zo = true;
		}
		else {
			if (zo) {
				zc++;
			}
			zo = false;
		}
	}
	Memo1->Lines->Add(zc);
	if (zc < 300) {
		return(false);
	}
	if (zc > 340) {
		return(false);
	}
	return(true);
}
// ---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Button12Click(TObject *Sender) {
	bool zo;
	int zc = 0;
	for (i = 1000; i < 15000; i++) {
		buf[i] /= 200;
		if (buf[i]) // H
		{
			zo = true;
		}
		else {
			if (zo) {
				zc++;
			}
			zo = false;
		}
	}
	Memo1->Lines->Add(zc);
}

// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::CheckAudio(void) {
	if (VocI && VocO) {
		if (!CheckVoc()) {
			if (AuL) {
				memoMsg->Lines->Add("(!)第一聲道音效測試異常");
				AuRPass = false;
			}
			else {
				memoMsg->Lines->Add("(!)第二聲道音效測試異常");
				AuLPass = false;
			}
		}
		else {
			if (AuL) {
				AuRPass = true;
			}
			else {
				AuLPass = true;
			}
		}
		if (AuL && AuR) {
//			memoMsg->Lines->Add("完成作業");
		}
		else {
			if (AuL) {
				AuR = true;
			}
			else {
				AuL = true;
				Sleep(1000);
				TestAudio(2, 8000, 8, 1);
			}

			if (PASS == 3) {
				Button7Click(NULL);
//				Memo2->Lines->Add("更換頻道");
			}
			else
				memoMsg->Lines->Add("(!)無法更換頻道測試.");

			if (AuL && AuR) {
//				memoMsg->Lines->Add("完成作業");
				if (autoSTEP != 3) {
					if (AuLPass && AuRPass) {
						lblStatus->Caption = "PASS 1";
						lblStatus->Color = clGreen;
					}
					else {
						Button19Click(NULL);
						// MessageBeep(0xFFFFFFFF);
						lblStatus->Caption = "!!! 異常發生 !!!";
						autoERRC = 30;
						lblStatus->Color = clRed;
					}
					btnGetInfo->Enabled = true;
					btnSetting->Enabled = true;
					// Button20->Enabled=true;
				}
			}
		}
	}
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::Button19Click(TObject *Sender) {
	AnsiString SS;
	OutputReport[0] = 0x03;
	OutputReport[1] = 0x5C;
	OutputReport[2] = 0x02;
	OutputReport[3] = 0x00;
	OutputReport[4] = 0x00;
	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],
		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);
	USBHIDForm->Memo1->Lines->Add(SS);
	Sleep(100);
	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnPrintClick(TObject *Sender) {
	AnsiString SSN, SMAC;
	AnsiString ver, buf1, buf2;
	int nLen, ret, len1, len2;
	char pbuf[128];
	ver = IntToStr(A_Get_DLL_Version(0));
	Button1->Caption = ver;
	Memo3->Lines->Add(ver);
	nLen = A_GetUSBBufferLen() + 1;
	Memo3->Lines->Add(nLen);
	if (nLen > 1) {
		len1 = 128;
		len2 = 128;
		buf1.SetLength(len1);
		buf2.SetLength(len2);
		A_EnumUSB(pbuf);
		A_GetUSBDeviceInfo(1, buf1.c_str(), &len1, buf2.c_str(), &len2);
		Memo3->Lines->Add(buf1);
		Memo3->Lines->Add(buf2);
		ret = A_CreatePrn(12, buf2); // open usb.

		if (0 < ret) {
			Memo3->Lines->Add("Open USB fail!");
		}
		else {
			Memo3->Lines->Add("Open USB OK!");
			SSN = "SN/:" + mskedtBurnSn->EditText;
			SMAC = "MAC:" + mskedBurntMacPre->EditText + ":" + mskedtBurnMac->EditText;
			A_Set_Unit('n');
			A_Set_Syssetting(1, 0, 0, 0, 0);
			A_Set_Darkness(16);
			A_Del_Graphic(1, "*"); // delete all picture.
			A_Clear_Memory(); // clear memory.
			A_Set_Backfeed(320);
			// A_Prn_Text( 20, 10,  1, 2, 0, 1, 1, 'N', 2, SSN.c_str());
			// A_Prn_Text( 20, 30,  1, 2, 0, 1, 1, 'N', 2, SMAC.c_str());
			A_Prn_Text(20, 2, 1, 2, 0, 1, 1, 'N', 2, SSN.c_str());
			A_Prn_Text(20, 14, 1, 2, 0, 1, 1, 'N', 2, SMAC.c_str());
			// A_Prn_Text( 20, 10,  1, 2, 0, 1, 1, 'N', 2, "01234567890123456789");
			// A_Prn_Text( 20, 30,  1, 2, 0, 1, 1, 'N', 2, "01234567890123456789");
			A_Print_Out(1, 1, 1, 1); // copy 1.
			A_ClosePrn();
		}
	}

}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::Timer3Timer(TObject *Sender) {
	Timer3->Enabled = false;
	btnPrintClick(0);
}
// ---------------------------------------------------------------------------
bool TUSBHIDForm::GetDutInfomation(USB_Hub_Tree &hubTree){

	if(cmbNumOfChip->ItemIndex == 0){
		MB_WARNING(Handle, DUT_STRING_NOT_SET_KIND);
		Label3->Font->Color = clRed;
		return false;
	}else{
		Label3->Font->Color = clBlack;
	}

	m_dut.SetKind(cmbNumOfChip->ItemIndex);

	if(!m_dut.GetId()){
		MB_WARNING(Handle, MB_FAILED_GET_DUT_ID);
		Label3->Font->Color = clRed;
		return false;
	}else{
    	Label3->Font->Color = clBlack;
	}

	if(ckb_wait10->Checked)
		Delay(10000);
	if(!m_dut.GetInfo()){
		memoMsg->Lines->Add(DUT_STRING_NOT_FIND);
		MB_WARNING(Handle, MB_FAILED_GET_DUT_INFORMATION);
		return false;
	}

	memoMsg->Lines->Add(DUT_STRING_INFO_FLASH);
	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	memoMsg->Lines->Add(DUT_STRING_NUM_OF_CHIP + IntToStr(m_dut._Kind));
	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	memoMsg->Lines->Add(m_dut._Dev1);
	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	memoMsg->Lines->Add(DUT_FT + m_dut._If1.FirmwareType);
	memoMsg->Lines->Add(DUT_RL + m_dut._If1.ReleaseLevel);
	memoMsg->Lines->Add(DUT_VN + m_dut._If1.Version);
	memoMsg->Lines->Add(DUT_SV + m_dut._If1.SvnVersion);
	memoMsg->Lines->Add(DUT_BD + m_dut._If1.BuildDate);
	memoMsg->Lines->Add(DUT_TE + m_dut._If1.Time);
	memoMsg->Lines->Add(DUT_BF + m_dut._If1.BackFirmware);
	memoMsg->Lines->Add(DUT_EN + m_dut._If1.Name);
	memoMsg->Lines->Add(DUT_ED + m_dut._If1.Date);
	memoMsg->Lines->Add(DUT_EM + m_dut._If1.MinFwVersion);
	memoMsg->Lines->Add(DUT_MAC + m_dut._If1.MAC);
	memoMsg->Lines->Add(DUT_SN + m_dut._If1.SN);

	if(m_dut._If1.HDCP != ""){
		memoMsg->Lines->Add(DUT_HDCP + m_dut._If1.HDCP);
	}

	memoMsg->Lines->Add(m_dut._If1.EDID);

	if(m_dut._Kind == 2){
		memoMsg->Lines->Add(_T("------------------------------------------------------------"));
		memoMsg->Lines->Add(m_dut._Dev2);
		memoMsg->Lines->Add(_T("------------------------------------------------------------"));
		memoMsg->Lines->Add(DUT_FT + m_dut._If2.FirmwareType);
		memoMsg->Lines->Add(DUT_RL + m_dut._If2.ReleaseLevel);
		memoMsg->Lines->Add(DUT_VN + m_dut._If2.Version);
		memoMsg->Lines->Add(DUT_SV + m_dut._If2.SvnVersion);
		memoMsg->Lines->Add(DUT_BD + m_dut._If2.BuildDate);
		memoMsg->Lines->Add(DUT_TE + m_dut._If2.Time);
		memoMsg->Lines->Add(DUT_BF + m_dut._If2.BackFirmware);
		memoMsg->Lines->Add(DUT_EN + m_dut._If2.Name);
		memoMsg->Lines->Add(DUT_ED + m_dut._If2.Date);
		memoMsg->Lines->Add(DUT_EM + m_dut._If2.MinFwVersion);
		memoMsg->Lines->Add(DUT_MAC + m_dut._If2.MAC);
		memoMsg->Lines->Add(DUT_SN + m_dut._If2.SN);

		if(m_dut._If2.HDCP != ""){
			memoMsg->Lines->Add(DUT_HDCP + m_dut._If2.HDCP);
		}

		memoMsg->Lines->Add(m_dut._If2.EDID);
	}

	if(!GetUsbHubTree(hubTree)){
    	return false;
	}

	return true;
}

bool TUSBHIDForm::LoadDutInfomation(){
	if(GetDutInfomation(refUsbHubTree)){
//		lblChipID1->Caption = m_dut._Dev1;
		edtBackupFirmwareSample->Text = m_dut._If1.BackFirmware;
		edtMinFwVersionSample->Text = m_dut._If1.MinFwVersion;

	  if(m_dut._Kind == 2){
//		lblChipID2->Caption = m_dut._Dev2;
		edtBackupFirmwareSample2->Text = m_dut._If2.BackFirmware;
		edtMinFwVersionSample2->Text = m_dut._If2.MinFwVersion;
      }

		return true;
	}

	return false;
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::mskedtBeginMacChange(TObject *Sender)
{
	const int maxCount = 0xffffff;

//	if(mskedtBeginMac->Text == _T("")){
//		mskedtEndSn->Text = mskedtBeginSn->Text;
//		return;
//    }

	int count = edtBurnCount->Text.ToInt();
	int beginMac = HexToInt(mskedtBeginMac->Text.UpperCase());
	int maxBeginMac = maxCount - count;

	if(beginMac > maxBeginMac){
		 ShowMessage(_T("MAC 範圍異常"));
		 beginMac = 1;
		 mskedtBeginMac->Text = IntToHex(beginMac, 6);
	}

	int endMac = beginMac + count - 1;

	mskedtEndMac->Text = IntToHex(endMac, 6);
	mskedtBurnMac->Text = mskedtBeginMac->Text;
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::edtBurnCountChange(TObject *Sender)
{
//	UnicodeString tmp;
	const int maxCount = 0xffffff;

	if(edtBurnCount->Text == _T("")){
		mskedtEndSn->Text = mskedtBeginSn->Text;
		mskedtEndMac->Text = mskedtBeginMac->Text;
		return;
	}

	int count = edtBurnCount->Text.ToInt();
	int beginSn = mskedtBeginSn->Text.ToInt();
	int beginMac = HexToInt(mskedtBeginMac->Text.UpperCase());

	if(count < 1){
		ShowMessage(_T("燒錄最小數量為 1"));
		count = 1;
	}else if(count > maxCount){
		ShowMessage(_T("燒錄最大數量為 16777215"));
		count = 1;
	}

	int endSn = beginSn + count - 1;
	int endMac = beginMac + count - 1;


	if(endSn > 99999999){
		ShowMessage(_T("序號 範圍異常"));
		count = 1;
		endMac = beginMac;
		endSn = beginSn;
	}

	if(endMac > maxCount){
		ShowMessage(_T("MAC 範圍異常"));
		count = 1;
		endMac = beginMac;
		endSn = beginSn;
	}

	edtBurnCount->Text = IntToStr(count);
	mskedtEndSn->Text = FormatFloat("00000000", endSn);
	mskedtEndMac->Text = IntToHex(endMac, 6);
	mskedtBurnSn->Text = mskedtBeginSn->Text;
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::mskedtBeginSnChange(TObject *Sender)
{
	UnicodeString tmp;
//	const int maxCount = 0xffffff;

	if(mskedtBeginSn->Text == _T("")){
		mskedtEndSn->Text = mskedtBeginSn->Text;
		return;
    }

	int count = edtBurnCount->Text.ToInt();
	int beginSn = mskedtBeginSn->Text.ToInt();
	int maxBeginSn = 99999999 - count;

	if(beginSn > maxBeginSn){
		 ShowMessage(_T("序號 範圍異常"));
		 beginSn = 1;
		 mskedtBeginSn->Text = FormatFloat("00000000", beginSn);
	}

	int endSn = beginSn + count - 1;

	mskedtEndSn->Text = FormatFloat("00000000", endSn);
	mskedtBurnSn->Text = mskedtBeginSn->Text;
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::mskedtMacPreChange(TObject *Sender)
{
	mskedBurntMacPre->Text = mskedtMacPre->Text;
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::ckbReworkClick(TObject *Sender)
{
	if(ckbRework->Checked){
		ckbAutoIncrease->Checked = false;
		ckbAutoIncrease->Enabled = false;
		mskedtBurnSn->ReadOnly = false;
		mskedBurntMacPre->ReadOnly = false;
		mskedtBurnMac->ReadOnly = false;
		oneSTEP = true;
	}else{
		ckbAutoIncrease->Checked = true;
		ckbAutoIncrease->Enabled = true;
		mskedtBurnSn->ReadOnly = true;
		mskedBurntMacPre->ReadOnly = true;
		mskedtBurnMac->ReadOnly = true;
		oneSTEP = false;
	}
}
//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::ckbAutoIncreaseClick(TObject *Sender)
{
	if(ckbAutoIncrease->Checked){
		mskedtBurnSn->ReadOnly = true;
		mskedBurntMacPre->ReadOnly = true;
		mskedtBurnMac->ReadOnly = true;
		edtBurnCount->Visible = true;
		mskedtBeginSn->Visible = true;
		mskedtEndSn->Visible = true;
		mskedtMacPre->Visible = true;
		mskedtBeginMac->Visible = true;
		mskedtEndMac->Visible = true;

		Label15->Visible = true;
		Label14->Visible = true;
		Label2->Visible = true;
		Label17->Visible = true;
		Label20->Visible = true;
		Label21->Visible = true;
	}else{
		mskedtBurnSn->ReadOnly = false;
		mskedBurntMacPre->ReadOnly = false;
		mskedtBurnMac->ReadOnly = false;
		edtBurnCount->Visible = false;
		mskedtBeginSn->Visible = false;
		mskedtEndSn->Visible = false;
		mskedtMacPre->Visible = false;
		mskedtBeginMac->Visible = false;
		mskedtEndMac->Visible = false;
		Label15->Visible = false;
		Label14->Visible = false;
		Label2->Visible = false;
		Label17->Visible = false;
		Label20->Visible = false;
		Label21->Visible = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::mskedtBurnMacExit(TObject *Sender)
{
    mskedtBurnMac->Text = mskedtBurnMac->Text.UpperCase();
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::mskedBurntMacPreExit(TObject *Sender)
{
	mskedBurntMacPre->Text = mskedBurntMacPre->Text.UpperCase();
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::Button7Click(TObject *Sender)
{
	AnsiString SS;
	OutputReport[0] = 0x03;
	OutputReport[1] = 0x5C;
	OutputReport[2] = 0x01;
	OutputReport[3] = 0x00;
	OutputReport[4] = 0x00;
	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],
		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);
	USBHIDForm->Memo1->Lines->Add(SS);
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Timer1Timer(TObject *Sender)
{
	Timer1->Enabled = false;

	Memo1->Clear();

	HidD_GetHidGuid(&HidGuid); // 取得HID群組
//	SS.printf("HID Guid:%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x-%02x%02x\n",
//		HidGuid.Data1, HidGuid.Data2, HidGuid.Data3, HidGuid.Data4[0],
//		HidGuid.Data4[1], HidGuid.Data4[2], HidGuid.Data4[3],
//		HidGuid.Data4[4], HidGuid.Data4[5], HidGuid.Data4[6],
//		HidGuid.Data4[7]);
//	Memo1->Lines->Add(SS);

	hDevInfo = SetupDiGetClassDevs(&HidGuid, // 取得所有HID訊息
		NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE) {
		Memo1->Lines->Add("NO USB HID SUPPLY");
		return;
	}
	Memo1->Lines->Add("Search USB HID...");
	DWORD MemberIndex = 0;
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData, DeviceInterfaceDataS;
	BOOL bSuccess;
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	PASS = 0;
	do {
		bSuccess = SetupDiEnumDeviceInterfaces(hDevInfo, // 辨識每一個HID介面
			NULL, &HidGuid, MemberIndex, &DeviceInterfaceData);
		if ((!bSuccess) && (GetLastError() == ERROR_NO_MORE_ITEMS)) {
//			SS.printf("NO MORE USB HID\n");
//			Memo1->Lines->Add(SS);
			SetupDiDestroyDeviceInfoList(hDevInfo); // 釋放HID資源
			return;
		}
		// 若找到了一個HID USB以上,取的信息
		DWORD Length = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, // 取得裝置路徑 Length
			&DeviceInterfaceData, NULL, 0, &Length, NULL);
		pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc
			(Length);
		pDeviceInterfaceDetailData->cbSize = sizeof
			(SP_DEVICE_INTERFACE_DETAIL_DATA);

		// 取得裝置路徑
		if (!SetupDiGetDeviceInterfaceDetail(
			hDevInfo,
			&DeviceInterfaceData,
			pDeviceInterfaceDetailData,
			Length,
			NULL,
			NULL)
		){
			SS.printf("(!) PATH ERROR");
			Memo1->Lines->Add(SS);
		}else {
			SS.printf("%s", (pDeviceInterfaceDetailData->DevicePath + 4));
			Memo1->Lines->Add("USB HID : " + SS);
			char *cPVID;
			AnsiString sPVID;
			Edit1->Text = "1406&mi_01&col02";
			sPVID = "hid#vid_2101&pid_" + Edit1->Text;
			sPVID = sPVID.LowerCase();
			cPVID = sPVID.c_str();
			if (strncmp(SS.c_str(), cPVID, 17 + Edit1->Text.Length()) == 0) {
				Memo1->Lines->Add("USB HID 連線 ====================");
				PASS = 1;
				break;
			}
		}

		if(pDeviceInterfaceDetailData){
			free(pDeviceInterfaceDetailData);
			pDeviceInterfaceDetailData = NULL;
		}

		MemberIndex++;
	}
	while (bSuccess);

	if (PASS == 1) {
		hDeviceHandleW = CreateFile(pDeviceInterfaceDetailData->DevicePath,
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);
		hDeviceHandleR = CreateFile(pDeviceInterfaceDetailData->DevicePath,
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);
		if (hDeviceHandleW == INVALID_HANDLE_VALUE || hDeviceHandleR ==
			INVALID_HANDLE_VALUE) {
			SS.printf("OPEN PATH ERROR");
			Memo1->Lines->Add(SS);
			PASS = 0;
		}
		else {
			PASS = 3;
		}
	}

	if(pDeviceInterfaceDetailData){
		free(pDeviceInterfaceDetailData);
		pDeviceInterfaceDetailData = NULL;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo); //釋放HID資源
}
//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::Button6Click(TObject *Sender)
{
	AnsiString SS;
	OutputReport[0] = 0x03;
	OutputReport[1] = 0x5C;
	OutputReport[2] = 0x00;
	OutputReport[3] = 0x00;
	OutputReport[4] = 0x00;
	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],
		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);
	USBHIDForm->Memo1->Lines->Add(SS);
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Button1Click(TObject *Sender)
{
//	AnsiString SS;
//	OutputReport[0] = (edtFirewareTypeSample->Text.ToInt());
//	OutputReport[1] = (edtReleaseLevelSample->Text.ToInt());
//	OutputReport[6] = (edtNameSample->Text.ToInt());
//	OutputReport[7] = (edtDateSample->Text.ToInt());
//	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
//	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],
//		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);
//	USBHIDForm->Memo1->Lines->Add(SS);
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Button8Click(TObject *Sender)
{
	AnsiString SS;
	OutputReport[0] = (Edit10->Text.ToInt());
	OutputReport[1] = (Edit11->Text.ToInt());
	OutputReport[2] = (Edit12->Text.ToInt());
	OutputReport[3] = (Edit13->Text.ToInt());
	OutputReport[4] = (Edit14->Text.ToInt());
	OutputReport[5] = (Edit15->Text.ToInt());
	OutputReport[6] = (Edit16->Text.ToInt());
	OutputReport[7] = (Edit17->Text.ToInt());
	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],
		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);
	USBHIDForm->Memo1->Lines->Add(SS);
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Button9Click(TObject *Sender)
{
	AnsiString SS;
	OutputReport[0] = (Edit18->Text.ToInt());
	OutputReport[1] = (Edit19->Text.ToInt());
	OutputReport[2] = (Edit20->Text.ToInt());
	OutputReport[3] = (Edit21->Text.ToInt());
	OutputReport[4] = (Edit22->Text.ToInt());
	OutputReport[5] = (Edit23->Text.ToInt());
	OutputReport[6] = (Edit24->Text.ToInt());
	OutputReport[7] = (Edit25->Text.ToInt());
	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],
		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);
	USBHIDForm->Memo1->Lines->Add(SS);
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Button10Click(TObject *Sender)
{
	AnsiString SS;
	OutputReport[0] = (Edit26->Text.ToInt());
	OutputReport[1] = (Edit27->Text.ToInt());
	OutputReport[2] = (Edit28->Text.ToInt());
	OutputReport[3] = (Edit29->Text.ToInt());
	OutputReport[4] = (Edit30->Text.ToInt());
	OutputReport[5] = (Edit31->Text.ToInt());
	OutputReport[6] = (Edit32->Text.ToInt());
	OutputReport[7] = (Edit33->Text.ToInt());
	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);
	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],
		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);
	USBHIDForm->Memo1->Lines->Add(SS);
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::Button11Click(TObject *Sender)
{	AnsiString SS;	OutputReport[0] = (Edit34->Text.ToInt());	OutputReport[1] = (Edit35->Text.ToInt());	OutputReport[2] = (Edit36->Text.ToInt());	OutputReport[3] = (Edit37->Text.ToInt());	OutputReport[4] = (Edit38->Text.ToInt());	OutputReport[5] = (Edit39->Text.ToInt());	OutputReport[6] = (Edit40->Text.ToInt());	OutputReport[7] = (Edit41->Text.ToInt());	WriteFile(hDeviceHandleW, OutputReport, 5, &NumberOfBytesWritten, 0);	SS.printf("發送 :\t 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", OutputReport[0],		OutputReport[1], OutputReport[2], OutputReport[3], OutputReport[4]);	USBHIDForm->Memo1->Lines->Add(SS);}

//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::btnReworkClick(TObject *Sender)
{
	btnRework->Enabled = false;
	gbBurnContent->Enabled = false;

	memoMsg->Clear();

	m_bAutoIncrease = false;
	autoClose = false;
	lblStatus->Color = clSkyBlue;
	lblStatus->Caption = REWORK;


	autoSTEP = 0;
	autoERRC = 0;
	oneERRC = 0;

	Timer2->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::ckbMtsModeClick(TObject *Sender)
{
	if(ckbMtsMode->Checked){
		m_bMts[0] = ckbBurn->Checked;
		m_bMts[1] = ckbBurnVerify->Checked;
		m_bMts[2] = ckbAutoPrint->Checked;
		m_bMts[3] = ckbAutoIncrease->Checked;
		m_bMts[4] = ckbRework->Checked;
		ckbBurn->Checked = false;
        ckbBurnVerify->Checked = false;
        ckbAutoPrint->Checked = false;
		ckbAutoIncrease->Checked = false;
		ckbRework->Checked = false;

		ckbBurn->Visible = false;
		ckbBurnVerify->Visible = false;
		ckbAutoPrint->Visible = false;
		ckbAutoIncrease->Visible = false;
		gbBurnSetting->Visible = false;
		gbBurnContent->Visible = false;
		ckbRework->Visible = false;
		gbRework->Visible = false;
	}else{
		ckbBurn->Checked = m_bMts[0];
		ckbBurnVerify->Checked = m_bMts[1];
		ckbAutoPrint->Checked = m_bMts[2];
		ckbAutoIncrease->Checked = m_bMts[3];
		ckbRework->Checked = m_bMts[4];

        ckbBurn->Visible = true;
		ckbBurnVerify->Visible = true;
		ckbAutoPrint->Visible = true;
		ckbAutoIncrease->Visible = true;
		gbBurnSetting->Visible = true;
		gbBurnContent->Visible = true;
		ckbRework->Visible = true;
		gbRework->Visible = true;
	}
}
//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::ckbBurnClick(TObject *Sender)
{
	if(ckbBurn->Checked){

	}else{

	}
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::MaskEdit9Change(TObject *Sender)
{
	if (MaskEdit9->Text == "debug") {
		Panel1->Visible = false;
	}else{
		Panel1->Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnGetInfoClick(TObject *Sender)
{
	USB_Hub_Tree hubTree;
    G_bCheckDevInfo = true;
	memoMsg->Clear();
	memoMsg->Refresh();

	btnGetInfo->Enabled = false;

	if(!GetDutInfomation(hubTree)){
    	memoMsg->Clear();
		memoMsg->Refresh();
        MB_WARNING(Handle, MB_FAILED_GET_DUT_INFORMATION);
	}

	//Memo設定到第一行
//	memoMsg->SelStart = 0;
//	memoMsg->SelLength = 0;
//	SendMessage(memoMsg->Handle, EM_SETSEL, 0, 0);
//	SendMessage(memoMsg->Handle, EM_SCROLLCARET, 0, 0);
    G_bCheckDevInfo = false;
	btnGetInfo->Enabled = true;}
//---------------------------------------------------------------------------
void TUSBHIDForm::ClearInformation(){
	trvReference->Items->Clear();
	memoMsg->Clear();
//	memoMsg->Refresh();

	edtBackupFirmwareSample->Text = _T("");
	edtMinFwVersionSample->Text = _T("");
	edtBackupFirmwareSample2->Text = _T("");
	edtMinFwVersionSample2->Text = _T("");

	USBHIDForm->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnLoadDutVersionClick(TObject *Sender)
{
	ClearInformation();

	cmbNumOfChip->Enabled = false;
	btnLoadDutVersion->Enabled = false;
	if(LoadDutInfomation()){		if(!Xml2Tree(&refUsbHubTree)){			ClearInformation();			MB_WARNING(Handle, MB_FAILED_LOAD_DUT_INFORMATION);		}	}	cmbNumOfChip->Enabled = true;	btnLoadDutVersion->Enabled = true;
}
//---------------------------------------------------------------------------



void __fastcall TUSBHIDForm::ckbDutMaxTimeGetIdChange(TObject *Sender)
{
	int i = ckbDutMaxTimeGetId->Items->Strings[ckbDutMaxTimeGetId->ItemIndex].ToInt();
	m_dut.SetDutMaxTimeGetId(i);
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::ckbVideo4k601Click(TObject *Sender)
{
	if(!ckbVideo4k602->Enabled)
		ckbVideo4k602->Enabled = true;
	else{
        ckbVideo4k602->Checked = false;
		ckbVideo4k602->Enabled = false;
	}
}
//---------------------------------------------------------------------------
// 撥放音檔
// 取得設備ID ，mmio API讀取wav檔將音訊資料放置緩衝區，waveout API讀取緩衝區資料並撥出
void TUSBHIDForm::PlayVoice(int tag) {

	// MMIO(讀wav檔)
	MMCKINFO	ChunkInfo;
	MMCKINFO	FormatChunkInfo;
	MMCKINFO	DataChunkInfo;

	WAVEFORMATEX   wfx; // 波形音頻流格式的數據結構

	// 選擇設備ID
	int g_selectOutputDevice = -1;
	if(tag == 0){
    	if (cb_myOutDevice->ItemIndex >= 0) {
			g_selectOutputDevice = (cb_myOutDevice->ItemIndex);
		}
	}else if(tag ==1){
		if (cbOutputDevice->ItemIndex >= 0){
			g_selectOutputDevice = (cbOutputDevice->ItemIndex);
        }
	}else if(tag ==2){
		if (cb_myOutDevice->ItemIndex >= 0){
			g_selectOutputDevice = (cb_myOutDevice->ItemIndex);
		}
	}else if(tag ==3){
		if (cbOutputDevice->ItemIndex >= 0){
			g_selectOutputDevice = (cbOutputDevice->ItemIndex);
		}
	}
	int DataSize;
	// Zero out the ChunkInfo structure.
	memset(&ChunkInfo, 0, sizeof(MMCKINFO));
	// 打開WAVE文件，返回一個HMMIO句柄
	HMMIO handle = mmioOpen(Wav_Name_Edit->Text.t_str(), 0, MMIO_READ);
	// 一個WAVE文件最少包含三個區塊，RIFF為其中最大，整個WAVE文件就是一個RIFF區塊
	// 進入RIFF區塊(RIFF Chunk)
	mmioDescend(handle, &ChunkInfo, 0, MMIO_FINDRIFF);

	// 進入fmt區塊(RIFF子塊，包含音訊結構的信息)
	FormatChunkInfo.ckid = mmioStringToFOURCC("fmt", 0); // 尋找fmt子塊
	mmioDescend(handle, &FormatChunkInfo, &ChunkInfo, MMIO_FINDCHUNK);
	// 進入fmt子塊
	// 讀取wav結構信息
	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	mmioRead(handle, (char*) & wfx, FormatChunkInfo.cksize);
	// 跳出fmt區塊
	mmioAscend(handle, &FormatChunkInfo, 0);
	// 進入data區塊(包含所有的數據波形)
	DataChunkInfo.ckid = mmioStringToFOURCC("data", 0);
	mmioDescend(handle, &DataChunkInfo, &ChunkInfo, MMIO_FINDCHUNK);
	// 獲得data區塊的大小
	DataSize = DataChunkInfo.cksize;
	// 分配緩衝區
	WaveOutData = new char[DataSize];
	mmioRead(handle, WaveOutData, DataSize);
	// 打開輸出設備
	waveOutOpen(&WaveOut, g_selectOutputDevice, &wfx, 0, 0, WAVE_FORMAT_QUERY);
	// 判斷格式
	waveOutOpen(&WaveOut, g_selectOutputDevice, &wfx, NULL, NULL,
		CALLBACK_NULL);

	// 設置wave header.
	memset(&WaveOutHeader, 0, sizeof(WaveOutHeader));
	WaveOutHeader.lpData = WaveOutData;
	WaveOutHeader.dwBufferLength = DataSize;
	// 準備wave header.
	waveOutPrepareHeader(WaveOut, &WaveOutHeader, sizeof(WAVEHDR));
	// 將緩衝區資料寫入撥放設備(開始撥放).
	waveOutWrite(WaveOut, &WaveOutHeader, sizeof(WAVEHDR));
	Sleep(100);
	Application->ProcessMessages();
	// 開始錄音
	TestStart(tag);      ///////////////////////////////////////這邊開始
}
/////--------------------------------------------------
void  TUSBHIDForm::TestStart(int tag) {
	// 取得設備ID
	int g_selectinputDevice = -1;
	if(tag == 0){
		if (cb_myInDevice->ItemIndex >= 0) {
			g_selectinputDevice = (cb_myInDevice->ItemIndex);
		}
	}else if(tag == 1){
		if (cb_myInDevice->ItemIndex >= 0) {
			g_selectinputDevice = (cb_myInDevice->ItemIndex);
		}
	}else if(tag == 2){
		if (cbInputDevice->ItemIndex >= 0) {
			g_selectinputDevice = (cbInputDevice->ItemIndex);
		}
	}else if(tag == 3){
		if (cbInputDevice->ItemIndex >= 0) {
			g_selectinputDevice = (cbInputDevice->ItemIndex);
		}
	}
	int res;
	buffers=1;
	//buffer_size=2048;
	buff=0;
	waveFormat.wFormatTag=WAVE_FORMAT_PCM;  // 格式
	waveFormat.nChannels=2;  // 聲道數量（即單聲道，立體聲...）
	waveFormat.nSamplesPerSec=sampledot;  // 每秒採樣次數(採樣率)
	waveFormat.wBitsPerSample=16;     // 每次採樣Bit(採樣精度)
	waveFormat.nBlockAlign=waveFormat.wBitsPerSample / 8 * waveFormat.nChannels; // 數據阻塞大小
	waveFormat.nAvgBytesPerSec=waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;  // 緩衝區估計
	waveFormat.cbSize=0;//PCM格式 此值忽略


	//  Audio buffers
	WaveBuffers=new WAVEHDR[buffers];
	//2048 * 每秒採樣byte * 聲道數量
	WaveBufSize=buffer_size * waveFormat.wBitsPerSample / 8 * waveFormat.nChannels;
	// waveInOpen開啟輸入設備
	if((res=waveInOpen(&hWaveIn, g_selectinputDevice, &waveFormat, (DWORD) this->Handle, (DWORD) this, CALLBACK_WINDOW)) == MMSYSERR_NOERROR)
	{
		//VirtualAlloc 申請虛擬內存空間
		//lpData: 緩衝區地址
		WaveBuffers[0].lpData= (char *)VirtualAlloc(0, WaveBufSize, MEM_COMMIT, PAGE_READWRITE);
		WaveBuffers[0].dwBufferLength=WaveBufSize; // 緩衝區長度
		WaveBuffers[0].dwFlags=0; // 緩衝區屬性
		WaveBuffers[0].dwLoops=0; // 播放循環的次數
		// 準備一個波形輸入緩衝區。一旦初始化時，此功能可讓音頻驅動程式和作業系統處理標頭檔或緩衝區。
		if((res=waveInPrepareHeader(hWaveIn, &WaveBuffers[0], sizeof(WAVEHDR))) == MMSYSERR_NOERROR)
		{   // 發送到指定的波形的音頻輸入裝置的輸入緩衝區。當緩衝區被填滿後，通知應用程式。
			if((res=waveInAddBuffer(hWaveIn, &WaveBuffers[0], sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
			{
				bStart=false;
				memoMsg->Lines->Add("開啟Hippo治具失敗");
			}
		}
		// 開始錄音
		if((res=waveInStart(hWaveIn)) == MMSYSERR_NOERROR)
		{
			bStart=true;
		}
		else
		{
			bStart=false;
			memoMsg->Lines->Add("開啟Hippo治具失敗");
		}
	}
	else
	{
		bStart=false;
		memoMsg->Lines->Add("開啟Hippo治具失敗");
	}
}
//-------------------------------------------------------------------------
bool TUSBHIDForm::CheckData(int tag){
	int count = buffer_size;
	bool zo, Tempfirst;
	int zcL = 0, zcR = 0;
	int dotH = 0, dotL = 0;
	int VHLtag = 0;
	int HLnum = 0;
	short tempdata;
	float totalD = 0.0, totalF = 0.0;
	float FQL = 0.0, FQR = 0.0;
	Tempfirst = false;
	//左聲道
	Memo3->Lines->Add("---左聲道---");
	for(int i=0; i < count - 1; i+=2){
		tempdata = my_data[i] / 15000;
		if (tempdata > 0) // H
		{
			if(VHLtag == 2){
				float duty = (float)dotH/float(dotH+dotL);
				float fq = (float)1/(float(dotH+dotL)*sampletime);
				//Memo1->Lines->Add((AnsiString)duty);
				//Memo2->Lines->Add((AnsiString)fq);
				dotH = dotL = 0;
				if(duty<=0.6 && duty>=0.4){
					totalD += duty;
					totalF += fq;
					HLnum++;
				}
				Tempfirst = true;
			}
			VHLtag = 1;
			if(VHLtag == 1)
				dotH++;
			zo = true;
		}
		else {
			VHLtag = 2;
			if(VHLtag == 2)
				dotL++;
			if (zo) {
				zcL++;
			}
			zo = false;
		}
		//Memo3->Lines->Add((AnsiString)my_data[i]/200);
	}
	if(totalD == 0 || totalF == 0){
		Memo3->Lines->Add("不是方波");
	}else{
		Memo3->Lines->Add("完整波數 = " + AnsiString(HLnum));
		Memo3->Lines->Add("duty = " + AnsiString((float)((int)(totalD/HLnum*100)*0.01)));
		FQL = totalF/HLnum;
		//FQL = (float)((int)(totalF/HLnum*100)*0.01);
		Memo3->Lines->Add("fq = " + AnsiString((float)((int)(FQL*100)*0.01)));
	}

	zcL = 0, zcR = 0;
	dotH = 0, dotL = 0;
	VHLtag = 0;
	HLnum = 0;
	totalD = 0.0, totalF = 0.0;
	Tempfirst = false;

	//右聲道
	Memo3->Lines->Add("---右聲道---");
	for(int i=1; i < count; i+=2){
		tempdata = my_data[i] / 15000;
		if (tempdata > 0) // H
		{
			if(VHLtag == 2){
				float duty = (float)dotH/float(dotH+dotL);
				float fq = (float)1/(float(dotH+dotL)*sampletime);
				//Memo1->Lines->Add((AnsiString)duty);
				//Memo2->Lines->Add((AnsiString)fq);
				dotH = dotL = 0;
				if(duty<=0.6 && duty>=0.4){
					totalD += duty;
					totalF += fq;
					HLnum++;
				}
				Tempfirst = true;
			}
			VHLtag = 1;
			if(VHLtag == 1)
				dotH++;
			zo = true;
		}
		else {
			VHLtag = 2;
			if(VHLtag == 2)
				dotL++;
			if (zo) {
				zcR++;
			}
			zo = false;
		}
		//Memo3->Lines->Add((AnsiString)my_data[i]/200);
	}
	if(totalD == 0 || totalF == 0){
		Memo3->Lines->Add("不是方波");
	}else{
		Memo3->Lines->Add("完整波數 = " + AnsiString(HLnum));
		Memo3->Lines->Add("duty = " + AnsiString((float)((int)(totalD/HLnum*100)*0.01)));
		FQR = totalF/HLnum;
		Memo3->Lines->Add("fq = " + AnsiString((float)((int)(FQR*100)*0.01)));
	}
	memoMsg->Lines->Add((int)FQL);
	memoMsg->Lines->Add((int)FQR);
	if(tag == 0){
		if(FQL>=900 && FQR>=900)
			return true;
		else
			return false;
	}else if(tag == 1){
		if(FQL>=950 && FQL<=1050 && FQR>=1950 && FQR<=2050)
			return true;
		else
			return false;
	}else if(tag == 2){
		if(FQL>=950 && FQL<=1050 && FQR>=950 && FQR<=1050)
			return true;
		else
			return false;
	}

}

void TUSBHIDForm::Delay(int iMilliSeconds) // 原版delay time 用在thread裡面
{
	int iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}

// 接收系統消息
void __fastcall TUSBHIDForm::WndProc(TMessage &Message)
{   // 接收聲音消息到緩衝區滿時
	if(Message.Msg == MM_WIM_DATA)
	{
		// 停止播放並重置管理器
		waveOutReset(WaveOut);
		// 關閉撥放設備
		waveOutClose(WaveOut);
		// 清理用WaveOutPrepareHeader準備的Wave。
		waveOutUnprepareHeader(WaveOut, &WaveOutHeader, sizeof(WAVEHDR));
		// 釋放內存
		if (WaveOutData != NULL) {
			delete[]WaveOutData;
			WaveOutData = NULL;
		}

		this->ProcessInput();
	}
	TForm::WndProc(Message);    //
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::ProcessInput(void)
{
	//  Prepare data buffer
	if(bStart)
	{
		bStart=false;
		WAVEHDR *in= &WaveBuffers[buff];
		::waveInStop(hWaveIn);
		::waveInUnprepareHeader(hWaveIn, in, sizeof(WAVEHDR));//釋放緩衝區空間
		::waveInClose(hWaveIn);

		//  Update views
		Oscilloscopes((short *)(in->lpData));
		//FillBuffer((short *)(in->lpData));
		delete []WaveBuffers;
	}
}
//---------------------------------------------------------------------------
void TUSBHIDForm::Oscilloscopes(short *idata)
{
	int count=buffer_size;
	memcpy(my_data, idata, count * sizeof(short));
	long    leftzerolevel=(Wave_View->ClientHeight / 2 - 10) / 2;   //左聲道 X軸位置
	long    rightzerolevel=Wave_View->ClientHeight / 2 + 10 + leftzerolevel;//右聲道 X軸位置
	double  timescale=leftzerolevel / 32768.0;//16位元  -32768 ~ +32767
	memcpy(my_data, idata, count * sizeof(short));
	Wave_View->Canvas->Lock();
	Wave_View->Canvas->Brush->Color=clBlack;
	Wave_View->Canvas->Pen->Color=clWhite;
	Wave_View->Canvas->Rectangle(0, 0, Wave_View->ClientWidth, Wave_View->ClientHeight);
	Wave_View->Canvas->Pen->Color=clYellow;
	Wave_View->Canvas->MoveTo(0, leftzerolevel);
	Wave_View->Canvas->LineTo(Wave_View->ClientWidth, leftzerolevel);
	Wave_View->Canvas->MoveTo(0, rightzerolevel);
	Wave_View->Canvas->LineTo(Wave_View->ClientWidth, rightzerolevel);
	Wave_View->Canvas->Pen->Color=clAqua;
	Wave_View->Canvas->Lock();
	double  ts=static_cast<double>(Wave_View->ClientWidth) / static_cast<double>(count); // 每一點相距的距離
    int     x;
	Wave_View->Canvas->MoveTo(0, leftzerolevel);
	for(int i=0; i < count - 1; i+=2)
    {
		x=i * ts;
		int y=leftzerolevel - static_cast<double>(my_data[i]) * timescale;
		Wave_View->Canvas->LineTo(x, y);
	}

	Wave_View->Canvas->MoveTo(0, rightzerolevel);
	for(int i=1; i < count; i+=2)
    {
		x=i * ts;
		int y=rightzerolevel - static_cast<double>(my_data[i]) * timescale;
		Wave_View->Canvas->LineTo(x, y);
	}

	Wave_View->Canvas->Unlock();
}
void __fastcall TUSBHIDForm::Timer4Timer(TObject *Sender)
{
	iniCB();
}
//---------------------------------------------------------------------------
void TUSBHIDForm::iniCB(){
	char n_outputDevices, n_inputDevices;
	WAVEINCAPS Wave_input;
	WAVEOUTCAPS Wave_output;
	n_inputDevices = waveInGetNumDevs();
	n_outputDevices = waveOutGetNumDevs();
	cbInputDevice->Clear();
	cb_myInDevice->Clear();
	cbOutputDevice->Clear();
	cb_myOutDevice->Clear();
	for (int nC1 = 0; nC1 < n_outputDevices; ++nC1) {
		if (waveOutGetDevCaps(nC1, &Wave_output, sizeof(Wave_output)) == 0) {
			cbOutputDevice->Items->Add(Wave_output.szPname);
			cb_myOutDevice->Items->Add(Wave_output.szPname);
		}
		if(!strstr(Wave_output.szPname,"USB Advanced"))
			cbOutputDevice->ItemIndex=nC1;
		if(strstr(Wave_output.szPname,"USB Advanced"))
			cb_myOutDevice->ItemIndex=nC1;
	}
	for (int nC2 = 0; nC2 < n_inputDevices; ++nC2) {
		if (waveInGetDevCaps(nC2, &Wave_input, sizeof(Wave_input)) == 0) {
			cbInputDevice->Items->Add(Wave_input.szPname);
			cb_myInDevice->Items->Add(Wave_input.szPname);
		}
		if(!strstr(Wave_input.szPname,"USB Advanced"))
			cbInputDevice->ItemIndex=nC2;
		if(strstr(Wave_input.szPname,"USB Advanced"))
			cb_myInDevice->ItemIndex=nC2;
	}
	if(cbInputDevice->ItemIndex == -1){
		cbInputDevice->ItemIndex = 0;
	}
	if(cbOutputDevice->ItemIndex == -1){
		cbOutputDevice->ItemIndex = 0;
	}
	USBHIDForm->Refresh();
}
/////////////////////////////////////////////////////////////////////////
bool TUSBHIDForm::CallCommand_C(AnsiString Command_line) {
	// 下Command指令 BY CreateProcess
	UINT Result;
	DWORD dwExitCode;
	STARTUPINFOA si = {
		0
	};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoA(&si);
	si.wShowWindow = SW_HIDE;
	AnsiString cmd = getenv("COMSPEC");
	AnsiString Command = cmd + " /c " + Command_line;
	Result = CreateProcessA(NULL, Command.c_str(), NULL, NULL, false, 0, NULL,
		NULL, &si, &pi);
	if (Result) {
		CloseHandle(pi.hThread);
		if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_FAILED) {
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
		}
		CloseHandle(pi.hProcess);
		return true;
	}
}

void __fastcall TUSBHIDForm::memoMsgMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	showaudiotup++;
	if(showaudiotup > 5){
		memoMsg->Height = 505;
		Panel4->Visible = true;
		showaudiotup=0;
	}
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::sb_value_audioChange(TObject *Sender)
{
	edt_value_audio->Text = IntToStr(sb_value_audio->Position);
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::btn_close_audioClick(TObject *Sender)
{
	Panel4->Visible = false;
	memoMsg->Height = 684;
	showaudiotup=0;
}
//---------------------------------------------------------------------------


