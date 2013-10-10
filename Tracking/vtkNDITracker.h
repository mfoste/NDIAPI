/*=========================================================================

  Program:   AtamaiTracking for VTK
  Module:    $RCSfile: vtkNDITracker.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkNDITracker - VTK interface for Northern Digital's tracking devices
// .SECTION Description
// The vtkNDITracker class provides an  interface to the AURORA and POLARIS
// (Northern Digital Inc., Waterloo, Canada) using the new "combined API" and
// should also support all newer NDI tracking devices.  Any POLARIS systems
// purchased before 2002 will not support the combined API, and should be
// used with vtkPOLARISTracker instead.
// .SECTION Caveats
// This class refers to ports 1,2,3,A,B,C as ports 0,1,2,3,4,5
// .SECTION see also
// vtkTrackerTool vtkFlockTracker


#ifndef __vtkNDITracker_h
#define __vtkNDITracker_h

#include "vtkTracker.h"
#include "ndicapi.h"

#include "vtkPolyData.h"
#include "vtkSmartPointer.h"

class vtkFrameToTimeConverter;

// the number of tools the polaris can handle
#define VTK_NDI_NTOOLS 12
#define VTK_NDI_REPLY_LEN 2048

// types of NDI systems that use the combined API.
#define NDI_AURORA_SYSTEM          0x01
#define NDI_POLARIS_SPECTRA_SYSTEM 0x02
#define NDI_POLARIS_VICRA_SYSTEM   0x04
#define NDI_POLARIS_SYSTEM         0x08 // any other system would be Polaris.

class VTK_EXPORT vtkNDITracker : public vtkTracker
{
public:

  static vtkNDITracker *New();
  vtkTypeMacro(vtkNDITracker,vtkTracker);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  // Description:
  // Log the communication stream to the NDI system to screen.
  void LogCommunication(int bLog) {this->bLogCommunication = bLog;}

  // Description:
  // Set the system to be a slave in hardware sync.
  void SetHardwareSync(int bSync) {this->bHardwareSync = bSync;}

  // Description:
  // Probe to see if the tracking system is present on the
  // specified serial port.  If the SerialPort is set to -1,
  // then all serial ports will be checked.
  int Probe();

  // Description:
  // Send a command to the NDI in the format INIT: or VER:0 (the
  // command should include a colon).  Commands can only be done after
  // either Probe() or StartTracking() has been called.
  // The text reply from the NDI is returned, without the CRC or
  // final carriage return.
  char *Command(const char *command);

  // Description:
  // Get the a string (perhaps a long one) describing the type and version
  // of the device.
  vtkGetStringMacro(Version);

  // Description:
  // Get the API revision.
  vtkGetStringMacro(APIRevision);

  // Description:
  // Set which serial port to use, 1 through 4.
  vtkSetMacro(SerialPort, int);
  vtkGetMacro(SerialPort, int);

  // Description:
  // Set which serial device to use.  If present, this overrides
  // the SerialPort number.
  vtkSetStringMacro(SerialDevice);
  vtkGetStringMacro(SerialDevice);

  // Description:
  // Get number of available volumes.
  vtkGetMacro(NumTrackingVolumes, int);

  // Description:
  // Return the shape type name for the given volume.
  std::string GetTrackingVolumeShapeType(int volume);

  // Description:
  // Return a pointer to the array containing the shape parameters.
  double* GetTrackingVolumeShapeParameters(int volume);

  virtual vtkSmartPointer<vtkPolyData> GeneratePolydataVolume(bool solidSurface=false);

  // Description:
  // Set the volume.
  int SetVolume(int volume);

  // Description:
  // Set the desired baud rate.  Default: 9600.  TODO: change this to auto-detect.
  vtkSetMacro(BaudRate, int);
  vtkGetMacro(BaudRate, int);

  // Description:
  // Enable a passive tool by uploading a virtual SROM for that
  // tool, where 'tool' is a number between 0 and 5.
  void LoadVirtualSROM(int tool, const char *filename);
  void ClearVirtualSROM(int tool);

  // Description:
  // Get an update from the tracking system and push the new transforms
  // to the tools.  This should only be used within vtkTracker.cxx.
  void InternalUpdate();
  virtual void InternalInterpretCommand(const char *c);

  // Description:
  // Get the full TX reply for a tool. 
  int GetFullTX(int tool, double transform[8]);
  double *GetFullTX(int tool) { this->ReturnValue[0] = (double) this->GetFullTX(tool, &this->ReturnValue[1]); return this->ReturnValue; };

protected:
  vtkNDITracker();
  ~vtkNDITracker();

  // Description:
  // Set the version information.
  vtkSetStringMacro(Version);

  // Description:
  // Read in the API revision and set it.
  vtkSetStringMacro(APIRevision);
  void ReadAPIRevision();

  // Description:
  // Parse the serial number.
  void ParseSerialNumber();
  void ParseFGSerialNumber(std::string scuSerialNo);

  // Description:
  // Find the optimal baud rate and Init the system.
  int InternalInitSystem();

  // Description:
  // Get all the info for the tracking system before we start tracking.
  // This gets called during Probe() and InternalStartTracking()
  int InternalGetSystemInfo();

  // Description:
  // Initialize the tools, load the Virtual ROMs and Enable the tools.
  int InternalInitTools();

  // Description:
  // Bring the system up to full configuration just before the TSTART.
  int InternalPreConfigureTracking();

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state into full tracking mode.  The device will
  // only be reset if communication cannot be established without
  // a reset.
  int InternalStartTracking();

  // Description:
  // Stop the tracking system and bring it back to its ground state:
  // Initialized, not tracking, at 9600 Baud.
  int InternalStopTracking();

  // Description:
  // Cause the device to beep the specified number of times.
  int InternalBeep(int n);

  // Description:
  // Set the specified tool LED to the specified state.
  int InternalSetToolLED(int tool, int led, int state);

  // Description:
  // This is a low-level method for loading a virtual SROM.
  // You must halt the tracking thread and take the device
  // out of tracking mode before you use it.
  void InternalLoadVirtualSROM(int tool, const unsigned char data[1024]);
  void InternalClearVirtualSROM(int tool);

  // Description:
  // Methods for detecting which ports have tools in them, and
  // auto-enabling those tools.
  void EnableToolPorts();
  void DisableToolPorts();

  // Description:
  // Find the tool for a specific port handle (-1 if not found).
  int GetToolFromHandle(int handle);

  // Description:
  // Class for updating the virtual clock that accurately times the
  // arrival of each transform, more accurately than is possible with
  // the system clock alone because the virtual clock averages out the
  // jitter.
  vtkFrameToTimeConverter *Timer;

  ndicapi *Device;
  char *Version;
  char *APIRevision;
  char *SerialDevice;

  vtkMatrix4x4 *SendMatrix;
  int SerialPort; 
  int NDISystemType;
  int NumTrackingVolumes;
  int TrackingVolumeShapeTypes[NDI_MAX_VOLUMES];
  double TrackingVolumeParameters[NDI_MAX_VOLUMES][10];
  int Volume;
  vtkSmartPointer<vtkPolyData> VolumePolyData;
  int BaudRate;
  int IsDeviceTracking;
  int bLogCommunication;

  // hardware sync -- used if slave.
  int bHardwareSync; // this is Polaris Spectra only where the Spectra must be a slave.

  int PortEnabled[VTK_NDI_NTOOLS];
  int PortHandle[VTK_NDI_NTOOLS];
  unsigned char *VirtualSROM[VTK_NDI_NTOOLS];

  char CommandReply[VTK_NDI_REPLY_LEN];

  double ReturnValue[9];

private:
  vtkNDITracker(const vtkNDITracker&);
  void operator=(const vtkNDITracker&);  
};

#endif
