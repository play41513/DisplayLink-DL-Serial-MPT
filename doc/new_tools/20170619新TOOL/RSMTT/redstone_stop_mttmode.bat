:: This batch file uninstalls the USBIO drivers. Its aim is to clean up after the install_usbio.bat script is used.
:: Copyright (c) 2017 DisplayLink (UK) Ltd.
@echo off

:: These parameters need to be modified to reflect the location of these tools on your PC
set MFG_TEST_TOOL=..\dl_mfg_test.exe
set DEVCONTOOL=devcon.exe

if NOT exist %MFG_TEST_TOOL% (@echo. & @echo Can't find the MTT tool %MFG_TEST_TOOL%. Please set the MFG_TEST_TOOL variable in this batch file. & @echo. & goto :EOF)
if NOT exist %DEVCONTOOL% (@echo. & @echo Can't find the devcon tool %DEVCONTOOL%. Please download the WDK from https://developer.microsoft.com/en-us/windows/hardware/windows-driver-kit and set the DEVCONTOOL variable in this batch file. & @echo. &  goto :EOF)

:: Find the name of the OEM driver package associated with the USBIO driver
%DEVCONTOOL% driverfiles ="USB Display Adapters" | find "Driver installed from" > %0.tmp
set /p driverfile= < %0.tmp
del %0.tmp

:: This is fairly fragile code - cuts out the "oemnn.inf" part of the output of the command above
:: using fairly strict knowledge of the format
set driverfile=%driverfile:    Driver installed from C:\WINDOWS\INF\=%
set driverfile=%driverfile:~0,9%

:: Find the hardware IDs of all the devices (including hidden ones) associated with the USBIO driver
%DEVCONTOOL% findall ="USB Display Adapters" | find "USB" > tmpfile.tmp
@echo.
@echo Rebooting and removing the devices.....

:: List all the device we can see and reboot them all
%MFG_TEST_TOOL% --dev list > mttDevList
for /F %%i in (mttDevList) do %MFG_TEST_TOOL% --dev %%i reboot
del mttDevList

:: Remove the devices (equivalent to "Uninstall device" in the device manager)
TIMEOUT /T 2 /NOBREAK >NUL
for /F %%i in (tmpfile.tmp) do %DEVCONTOOL% remove "@%%i" >NUL
del tmpfile.tmp

TIMEOUT /T 3 /NOBREAK >NUL
@echo.

:: Remove the driver package, to make sure that IDD drivers are always used for these devices
%DEVCONTOOL% rescan
TIMEOUT /T 3 /NOBREAK >NUL

timeout 4
rem echo Removing OEM driver package %driverfile%
%DEVCONTOOL% dp_delete %driverfile% >NUL