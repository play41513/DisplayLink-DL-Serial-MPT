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

#include "SetupDi.h"
#include "DeviceNotify.h"
#include "Gauges.hpp"
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
//---------------------------------------------------------------------------
class TUSBHIDForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *Timer2;
	TMemo *moMACRecord;
	TEdit *Wav_Name_Edit;
	TComboBox *cb_myOutDevice;
	TComboBox *cb_myInDevice;
	TComboBox *cbOutputDevice;
	TComboBox *cbInputDevice;
	TPanel *Panel2;
	TPanel *Panel1;
	TLabel *lblStatus;
	TMemo *memoMsg;
	TPanel *Panel3;
	TPanel *Panel4;
	TGauge *ProgressBar;
	TPanel *plMsg;
	TTimer *Timer1;
	TPanel *plSwitch;
	TPanel *Panel5;
	TGroupBox *gbBurnSetting;
	TLabel *Label2;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label17;
	TLabel *Label20;
	TLabel *Label21;
	TMaskEdit *mskedtEndSn;
	TCheckBox *ckbAutoIncrease;
	TMaskEdit *mskedtMacPre;
	TMaskEdit *mskedtEndMac;
	TCheckBox *ckbBurnSN;
	TEdit *edtBurnCount;
	TMaskEdit *mskedtBeginSn;
	TMaskEdit *mskedtBeginMac;
	TCheckBox *ckbBurnVerify;
	TCheckBox *ckbBurnMAC;
	TGroupBox *gbBurnContent;
	TLabel *Label1;
	TLabel *Label16;
	TLabel *Label22;
	TMaskEdit *mskedtBurnSn;
	TMaskEdit *mskedBurntMacPre;
	TMaskEdit *mskedtBurnMac;
	TGroupBox *gbUSBHUB;
	TLabel *lbNumberOfChip;
	TComboBox *cmbNumOfChip;
	TButton *btnGetInfo;
	TGroupBox *gbTest;
	TCheckBox *ckbCheckFW;
	TPanel *Panel6;
	TPanel *Panel7;
	TPanel *Panel8;
	TPanel *Panel9;
	TPanel *Panel10;
	TEdit *edtBackupFW;
	TEdit *edtMinFW;
	TPopupMenu *PopupMenu1;
	TPanel *Panel11;
	TButton *btnAutoTest;
	TPanel *Panel16;
	TPanel *Panel13;
	TPanel *Panel14;
	TPanel *plBackupFW;
	TPanel *Panel12;
	TPanel *Panel15;
	TPanel *plMinFW;
	TCheckBox *ckbFakeMAC;
	TPanel *Panel17;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnAutoTestClick(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);
	void __fastcall mskedtBeginMacChange(TObject *Sender);
	void __fastcall edtBurnCountChange(TObject *Sender);
	void __fastcall mskedtBeginSnChange(TObject *Sender);
	void __fastcall mskedtMacPreChange(TObject *Sender);
	void __fastcall mskedtBurnMacExit(TObject *Sender);
	void __fastcall mskedBurntMacPreExit(TObject *Sender);
	void __fastcall btnGetInfoClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall plSwitchClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	int getKindCnt;
	DUT m_dut;
	bool m_bAutoIncrease; //�N�����\

	void Delay(DWORD iMilliSeconds);
	bool CallCommand_C(AnsiString Command_line);
	bool CheckWin10Driver();
	bool GetDutInfo();
	AnsiString DosCommand(AnsiString sCmdline);
	void RemoveDisplayLinkDriver();
protected:
	void AutoIncrease();  // �۰ʸ���
	bool GetDutInfomation();
public:		// User declarations
	__fastcall TUSBHIDForm(TComponent* Owner);

	int  __fastcall HexToInt(AnsiString HexStr);
	char __fastcall DUTB(void);
	char __fastcall DUTV(void);

	//==============================================================

	AnsiString      SS;                                 //�@�Φr��
	bool            SkypeAPIAvail;
	char            PASS;                               //USB Phone����
	char            KIND;                               //USB Phone����
	char            MODE;                               //USB Phone�Ҧ�
	char            RING;                               //USB Phone�a�T
	char            RINGIMG;                            //USB Phone�a�T
	char            REC;                                //USB Phone����
	char            LCM_SKYPE_PAGE;                     //USB Phone����
	int             LCM_SKYPE_LIST_NO;                  //USB Phone����
	char            LINEST;                             //�W�u���p
	char            LINEST0;                            //�W�Ӭd�`�W�u���p
	char            SEARCHS;                            //�d�߶���
	char            CALLOUT;                            //�d�߶���
	char            CHANGEIMG;                          //�Ϲ���s
	unsigned char   pix[128][8];
	unsigned char   pix_tmp;
	int             i,j,k;
	int             L1,L2,L3;                           //���ƶq
	char            NUMMODE;                            //�Ʀr��J���A
	char            NUMSEND;                            //�Ʀr��J���X
	int             LX,LY;                              //LCM�B��
	AnsiString      CALLUSERNAME;                       //
	int             HID_COUNT;                          //HID RW COUNT
	char            Key_SN;
	int             Get_No_Key_Time;                    //�å����䦸�ƭp��
	unsigned int    OGKey,OGKno;                        //�W�������
	AnsiString      OTime;                              //�W��LCM�ɶ����
	char            Destination,Source;
	int             LVol,RVol,Mute,CtrlType;
	int             debug;
	HINSTANCE       g_hInstance;
	HANDLE          m_hMCIWnd;
	int             deviceID;
	AnsiString      FullName;
	char            Skype_ST;
	int             Skype_CallID;
	LPVOID          *CMLP;
};
//---------------------------------------------------------------------------
extern PACKAGE TUSBHIDForm *USBHIDForm;
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------