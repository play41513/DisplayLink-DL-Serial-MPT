//---------------------------------------------------------------------------
#ifndef Uint1H
#define Uint1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <MPlayer.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <jpeg.hpp>
#include <Db.hpp>
#include <DBTables.hpp>
#include "Trayicon.h"
#include <Mask.hpp>

//#include "HidInfoSet.h"
#include "SetupDi.h"
#include "HID.h"
#include "DeviceNotify.h"
#include <vector>
#include "USB_Hub_Tree.h"
//---------------------------------------------------------------------------
using namespace std;

#define CALLDLL extern "C" __declspec(dllexport)
CALLDLL int  A_Get_DLL_Version(int nShowMessage);
CALLDLL int  A_GetUSBBufferLen(void);
CALLDLL int  A_EnumUSB(char *buf);
CALLDLL int  A_GetUSBDeviceInfo(int nPort,char *pDeviceName,int *pDeviceNameLen,char* pDevicePath,int *pDevicePathLen);
CALLDLL int  A_CreatePrn(int selection,AnsiString filename);
CALLDLL void A_ClosePrn(void);

CALLDLL int  A_Set_Unit(char unit_Renamed);
CALLDLL int  A_Set_Syssetting(int transfer,int cut_peel,int length,int zero,int pause);

CALLDLL int  A_Set_Darkness(unsigned int heat);
CALLDLL int  A_Del_Graphic(int mem_mode,char *graphic);
CALLDLL void A_Clear_Memory(void);
CALLDLL int  A_Set_Backfeed(int back);
CALLDLL int  A_Prn_Text(int x,int y,int ori,int font,int type_Renamed,int hor_factor,int ver_factor,char mode,int numeric,char *data);
CALLDLL int  A_Print_Out(int width,int height,int copies,int amount);

//---------------------------------------------------------------------------
class TReadHIDThread : public TThread
{
public:

private:
};
//---------------------------------------------------------------------------
class TUSBHIDForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TGroupBox *GroupBox5;
	TComboBox *ckbDutMaxTimeGetId;
	TPanel *Panel12;
	TPanel *Panel6;
	TPanel *Panel7;
	TPanel *Panel8;
	TComboBox *cmbNumOfChip;
	TPanel *Panel22;
	TPanel *plChipName;
	TPanel *Panel11;
	TPanel *Panel10;
	TPanel *plUSBSN;
	TPanel *Panel19;
	TPanel *Panel20;
	TPanel *plMAC;
	TPanel *Panel13;
	TPanel *Panel14;
	TPanel *plMinFWVer;
	TPanel *Panel16;
	TPanel *Panel17;
	TPanel *plBackupFW;
	TPanel *Panel9;
	TBitBtn *btnStart;
	TPanel *plSwitch;
	TPanel *Panel18;
	TPanel *Panel21;
	TPanel *Panel23;
	TPanel *plMAC2;
	TPanel *Panel25;
	TPanel *Panel26;
	TPanel *plUSBSN2;
	TPanel *plChipName2;
	TPanel *Panel29;
	TPanel *Panel30;
	TPanel *plBackupFW2;
	TPanel *Panel32;
	TPanel *Panel33;
	TPanel *plMinFWVer2;
	TCheckBox *ckbWin10;
	TCheckBox *ckb2k;
	TCheckBox *ckb4k30;
	TCheckBox *ckb4k60;
	TButton *btnGetInfo4K;
	TPanel *Panel15;
	TMemo *memoMsg;
	TBitBtn *BitBtn1;
	TTimer *Timer5;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ckbDutMaxTimeGetIdChange(TObject *Sender);
	void __fastcall btnGetInfo4KClick(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall Timer5Timer(TObject *Sender);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall plSwitchClick(TObject *Sender);
	void __fastcall ckbWin10Click(TObject *Sender);
	void __fastcall ckb2kClick(TObject *Sender);
	void __fastcall ckb4k30Click(TObject *Sender);
	void __fastcall ckb4k60Click(TObject *Sender);
private:	// User declarations

	DUT m_dut;

	void Delay(int iMilliSeconds);
	bool CallCommand_C(AnsiString Command_line);
	bool CheckWin10Driver();
	AnsiString DosCommand(AnsiString sCmdline);

protected:
	bool GetDutInfomation(USB_Hub_Tree &hubTree);
public:		// User declarations
	__fastcall TUSBHIDForm(TComponent* Owner);

	int  __fastcall HexToInt(AnsiString HexStr);
	bool __fastcall TUSBHIDForm::VerifyInfoFlash();
};
//---------------------------------------------------------------------------
extern PACKAGE TUSBHIDForm *USBHIDForm;
//---------------------------------------------------------------------------
#endif

