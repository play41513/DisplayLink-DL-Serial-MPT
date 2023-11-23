Draft B. 12th May 2017

Readme file for Windows 10 Anniversary Update USBIO MTT Batch Files.
--------------------------------------------------------------------

This zip file contains a number of Windows batch files designed to aid the use
of the Manufacturing Test Tool on a PC running Windows 10 Anniversary Update (or
newer). This version of Windows is also known as Redstone. They are necessary 
because Redstone uses the IDD driver, which no longer has support for the USBIO 
interface, so to use the Manufacturing Test Tool we must install special USBIO 
drivers.

They are a workaround until native IDD support is added to the Manufacturing 
Test Tool.

To use these batch files, you need to download the 8.2 M1 version of USBIO 
drivers from the DisplayLink Extranet (NR-134568-LS) and extract them to a 
directory, and also install the Windows Driver Kit (WDK) to give you the 
devcon.exe tool required by the batch files. 

The WDK setup file is available from this link
http://go.microsoft.com/fwlink/p/?LinkId=526733

If this link goes obsolete, then it should be available from this page
https://developer.microsoft.com/en-us/windows/hardware/windows-driver-kit


Using the batch files
---------------------

To use these batch files, you will need to edit them and modify the following 
variables (if present in the file you want to use):

USBIODRIVER needs to be the the path of the DisplayLinkUSBIOMTT.inf file of 
  the USBIO driver, so you need to add the directory where you extracted it.

DEVCONTOOL needs to point to devcon.exe. It may be OK to leave it as it is.

MFG_TEST_TOOL needs to specify the path of the Manufacturing Test Tool 
  dl_mfg_test.exe.

All of the batch files should be run from a command prompt that has been 
started in Administrator mode. If you don't do this you will get 
"Access Denied" messages.

Description of batch files
--------------------------

redstone_install_usbio.bat
  With no command line parameters, lists all of the DisplayLink devices that 
  are connected to the PC. For DL3 chips, there will be 2 entries for each 
  device.
  
  If you include a (case-sensitive) PID on the command line, the USBIO drivers 
  will be installed and associated with this device. You will then be able to 
  list the device using "dl_mfg_test.exe --dev list", and put it into MTT mode 
  as on a non-Redstone PC. Note that once you have put the product into MTT 
  mode, you'll have to run this batch file again to re-associate the drivers 
  with the product in MTT mode.
  
redstone_uninstall_usbio.bat
  Un-installs the USBIO drivers. If you reboot the device after running this 
  batch file, it will go back to using the IDD drivers.

redstone_start_mttmode.bat
  An extension of the redstone_install_usbio.bat batch file. This file does the 
  following:
  * Installs the USBIO drivers for the product with the PID specified on the 
    command line
  * Puts the product into MTT mode (only DL3 devices are supported)
  * Waits for 10 seconds for the product to complete its reboot
  * Re-associates the drivers with the rebooted product
  * Runs a couple of test MTT commands to ensure that the driver install has 
    worked (info flash and info otp)
  For a multi-device product (e.g. Dell Domino) all devices will be put into 
  MTT mode.
  
redstone_stop_mttmode.bat
  An extension of the redstone_uninstall_usbio.bat batch file. This will 
  uninstall the USBIO drivers, and reboot any (DL3) device found in MTT mode.
  When the device reboots it will use the IDD drivers.
