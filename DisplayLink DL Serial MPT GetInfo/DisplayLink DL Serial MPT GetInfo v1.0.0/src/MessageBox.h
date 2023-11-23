//---------------------------------------------------------------------------

#ifndef MessageBoxH
#define MessageBoxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmMsg : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel4;
	TPanel *plPASS_OK;
	TPanel *Panel2;
	TPanel *Panel1;
	TPanel *Panel6;
	TPanel *Panel7;
	TPanel *plError3_3;
	TPanel *plError3_1;
	TPanel *plError3_2;
	TPanel *Panel33;
	TPanel *plError2_1;
	TPanel *plError2_2;
	TPanel *Panel36;
	TPanel *plError1_2;
	TPanel *plError1_1;
	TPanel *plError7;
	TPanel *Panel17;
	TPanel *plError8_1;
	TPanel *plError8_2;
	TPanel *Panel8;
	TPanel *Panel5;
	TPanel *Panel10;
	TPanel *plError4_3;
	TPanel *plError4_1;
	TPanel *plError4_2;
	TPanel *Panel42;
	TPanel *plError5_3;
	TPanel *plError5_1;
	TPanel *plError5_2;
	TPanel *Panel46;
	TPanel *plError6_3;
	TPanel *plError6_1;
	TPanel *plError6_2;
	TPanel *plError9_2;
	TPanel *plError9_1;
	void __fastcall plPASS_OKClick(TObject *Sender);
	void __fastcall plError1_1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmMsg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
#endif
