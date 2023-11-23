//---------------------------------------------------------------------------

#include <vcl.h>
#include <tchar.h>
#include "ConstantString.h"
#pragma hdrstop
USEFORM("Uint1.cpp", USBHIDForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
HANDLE ASMC_USBPHONE_AP_HANDLE;

HWND hwnd = NULL;
hwnd = FindWindowA(NULL,APP_TITLE);
if(hwnd)
{ //�{���w����...
	MessageBoxA(NULL,"Application is already running","WARNING", MB_ICONEXCLAMATION);
 	return 0;
}

try
    {
    /*
    const char ASMC_USBPHONE_AP_NAME[]="ASMCUSBPHONEAP";
    ASMC_USBPHONE_AP_HANDLE=OpenMutex(0,false,ASMC_USBPHONE_AP_NAME);
    if (ASMC_USBPHONE_AP_HANDLE == NULL)
				ASMC_USBPHONE_AP_HANDLE=CreateMutex(NULL,true,ASMC_USBPHONE_AP_NAME);
    else
        return 0;
    */
    Application->Initialize();
    Application->CreateForm(__classid(TUSBHIDForm), &USBHIDForm);
		Application->Run();
    }
catch (Exception &exception)
    {
    Application->ShowException(&exception);
    }
//ReleaseMutex(ASMC_USBPHONE_AP_HANDLE);
return 0;
}
//---------------------------------------------------------------------------

