:: This is an example batch file for running the NIVO test scripts.

@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

:: The test program name
set test_prog="dl_mfg_test.exe"
set DL3951_config="DL3951-config.mem"
set DL3955_config="DL3955-config.mem"
set DL3950_firmware="DL3950-firmware.spkg"

set search_3951=DL3951
set search_3952=DL3952
set search_3955=DL3955
set search_3956=DL3956

:: The serial numbers log file.  Serial numbers must start at 100000 or higher.
set serial_nos_file="C:\test\serialnos.log"
:: The MAC address log file. MAC address in xx:xx:xx:xx:xx:xx format.
set mac_addresses_file="C:\test\macaddrs.log"

:: Print out the manufacturing tests version number.
%test_prog% --version

::Stop the DisplayLink Service, hiding any output.
net stop DisplayLinkService 2>&1 | FIND "blah"

echo.

::Program config
%test_prog% config %DL3951_config% %DL3955_config%
if %ERRORLEVEL% GTR 0 (
  echo Programming config FAILED.
  exit /b 1
) else (
  echo Programming config passed.
)

echo.

::Program firmware
%test_prog% program %DL3950_firmware%
if %ERRORLEVEL% GTR 0 (
  echo Programming firmware FAILED.
  exit /b 1
) else (
  echo Programming firmware passed.
)

echo.

::Program backup firmware
%test_prog% program backup %DL3950_firmware%
if %ERRORLEVEL% GTR 0 (
  echo Programming backup firmware FAILED.
  exit /b 1
) else (
  echo Programming backup firmware passed.
)

echo.

set dev_3951=0
set dev_3955=0

for /F %%s in ('%test_prog% --dev list') do (

set var=%%s

set var|findstr %search_3951% >nul
if not errorlevel 1 (
  set dev_3951=%%s
)

set var|findstr %search_3952% >nul
if not errorlevel 1 (
  set dev_3951=%%s
)

set var|findstr %search_3955% >nul
if not errorlevel 1 (
  set dev_3955=%%s
)

set var|findstr %search_3956% >nul
if not errorlevel 1 (
  set dev_3955=%%s
)

)

rem echo.3951: %dev_3951%
rem echo.3955: %dev_3955%

echo.

:: Do the tests
call example_DL3xxx_DL5xxx_tests.bat %dev_3951%
if %ERRORLEVEL% GTR 0 (
  exit /b 1
)

echo.

call example_DL3xxx_DL5xxx_tests.bat %dev_3955%
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
    exit /b
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
     exit /b
   )
   
   set /a dec_mac_addr+=1
   call:toHex !dec_mac_addr! hex_mac_addr
   
   set "mac_address=!mac1!:!mac2!:!mac3!:!hex_mac_addr:~2,2!:!hex_mac_addr:~4,2!:!hex_mac_addr:~6,2!"
) else (
  set yesno=y
  set /p yesno="No MAC address log file found - create a new one? (y/n)"
  if !yesno!.==n. (
    echo Exiting...
    exit /b
  )
  :: Default MAC address
  set mac_address=00:00:00:00:00:01
  set /p mac_address="Enter the new MAC address (nn:nn:nn:nn:nn:nn:nn):"
)

:: Program serial number and MAC address
::set /p serial_number="Enter the serial number:"
::set /p mac_address="Enter the MAC address:"
set /a serial_number_2 = %serial_number%+1
echo. & echo New serial number 1 (3951): %serial_number%
echo New serial number 2 (3955): %serial_number_2%
echo. & echo New MAC address: %mac_address%
echo. & echo Programming upd...
%test_prog% upd -s %serial_number% %serial_number_2% -a %mac_address%
if %ERRORLEVEL% GTR 0 (
  echo Programming upd FAILED.
  exit /b
) else (
  echo Programming upd passed.
)

echo.

:: write the serial number back to the file.
if not exist c:\test (
   mkdir c:\test
)
echo %serial_number% >> %serial_nos_file%
echo %serial_number_2% >> %serial_nos_file%

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
