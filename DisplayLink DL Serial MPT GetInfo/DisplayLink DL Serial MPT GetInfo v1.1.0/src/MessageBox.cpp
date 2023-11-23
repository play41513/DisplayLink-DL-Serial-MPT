//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MessageBox.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
__fastcall TfrmMsg::TfrmMsg(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::plPASS_OKClick(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------



void __fastcall TfrmMsg::plError1_1Click(TObject *Sender)
{
	TPanel * plError = (TPanel*) Sender;
	frmMsg->Hint = AnsiString(plError->Tag);
	ModalResult = mbCancel;
}
//---------------------------------------------------------------------------

