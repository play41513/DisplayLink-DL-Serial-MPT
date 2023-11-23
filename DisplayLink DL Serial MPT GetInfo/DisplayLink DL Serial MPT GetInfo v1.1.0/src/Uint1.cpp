// Module:
// Notices: Copyright(c) 2017 Leno.Lee.
// Purpose:
// Compiler: Embarcadero RAD Studio 2017
// OS: Windows 8.1 x64
// Version: 3.2.2.80__declspec(uuid("{2DD69FE9-6425-4DB2-A21F-7987EDF08B77}"))
// --------------------------------------------------------------------------
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
#include "MessageBox.h"

char autoERRC, autoSTEP;
char oneERRC;
bool VocI, VocO, AuL, AuR, oneSTEP;
bool AuLPass, AuRPass, autoClose;
USB_Hub_Tree *g_pUsb_Hub_Tree = NULL;
int showaudiotup;
bool G_bCheckDevInfo = false;
bool bWin10 = false;
bool bStart = false;
bool bPlugIn = false;
bool bSaveFile = false;
int TestTime = 0;
AnsiString ErrorCode;

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
	if(ckbWin10->Checked){
		bWin10 = true;
	}
	Caption = APP_TITLE;
	ckbDutMaxTimeGetId->ItemIndex = 20;}
// ---------------------------------------------------------------------------
bool TUSBHIDForm::GetDutInfomation(USB_Hub_Tree &hubTree){

	if(cmbNumOfChip->ItemIndex == 0){
		MB_WARNING(Handle, DUT_STRING_NOT_SET_KIND);
		return false;
	}else{

	}

	m_dut.SetKind(cmbNumOfChip->ItemIndex);

	if(!m_dut.GetId()){
		//MB_WARNING(Handle, MB_FAILED_GET_DUT_ID);
		return false;
	}else{
	}

	if(!m_dut.GetInfo()){
		//memoMsg->Lines->Add(DUT_STRING_NOT_FIND);
		//MB_WARNING(Handle, MB_FAILED_GET_DUT_INFORMATION);
		return false;
	}

	if(m_dut._Kind == 2)
		USBHIDForm->Height = 613;
	else USBHIDForm->Height = 365;
	plChipName->Caption = m_dut._Dev1;
	plBackupFW->Caption = m_dut._If1.BackFirmware;
	plMinFWVer->Caption = m_dut._If1.MinFwVersion;
	plMAC->Caption = m_dut._If1.MAC;
	plUSBSN->Caption = m_dut._If1.SN;

	memoMsg->Lines->Add(DUT_STRING_INFO_FLASH);
	memoMsg->Lines->Add(_T("----------------------------------------------------"));
	memoMsg->Lines->Add(DUT_STRING_NUM_OF_CHIP + IntToStr(m_dut._Kind));
	memoMsg->Lines->Add(_T("----------------------------------------------------"));
	memoMsg->Lines->Add(m_dut._Dev1);
	memoMsg->Lines->Add(_T("----------------------------------------------------"));
	memoMsg->Lines->Add(DUT_FT + m_dut._If1.FirmwareType);
	memoMsg->Lines->Add(DUT_RL + m_dut._If1.ReleaseLevel);
	memoMsg->Lines->Add(DUT_VN + m_dut._If1.Version);
	memoMsg->Lines->Add(DUT_SV + m_dut._If1.SvnVersion);
	memoMsg->Lines->Add(DUT_BD + m_dut._If1.BuildDate);
	memoMsg->Lines->Add(DUT_TE + m_dut._If1.Time);
	memoMsg->Lines->Add(DUT_BF + m_dut._If1.BackFirmware);
	plBackupFW->Caption = m_dut._If1.BackFirmware;
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
		plChipName2->Caption = m_dut._Dev2;
		plBackupFW2->Caption = m_dut._If2.BackFirmware;
		plMinFWVer2->Caption = m_dut._If2.MinFwVersion;
		plMAC2->Caption = m_dut._If2.MAC;
		plUSBSN->Caption = m_dut._If2.SN;
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
	return true;
}

// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::ckbDutMaxTimeGetIdChange(TObject *Sender)
{
	int i = ckbDutMaxTimeGetId->Items->Strings[ckbDutMaxTimeGetId->ItemIndex].ToInt();
	m_dut.SetDutMaxTimeGetId(i);

}
//---------------------------------------------------------------------------
void TUSBHIDForm::Delay(int iMilliSeconds) // 原版delay time 用在thread裡面
{
	int iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
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
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnGetInfo4KClick(TObject *Sender)
{
	TestTime = GetTickCount();
	BitBtn1->Enabled = false;
	btnGetInfo4K->Enabled = false;
	btnStart->Enabled = false;
	bool bResult = true;
	plChipName->Caption = "-";
	plBackupFW->Caption  = "";
	plBackupFW2->Caption  = "";
	plMinFWVer->Caption  = "";
	plMinFWVer2->Caption  = "";
	plUSBSN->Caption  = "";
	plUSBSN2->Caption  = "";
	plMAC->Caption  = "";
	plMAC2->Caption  = "";
	USBHIDForm->Refresh();

	if(bWin10)
		if(!CheckWin10Driver()) bResult = false;
	if(bResult)
	{
		bool bl = false;
		btnGetInfo4K->Enabled = false;
		USB_Hub_Tree hubTree;
		G_bCheckDevInfo = true;
		memoMsg->Clear();
		memoMsg->Refresh();

		plChipName->Caption  = "Flash Data Loading...";
		if(!GetDutInfomation(hubTree)){
			memoMsg->Clear();
			memoMsg->Refresh();
			if(bWin10)
			{
				if(!CheckWin10Driver())
				{
					memoMsg->Lines->Add("Get Device information Fail,plugin device again");
					bl = true;
				}
				else
				{
					memoMsg->Clear();
					plChipName->Caption  = "Flash Data Loading...";
					if(!GetDutInfomation(hubTree))
					{
						memoMsg->Lines->Add("Get Device information Fail,plugin device again");
						bl = true;
					}
                }
			}
		}
		G_bCheckDevInfo = false;
		if(!bl)
		{
			Delay(5000);
			if(ckb4k30->Checked)
			{
				if(!m_dut.ShowVideo4K()){
					Delay(3000);
					memoMsg->Lines->Add("4K 30fps Fail try again...");
					if(!m_dut.ShowVideo4K()){
						memoMsg->Lines->Add("(!)4K 30fps Fail");
						plChipName->Caption = "(!)4K 30fps Fail";
					}
					else memoMsg->Lines->Add("4K 30fps PASS");
				}
			}
			else if(ckb4k60->Checked)
			{
				if(!m_dut.ShowVideo4K60()){
					Delay(3000);
					memoMsg->Lines->Add("4K 60fps Fail try again...");
					if(!m_dut.ShowVideo4K60()){
						memoMsg->Lines->Add("(!)4K 60fps Fail");
						plChipName->Caption = "(!)4K 60fps Fail";
					}
					else memoMsg->Lines->Add("4K 60fps PASS");
				}
			}
			else if(ckb2k->Checked)
			{
				if(!m_dut.ShowVideo2K()){
					Delay(3000);
					memoMsg->Lines->Add("2K 60fps Fail try again...");
					if(!m_dut.ShowVideo2K()){
						memoMsg->Lines->Add("(!)2K 60fps Fail");
						plChipName->Caption = "(!)2K 60fps Fail";
					}
					else memoMsg->Lines->Add("2K 60fps PASS");
				}
			}
		}
	}
	for(int i =0 ; i<=memoMsg->Lines->Count;i++)
	{
		if(memoMsg->Lines->Strings[i]=="")
		{
			memoMsg->Lines->Delete(i);
		}
	}
	if(btnStart->Caption!="Stop")
	{
		BitBtn1->Enabled = true;
		btnGetInfo4K->Enabled = true;
	}
	bSaveFile = true;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::CheckWin10Driver()
{
	//列舉Display產品
	memoMsg->Clear();
	AnsiString Resultdata = DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
	char *pch = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	memoMsg->Lines->Add("enum Display Chip...");
	while (pch != NULL)
	{
		memoMsg->Lines->Add(pch);
		if(strstr(pch,"MI_00")||strstr(pch,"MI_01"))
			NList->Add(pch);
		pch = strtok(NULL, "\r\n");
		if(strstr(AnsiString(memoMsg->Text).c_str(),"No matching devices found"))
		{
			plChipName->Caption  = "No matching devices found...";
			plChipName2->Caption = "No matching devices found...";
			delete NList;
			return false;
		}
	}
	if(NList->Count>2)
	{
		memoMsg->Lines->Add("Display Chip num error");
		plChipName->Caption  = "(!)Display Chip num error";
		delete NList;
		return false;
	}
	//確認是否在USBIO模式
	bool bResult = true;
	for(int x=0;x<NList->Count;x++)
	{
		if(!strstr(AnsiString(NList->Strings[x]).c_str(),"USBIO"))
			bResult = false;
		NList->Strings[x] = NList->Strings[x].SubString(1,27);
	}
	if(bResult)
	{
		memoMsg->Lines->Add("Display Chip Pass");
		delete NList;
		return true;
	}
	//灌驅動
	memoMsg->Lines->Add("USBIO install...");
	plChipName->Caption  = "USBIO Driver install...";
	for(int x=0;x<NList->Count;x++)
	{

		AnsiString cmd = "devcon updateni usbio_driver\\DisplayLinkUSBIOMTT.inf \""+NList->Strings[x]+"\"";
		Resultdata = DosCommand(cmd);
		if(!strstr(Resultdata.c_str(),"Drivers installed successfully"))
		{
			memoMsg->Lines->Add(NList->Strings[x]);
			memoMsg->Lines->Add(Resultdata);
			plChipName->Caption  = "(!)Driver installed failed";
			return false;
		}
		memoMsg->Lines->Add(NList->Strings[x]+" Drivers installed successfully");
	}
	//重新抓裝置
	memoMsg->Lines->Add("restart device...");
	plChipName->Caption  = "restart device...";
	DosCommand("devcon restart \"\*usb\\vid_17e9\*\"");
	delete NList;

	//再次列舉Display產品
	memoMsg->Lines->Add("enum device");
	Resultdata = DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
	char *pch2 = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList2 = new TStringList();
	while (pch2 != NULL)
	{
		if(strstr(pch2,"MI_00")||strstr(pch2,"MI_01"))
			NList2->Add(pch2);
		pch2 = strtok(NULL, "\r\n");
	}
	//確認是否在USBIO模式
	bResult = true;
	for(int x=0;x<NList2->Count;x++)
	{
		if(!strstr(AnsiString(NList2->Strings[x]).c_str(),"USBIO"))
		{
			if(!strstr(AnsiString(NList2->Strings[x]).c_str(),"Test Mode"))
				bResult = false;
			memoMsg->Lines->Add(NList2->Strings[x]);
		}
		else memoMsg->Lines->Add(NList2->Strings[x]);
	}
	delete NList2;
	if(bResult)
	{
		memoMsg->Lines->Add("USBIO driver installation Successful");
		plChipName->Caption  = "Flash Data Loading...";
		return true;
	}
	else
	{
		memoMsg->Lines->Add("USBIO driver installation failed");
		plChipName->Caption  = "(!)Driver installation failed";
		return false;
	}
}
AnsiString TUSBHIDForm::DosCommand(AnsiString sCmdline)
{
	PROCESS_INFORMATION proc = {0}; //關於進程資訊的一個結構
	long ret;
	bool sPipe;
	STARTUPINFO start = {0};
	SECURITY_ATTRIBUTES sa = {0};
	HANDLE hReadPipe ;
	HANDLE hWritePipe;
	AnsiString sOutput;
	AnsiString sBuffer;
	unsigned long lngBytesRead;
	char cBuffer[256];
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor=0;
	sa.bInheritHandle = TRUE;
	sPipe=::CreatePipe(&hReadPipe, &hWritePipe,&sa, 0); //創建管道
	if (!sPipe)
	{
	sOutput="CreatePipe failed. Error: "+AnsiString(GetLastError());
	return sOutput;
	}
	start.cb = sizeof(STARTUPINFOA);
	start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	start.hStdOutput = hWritePipe;
	start.hStdError = hWritePipe;
	start.wShowWindow = SW_HIDE;
	sBuffer = sCmdline;
	ret =::CreateProcess(0, sBuffer.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc);
	if (ret == 0)
	{
	sOutput="Bad command or filename";
	return sOutput;
	}
	::CloseHandle(hWritePipe);
	do
	{
	memset(cBuffer,'\0',sizeof(cBuffer));
	ret = ::ReadFile(hReadPipe, &cBuffer, 255, &lngBytesRead, 0);
	sBuffer=StrPas(cBuffer);
	sOutput = sOutput+sBuffer;
	Application->ProcessMessages();

	} while (ret != 0 );
	::CloseHandle(proc.hProcess);
	::CloseHandle(proc.hThread);
	::CloseHandle(hReadPipe);
	return sOutput;
}




void __fastcall TUSBHIDForm::BitBtn1Click(TObject *Sender)
{
	BitBtn1->Enabled = false;
	btnGetInfo4K->Enabled = false;
	btnStart->Enabled = false;
	memoMsg->Lines->Add("remove the test driver ...");
	DosCommand("redstone_uninstall_usbio.bat");
	memoMsg->Clear();
	BitBtn1->Enabled = true;
	btnGetInfo4K->Enabled = true;
	btnStart->Enabled = true;
	memoMsg->Lines->Add("Unplug the device...");
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::btnStartClick(TObject *Sender)
{
	if(btnStart->Caption=="Start Auto Test")
	{
		btnStart->Caption="Stop";
		cmbNumOfChip->Enabled = false;
		btnGetInfo4K->Enabled = false;
		BitBtn1->Enabled      = false;
		bStart = true;
	}
	else
	{
		btnStart->Caption="Start Auto Test";
		bStart = false;
		cmbNumOfChip->Enabled = true;
		btnGetInfo4K->Enabled = true;
		BitBtn1->Enabled      = true;
		bPlugIn = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Timer5Timer(TObject *Sender)
{
	if(bStart)
	{
		try
		{
		AnsiString Resultdata = DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
		char *pch = strtok(Resultdata.c_str(), "\r\n");
		TStringList* NList = new TStringList();
		while (pch != NULL)
		{
			if(strstr(pch,"MI_00")||strstr(pch,"MI_01"))
				NList->Add(pch);
			else if(strstr(AnsiString(pch).c_str(),"No matching devices found"))
			{
				memoMsg->Text = "No matching devices found";
				plChipName->Caption  = "No matching devices found...";
				plChipName2->Caption = "No matching devices found...";
				plBackupFW->Caption  = "";
				plBackupFW2->Caption  = "";
				plMinFWVer->Caption  = "";
				plMinFWVer2->Caption  = "";
				plUSBSN->Caption  = "";
				plUSBSN2->Caption  = "";
				plMAC->Caption  = "";
				plMAC2->Caption  = "";
				bPlugIn = false;
				break;
			}
			pch = strtok(NULL, "\r\n");
		}
		if(NList->Count>2)
		{
			memoMsg->Lines->Add("Display Chip num error");
			plChipName->Caption  = "(!)Display Chip num error";
			delete NList;
		}
		else if(NList->Count!=0)
		{
			delete NList;
			bStart = false;
			if(!bPlugIn) btnGetInfo4KClick(NULL);
			bPlugIn = true;
			bStart = true;
		}
		}
		catch(...)
		{
			bStart = true;
        }
	}
}
//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::N1Click(TObject *Sender)
{
	m_dut.ShowVideo4K60();
}
//---------------------------------------------------------------------------



void __fastcall TUSBHIDForm::plSwitchClick(TObject *Sender)
{
	if(plSwitch->Caption == ">")
	{
		plSwitch->Caption = "<";
		USBHIDForm->Width = 850;
	}
	else
	{
		plSwitch->Caption = ">";
		USBHIDForm->Width = 473;
	}
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::ckbWin10Click(TObject *Sender)
{
	bWin10 = ckbWin10->Checked ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::ckb2kClick(TObject *Sender)
{
	if(ckb2k->Checked)
	{
		ckb4k30->Checked = false;
		ckb4k60->Checked = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::ckb4k30Click(TObject *Sender)
{
	if(ckb4k30->Checked)
	{
		ckb2k->Checked = false;
		ckb4k60->Checked = false;
	}
}
//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::ckb4k60Click(TObject *Sender)
{
	if(ckb4k60->Checked)
	{
		ckb2k->Checked = false;
		ckb4k30->Checked = false;
	}
}
//---------------------------------------------------------------------------


