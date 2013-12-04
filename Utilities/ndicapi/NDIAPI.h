// NDIAPI.h : Defines the exported functions for the DLL application.
//
/*=======================================================================

  Creator:   Matt Foster <mfoste@gmail.com>
  Language:  C++
  Author:    $Author: mfoste $
  Date:      $Date: 2013/11/15 $
  Version:   $Revision: 0.1 $

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

=======================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDIAPI_EXPORTS
#define NDIAPI __declspec(dllexport)
#else
#define NDIAPI __declspec(dllimport)
#endif

NDIAPI typedef struct
{
	double x;
	double y;
	double z;
	double q0;
	double q1;
	double q2;
	double q3;
	double rmsError;
	int frameNumber;
	int status;
} QuaternionTransformStruct;

NDIAPI typedef struct
{
	double m11;
	double m12;
	double m13;
	double m14;
	double m21;
	double m22;
	double m23;
	double m24;
	double m31;
	double m32;
	double m33;
	double m34;
	double m41;
	double m42;
	double m43;
	double m44;
	double rmsError;
	int frameNumber;
	int status;
} MatrixTransformStruct;

NDIAPI typedef struct
{
	int portNumber;
	bool bToolInPort;
	bool bGPIO1;
	bool bGPIO2;
	bool bGPIO3;
	bool bInitialized;
	bool bEnabled;
	bool bOutOfVolume;
	bool bPartiallyOutOfVolume;
	bool bDisturbanceDet;
	bool bSignalTooSmall;
	bool bSignalTooBig;
	bool bProcessingException;
	bool bHardwareFailure;
} ToolStatusStruct;

NDIAPI typedef struct 
{
	bool bCommunicationSyncError;
	bool bTooMuchInterference;
	bool bSystemCRCError;
	bool bRecoverableException;
	bool bHardwareFailure;
	bool bHardwareChange;
	bool bPortOccupied;
	bool bPortUnoccupied;
	bool bDiagnosticsPending;
	bool bTemperatureOutOfRange;
} DeviceStatusStruct;

NDIAPI const char* GetConnectedDevicePortName();
NDIAPI const char* GetConnectedDeviceName();
NDIAPI const char* GetConnectedDeviceVersion();
NDIAPI const char* GetConnectedDeviceSerialNumber();
NDIAPI bool GetDeviceTracking();

NDIAPI int Probe();
NDIAPI int Open(const char* portNameString, bool enableCommandLogging);
NDIAPI bool LoadToolFromFile(unsigned int toolNumber, const char* filepath);
NDIAPI bool RemoveTool(unsigned int toolNumber);
NDIAPI bool StartTracking(bool resetFrameCount);
NDIAPI bool UpdateTransforms();
NDIAPI MatrixTransformStruct* GetMatrixTransform(unsigned int toolNumber);
NDIAPI QuaternionTransformStruct* GetQuaternionTransform(unsigned int toolNumber);
NDIAPI ToolStatusStruct* GetToolStatus(unsigned int toolNumber);
NDIAPI DeviceStatusStruct* GetDeviceStatus();
NDIAPI bool StopTracking();

#ifdef __cplusplus
}
#endif
