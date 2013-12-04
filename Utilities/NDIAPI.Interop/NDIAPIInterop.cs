// NDIAPI.Interop.cs : Defines the exported functions for the DLL application.
//
/*=======================================================================

  Creator:   Matt Foster <mfoste@gmail.com>
  Language:  C#
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

using System;
using System.Runtime.InteropServices;

namespace NDIAPI.Interop
{
    public class NDIAPIInterop
    {
        public struct QuaternionTransformStruct
        {
            public double x;
            public double y;
            public double z;
            public double q0;
            public double q1;
            public double q2;
            public double q3;
            public double rmsError;
            public int frameNumber;
            public int status;
        };

        public struct MatrixTransformStruct
        {
            public double m11;
            public double m12;
            public double m13;
            public double m14;
            public double m21;
            public double m22;
            public double m23;
            public double m24;
            public double m31;
            public double m32;
            public double m33;
            public double m34;
            public double m41;
            public double m42;
            public double m43;
            public double m44;
            public double rmsError;
            public int frameNumber;
            public int status;
        };
        
        public struct ToolStatusStruct
        {
	        public int portNumber;
	        public bool bToolInPort;
	        public bool bGPIO1;
	        public bool bGPIO2;
	        public bool bGPIO3;
	        public bool bInitialized;
	        public bool bEnabled;
	        public bool bOutOfVolume;
	        public bool bPartiallyOutOfVolume;
	        public bool bDisturbanceDet;
	        public bool bSignalTooSmall;
	        public bool bSignalTooBig;
	        public bool bProcessingException;
	        public bool bHardwareFailure;
        } ;

        public struct DeviceStatusStruct
        {
	        public bool bCommunicationSyncError;
	        public bool bTooMuchInterference;
	        public bool bSystemCRCError;
	        public bool bRecoverableException;
	        public bool bHardwareFailure;
	        public bool bHardwareChange;
	        public bool bPortOccupied;
	        public bool bPortUnoccupied;
	        public bool bDiagnosticsPending;
	        public bool bTemperatureOutOfRange;
        };

        #region imports

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetConnectedDevicePortName")]
        private static extern IntPtr ndiapiGetConnectedDevicePortName();
        public static string GetConnectedDevicePortName()
        {
            return Marshal.PtrToStringAnsi(ndiapiGetConnectedDevicePortName());
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetConnectedDeviceName")]
        private static extern IntPtr ndiapiGetConnectedDeviceName();
        public static string GetConnectedDeviceName()
        {
            return Marshal.PtrToStringAnsi(ndiapiGetConnectedDeviceName());
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetConnectedDeviceVersion")]
        private static extern IntPtr ndiapiGetConnectedDeviceVersion();
        public static string GetConnectedDeviceVersion()
        {
            return Marshal.PtrToStringAnsi(ndiapiGetConnectedDeviceVersion());
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetConnectedDeviceSerialNumber")]
        private static extern IntPtr ndiapiGetConnectedDeviceSerialNumber();
        public static string GetConnectedDeviceSerialNumber()
        {
            return Marshal.PtrToStringAnsi(ndiapiGetConnectedDeviceSerialNumber());
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetDeviceTracking();

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Probe();

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Open(string portName, bool enableCommandLogging);

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool LoadToolFromFile(uint toolNumber, [MarshalAs(UnmanagedType.LPStr)] String filepath);

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool StartTracking(bool resetFrameCount);

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool UpdateTransforms();

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetMatrixTransform")]
        private static extern IntPtr ndiapiGetMatrixTransform(uint toolNumber);
        public static MatrixTransformStruct GetMatrixTransform(uint toolNumber)
        {
            return (MatrixTransformStruct)Marshal.PtrToStructure(ndiapiGetMatrixTransform(toolNumber), typeof(MatrixTransformStruct));
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetQuaternionTransform")]
        private static extern IntPtr ndiapiGetQuaternionTransform(uint toolNumber);
        public static QuaternionTransformStruct GetQuaternionTransform(uint toolNumber)
        {
            return (QuaternionTransformStruct)Marshal.PtrToStructure(ndiapiGetQuaternionTransform(toolNumber), typeof(QuaternionTransformStruct));
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetToolStatus")]
        private static extern IntPtr ndiapiGetToolStatus(uint toolNumber);
        public static ToolStatusStruct GetToolStatus(uint toolNumber)
        {
            return (ToolStatusStruct)Marshal.PtrToStructure(ndiapiGetToolStatus(toolNumber), typeof(ToolStatusStruct));
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetDeviceStatus")]
        private static extern IntPtr ndiapiGetDeviceStatus();
        public static DeviceStatusStruct GetDeviceStatus()
        {
            return (DeviceStatusStruct)Marshal.PtrToStructure(ndiapiGetDeviceStatus(), typeof(DeviceStatusStruct));
        }

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool StopTracking();

        [DllImport("NDIAPI.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool RemoveTool(uint toolNumber);

        #endregion
    }
}