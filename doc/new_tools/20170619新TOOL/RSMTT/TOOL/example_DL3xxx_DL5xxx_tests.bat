:: This is an example batch file for running the NIVO test scripts.

@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

echo =====================
echo Test run: %1
echo =====================


set dev=
if "%~1" NEQ "" set dev=--dev %1

:: The test program name
set test_prog="dl_mfg_test.exe"

::Stop the DisplayLink Service, hiding any output.
net stop DisplayLinkService 2>&1 | FIND "blah"

echo.

:: Do the tests
:: Power-On Self Test results
%test_prog% post %dev%
if %ERRORLEVEL% GTR 0 (
  exit /b 1
)

echo.

:: DDR tests
%test_prog% ddr data %dev%
if %ERRORLEVEL% GTR 0 (
  exit /b 1
)

%test_prog% ddr address 8 %dev%
if %ERRORLEVEL% GTR 0 (
  exit /b 1
)

:: Video tests
%test_prog% video map 0 0 %dev%
%test_prog% video map 1 0 %dev%
%test_prog% video power 0 on %dev%
echo.
echo Hit the Enter key to pass a test.
echo.

%test_prog% video cr r -m 1024 768 60 %dev%
set /p yesno="Red Colour Ramp OK? (y/n)"
if %yesno%.==n. (
  echo Exiting tests...
  exit /b 1
)

%test_prog% video cr g %dev%
set /p yesno="Green Colour Ramp OK? (y/n)"
if %yesno%.==n. (
  echo Exiting tests...
  exit /b 1
)

%test_prog% video cr b %dev%
set /p yesno="Blue Colour Ramp OK? (y/n)"
if %yesno%.==n. (
  echo Exiting tests...
  exit /b 1
)

%test_prog% video gr 1 %dev%
set /p yesno="Grey Ramp 1 OK? (y/n)"
if %yesno%.==n. (
  echo Exiting tests...
  exit /b 1
)

%test_prog% video bw 1 %dev%
set /p yesno="Black/White bars OK? (y/n)"
if %yesno%.==n. (
  echo Exiting tests...
  exit /b 1
)

echo Displaying bitmap...
%test_prog% video black %dev%
%test_prog% video bitmap leaves.bmp -m 1024 768 60 %dev%
set /p yesno="Bitmap OK? (y/n)"
if %yesno%.==n. (
  echo Exiting tests...
  exit /b 1
)
%test_prog% video black %dev%

ENDLOCAL
