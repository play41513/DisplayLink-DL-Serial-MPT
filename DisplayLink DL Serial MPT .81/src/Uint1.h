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
	TGroupBox *gbTestItem;
	TCheckBox *ckbCheckerboardTest;
	TCheckBox *ckbAddressBusTest;
	TCheckBox *ckbDataBusTest;
	TCheckBox *ckbVideo1080p;
	TCheckBox *ckbEthernetLoopbackTest;
	TCheckBox *ckbByteCountDataTest;
	TCheckBox *ckbAudio;
	TGroupBox *gbBurnSetting;
	TLabel *Label2;
	TCheckBox *ckbAutoIncrease;
	TGroupBox *gbRework;
	TGroupBox *gbTest;
	TButton *btnAutoTest;
	TGroupBox *gbUSBHUB;
	TButton *btnSetting;
	TMaskEdit *mskedtEndSn;
	TMaskEdit *mskedtMacPre;
	TMaskEdit *mskedtEndMac;
	TTimer *Timer2;
	TMaskEdit *MaskEdit9;
	TPanel *Panel1;
	TMemo *memoMsg;
	TLabel *lblStatus;
	TButton *btnGetInfo;
	TMemo *moMACRecord;
	TCheckBox *ckbBurn;
	TButton *btnPrint;
	TTimer *Timer3;
	TButton *btnLoadDutVersion;
	TLabel *Label14;
	TLabel *Label15;
	TEdit *edtBurnCount;
	TMaskEdit *mskedtBeginSn;
	TMaskEdit *mskedtBeginMac;
	TLabel *Label17;
	TCheckBox *ckbVerifyUsbHubVersion;
	TLabel *Label20;
	TLabel *Label21;
	TCheckBox *ckbVerifyDisplayLinkVersion;
	TCheckBox *ckbVideo4k;
	TCheckBox *ckbRework;
	TGroupBox *gbBurnContent;
	TLabel *Label1;
	TLabel *Label16;
	TMaskEdit *mskedtBurnSn;
	TMaskEdit *mskedBurntMacPre;
	TLabel *Label22;
	TMaskEdit *mskedtBurnMac;
	TCheckBox *ckbBurnVerify;
	TCheckBox *ckbAutoPrint;
	TButton *btnRework;
	TCheckBox *ckbMtsMode;
	TLabel *lbNumberOfChip;
	TLabel *Label4;
	TComboBox *ckbDutMaxTimeGetId;
	TComboBox *cmbNumOfChip;
	TPanel *Panel2;
	TLabel *Label19;
	TLabel *Label8;
	TEdit *edtBackupFirmwareSample;
	TLabel *Label18;
	TTreeView *trvReference;
	TPanel *Panel3;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *lblChipID1;
	TLabel *lblChipID2;
	TEdit *edtMinFwVersionSample;
	TEdit *edtMinFwVersionSample2;
	TEdit *edtBackupFirmwareSample2;
	TCheckBox *ckbVideo4k601;
	TCheckBox *ckbVideo4k602;
	TEdit *Wav_Name_Edit;
	TComboBox *cb_myOutDevice;
	TComboBox *cb_myInDevice;
	TComboBox *cbOutputDevice;
	TComboBox *cbInputDevice;
	TPanel *Panel4;
	TPanel *Panel5;
	TButton *btn_close_audio;
	TScrollBar *sb_value_audio;
	TEdit *edt_value_audio;
	TImage *Wave_View;
	TButton *btnGetInfo4K;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnAutoTestClick(TObject *Sender);
	void __fastcall btnSettingClick(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);
	void __fastcall btnPrintClick(TObject *Sender);
	void __fastcall Timer3Timer(TObject *Sender);
	void __fastcall mskedtBeginMacChange(TObject *Sender);
	void __fastcall edtBurnCountChange(TObject *Sender);
	void __fastcall mskedtBeginSnChange(TObject *Sender);
	void __fastcall mskedtMacPreChange(TObject *Sender);
	void __fastcall ckbReworkClick(TObject *Sender);
	void __fastcall ckbAutoIncreaseClick(TObject *Sender);
	void __fastcall mskedtBurnMacExit(TObject *Sender);
	void __fastcall mskedBurntMacPreExit(TObject *Sender);
	void __fastcall btnReworkClick(TObject *Sender);
	void __fastcall ckbMtsModeClick(TObject *Sender);
	void __fastcall MaskEdit9Change(TObject *Sender);
	void __fastcall btnGetInfoClick(TObject *Sender);
	void __fastcall btnLoadDutVersionClick(TObject *Sender);
	void __fastcall ckbDutMaxTimeGetIdChange(TObject *Sender);
	void __fastcall ckbVideo4k601Click(TObject *Sender);
	void __fastcall memoMsgMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall sb_value_audioChange(TObject *Sender);
	void __fastcall btn_close_audioClick(TObject *Sender);
	void __fastcall btnGetInfo4KClick(TObject *Sender);
	void __fastcall edt_value_audioChange(TObject *Sender);
private:	// User declarations
	int getKindCnt;
	DUT m_dut;
//	String m_hwid[14];
	bool m_bAutoIncrease; //�N�����\
	bool m_bMts[5];
	USB_Hub_Tree refUsbHubTree;

	bool Xml2Tree(tinyxml2::XMLDocument *doc);
	bool ProcessXmlItem(tinyxml2::XMLElement *pParentNode, TTreeNode *pTreeNode);
	bool CompareUsbHubTree(USB_Hub_Tree &refTree, USB_Hub_Tree &tstTree);
	bool CompareTreeNode(tinyxml2::XMLElement *pRef, tinyxml2::XMLElement *pTst);
//	void __fastcall TUSBHIDForm::CountBurnRange();
	bool LoadDutInfomation();
	bool CompareInfoFlash(InfoFlash &di, String backupFirmware, String MinFwVersion, String &msg);
	bool VerifyUsbHubHardwareId();
	void ClearInformation();
	void PlayVoice(int tag);
	bool CheckData(int tag);
	void TestStart(int tag);
	void Delay(DWORD iMilliSeconds);
	void __fastcall WndProc(TMessage &Message);
	void __fastcall ProcessInput(void);
	void Oscilloscopes(short *idata);
	void iniCB();
	bool CallCommand_C(AnsiString Command_line);
	bool CheckWin10Driver();
	AnsiString DosCommand(AnsiString sCmdline);
	// WaveOut
	HWAVEOUT	    WaveOut; // ���W��X�y�`
	WAVEHDR		    WaveOutHeader;
	char*		    WaveOutData;
	WAVEFORMATEX    waveFormat;
	WAVEHDR         *WaveBuffers;
	int             WaveBufSize;
	HWAVEIN         hWaveIn;
	bool            bStart;
	short           *my_data;
protected:
	void AutoIncrease();  // �۰ʸ���
	bool GetUsbHubTree(USB_Hub_Tree &hubTree);
	bool DisplayHub(tinyxml2::XMLElement *pHub, int tier);
	bool GetDutInfomation(USB_Hub_Tree &hubTree);
public:		// User declarations
	__fastcall TUSBHIDForm(TComponent* Owner);

	int  __fastcall HexToInt(AnsiString HexStr);
	char __fastcall DUTT(void);
	char __fastcall DUTB(void);
	char __fastcall DUTV(void);
	bool __fastcall VerifyInfoFlash();

	void WaveFreeHeader(HGLOBAL &HData, HGLOBAL &HWaveHdr);
	void WaveRecordEnd(HWAVEIN hwi, LPWAVEHDR &lpWaveHdr);
	void WaveRecordClose(HWAVEIN hwi);
	void WavePlayEnd(HWAVEOUT hwo, LPWAVEHDR &lpWaveHdr);
	void WavePlayClose(HWAVEOUT hwo);
	bool AudioItemTest();
	void __fastcall MMWimData(TMessage &Msg);
	void __fastcall MMWomDone(TMessage &Msg);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(MM_WIM_DATA, TMessage, MMWimData)
	MESSAGE_HANDLER(MM_WOM_DONE, TMessage, MMWomDone)
	END_MESSAGE_MAP(TForm)

	bool FRecording;
	bool FPlaying;

	int len;
	HWAVEIN hwi;
	HWAVEOUT hwo;
	HGLOBAL hgData_I, hgData_O, hgWaveHdr_I, hgWaveHdr_O;
	LPSTR lpData_I, lpData_O;
	LPWAVEHDR lpWaveHdr_I, lpWaveHdr_O;

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
	AnsiString      CurrentUserHandle;                  //����skype�ϥΪ�
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