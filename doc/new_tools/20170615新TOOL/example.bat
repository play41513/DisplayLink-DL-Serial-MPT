:: This is an example batch file for running the Manufacturing Test Tool

@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

:: The test program name
set test_prog="dl_mfg_test.exe"

:: The serial numbers log file.  Serial numbers must start at 100000 or higher.
set serial_nos_file="C:\test\serialnos.log"

:: Print out the manufacturing tests version number.
%test_prog% --version

::Stop the DisplayLink Service, hiding any output.
net stop DisplayLinkService 2>&1 | FIND "blah"

:beginning

:: Do the tests

:: Video tests
echo.
echo Hit the Enter key to pass a test.
echo.

%test_prog% video cr r
set /p yesno="Red Colour Ramp OK? (Y/N)"
if %yesno%.==n. (
  echo Exiting...
  exit /b
)

%test_prog% video cr g
set /p yesno="Green Colour Ramp OK? (Y/N)"
if %yesno%.==n. (

  echo Exiting...
  exit /b
)

%test_prog% video cr b
set /p yesno="Blue Colour Ramp OK? (Y/N)"
if %yesno%.==n. (
  echo Exiting...
  exit /b
)

%test_prog% video gr 1
set /p yesno="Grey Ramp 1 OK? (Y/N)"
if %yesno%.==n. (
  echo Exiting...
  exit /b
)

%test_prog% video bw 1
set /p yesno="Black/White bars OK? (Y/N)"
if %yesno%.==n. (
  echo Exiting...
  exit /b
)

echo Displaying bitmap...
%test_prog% video black
%test_prog% video bitmap leaves.bmp -m 1024 768 60
set /p yesno="Bitmap OK? (Y/N)"
if %yesno%.==n. (
  echo Exiting...
  exit /b
)
%test_prog% video black

:: Read the last serial number from the file:
echo %serial_nos_file%
if exist %serial_nos_file% (
  for /f "usebackq tokens=*" %%* in (
    %serial_nos_file%
  ) do (
   set /a serial_number = %%*
  )

  :: add one to the number:
  set /a serial_number+=1
  set serial_number=00000!serial_number!
  set serial_number=!serial_number:~-6!
) else (
  set yesno=y
  set /p yesno="No serial number log file found - create a new one? (Y/N)"
  if !yesno!.==n. (
    echo Exiting...
    exit /b
  )
  :: Default serial number
  set /a serial_number=100001
  set /p serial_number="Enter the new serial number (6 numbers):"
)

:: Program serial number
::set /p serial_number="Enter the serial number:"

echo. & echo New serial number: %serial_number%
echo. & echo Programming...
%test_prog% upd -s %serial_number%
if %ERRORLEVEL% GTR 0 (
  echo Programming FAILED.
  exit /b
) else (
  echo Programming passed.
)

echo.

:: write the serial number back to the file.
if not exist c:\test (
   mkdir c:\test
)
echo %serial_number% >> %serial_nos_file%

set /p loop="Test another board? (Y/N)"
if /i "%loop%." NEQ "n." (
  goto :beginning
)

goto :end

::--------------------------------------------------------
::-- Function section starts below here
::--------------------------------------------------------

:toHex dec hex -- convert a decimal number to hexadecimal, i.e. -20 to FFFFFFEC or 26 to 0000001A
::             -- dec [in]      - decimal number to convert
::             -- hex [out,opt] - variable to store the converted hexadecimal number in
::Thanks to 'dbenham' dostips forum users who inspired to improve this function
:$created 20091203 :$changed 20110330 :$categories Arithmetic,Encoding
:$source http://www.dostips.com
SETLOCAL ENABLEDELAYEDEXPANSION
set /a dec=%~1
set "hex="
set "map=0123456789ABCDEF"
for /L %%N in (1,1,8) do (
    set /a "d=dec&15,dec>>=4"
    for %%D in (!d!) do set "hex=!map:~%%D,1!!hex!"
)
rem !!!! REMOVE LEADING ZEROS by activating the next line, e.g. will return 1A instead of 0000001A
rem for /f "tokens=* delims=0" %%A in ("%hex%") do set "hex=%%A"&if not defined hex set "hex=0"
( ENDLOCAL & REM RETURN VALUES
    IF "%~2" NEQ "" (SET %~2=%hex%) ELSE ECHO.%hex%
)
EXIT /b

:end

ENDLOCAL
