// NDIAPI.cpp : Defines the exported functions for the DLL application.
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

#include "stdafx.h"
#include "NDIAPI.h"

#include "ndicapi.h"
#include "ndicapi_math.h"
#include <string>
#include <limits>

ndicapi* device = NULL;

char* PortNameString = NULL;
const char* DeviceNameString = NULL;
const char* VersionString = NULL;
const char* SerialNumberString = NULL;

const unsigned int max_tools = 12;
const unsigned int max_srom_size = 1024;

bool IsDeviceTracking = false;

NDIAPI typedef struct
{
	double transform[8];
	int status;
	int frame;
} ToolTransformStruct;

typedef struct
{
	unsigned char name[16];
	unsigned char* sromBuffer;
	ToolTransformStruct toolTransform;
	ToolStatusStruct status;
} toolStruct;

toolStruct* tools[max_tools];
DeviceStatusStruct status;

bool initializeTool(unsigned int toolNumber)
{
	if (device == NULL)
	{
		return false;
	}

	if (tools[toolNumber] == NULL || tools[toolNumber]->sromBuffer == NULL)
	{
		return false;
	}

	// Request a port number - passive only
    ndiCommand(device, "PHRQ:*********1****");
    tools[toolNumber]->status.portNumber = ndiGetPHRQHandle(device);

	int errnum = ndiGetError(device);
	if (errnum != NDI_OKAY)
	{
		return false;
	}

	char hexbuffer[128];
	for ( int i = 0; i < max_srom_size; i += 64)
	{
		ndiCommand(device, "PVWR:%02X%04X%.128s",
			tools[toolNumber]->status.portNumber, i, ndiHexEncode(hexbuffer, &(tools[toolNumber]->sromBuffer[i]), 64));
	}

	errnum = ndiGetError(device);
	if (errnum != NDI_OKAY)
	{
		return false;
	}

	// Init the port - That is associate the previously loaded
	// SROM with the handle.
	ndiPINIT(device, tools[toolNumber]->status.portNumber);
	errnum = ndiGetError(device);
	if (errnum != NDI_OKAY)
	{
		return false;
	}

	// Enable the port - 'D' = dynamic or moving object
	ndiPENA(device, tools[toolNumber]->status.portNumber, 'D');
	errnum = ndiGetError(device);
	if (errnum != NDI_OKAY)
	{
		return false;
	}

	return true;
}

bool removeTool(unsigned int toolNumber)
{
	// TODO - disable and remove tool from device
	return true;
}

void setPortName(const char* portName)
{
	if (PortNameString != NULL)
	{
		delete PortNameString;
	}

	PortNameString = new char[5];

	strncpy(PortNameString, portName, sizeof(char[5]));
	PortNameString[4]='\0';
}

const char* ParseVersion(std::string responseBuffer)
{
	std::string versionString;
	std::string versionKeyString = "Rev ";
	int versionStringLengthMax = 3;
	int loc = responseBuffer.find(versionKeyString);
	if( loc != std::string::npos )
	{
		int endChar = versionStringLengthMax;
		for(int i=0; i<versionStringLengthMax; i++)
		{
			if(responseBuffer[loc+versionKeyString.length()+i] == ' ' || responseBuffer[loc+versionKeyString.length()+i] == '\n' )
			{
				endChar = i;
				break;
			}
		}

		versionString = responseBuffer.substr(loc+versionKeyString.length(), endChar);
	}

	char* versionStringBuffer = new char[versionString.length()+1];
	versionString.copy(versionStringBuffer,versionString.length());
	versionStringBuffer[versionString.length()] = '\0';

	return versionStringBuffer;
}

const char* ParseSerialNumber(std::string responseBuffer)
{
	std::string serialNumberString;
	std::string snKeyString = "NDI S/N: ";
	int serialNumberStringLengthMax = 8;
	int loc = responseBuffer.find(snKeyString);
	if( loc != std::string::npos )
	{
		int endChar = serialNumberStringLengthMax;
		for(int i=0; i<serialNumberStringLengthMax; i++)
		{
			if(responseBuffer[loc+snKeyString.length()+i] == ' ' || responseBuffer[loc+snKeyString.length()+i] == '\n' )
			{
				endChar = i;
				break;
			}
		}

		serialNumberString = responseBuffer.substr(loc+snKeyString.length(), endChar);
	}

	char* serialNumberBuffer = new char[serialNumberString.length()+1];
	serialNumberString.copy(serialNumberBuffer,serialNumberString.length());
	serialNumberBuffer[serialNumberString.length()] = '\0';

	return serialNumberBuffer;
}

// TODO - Find a more generic way to parse the response buffer
const char* ParseDeviceName(std::string responsBuffer)
{
	if( responsBuffer.find("Polaris Spectra") != std::string::npos )
	{
		return "Polaris Spectra";
	}
	else if( responsBuffer.find("Polaris Vicra") != std::string::npos )
	{
		return "Polaris Vicra";
	}
	else if( responsBuffer.find("Polaris") != std::string::npos )
	{
		return "Polaris";
	}
	else if (responsBuffer.find("Aurora") != std::string::npos )
	{
		return "Aurora";
	}

	return "Unknown Device";
}

bool GetDeviceTracking()
{
	return IsDeviceTracking;
}

const char* GetConnectedDeviceVersion()
{
	return VersionString;
}

const char* GetConnectedDeviceSerialNumber()
{
	return SerialNumberString;
}

const char* GetConnectedDevicePortName()
{
	return PortNameString;
}

const char* GetConnectedDeviceName()
{
	return DeviceNameString;
}

int Probe()
{
	char* portName = NULL;
	int errnum = 0;

    for (int i = 0; i < 8; i++)
    {
      portName = ndiDeviceName(i);

      if (portName)
      {
        errnum = ndiProbe(portName);

        if (errnum == NDI_OKAY)
        {
			setPortName(portName);
            break;
        }
      }
    }

	return errnum;
}

int updateSystemInfo()
{
	int errnum;

	// get information about the device
	char* responseBuffer = ndiVER(device,0);
	
	// update the serial number.
	VersionString = ParseVersion(responseBuffer);
	SerialNumberString = ParseSerialNumber(responseBuffer);
	DeviceNameString = ParseDeviceName(responseBuffer);
	
	// read the API revision.
	//ndiAPIRevision();

	// Call sflist, get features. Note that this information
	// is stored in the lower level driver until it's retrieved by API calls.
	ndiSFLIST(device, NDI_FEATURES_SUMMARY);
	errnum = ndiGetError(device);
	if (errnum != NDI_OKAY)
	{
		ndiClose(device);
		device = NULL;
		return errnum;
	}

	// Call sflist, get volume info. Note that this information
	// is stored in the lower level driver until it's retrieved by API calls.
	ndiSFLIST(device, NDI_VOLUME_INFO);
	errnum = ndiGetError(device);
	if (errnum != NDI_OKAY)
	{
		ndiClose(device);
		device = 0;
		return errnum;
	}

	// Retrieve information queried above.
	// Support only a single volume for now
	int numTrackingVolumes = ndiGetSFLISTNumberVolumes(device);
	double trackingVolumeParameters[10];
	int trackingVolumeShapeType;
	ndiGetSFLISTVolumeParameters(device, 0, &trackingVolumeShapeType, trackingVolumeParameters);

	// all good, return true.
	return NDI_OKAY;
}

int initializeDevice()
{
	int errnum;
	ndiCommand(device,"INIT:");
	if (ndiGetError(device) != NDI_OKAY)
	{
		ndiRESET(device);
		if (ndiGetError(device) != NDI_OKAY)
		{
			ndiClose(device);
			device = NULL;
			return errnum;
		}

		ndiCommand(device,"INIT:");
		if (ndiGetError(device) != NDI_OKAY)
		{
			ndiClose(device);
			device = NULL;
			return errnum;
		}
	}

	return NDI_OKAY;
}

int setBaud()
{
	int errnum;

	// Search for acceptible baud rate, starting with fastest speed.
	for( int autoBaud = 6; autoBaud >-1; autoBaud--)
	{
		ndiCommand(device,"COMM:%d%03d%d",autoBaud,NDI_8N1,NDI_NOHANDSHAKE);
		errnum = ndiGetError(device);
		if(errnum == NDI_OKAY)
		{
			break;
		}
	}

	if(errnum != NDI_OKAY)
	{
		ndiClose(device);
		device = NULL;
		return errnum;
	}

	return NDI_OKAY;
}

// Open using specific port
int Open(const char* portNameString, bool enableCommandLogging = false)
{
	int errnum;

	if (portNameString != NULL)
	{
		// Open the device
		device = ndiOpen(portNameString);
	}
	else if (PortNameString != NULL)
	{
		device = ndiOpen(PortNameString);
	}
	else
	{
		return NDI_BAD_COMM;
	}

	if (device == NULL)
	{
		return NDI_BAD_COMM;
	}

	// Turn on communications log if requested
	ndiLogCommunication(device, enableCommandLogging);

	// Initialize device
	errnum = initializeDevice();
	if (errnum != NDI_OKAY)
	{
		return errnum;
	}

	// Set baud rate
	errnum = setBaud();
	if (errnum != NDI_OKAY)
	{
		return errnum;
	}

	return updateSystemInfo();
}

bool LoadToolFromFile(unsigned int toolNumber, const char* filepath)
{
	FILE *file = fopen(filepath,"rb");
    if (file == NULL)
    {
        return false;
    }

    if (tools[toolNumber] == NULL)
    {
		tools[toolNumber] = new toolStruct;
		tools[toolNumber]->sromBuffer = new unsigned char[max_srom_size];
    }

    memset(tools[toolNumber]->sromBuffer,0,max_srom_size);
    fread(tools[toolNumber]->sromBuffer,1,max_srom_size,file);
    fclose(file);

	return initializeTool(toolNumber);
}

bool StartTracking(bool resetFrameCount)
{
	if (device == NULL)
	{
		return false;
	}

	if (resetFrameCount)
	{
		ndiCommand(device,"TSTART:80");
	}
	else
	{
		ndiTSTART(device);
	}

    int errnum = ndiGetError(device);
    if (errnum != NDI_OKAY)
    { 
		return false;
	}

	IsDeviceTracking = true;

	return true;
}

bool StopTracking()
{
	if (device == NULL)
	{
		return false;
	}

	ndiTSTOP(device);
    int errnum = ndiGetError(device);
    if (errnum != NDI_OKAY)
    { 
		return false;
	}

	IsDeviceTracking = false;

	return true;
}

bool RemoveTool(unsigned int toolNumber)
{
	if (toolNumber > max_tools)
	{
		return false;
	}

	if (tools[toolNumber] != NULL)
	{
		if (tools[toolNumber]->sromBuffer != NULL)
		{
			delete [] tools[toolNumber]->sromBuffer;
			tools[toolNumber]->sromBuffer = NULL;
		}
		
		removeTool(toolNumber);

		delete tools[toolNumber];
		tools[toolNumber] = NULL;

		return true;
	}
	return false;
}

bool UpdateTransforms()
{
	int nHandles = 0;
	int nHandle = 0;
	int errnum = 0;
	int tool = 0;
	double transform[8];

	if (!IsDeviceTracking)
	{
		return false;
	}

	// get the transforms for all tools from the NDI
	const char* replyBuffer = ndiCommand(device, "TX:0803");
	errnum = ndiGetError(device);
	if (errnum != NDI_OKAY)
	{
		return false;
	}

	for ( int tool = 0; tool < max_tools; tool++ )
	{
		if (tools[tool] == NULL)
		{
			continue;
		}

		// Initialize transform to identity;
		transform[0] = 1.0;
		transform[1] = transform[2] = transform[3] = 0.0;
		transform[4] = transform[5] = transform[6] = 0.0;
		transform[7] = 0.0;

		// TODO - Need a mutex here to make this thread safe.
		tools[tool]->toolTransform.status = ndiGetTXTransform(device, tools[tool]->status.portNumber, tools[tool]->toolTransform.transform);
		int portStatus = ndiGetTXPortStatus(device, tools[tool]->status.portNumber);
		int toolInfo = ndiGetTXToolInfo(device, tools[tool]->status.portNumber);
		tools[tool]->toolTransform.frame = ndiGetTXFrame(device, tools[tool]->status.portNumber);

		tools[tool]->status.bToolInPort = ( portStatus & NDI_TOOL_IN_PORT ? 1 : 0 );
		tools[tool]->status.bGPIO1 = ( portStatus & NDI_SWITCH_1_ON ? 1 : 0 );
		tools[tool]->status.bGPIO2 = ( portStatus & NDI_SWITCH_2_ON ? 1 : 0 );
		tools[tool]->status.bGPIO3 = ( portStatus & NDI_SWITCH_3_ON ? 1 : 0 );
		tools[tool]->status.bInitialized = ( portStatus & NDI_INITIALIZED ? 1 : 0 );
		tools[tool]->status.bEnabled = ( portStatus & NDI_ENABLED ? 1 : 0 );
		tools[tool]->status.bOutOfVolume = ( portStatus & NDI_OUT_OF_VOLUME ? 1 : 0 );
		tools[tool]->status.bPartiallyOutOfVolume = ( portStatus & NDI_PARTIALLY_IN_VOLUME ? 1 : 0 );
		tools[tool]->status.bDisturbanceDet = ( portStatus & NDI_IR_INTERFERENCE ? 1 : 0 );
		tools[tool]->status.bSignalTooSmall = ( portStatus & 0x400 ? 1 : 0 );
		tools[tool]->status.bSignalTooBig = ( portStatus & 0x800 ? 1 : 0 );
		tools[tool]->status.bProcessingException = ( portStatus & NDI_PROCESSING_EXCEPTION ? 1 : 0 );
		tools[tool]->status.bHardwareFailure = ( portStatus & 0x2000 ? 1 : 0 );
	}

	int systemStatus = ndiGetTXSystemStatus(device);
	status.bCommunicationSyncError = ( systemStatus & NDI_COMM_SYNC_ERROR ? 1 : 0 );
	status.bTooMuchInterference = ( systemStatus & NDI_TOO_MUCH_EXTERNAL_INFRARED ? 1 : 0 );
	status.bSystemCRCError = ( systemStatus & NDI_COMM_CRC_ERROR ? 1 : 0 );
	status.bRecoverableException = ( systemStatus & NDI_COMM_RECOVERABLE ? 1 : 0 );
	status.bHardwareFailure = ( systemStatus & NDI_HARDWARE_FAILURE ? 1 : 0 );
	status.bHardwareChange = ( systemStatus & NDI_HARDWARE_CHANGE ? 1 : 0 );
	status.bPortOccupied = ( systemStatus & NDI_PORT_OCCUPIED ? 1 : 0 );
	status.bPortUnoccupied = ( systemStatus & NDI_PORT_UNOCCUPIED ? 1 : 0 );
	status.bDiagnosticsPending = ( systemStatus & 0x100 ? 1 : 0 );
	status.bTemperatureOutOfRange = ( systemStatus & 0x200 ? 1 : 0 );

	return true;
}

QuaternionTransformStruct* GetQuaternionTransform(unsigned int toolNumber)
{
	// TODO - need a mutex here to make this thread safe
	if (IsDeviceTracking && toolNumber < max_tools && tools[toolNumber] != NULL)
	{
		QuaternionTransformStruct* quaternionTransform = new QuaternionTransformStruct;
		quaternionTransform->status = tools[toolNumber]->toolTransform.status;
		quaternionTransform->frameNumber = tools[toolNumber]->toolTransform.frame;

		if (quaternionTransform->status == NDI_OKAY)
		{
			quaternionTransform->q0 = tools[toolNumber]->toolTransform.transform[0];
			quaternionTransform->q1 = tools[toolNumber]->toolTransform.transform[1];
			quaternionTransform->q2 = tools[toolNumber]->toolTransform.transform[2];
			quaternionTransform->q3 = tools[toolNumber]->toolTransform.transform[3];
			quaternionTransform->x = tools[toolNumber]->toolTransform.transform[4];
			quaternionTransform->y = tools[toolNumber]->toolTransform.transform[5];
			quaternionTransform->z = tools[toolNumber]->toolTransform.transform[6];
			quaternionTransform->rmsError = tools[toolNumber]->toolTransform.transform[7];
		}

		return quaternionTransform;
	}
	else
	{
		return NULL;
	}
}

MatrixTransformStruct* GetMatrixTransform(unsigned int toolNumber)
{
	// TODO - need a mutex here to make this thread safe
	if (IsDeviceTracking && toolNumber < max_tools && tools[toolNumber] != NULL)
	{
		MatrixTransformStruct* matrixTransform = new MatrixTransformStruct;
		matrixTransform->status = tools[toolNumber]->toolTransform.status;
		matrixTransform->frameNumber = tools[toolNumber]->toolTransform.frame;

		if (matrixTransform->status == NDI_OKAY)
		{
			double matrix[16];

			ndiTransformToMatrixd(tools[toolNumber]->toolTransform.transform, matrix);

			matrixTransform->m11 = matrix[0];
			matrixTransform->m21 = matrix[1];
			matrixTransform->m31 = matrix[2];
			matrixTransform->m41 = matrix[3];
			matrixTransform->m12 = matrix[4];
			matrixTransform->m22 = matrix[5];
			matrixTransform->m32 = matrix[6];
			matrixTransform->m42 = matrix[7];
			matrixTransform->m13 = matrix[8];
			matrixTransform->m23 = matrix[9];
			matrixTransform->m33 = matrix[10];
			matrixTransform->m43 = matrix[11];
			matrixTransform->m14 = matrix[12];
			matrixTransform->m24 = matrix[13];
			matrixTransform->m34 = matrix[14];
			matrixTransform->m44 = matrix[15];
			matrixTransform->rmsError = tools[toolNumber]->toolTransform.transform[7];
		}

		return matrixTransform;
	}
	else
	{
		return NULL;
	}
}

ToolStatusStruct* GetToolStatus(unsigned int toolNumber)
{
	if (IsDeviceTracking && toolNumber < max_tools && tools[toolNumber] != NULL)
	{
		return &tools[toolNumber]->status;
	}
	else
	{
		return NULL;
	}
}

DeviceStatusStruct* GetSystemStatus()
{
	if (IsDeviceTracking)
	{
		return &status;
	}
	else
	{
		return NULL;
	}
}

