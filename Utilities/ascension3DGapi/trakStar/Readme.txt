October 27, 2009
P/N: 940038 Rev D

*******************************************************
This CD-ROM contains the relevant device drivers for the 
ATC trakSTAR system. It also contains currently available 
Utilities. 
Note: 
For Multi-Unit-Sync (MUS) Operation, see the MUS Installation 
Guide Addendum for quick setup and checkout instructions.
See the trakSTAR Installation and Operation Guide for full 
description of API and tracker operation.

*******************************************************

Specific items new for this release:

*64-bit Support - 64bit versions of Windows Vista OS now 
		  supported. Requires installation of the 
 		  winusb.sys driver

*Installer- Install.msi file now used to load software, 
	    documentation, and all required driver install
	    files.

*ATC3DGm.DLL - version 32.0.7.41
	**Enhancements
		1. New parameters added to GetXXXXParamter calls,
 		   to provide user with Part number and Model 
		   string of each component (board, sensor, and
		   transmitter).
		2. Support for user accessible memory storage in
		   sensors -see VPD parameter type. 
		3. Support for 64-bit driver
		4. New user API access to Report Rate parameter
		5. New 'Pre-init' parameters - gives API access to
		   the settings stored in the ATC3DG.ini file. 
		   These can be manipulated PRIOR to initialization.
		6. Improved efficiency of error retrieval method 
		   during GetAsynchronous calls - improves 
		   throughput.
	**Bug Fixes-
		1. Additional 500mS added to the initialize delay
		   following a reset. Corrects a timeout bug observed
		   on some Vista OS machines. 
		2. Modified items preventing Unicode support.
		3. Corrected timestamp synchronization method for 
		   multi-unit configurations. 
		
*Cubes - version 32.0.20.9
	**Enhancements
		1. Implements enhanced Status Bit information, as 
		   returned by GetSensorStatus.  
		2. Implements support for new media parameter types.
		   Allows operation with the RS232 interface 
		   (single unit only).

*DSP firmware Revisions -versions: X.53
	**Enhancements-
		1. Added support for user accessible memory in 
		   individual tracker components. See new VPD
		   parameters.
		2. Additional information added to that returned 
		   with the Status bits.
		3. Improved WRT performance - Implemented algorithm
		   changes which allow sensor operation in closer 
		   to the transmitter.

	**Bug Fixes-
		1. Corrected several low-level DSP communication bugs.
		2. Corrected a bug resutled in incomplete response 
		   to the system status examine command.
		3. Corrected a bug that resulted in the wrong error
		   code being returned when an invalid prom was 
		   detected.  


*APITest (formerly PCITalk)- version 32.0.0.7
	**Enhancements
		1. Adopted medSAFE version of this app for consistency
		   across 3DGuidance product line.

*trakSTAR Utility - version 32.3.0.15

*tracker.dll - version 32.0.0.7
	

*Other
	1. pciTalk.exe renamed to APITest.exe - for consistency with 
	   3DG medSAFE.
	2. Updated Sample projects to include 2008.NET build support

**********************************************************
CONTENT LISTING:

[3D Guidance API]
   [ATC3DG API] - Contains latest .h,.lib,and DLL files. Use these 
		  when compiling applications to utilize latest
		  functionality.
		  -ATC3DG.h	n/a	   9/24/09
		  -ATC3DG.lib	n/a	   10/06/09
		  -ATC3DG.DLL	32.0.7.41  10/06/09

[ACROBAT]	Installation File for Adobe Reader v8.0 

[Cubes]	- Contains program demonstrating communication via USB
	  using the 3DGuidance API
		-Cubes	  32.0.20.9   10/08/09

[trakSTAR_Utility] - Contains Utility used for loading firmware 
[driveBAY_Utility]   updates and configuring power-up settings.
			-trakSTAR Utility 33.3.0.15  10/06/09
			-driveBAY Utility 32.2.0.15  10/06/09
			-tracker.dll	  32.0.0.7  10/06/09

		[DSP-Loader files] - Contains current firmware 
				     loader files 
			-dipole PO	6.53
			-mdsp		7.53
			-diag_strings	8.53
			-error_strings	9.53

[FOB Direct] 	Contains subdirectories CBird and Terminal that
		contain sample programs with source code for 
		communicating with the tracker directly using the 
		RS232 Interface protocol.

		[FLOCK232]	Contains subdirectories BASIC, ASM
				and C containing source code for
				running	either single or multiple 
				sensor unit on the RS-232C interface. 
				Consult B_FILES.TXT, A_FILES.TXT, and
				C_FILES.TXT in the appropriate subdirectory
				for a description of the source code files.

[FOB_WIN32]	MS Windows legacy driver for devices compatible with the 
		RS232 Interface (FOB)protocol. DOC subdirectory contains API
		reference document. Consult readme file in Driver 
		subdirectory for desciption of files.

[IMAGES] 	File used by index.html 


[APITest]	Windows utility that permits communication with tracker using 
		any single command/parameter defined in the 3DGuidance API, 
		and displays tracker response.

[Samples]	Contains subdirectories Sample,Sample2,and GetSynchRecordSample
		containing C++ Project files/sample code for fundamental 
		communication with the tracker using 3DGuidance API. 
		Sample 2 project shows appropriate use of each of the 
		GETXXX/SETXXXX calls, giving access to all configurable tracker
		parameters. GetSynchronousRecord Sample gives example of the
		GetSynchronousRecord() call.


[USB Install] - Contains installation files (.inf) and low level USB drivers for
		3DGuidance driveBAY and trakSTAR systems

[WINBIRD]	Sample application that uses the Windows driver for the RS232 
		Interface protocol.


******************************************************


Device Driver Installation
*Install software before connecting the tracker*
1 - Insert the CD-ROM into the PC CD-ROM tray.
2 - Follow prompts in the installer to copy all required files.
3 - When software installation complete, connect power and the USB 
    interface cable to the Tracker.
4 - Windows will prompt the user with the New Hardware Wizard.
5 - Select the 'Install Automatically(Default)' radio button option.
6 - Press the 'Next' Button.
7 - A Window's message will appear, indicating that this driver 
    has not passed 'Windows Logo Testing'
8 - Press the 'Continue Anyway' button.
9 - Windows will install the appropriate files.
10 - Press the 'Finish' Button.


