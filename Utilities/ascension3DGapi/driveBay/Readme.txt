July 7, 2008
P/N: 940041 Rev B

*******************************************************
This CD-ROM contains the relevant device drivers for the ATC driveBAY system.
It also contains currently available Utilities. 
Note: 
See the Installation and Operation Guide for quick setup and checkout instructions
See text below this content listing, for device driver installation instructions.


*******************************************************
Specific items new for this release:


*USB device strings - driveBAY.inf (32.2.0.1) and FX2 (2.2.24)
	**Enhancements
		1. Device description strings and responses now conform to Chapter 9 of the 
		   USB 2.0 specification, as determined by testing with the USB Command 
                   Verifier Compliance Test application Version 1.3.2.  

*DSP Firmware Revisions - versions: X.46
	**Bug Fixes
		1. Corrected bug in Position/Orientation algorithm that resulted in
		   data jumps under conditions of heavy metal distortion.
		2. Corrected bug in timestamping of the data record which could falsely reduce resolution 
		3. Corrected improper output that occurred when quaternions data selected
		4. Corrected lower level communication bugs associated with Stream Mode 
		5. Fixed METAL command to conform to flock command spec	
		6. Added additional checks to the adaptive filter processing to use only 
		   valid numerical inputs

*ATC3DGm.DLL - version 32.0.7.35
	**Enhancements
		1. Added a multi-media timer to the DLL. This helps prevent the data collection from being
		   paged out by Windows, as was observed on newer laptop PCs with dual-core processors.		   
		2. Improved error reporting mechanisms when unit in Stream Mode.

	**Bug Fixes
		1. Corrected bug in Diagnostic tests (SetSystemParameter) 
		   that improperly handled setting the device into and out of test mode 
		   (used by diagnostic test suites).
		2. Corrected lower level communication Stream Mode bugs

*Cubes - version 32.0.20.5
	**Bug Fixes
		1. Corrected a bug associated with timestamped records that were saved to file.
		   Precision not set correctly, resulting in occassional 'advance'time stamp events.

*driveBAY Utility - version 32.2.0.9 
	**Bug Fixes
		1. Fixed bug associated with incorrectly enabling the XYZ reference frame
		   as entered in the Settings tab.

**********************************************************
CONTENT LISTING:

[3D Guidance API]
   ATC3DG API] - Contains latest .h,.lib,and DLL files. Use these when compiling applications
   		  to utilize latest functionality.
		  -ATC3DG.h	n/a	   4/21/08
		  -ATC3DG.lib	n/a	   6/27/08
		  -ATC3DG.DLL	32.0.7.35  6/27/08

[ACROBAT]	Installation File for Adobe Reader v8.0

[Cubes]	- Contains installation file and program demonstrating communication
		via USB using the 3DGuidance API
		-Cubes	  32.0.20.5   06/11/08

[driveBAY_Utility] - Contains Utility used for loading firmware updates and configuring power-up settings
		-driveBAY Utility 32.2.0.9  06/10/08
		-tracker.dll	  32.0.0.2  04/03/08

		[DSP-Loader files] - Contains current firmware loader files 
		-dipole PO	6.46
		-mdsp		7.46
		-diag_strings	8.46
		-error_strings	9.46

[IMAGES] 	Files used by index.html Autoruns on insertion of CD-ROM 


[PCIBirdTalk]	Windows utility that permits communication with tracker using any 
		single command/parameter defined in the 3DGuidance API, and
		displays tracker response.

[Samples]	Contains subdirectories Sample and Sample2, containing C++ Project
		files/sample code for fundamental communication with the tracker
		using 3DGuidance API. Sample 2 project	shows appropriate
		use of each of the GETXXX/SETXXXX calls, giving access to all
		configurable tracker parameters.
		GetSynchronousRecord Sample gives example of newly supported
		GetSynchronousRecord() call.


[USB Driver] - Contains installation file (.inf) and low level USB driver for driveBAY
		systems
		-cyusb.sys     1.7.0.0	  10/11/05
		-cyusbme.sys   1.7.0.0	  10/11/05
		-driveBAY.inf  32.2.0.1    4/04/08



******************************************************


Device Driver Installation

1 - Copy the distribution files from the distribution media to an appropriate subdirectory on the user's computer.
2 - Connect power and the USB interface cable to the Tracker.
3 - Windows will prompt the user with the New Hardware Wizard.
4 - Select the 'Install from a list or a specific location (Advanced)' radio button option.
5 - Press the 'Next' Button.
6 - Select the 'Include this location in the search' check box.
7 - Press the 'Browse' button.
8 - Navigate to the distribution directory as defined in step 1.
9 - Navigate to the 'USB Driver' sub-directory.
10 - Press the 'OK' button.
11 - Press the 'Next' button.
12 - A Window's message will appear, indicating that this driver has not passed 'Windows Logo Testing'
13 - Press the 'Continue Anyway' button.
14 - Windows will install the appropriate files.
15 - Press the 'Finish' Button.


