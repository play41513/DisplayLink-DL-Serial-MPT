:: This is an example batch file for running the NIVO test scripts.
:: It takes an optional device string

@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

set dev=
if "%~1" NEQ "" set dev=--dev %1

:: The test program name
set test_prog="dl_mfg_test.exe"

:: The serial numbers log file.  Serial numbers must start at 100000 or higher.
set serial_nos_file="C:\test\serialnos.log"
:: The MAC address log file. MAC address in xx:xx:xx:xx:xx:xx format.
set mac_addresses_file="C:\test\macaddrs.log"

:: Print out the manufacturing tests version number.
%test_prog% --version

echo.

::Stop the DisplayLink Service, hiding any output.
net stop DisplayLinkService 2>&1 | FIND "blah"

:: Do the tests

call example_DL3xxx_DL5xxx_tests.bat %1
if %ERRORLEVEL% GTR 0 (
  exit /b 1
)

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
  set /p yesno="No serial number log file found - create a new one? (y/n)"
  if !yesno!.==n. (
    echo Exiting...
    exit /b 1
  )
  :: Default serial number
  set /a serial_number=100001
  set /p serial_number="Enter the new serial number (6 numbers):"
)

:: Read the last MAC address from the file:
echo %mac_addresses_file%
if exist %mac_addresses_file% (
  for /f "usebackq tokens=1-6 delims=:" %%1 in (
    %mac_addresses_file%
  ) do (
    set "mac1=%%1"
    set "mac2=%%2"
    set "mac3=%%3"
    set /a mac4 = 0x%%4
    set /a mac5 = 0x%%5
    set /a mac6 = 0x%%6
  )
  
   set /a dec_mac_addr = !mac4!*256*256+!mac5!*256+!mac6!
   
   if !dec_mac_addr! GTR 16777214 (
     echo Max MAC address reached!
     echo Exiting...
     exit /b 1
   )
   
   set /a dec_mac_addr+=1
   call:toHex !dec_mac_addr! hex_mac_addr
   
   set "mac_address=!mac1!:!mac2!:!mac3!:!hex_mac_addr:~2,2!:!hex_mac_addr:~4,2!:!hex_mac_addr:~6,2!"
) else (
  set yesno=y
  set /p yesno="No MAC address log file found - create a new one? (y/n)"
  if !yesno!.==n. (
    echo Exiting...
    exit /b 1
  )
  :: Default MAC address
  set mac_address=00:00:00:00:00:01
  set /p mac_address="Enter the new MAC address (nn:nn:nn:nn:nn:nn):"
)

:: Program serial number and MAC address
::set /p serial_number="Enter the serial number:"
::set /p mac_address="Enter the MAC address:"

echo. & echo New serial number: %serial_number%
echo. & echo New MAC address: %mac_address%
echo. & echo Programming...
%test_prog% upd -s %serial_number% -a %mac_address% %dev%
if %ERRORLEVEL% GTR 0 (
  echo Programming FAILED.
  exit /b 1
) else (
  echo Programming passed.
)

echo.

:: write the serial number back to the file.
if not exist c:\test (
   mkdir c:\test
)
echo %serial_number% >> %serial_nos_file%

:: write the MAC address back to the file.
if not exist c:\test (
   mkdir c:\test
)
echo %mac_address% >> %mac_addresses_file%

echo.

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
