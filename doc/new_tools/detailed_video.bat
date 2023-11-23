@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

:: The test program name
set test_prog="dl_mfg_test.exe"
set video_timing_file="%1.txt"

  for /f "usebackq tokens=1,2,3,4,5,6,7,8,9,10,11" %%a in (
    %video_timing_file%
  ) do (
   set /a htotal = %%a
   set /a hwidth = %%b
   set /a hsync = %%c
   set /a hfporch = %%d
   set /a hpol = %%e
   set /a vtotal = %%f
   set /a vlength = %%g
   set /a vsync = %%h
   set /a vfporch = %%i
   set /a vpol = %%j
   set /a pclk = %%k             
  )

echo H-Total      : %htotal%
echo H-Width      : %hwidth%
echo H-Sync       : %hsync%
echo H-FrontPorch : %hfporch%
echo H-Polarity   : %hpol%
echo V-Total      : %vtotal%
echo V-Length     : %vlength%
echo V-sync       : %vsync%
echo V-FrontPorch : %vfporch%
echo V-Polarity   : %vpol%
echo Pixel Clock  : %pclk%


%test_prog% video --detailedmode  %htotal% %hwidth% %hsync% %hfporch% %hpol% %vtotal% %vlength% %vsync% %vfporch% %vpol% %pclk%      
