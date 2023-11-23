:: This batch file substitutes the USBIO drivers for the IDD drivers to allow the Manufacturing Test Tool to be
:: used on a Windows Redstone platform. Requires the 8.2 M1 version of the USBIO drivers (Cognidox NR-134568-LS) 
:: Copyright (c) 2017 DisplayLink (UK) Ltd.
@echo off

:: These parameters need to be modified to reflect the location of these tools on your PC
set USBIODRIVER=usbio_driver\DisplayLinkUSBIOMTT.inf
set MFG_TEST_TOOL=..\dl_mfg_test.exe
set DEVCONTOOL=devcon.exe

set VID=17E9
set DRIVERNAME="DisplayLink USBIO and MTT"

if NOT exist %USBIODRIVER% (@echo. & @echo Can't find the USBIO driver %USBIODRIVER%. Please download it from Cognidox, extract it to a directory and set the USBIODRIVER variable in this batch file. & @echo. & goto :EOF)
if NOT exist %MFG_TEST_TOOL% (@echo. & @echo Can't find the MTT tool %MFG_TEST_TOOL%. Please set the MFG_TEST_TOOL variable in this batch file. & @echo. & goto :EOF)
if NOT exist %DEVCONTOOL% (@echo. & @echo Can't find the devcon tool %DEVCONTOOL%. Please download the WDK from https://developer.microsoft.com/en-us/windows/hardware/windows-driver-kit and set the DEVCONTOOL variable in this batch file. & @echo. &  goto :EOF)

if NOT "%1%"=="" (
		   set PID=%1%
		   goto checkdrivers)

:: --------------------------------------------------------------------------------------
:: No command line parameters supplied - show usage and exit
:: --------------------------------------------------------------------------------------
:usage
:: No command line parameters - just list the DisplayLink devices that are present
rem @echo DisplayLink USBIO driver switching tool.
rem @echo This script is designed to put a board into MTT mode from a PC running Windows Redstone, by installing the USBIO drivers.
rem @echo To put a board into MTT mode, run
rem @echo %0% "<board product ID (PID)>"
rem @echo.
rem @echo Note that the PID is case-sensitive
@echo.
@echo IDD boards detected:
%DEVCONTOOL% find =Display "USB\VID_%VID%*" | find "USB"
@echo.
@echo USBIO boards detected:
%DEVCONTOOL% find ="USB Display Adapters" "USB\VID_%VID%*"  | find "USB"
@echo.

@echo.
set /p PID=Please input the PID : 
if /i "%PID%." NEQ "." ( goto checkdrivers )

cls
goto usage

rem goto :EOF

:: --------------------------------------------------------------------------------------
:: Subroutine to install the USBIO drivers
:: --------------------------------------------------------------------------------------
:installdrivers
  :: List all the devices that match the VID, PID and interface number that AREN'T currently using the USBIO driver.
  :: Doing this check reduces the execution time.
rem   %DEVCONTOOL% find "USB\VID_%VID%&PID_%PID%&MI_00" > tmpFile
rem   findstr /c:%DRIVERNAME% tmpFile
rem   if "%errorlevel%"=="0" (exit /b)

  %DEVCONTOOL% updateni %USBIODRIVER% "USB\VID_%VID%&PID_%PID%&MI_00" >NUL
  %DEVCONTOOL% updateni %USBIODRIVER% "USB\VID_%VID%&PID_%PID%&MI_01" >NUL

  %DEVCONTOOL% restart "USB\VID_%VID%&PID_%PID%" >NUL

  exit /b
  
:: --------------------------------------------------------------------------------------
:: Install the USBIO drivers for the device with the given PID
:: --------------------------------------------------------------------------------------
:checkdrivers

:: See if the PID that was entered matches a board that we can see
%DEVCONTOOL% find "USB\VID_%VID%&PID_%PID%&MI_00" > tmpFile
findstr /c:"USB\VID_%VID%&PID_%PID%&MI_00" tmpFile
if "%errorlevel%"=="1" (@echo ERROR - No board found with PID %PID% & echo. & goto usage)

@echo.
@echo Installing USBIO Driver for selected device.......
call :installdrivers
@echo.

@echo Starting MTT mode
:: List all the devices found and put them all into MTT mode
%MFG_TEST_TOOL% --dev list > tmpFile
for /F %%i in (tmpFile) do %MFG_TEST_TOOL% --dev %%i mtt
del tmpFile
timeout /t 5 /NOBREAK >NUL
timeout 5

:: --------------------------------------------------------------------------------------
:: NOW WE SHOULD BE IN MTT MODE - re-associate the drivers with the new device instances
:: --------------------------------------------------------------------------------------
call :installdrivers

:: Run some test MTT commands, on all devices in MTT mode
%MFG_TEST_TOOL% --dev list > tmpFile
for /F %%i in (tmpFile) do @echo. & @echo ************************************* & @echo DEVICE %%i & @echo. & %MFG_TEST_TOOL% --dev %%i info flash & %MFG_TEST_TOOL% --dev %%i info otp

del tmpFile

@echo.
@echo.

