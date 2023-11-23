:: This is an example batch file for running the example script on all devices

@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

:: The test program name
set test_prog="dl_mfg_test.exe"

rem set search_3900=DL3900

:beginning

echo.
echo =====================
echo.Connected Devices:
%test_prog% --dev list
echo =====================
echo.

for /F %%s in ('%test_prog% --dev list') do (

:: Example code for calling SKU specific batch file
rem set var=%%s
rem set var|findstr %search_3900% >nul
rem if not errorlevel 1 (
rem   echo found %search_3900%
rem   call example.bat %%s
rem )

call example_DL3xxx_DL5xxx.bat %%s

echo.
echo.
)

set /p loop="Test more boards? (y/n)"
if /i "%loop%." NEQ "n." (
  goto :beginning
)