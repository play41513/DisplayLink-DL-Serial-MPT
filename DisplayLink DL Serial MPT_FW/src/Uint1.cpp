﻿// Module:
// Notices: Copyright(c) 2017 Leno
// Purpose:
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 3.2.2.81__declspec(uuid("{2DD69FE9-6425-4DB2-A21F-7987EDF08B77}"))
// --------------------------------------------------------------------------
#define buffer_size 2048

#include <vcl.h>
#include <vfw.h>
#include <direct.h>
#include <stdio.h>
#pragma hdrstop


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
#include <Registry.hpp>
#include "Iphlpapi.h"
#pragma link "Iphlpapi.lib"

#include <windows.h>
#include "wininet.h"
#pragma link "wininet.lib"

char autoERRC, autoSTEP;
char oneERRC;
bool oneSTEP;
bool autoClose;
bool G_bCheckDevInfo = false;
bool bWin10 = false;
bool bLOAD = false;
bool bPlugIn = false;
bool bStart = false;
AnsiString SPKG_FW_File_1 = "",SPKG_FW_File_2 = "",MEM_FW_File = "";
bool bSPKG_1 = false,bSPKG_2 = false,bMEM = false;
double Test_Time = 0;
// ---------------------------------------------------------------------------
int __fastcall TUSBHIDForm::HexToInt(AnsiString HexStr) {
	int iIndex, iHex, totalChar;
	TCHAR HexChar[8];

	HexStr.UpperCase(); // 把字串轉成全大寫

	_tcscpy_s(HexChar, 8, HexStr.c_str());
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
}
#pragma package(smart_init)
#pragma link "Gauges"
#pragma resource "*.dfm"
TUSBHIDForm *USBHIDForm;
// ---------------------------------------------------------------------------

__fastcall TUSBHIDForm::TUSBHIDForm(TComponent* Owner) : TForm(Owner) {;
}
// ---------------------------------------------------------------------------
AnsiString FileName;

void __fastcall TUSBHIDForm::FormClose(TObject *Sender, TCloseAction &Action) {
	Timer2->Enabled = false;

	TIniFile *ini;
	ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));

	//Thired step. 儲存所有設定
	ini->WriteString(_T("KIND"), _T("KIND"), cmbNumOfChip->Text);
	//BURN_SETTING
	ini->WriteBool("BURN_SETTING", "BURN_SN", ckbBurnSN->Checked);
	ini->WriteBool("BURN_SETTING", "BURN_MAC", ckbBurnMAC->Checked);
	ini->WriteBool("BURN_SETTING", "VERIFY", ckbBurnVerify->Checked);
	ini->WriteBool("BURN_SETTING", "AUTO", ckbAutoIncrease->Checked);
	ini->WriteBool("BURN_SETTING", "CHECK_FW", ckbCheckFW->Checked);
	ini->WriteString(_T("BURN_SETTING"), _T("BUTN_COUNT"), edtBurnCount->Text);
	ini->WriteString(_T("BURN_SETTING"), _T("BEGIN_SN"), mskedtBeginSn->Text);
	ini->WriteString(_T("BURN_SETTING"), _T("MAC_PREFIX"), mskedtMacPre->Text);
	ini->WriteString(_T("BURN_SETTING"), _T("BEGIN_MAC_SN"), mskedtBeginMac->Text);
	//BURN_CONTENT
	ini->WriteString(_T("BURN_CONTENT"), _T("BURN_SN"), mskedtBurnSn->Text);
	ini->WriteString(_T("BURN_CONTENT"), _T("MAC_PREFIX"), mskedBurntMacPre->Text);

	ini->WriteBool(_T("FW"), "SPKG_FILE_UPDATE_1", bSPKG_1);
	ini->WriteString(_T("FW"), _T("SPKG_FILE"), SPKG_FW_File_1);
	ini->WriteBool(_T("FW"), "SPKG_FILE_UPDATE_2", bSPKG_2);
	ini->WriteString(_T("FW"), _T("SPKG_FILE"), SPKG_FW_File_2);
	ini->WriteBool(_T("FW"), "MEM_FILE_UPDATE", bMEM);
	ini->WriteString(_T("FW"), _T("MEM_FILE"), MEM_FW_File);
	ini->WriteString(_T("FW"), _T("BACKUP_FW"), edtBackupFW->Text);
	ini->WriteString(_T("FW"), _T("MIN_FW"), edtMinFW->Text);

	delete ini;
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
		ProgressBar->Progress += 5;
		plMsg->Caption = "燒錄序號...";
		updcode = " upd";
		SN = "\t" + mskedtBurnSn->Text;

		if(m_dut._Kind == 2 && m_dut._Dev1.Pos(_T("4301_")) != 0){
			updcode += " -s " + AnsiString(mskedtBurnSn->Text);
			MAC = AnsiString("\t") + _T("000000000000");;
		}
		else
		{
			if(ckbBurnSN->Checked)
				updcode += " -s " +AnsiString(mskedtBurnSn->Text);
			if(ckbBurnMAC->Checked)
			{
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
		}

		if(!m_dut.ProgramUPD(updcode)){
			ERRC = 0x01;
		}else{
			ERRC = 0x00;
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

		if(oneSTEP)
			strMessage += "REWORK";
		else
			strMessage += "NORMAL";

		//moMACRecord->Lines->Add(strMessage);
		//moMACRecord->Lines->SaveToFile(FileName);
	}else{
		plMsg->Caption = "(!)燒錄失敗";
		plMsg->Font->Color = clRed;
		memoMsg->Lines->Add("(!)燒錄失敗");
		ProgressBar->Progress = 100;
		ProgressBar->ForeColor = clRed;
	}

	return(ERRC);
}

// ---------------------------------------------------------------------------
char __fastcall TUSBHIDForm::DUTV(void) {
	int ERRC;
	UnicodeString mac1, sn1, mac2, sn2, msg ,min_fw,backup_fw;
	UnicodeString refMac = mskedBurntMacPre->Text + mskedtBurnMac->Text;
	//重新讀取晶片info flash
	if(m_dut.GetInfo()){
		//讀取成功
		ERRC = 0x00;
		ProgressBar->Progress += 5;
		sn1 = m_dut._If1.SN;
		mac1 = m_dut._If1.MAC;
		min_fw = m_dut._If1.MinFwVersion;
		backup_fw = m_dut._If1.BackFirmware;

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
	//---------------------------------------------------------Verify
	if (!ERRC) {
		if(mac1=="" && ckbFakeMAC->Checked)
		{
				plBackupFW->Color = clRed;
				plMsg->Caption = "(!)Fake MAC ERROR(MAC :"+AnsiString(mac1)+")";
				plMsg->Font->Color = clRed;
				memoMsg->Lines->Add("(!)Fake MAC ERROR(MAC :"+AnsiString(mac1)+")");
				ERRC = 0x40;
        }
		if(ckbCheckFW->Checked)
		{
			plBackupFW->Caption = backup_fw;
			plMinFW->Caption = min_fw;
			if(backup_fw != edtBackupFW->Text)
			{
				plBackupFW->Color = clRed;
				plMsg->Caption = "(!)Backup FW Version ERROR(MinFW :"+AnsiString(backup_fw)+")";
				plMsg->Font->Color = clRed;
				memoMsg->Lines->Add("(!)Backup FW Version ERROR(MinFW :"+AnsiString(backup_fw)+")");
				ERRC = 0x40;
			}
			if(min_fw != edtMinFW->Text)
			{
				plMinFW->Color = clRed;
				plMsg->Caption = "(!)Min FW Version ERROR(MinFW :"+AnsiString(min_fw)+")";
				plMsg->Font->Color = clRed;
				memoMsg->Lines->Add("(!)Min FW Version ERROR(MinFW :"+AnsiString(min_fw)+")");
				ERRC = 0x40;
			}
		}
		if(ckbBurnSN->Checked)
		{   //比對SN
			memoMsg->Lines->Add(VERIFY_DUT_SERIAL);
			if(m_dut._Kind == 1){
				if(sn1 != mskedtBurnSn->Text){
					plMsg->Caption = "(!)序號不符";
					plMsg->Font->Color = clRed;
					memoMsg->Lines->Add("(!)序號不符");
					ERRC = 0x40;
				}
			}else{
				if(sn1.Length() == 8){
					if(sn1 != mskedtBurnSn->Text){
						plMsg->Caption = "(!)序號不符";
						plMsg->Font->Color = clRed;
						memoMsg->Lines->Add("(!)序號不符 on flash1");
						ERRC = 0x40;
					}

					if(sn2 != mskedtBurnSn->Text + L"B"){
						plMsg->Caption = "(!)序號不符";
						plMsg->Font->Color = clRed;
						memoMsg->Lines->Add("(!)序號不符 on flash2");
						ERRC = 0x40;
					}
				}else{
					if(sn1 != mskedtBurnSn->Text + L"B"){
						plMsg->Caption = "(!)序號不符";
						plMsg->Font->Color = clRed;
						memoMsg->Lines->Add("(!)序號不符 on flash1");
						ERRC = 0x40;
					}

					if(sn2 != mskedtBurnSn->Text){
						plMsg->Caption = "(!)序號不符";
						plMsg->Font->Color = clRed;
						memoMsg->Lines->Add("(!)序號不符 on flash2");
						ERRC = 0x40;
					}
				}
			}
		}
		if(ckbBurnMAC->Checked)
		{
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
		if(ERRC) plMsg->Caption = "End Of Verification!";
		ProgressBar->Progress = 90;
	}
	return(ERRC);
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnAutoTestClick(TObject *Sender) {
	if(btnAutoTest->Caption=="Start")
	{
		ProgressBar->Progress = 0;
		ProgressBar->ForeColor = clTeal;
		plBackupFW->Caption = "";
		plBackupFW->Color = clWhite;
		plMinFW->Caption = "";
		plMinFW->Color = clWhite;
		plMsg->Caption = "No matching devices found...";
		plMsg->Font->Size = 16;
		plMsg->Font->Color = clBlue;
		lblStatus->Caption = "等候中";
		lblStatus->Color = clSkyBlue;
		btnAutoTest->Caption="Stop";
		bStart = true;
	}
	else
	{
		btnAutoTest->Caption="Start";
		bStart = false;
		bPlugIn = false;
	}
}

// ---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::Timer2Timer(TObject *Sender) {
	int kind;
	AnsiString CMD_RESPORNSE;
	if(!G_bCheckDevInfo){
		Timer2->Enabled = false;

		switch(autoSTEP) {
		case 0:
			kind = m_dut.GetKind();
			m_dut.SetKind(kind);
			getKindCnt = 0;
			autoERRC = 0;
			if(bSPKG_1)
				autoSTEP = 1;
			else if(bSPKG_2)
				autoSTEP = 2;
			else if(bMEM)
				autoSTEP = 3;
			else autoSTEP = 11;

			memoMsg->Lines->Add("開始作業");
			lblStatus->Color = clSkyBlue;
			lblStatus->Caption = "生產作業";

			break;
		case 1:
			memoMsg->Lines->Add("Firmware Updating...");
			plMsg->Caption ="Firmware Updating...";
			USBHIDForm->Refresh();
			CMD_RESPORNSE = DosCommand("dl_mfg_test program "+SPKG_FW_File_1);
			memoMsg->Lines->Add(CMD_RESPORNSE);
			if(CMD_RESPORNSE.Pos("No such file or directory"))
			{
				autoERRC = 1;
				autoSTEP = 9;
				plMsg->Caption = "No such file or directory";
			}
			else if(CMD_RESPORNSE.Pos("Bad magic value"))
			{
				autoERRC = 1;
				autoSTEP = 9;
				plMsg->Caption = "Bad magic value";
			}
			else if(bSPKG_2)
				autoSTEP = 2;
			else if(bMEM)
				autoSTEP = 3;
			else autoSTEP = 11;

			ProgressBar->Progress+=30;

			break;
		case 2:
			memoMsg->Lines->Add("Backup Firmware Updating...");
			plMsg->Caption ="Backup Firmware Updating...";
			USBHIDForm->Refresh();
			CMD_RESPORNSE = DosCommand("dl_mfg_test program backup "+SPKG_FW_File_2);
			memoMsg->Lines->Add(CMD_RESPORNSE);
			if(CMD_RESPORNSE.Pos("No such file or directory"))
			{
				autoERRC = 1;
				plMsg->Caption = "No such file or directory";
			}
			else if(CMD_RESPORNSE.Pos("Bad magic value"))
			{
				autoERRC = 1;
				plMsg->Caption = "Bad magic value";
			}
			ProgressBar->Progress=70;
			if(bMEM) autoSTEP = 3;
			else if(ckbFakeMAC->Checked) autoSTEP = 11;
			else autoSTEP = 4;
			break;
		case 3:
			memoMsg->Lines->Add("Mini Firmware Updating...");
			plMsg->Caption ="Mini Firmware Updating...";
			USBHIDForm->Refresh();
			CMD_RESPORNSE = DosCommand("dl_mfg_test config "+MEM_FW_File);
			memoMsg->Lines->Add(CMD_RESPORNSE);
			if(CMD_RESPORNSE.Pos("No such file or directory"))
			{
				autoERRC = 1;
				plMsg->Caption = "No such file or directory";
			}
			ProgressBar->Progress=90;
			if(!ckbFakeMAC->Checked) autoSTEP = 4;
			else autoSTEP = 11;
			break;
		case 11:
			memoMsg->Lines->Add("Fake MAC Burning...");
			plMsg->Caption ="Fake MAC Burning...";
			USBHIDForm->Refresh();
			CMD_RESPORNSE = DosCommand("dl_mfg_test upd -s 00000000 -a 00:24:9B:00:00:00");
			memoMsg->Lines->Add(CMD_RESPORNSE);
			autoSTEP = 4;
			break;
		case 4:
			plMsg->Caption ="Firmware Verify";
			lblStatus->Caption = "FW驗證程序";
			autoERRC = DUTV();

			autoSTEP = 9;
			break;
		case 5:
				lblStatus->Caption = "燒錄程序";
				autoERRC = DUTB();

				if (autoERRC) {
					autoSTEP = 9;
					break;
				}

			autoSTEP = 7;
			break;
		case 7:
			if(ckbBurnVerify->Checked){
				lblStatus->Caption = "驗證程序";
				autoERRC = DUTV();

				if (!autoERRC) {
					m_bAutoIncrease = true;
				}
			}else{
				m_bAutoIncrease = true;
			}

			autoSTEP = 9;
			break;
		case 9:
			if (autoERRC) {
				ProgressBar->ForeColor = clRed;
				lblStatus->Caption = "!!! 異常發生 !!!";
				lblStatus->Color = clRed;
			}
			else {
				memoMsg->Lines->Add("完成作業");
				lblStatus->Caption = "PASS 0";
				lblStatus->Color = clGreen;
				plMsg->Caption = "Update Success!";
				ProgressBar->Progress = 100;
				ProgressBar->ForeColor = clLime;
			}

			if(!oneSTEP){
				btnAutoTest->Enabled = true;
			}

			btnGetInfo->Enabled = true;
			autoSTEP = 10;
			memoMsg->Lines->Add(AnsiString((GetTickCount()-Test_Time)/1000)+"(sec))");

			break;
		case 10:
			if(oneSTEP){
				autoSTEP = 99;
				gbBurnContent->Enabled = true;
				break;
			}

			/*if(autoERRC){
				MessageBeep(MB_ICONERROR);
			}else{
				MessageBeep(MB_OK);
			}*/


			kind = m_dut.GetKind();
			if(kind == 0){
				m_dut.SetMtt(false);
				//autoSTEP = 0;
				Timer2->Enabled = false;

				if(ckbBurnVerify->Checked == false){
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
			if(ckbBurnSN->Checked)
			{
				tmp++;
				SS = IntToStr(tmp);
				SD = "";

				for (j = 0; j < (8 - SS.Length()); j++) {
					SD += "0";
				}

				SD += SS.c_str();
				mskedtBurnSn->Text = SD;
			}
		}else {
			// close Auto ......
			autoClose = true;
		}

		tmp = HexToInt(mskedtBurnMac->Text);
		if (tmp < HexToInt(mskedtEndMac->Text)) {
			if(ckbBurnSN->Checked)
			{
				tmp++;
				SD = IntToHex(tmp, 6);
				mskedtBurnMac->Text = SD;
			}
		}else{
			// close Auto ......
			autoClose = true;
		}
	}
}
// ---------------------------------------------------------------------------
bool TUSBHIDForm::GetDutInfomation(){

	if(cmbNumOfChip->ItemIndex == 0){
		MB_WARNING(Handle, DUT_STRING_NOT_SET_KIND);
		lbNumberOfChip->Font->Color = clRed;
		return false;
	}else{
		lbNumberOfChip->Font->Color = clBlack;
	}

	m_dut.SetKind(cmbNumOfChip->ItemIndex);

	if(!m_dut.GetId()){
		//MB_WARNING(Handle, MB_FAILED_GET_DUT_ID);
		lbNumberOfChip->Font->Color = clRed;
		return false;
	}else{
		lbNumberOfChip->Font->Color = clBlack;
	}

	if(!m_dut.GetInfo()){
		memoMsg->Lines->Add(DUT_STRING_NOT_FIND);
		//MB_WARNING(Handle, MB_FAILED_GET_DUT_INFORMATION);
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

	return true;
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::mskedtBeginMacChange(TObject *Sender)
{
	const int maxCount = 0xffffff;


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


void __fastcall TUSBHIDForm::btnGetInfoClick(TObject *Sender)
{
	if(GetDutInfo())
	{
		Test_Time = GetTickCount();
		UnicodeString min_fw,backup_fw;
		min_fw = m_dut._If1.MinFwVersion;
		backup_fw = m_dut._If1.BackFirmware;
		plBackupFW->Caption =backup_fw;
		plMinFW->Caption =min_fw;
		//2018/12/12 for DS-UCB5 強制更新FW
		/*if(min_fw == edtMinFW->Text && backup_fw == edtBackupFW->Text)
		{
			plMsg->Font->Size = 14;
			plMsg->Caption = "Firmware is already the latest Version";
			//ProgressBar->ForeColor=clRed;
			ProgressBar->Progress = 100;
			ProgressBar->ForeColor= clLime;
			memoMsg->Lines->Add(GetTickCount()-Test_Time);
		}
		else
		{*/
			autoSTEP = 0;
			ProgressBar->ForeColor = clTeal;
			Timer2->Enabled = true;
		//}
	}
	else
		ProgressBar->ForeColor=clRed;
}
bool TUSBHIDForm::GetDutInfo()
{
	plMsg->Caption = "Get Device information...";
	bool bResult = true;
	if(bWin10)
		if(!CheckWin10Driver()) bResult = false;
	if(bResult)
	{
		G_bCheckDevInfo = true;
		memoMsg->Clear();
		memoMsg->Refresh();

		btnGetInfo->Enabled = false;

		if(!GetDutInfomation()){
			memoMsg->Clear();
			memoMsg->Refresh();
			if(bWin10)
			{
				if(!CheckWin10Driver())
				{
					ProgressBar->Progress -= 15;
					memoMsg->Lines->Add("Get Device information Fail,plugin device again");
					//MB_WARNING(Handle, MB_FAILED_GET_DUT_INFORMATION);
					plMsg->Caption = "(!)Load Data Fail";
					bResult = false;
				}
				else
				{
					if(!GetDutInfomation())
					{
						memoMsg->Lines->Add("Get Device information Fail,plugin device again");
						//MB_WARNING(Handle, MB_FAILED_GET_DUT_INFORMATION);
						plMsg->Caption = "(!)Load Data Fail";
						bResult = false;
					}
					else bLOAD = true;
                }
			}
		}
		else bLOAD = true;

		G_bCheckDevInfo = false;
		btnGetInfo->Enabled = true;
	return bResult;
}
//---------------------------------------------------------------------------


void TUSBHIDForm::Delay(DWORD iMilliSeconds) // 原版delay time 用在thread裡面
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}
//---------------------------------------------------------------------------
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
	return false;
}



bool TUSBHIDForm::CheckWin10Driver()
{
	plMsg->Font->Size = 16;
	//列舉Display產品
	memoMsg->Clear();
	AnsiString Resultdata = DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
	char *pch = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	memoMsg->Lines->Add("Enum Display Chip...");
	while (pch != NULL)
	{
		memoMsg->Lines->Add(pch);
		if(strstr(pch,"MI_00")||strstr(pch,"MI_01"))
			NList->Add(pch);
		pch = strtok(NULL, "\r\n");
		if(strstr(AnsiString(memoMsg->Text).c_str(),"No matching devices found"))
		{
			plMsg->Caption = "No matching devices found...";
			delete NList;
			return false;
		}
	}
	if(NList->Count>2)
	{
		memoMsg->Lines->Add("Display Chip num Error");
		plMsg->Caption = "(!)Display Chip num Error";
		plMsg->Font->Color = clRed;
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
	plMsg->Caption = "USBIO Driver install...";
	USBHIDForm->Refresh();

	for(int x=0;x<NList->Count;x++)
	{
		ProgressBar->Progress += 10;
		AnsiString cmd = "devcon updateni usbio_driver\\DisplayLinkUSBIOMTT.inf \""+NList->Strings[x]+"\"";
		Resultdata = DosCommand(cmd);
		if(!strstr(Resultdata.c_str(),"Drivers installed successfully"))
		{
			memoMsg->Lines->Add(NList->Strings[x]);
			memoMsg->Lines->Add(Resultdata);
			plMsg->Caption = "(!)Driver installation failed";
			plMsg->Font->Color = clRed;
			return false;
		}
	}
	//重新抓裝置
	plMsg->Caption = "Restart Device...";
	memoMsg->Lines->Add("restart device...");
	USBHIDForm->Refresh();

	DosCommand("devcon restart \"\*usb\\vid_17e9\*\"");
	delete NList;

	//再次列舉Display產品
	memoMsg->Lines->Add("enum device...");
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
		ProgressBar->Progress += 5;
	if(bResult)
	{
		memoMsg->Lines->Add("USBIO driver installation Successful");
		memoMsg->Lines->Add("Load Device Info...");
		return true;
	}
	else
	{
		memoMsg->Lines->Add("USBIO driver installation failed");
		plMsg->Caption = "(!)Driver installation failed";
		plMsg->Font->Color = clRed;
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
	if(!sCmdline.Pos("DisplayLinkUSBIOMTT.inf"))
	{
		DWORD dw = WaitForSingleObject(proc.hProcess, 300000);
		if(dw == WAIT_TIMEOUT)
		{
			::CloseHandle(proc.hProcess);
			::CloseHandle(proc.hThread);
			::CloseHandle(hReadPipe);
			return "";
		}
	}
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
void TUSBHIDForm::RemoveDisplayLinkDriver()
{
	btnAutoTest->Enabled = false;
	HKEY hSoftKey = NULL;
	AnsiString tempDeviceGUID;
	LONG lResult;
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\DisplayLink\\Core", 0, KEY_READ|KEY_WOW64_64KEY,&hSoftKey);
	if(lResult  == ERROR_SUCCESS)
	{
		DWORD dwType=REG_SZ;
		DWORD dwCount=0;
		LONG lResult = RegQueryValueEx(hSoftKey, "GUID", NULL, &dwType,NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			char* company_Get=new char[100];
			lResult = RegQueryValueEx(hSoftKey, "GUID", NULL, &dwType,
			company_Get, &dwCount);
			if(lResult != ERROR_SUCCESS)
				tempDeviceGUID = "";
			else
				tempDeviceGUID = (AnsiString((char*)company_Get));

			delete[] company_Get;
		}
	}

	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if(tempDeviceGUID!="")
	{
		FILE * fp;
		fp = fopen("uninstall.bat","w");
		fseek( fp, 0, SEEK_END);
		tempDeviceGUID = "MsiExec.exe /X"+tempDeviceGUID+" /norestart";
		fwrite(tempDeviceGUID.c_str(),tempDeviceGUID.Length(),1,fp); //寫入一筆資料
		fclose(fp);
		plMsg->Caption = "解除驅動...";
		DosCommand("uninstall.bat");
		plMsg->Caption = "Wait...";
	}
	AnsiString Resultdata = DosCommand("devcon dp_enum");
	AnsiString OemNum = "";
	char *pch = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	while (pch != NULL)
	{
		if(strstr(pch,"oem"))
			OemNum = AnsiString(pch);
		if(strstr(pch,"DisplayLink"))
			NList->Add(OemNum);
		pch = strtok(NULL, "\r\n");
	}
	for(int i=0;i<NList->Count;i++)
	{
		memoMsg->Lines->Add(NList->Strings[i]);
		OemNum = NList->Strings[i];
		OemNum = OemNum.SubString(OemNum.Pos("oem"),OemNum.Length()-OemNum.Pos("oem")+1);
		DosCommand("devcon -f dp_delete "+OemNum);
	}
	delete NList;
	btnAutoTest->Enabled = true;
}


void __fastcall TUSBHIDForm::Timer1Timer(TObject *Sender)
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
				ProgressBar->Progress = 0;
				ProgressBar->ForeColor = clTeal;
				plBackupFW->Caption = "";
				plMinFW->Caption = "";
				plBackupFW->Color = clWhite;
				plMinFW->Color = clWhite;
				plMsg->Caption = "No matching devices found...";
				plMsg->Font->Color = clBlue;
				plMsg->Font->Size = 16;
				lblStatus->Caption = "等候中";
				lblStatus->Color = clSkyBlue;
				memoMsg->Text = "No matching devices found";
				bPlugIn = false;
				break;
			}
			pch = strtok(NULL, "\r\n");
		}
		if(NList->Count>2)
		{
			memoMsg->Lines->Add("Display Chip num error");
			delete NList;
		}
		else if(NList->Count!=0)
		{
			delete NList;
			bStart = false;
			if(!bPlugIn) btnGetInfoClick(NULL);
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

void __fastcall TUSBHIDForm::plSwitchClick(TObject *Sender)
{
	if(plSwitch->Caption != "V")
	{
		plSwitch->Caption = "V";
		USBHIDForm->Height = 241;
	}
	else
	{
		plSwitch->Caption = "︿";
		USBHIDForm->Height = 601;
	}
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::FormShow(TObject *Sender)
{
	//
	Caption = APP_TITLE;
	cmbNumOfChip->ItemIndex = 0;
	if(FileExists("loging.log")){
		DeleteFile("loging.log");
	}

	//檢測windows版本
	OSVERSIONINFO osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	int windowsVer = osvi.dwMajorVersion *10 + osvi.dwMinorVersion;
	if(windowsVer == 62){
		bWin10 = true;
		DWORD dwFlags = 0;
		if(InternetCheckConnection("http://google.com",FLAG_ICC_FORCE_CONNECTION,dwFlags))
		{
			RemoveDisplayLinkDriver();
			MessageBox(NULL, "此程式需將網際網路關閉!!\r\n請關閉網路或拔除網路線再開啟程式", "警告訊息", MB_ICONEXCLAMATION);
			PostQuitMessage(WM_QUIT);
		}
	}
}
//---------------------------------------------------------------------------
