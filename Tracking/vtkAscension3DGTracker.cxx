/*=========================================================================

Program:   Tracking for VTK
Module:    $RCSfile: vtkAscension3DGTracker.cxx,v $
Creator:   Andrew Wiles <awiles@ndigital.com>
Language:  C++
Author:    $Author: awiles $
Date:      $Date: 2011/06/07 8:10:00 $
Version:   $Revision: 1.0 $

==========================================================================

Copyright (c) 2000-2004
All rights reserved.

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
#if WIN32
#define snprintf sprintf_s
#endif

#define USE_EULER 0

#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "vtkMath.h"
#include "vtkTimerLog.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkCriticalSection.h"
#include "vtkAscension3DGTracker.h"


#include "vtkTrackerTool.h"
#include "vtkFrameToTimeConverter.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkAscension3DGTracker* vtkAscension3DGTracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkAscension3DGTracker");
  if(ret)
  {
    return (vtkAscension3DGTracker*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkAscension3DGTracker;
}

//----------------------------------------------------------------------------
vtkAscension3DGTracker::vtkAscension3DGTracker()
{
  this->SendMatrix = vtkMatrix4x4::New();
  this->IsTracking = 0;
  this->SetNumberOfTools(VTK_3DG_NTOOLS);
  this->pSensor = 0;
  this->pXmtr = 0;

  this->m_bUseSynchronous = false;
  this->m_bUseAllSensors = false;

  // for accurate timing
  pRecord = &record[0];
}

//----------------------------------------------------------------------------
vtkAscension3DGTracker::~vtkAscension3DGTracker() 
{
  if (this->Tracking)
  {
    this->StopTracking();
  }
  this->SendMatrix->Delete();
}

//----------------------------------------------------------------------------
void vtkAscension3DGTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkTracker::PrintSelf(os,indent);

  os << indent << "SendMatrix: " << this->SendMatrix << "\n";
  this->SendMatrix->PrintSelf(os,indent.GetNextIndent());
}

//----------------------------------------------------------------------------
int vtkAscension3DGTracker::Probe()
{
  if (IsTracking)
  {
    return 1;
  }
  else 
  {
    errorCode = InitializeBIRDSystem();
    if(errorCode != BIRD_ERROR_SUCCESS) 
    { 
      errorHandler(errorCode, "vtkAscension3DGTracker::Probe() - InitializeBIRDSystem()"); 
      return 0;
    }

    errorCode = GetBIRDSystemConfiguration(&System);
    if(errorCode != BIRD_ERROR_SUCCESS) 
    { 
      errorHandler(errorCode, "vtkAscension3DGTracker::Probe() - GetBIRDSystemConfiguration(&System)"); 
      return 0;
    }

    if (System.numberBoards > 0) 
    {
      // so far the system is connected.  
      // Get the transmitter info so you have the serial number.
      if( pXmtr ) {delete[] pXmtr;}
      pXmtr = new TRANSMITTER_CONFIGURATION[System.numberTransmitters];
      for(int i=0;i<System.numberTransmitters;i++)
      {
        errorCode = GetTransmitterConfiguration(i, &pXmtr[i]);
        if(errorCode!=BIRD_ERROR_SUCCESS) 
        { 
          errorHandler(errorCode, "vtkAscension3DGTracker::GetCurrentSettings() - GetTransmitterConfiguration(i, &pXmtr[i])"); 
          return 0;
        }
      }
      // update the serial number.
      std::stringstream snStream;
      snStream << "ATC-SN" << pXmtr->serialNumber << "-DT" << pXmtr->type;
      this->SetSerialNumber(snStream.str().c_str());
      // now close the sytem until you start tracking.
      errorCode = CloseBIRDSystem();
      if( errorCode != BIRD_ERROR_SUCCESS)
      { 
        errorHandler(errorCode, "vtkAscension3DGTracker::Probe() - CloseBIRDSystem()");
        return 0;
      }
      return 1;
    }
    else 
    {
      errorCode = CloseBIRDSystem();
      if( errorCode!= BIRD_ERROR_SUCCESS) 
      {
        errorHandler(errorCode, "vtkAscension3DGTracker::Probe() - CloseBIRDSystem()");
        return 0;
      }
      return 0;
    }
  }
} 

int vtkAscension3DGTracker::GetSerialPort()
{
  return 0;
} 

int vtkAscension3DGTracker::SetSerialPort(int port)
{
  return 0;
} 

//----------------------------------------------------------------------------
int vtkAscension3DGTracker::InternalStartTracking()
{
  if (this->IsTracking)
  {
    return 1;
  }

  std::cout << "Initializing system... ";
  errorCode = InitializeBIRDSystem();
  if(errorCode != BIRD_ERROR_SUCCESS) 
  { 
    errorHandler(errorCode, "vtkAscension3DGTracker::InternalStartTracking() - InitializeBIRDSystem()"); 
    return 0;
  }

  if( !this->GetCurrentSettings()) 
  {
    return 0;
  }
  this->EnableTransmitter();
  this->EnableToolPorts();

  DATA_FORMAT_TYPE type = DOUBLE_ALL_TIME_STAMP_Q;
  for(sensorID=0;sensorID<System.numberSensors;sensorID++)
  {
    if( !this->SetSensorDataFormat(sensorID,type)) {return 0;}
  }
  //QUALITY_PARAMETERS quality = this->GetSensorQuality(1);
  //printf("%u, %u, %u, %u\n", quality.error_slope,quality.error_offset,quality.error_sensitivity,quality.filter_alpha);

  this->IsTracking = 1;

  return 1;
}

int  vtkAscension3DGTracker::GetCurrentSettings()
{
  int i;
  errorCode = GetBIRDSystemConfiguration(&System);
  if(errorCode!=BIRD_ERROR_SUCCESS) 
  { 
    errorHandler(errorCode, "vtkAscension3DGTracker::GetCurrentSettings() - GetBIRDSystemConfiguration(&System)"); 
    return 0;
  }

  pBoard = new BOARD_CONFIGURATION[System.numberBoards];
  for(i=0;i<System.numberBoards;i++)
  {
    errorCode = GetBoardConfiguration(i, &pBoard[i]);
    if(errorCode!=BIRD_ERROR_SUCCESS) 
    { 
      errorHandler(errorCode, "vtkAscension3DGTracker::GetCurrentSettings() - GetBoardConfiguration(i, &pBoard[i])"); 
      return 0;
    }
  }

  if( pSensor ) {delete[] pSensor;}
  pSensor = new SENSOR_CONFIGURATION[System.numberSensors];
  for(i=0;i<System.numberSensors;i++)
  {
    errorCode = GetSensorConfiguration(i, &pSensor[i]);
    if(errorCode!=BIRD_ERROR_SUCCESS) 
    { 
      errorHandler(errorCode, "vtkAscension3DGTracker::GetCurrentSettings() - GetSensorConfiguration(i, &pSensor[i])"); 
      return 0;
    }
  }

  if( pXmtr ) {delete[] pXmtr;}
  pXmtr = new TRANSMITTER_CONFIGURATION[System.numberTransmitters];
  for(i=0;i<System.numberTransmitters;i++)
  {
    errorCode = GetTransmitterConfiguration(i, &pXmtr[i]);
    if(errorCode!=BIRD_ERROR_SUCCESS) 
    { 
      errorHandler(errorCode, "vtkAscension3DGTracker::GetCurrentSettings() - GetTransmitterConfiguration(i, &pXmtr[i])"); 
      return 0;
    }
  }

  // set to use metric.
  this->SetMetric(true);

  // update the serial number.
  std::stringstream snStream;
  //TODO: do not use device type, use get transmitter part number.
  snStream << "ATC-SN" << pXmtr->serialNumber << "-DT" << pXmtr->type;
  this->SetSerialNumber(snStream.str().c_str());
  return 1;
}

//----------------------------------------------------------------------------
int vtkAscension3DGTracker::InternalStopTracking()
{
  this->RequestUpdateMutex->Lock();
  this->UpdateMutex->Lock();
  this->RequestUpdateMutex->Unlock();
  this->DisableTransmitter();
  this->UpdateMutex->Unlock();
  this->IsTracking = 0;
  if( CloseBIRDSystem()!=BIRD_ERROR_SUCCESS) 
  { 
    errorHandler(errorCode, "vtkAscension3DGTracker::InternalStopTracking() - CloseBIRDSystem()"); 
    return 0;
  }

  return 1;
}

void vtkAscension3DGTracker::InternalUpdate()
{
  int absent[VTK_3DG_NTOOLS], saturated[VTK_3DG_NTOOLS], inMotionBox[VTK_3DG_NTOOLS];
  int tool;
  DOUBLE_ALL_TIME_STAMP_Q_RECORD *referenceTransform = 0;
  double nextcount = 0;
  int i;

  //debug: for some timing.
  //clock_t start, curr;
  //double tElapsed = 0;


  // for the sensor status.
  bool attached, transmitterRunning, transmitterAttached, globalError;

  if (!this->IsTracking) {
    vtkWarningMacro( << "called Update() when 3DG was not tracking");
    return;
  }
  if( this->m_bUseAllSensors )
  {
    
    // debug:
    //start = clock();

    //use all sensors.
    if(this->m_bUseSynchronous)
    {
      errorCode = GetSynchronousRecord(-1, pRecord, System.numberSensors*sizeof(record[0]));
      
      // debug:
      //curr = clock();
      //tElapsed = ((double)(curr - start)) / CLOCKS_PER_SEC;
      //std::cout << "GetSynchronous-ALL_SENORS time elapsed: " << tElapsed << std::endl;

      if(errorCode!=BIRD_ERROR_SUCCESS) 
      {
        errorHandler(errorCode, "vtkAscension3DGTracker::InternalUpdate() - GetSynchronousRecord(ALL_SENSORS, pRecord, System.numberSensors*sizeof(record[sensorID]))");
      }
    }
    else
    {
      errorCode = GetAsynchronousRecord(-1, pRecord, System.numberSensors*sizeof(record[0]));
      
      // debug:
      //curr = clock();
      //tElapsed = ((double)(curr - start)) / CLOCKS_PER_SEC;
      //std::cout << "GetAsynchronous - ALL_SENSORS time elapsed: " << tElapsed << std::endl;

      if(errorCode!=BIRD_ERROR_SUCCESS) 
      {
        errorHandler(errorCode, "vtkAscension3DGTracker::InternalUpdate() - GetAsynchronousRecord(ALL_SENSORS, pRecord, System.numberSensors*sizeof(record[sensorID]))");
      }
    }

    for(sensorID=0;sensorID<System.numberSensors;sensorID++)
    {
      // check the sensor status.
      DEVICE_STATUS status = GetSensorStatus(sensorID);
      saturated[sensorID] = status & SATURATED;
      attached = !(status & NOT_ATTACHED);
      //TODO: review the rev C. documentation and check the status bits -- different sets of status bits for each transmitter.
#ifndef Ascension3DG_DriveBay  // these are not implemented for the driveBay -- only trakStar apparently.
      inMotionBox[sensorID] = !(status & OUT_OF_MOTIONBOX); // not implemented with the MedSafe.  Always true.
      transmitterRunning = !(status & NO_TRANSMITTER_RUNNING);
      transmitterAttached = !(status & NO_TRANSMITTER_ATTACHED);
#else
      inMotionBox[sensorID] = 1;
      transmitterRunning = true;
      transmitterAttached = true;
#endif
      globalError = status & GLOBAL_ERROR;

      if( attached && PortEnabled[sensorID])
      {   
        // Not absent.
        absent[sensorID] = 0;

        if (record[sensorID].time > nextcount) 
        { 
          nextcount = record[sensorID].time; 
        }
      }
      else if(!attached && !PortEnabled[sensorID])
      {
        absent[sensorID] = 1;
      }
      else 
      {
        this->EnableToolPorts();
      }
    } /* end for sensors */
  }
  else //do each sensor individual.
  {
    // scan the sensors and request a record if the sensor is physically attached
    for(sensorID=0;sensorID<System.numberSensors;sensorID++)
    {
      // check the sensor status.
      DEVICE_STATUS status = GetSensorStatus(sensorID);
      saturated[sensorID] = status & SATURATED;
      attached = !(status & NOT_ATTACHED);
#if defined (Ascension3DG_TrakStar)
      // TODO: clarify these.
      // these are not implemented for the driveBay -- only trakStar apparently, but not trakSTAR 2.
      inMotionBox[sensorID] = !(status & OUT_OF_MOTIONBOX); // not implemented with the MedSafe.  Always true.
      transmitterRunning = !(status & NO_TRANSMITTER_RUNNING);
      transmitterAttached = !(status & NO_TRANSMITTER_ATTACHED);
#else
      inMotionBox[sensorID] = 1;
      transmitterRunning = true;
      transmitterAttached = true;
#endif
      globalError = status & GLOBAL_ERROR;

      if( attached && PortEnabled[sensorID])
      {
        // debug:
        //start = clock();

        // TODO: remove - doesn't work.
        if(this->m_bUseSynchronous)
        {
          errorCode = GetSynchronousRecord(sensorID, (pRecord+sensorID), sizeof(record[sensorID]));
          
          // debug:
          //curr = clock();
          //tElapsed = ((double)(curr - start)) / CLOCKS_PER_SEC;
          //std::cout << "GetSynchronous - sensorID time elapsed: " << tElapsed << std::endl;

          if(errorCode!=BIRD_ERROR_SUCCESS) 
          {
            errorHandler(errorCode, "vtkAscension3DGTracker::InternalUpdate() - GetSynchronousRecord(sensorID, (pRecord+sensorID), sizeof(record[sensorID]))");
          }
        }
        else
        {

          errorCode = GetAsynchronousRecord(sensorID, (pRecord+sensorID), sizeof(record[sensorID]));
          
          // debug:
          //curr = clock();
          //tElapsed = ((double)(curr - start)) / CLOCKS_PER_SEC;
          //std::cout << "GetAsynchronous - sensorID time elapsed: " << tElapsed << std::endl;

          if(errorCode!=BIRD_ERROR_SUCCESS) 
          {
            errorHandler(errorCode, "vtkAscension3DGTracker::InternalUpdate() - GetAsynchronousRecord(sensorID, (pRecord+sensorID), sizeof(record[sensorID]))");
          }
        }
        

        // Not absent.
        absent[sensorID] = 0;

        if (record[sensorID].time > nextcount) 
        { 
          nextcount = record[sensorID].time; 
        }
      }
      else if(!attached && !PortEnabled[sensorID])
      {
        absent[sensorID] = 1;
      }
      else 
      {
        this->EnableToolPorts();
      }
    } /* end for sensors */
  }

  if (this->ReferenceTool >= 0) 
  { // copy reference tool transform
    referenceTransform = &record[this->ReferenceTool];
  }

  for (tool = 0; tool < VTK_3DG_NTOOLS; tool++) 
  {
    // assign flags using vtkTracker format
    int flags = 0;
    // is the tool plugged in?
    if (absent[tool]) 
    {
      flags |= TR_MISSING;
    }
    else 
    {
      if (saturated[tool]) 
      { 
        flags |= TR_OUT_OF_VIEW;  
      }
      if( !inMotionBox[tool] )
      {
        flags |= TR_OUT_OF_VOLUME;
      }
    }

    // if tracking relative to another tool
    if (this->ReferenceTool >= 0 && tool != this->ReferenceTool) 
    {
      if (!absent[tool]) 
      {
        if (absent[this->ReferenceTool]) 
        {
          flags |= TR_OUT_OF_VIEW;
        }
      }
      this->RelativeTransform(record[tool],referenceTransform,record[tool]);
    }

    this->TransformToMatrixd(record[tool],*this->SendMatrix->Element);
    this->SendMatrix->Transpose();

    // debug print out the orientations for comparison.
    //std::cout << "Tool " << tool << ": \n";
    //std::cout << "Quat: " << record[tool].q[0] << ", " << record[tool].q[1] << ", "
    //  << record[tool].q[2] << ", " << record[tool].q[3] << "\n";
    //std::cout << "Rot Matrix: \n";
    //std::cout << record[tool].s[0][0] << ", " << record[tool].s[0][1] << ", " << record[tool].s[0][2] << "\n";
    //std::cout << record[tool].s[1][0] << ", " << record[tool].s[1][1] << ", " << record[tool].s[1][2] << "\n";
    //std::cout << record[tool].s[2][0] << ", " << record[tool].s[2][1] << ", " << record[tool].s[2][2] << "\n";
    //std::cout << "Angles: " << record[tool].a << ", " << record[tool].e << ", " << record[tool].r << "\n";

    double tooltimestamp = nextcount;

    // due to legacy issues, the value needs to be shifted 8 bits.
    double qDouble = (double) (record[tool].quality >> 8);
    
    //debug:
    //std::cout << "Quality Tool #" << tool << ": " <<  qDouble << std::endl;

    this->ToolUpdate(tool,this->SendMatrix,flags,tooltimestamp, qDouble );

    // print out time stamp.
    // std::cout << "Tool: " << tool << ", Time stamp: " << tooltimestamp << std::endl;
  } /* for each tool */
}
//----------------------------------------------------------------------------
// Protected Methods

// helper method to strip whitespace
static char *vtkStripWhitespace(char *text)
{
  int n = strlen(text);
  // strip from right
  while (--n >= 0) {
    if (isspace(text[n])) {
      text[n] = '\0';
    }
    else {
      break;
    }
  }
  // strip from left
  while (isspace(*text)) {
    text++;
  }
  return text;
}

void vtkAscension3DGTracker::errorHandler(int error, char *func)
{
  char			buffer[1024];
  char			*pBuffer = &buffer[0];
  int				currentError = error;
  int				nextError;

  do{
    nextError = GetErrorText(currentError, pBuffer, sizeof(buffer), SIMPLE_MESSAGE);

    vtkWarningMacro(<< buffer);
    if( func )
    {
      std::cerr << "API Error (" << func << "): " << buffer << std::endl;
    }
    else
    {
      std::cerr << "API Error: " << buffer << std::endl;
    }

    currentError = nextError;
  }while(currentError!=BIRD_ERROR_SUCCESS);
}

void vtkAscension3DGTracker::RelativeTransform(const DOUBLE_ALL_TIME_STAMP_Q_RECORD aRecord, const DOUBLE_ALL_TIME_STAMP_Q_RECORD *bRecord, DOUBLE_ALL_TIME_STAMP_Q_RECORD cRecord)
{
  double f,x,y,z,w1,x1,y1,z1,w2,x2,y2,z2;

  // Note: There is a bug in the Ascension quat code.  The quaternion returned is the 
  //       of the orientation returned via euler angles.  i.e. the vector portion of 
  //       the quat is multiplied by -1.  We fix this here:

  w1 = bRecord->q[0];
  x1 = - bRecord->q[1];
  y1 = - bRecord->q[2];
  z1 = - bRecord->q[3];

  w2 = aRecord.q[0];
  x2 = - aRecord.q[1];
  y2 = - aRecord.q[2];
  z2 = - aRecord.q[3];

  // for rotation part of transformation: q = q1\q2  (divide on the right to get new orientation)
  cRecord.q[0] = w1*w2 + x1*x2 + y1*y2 + z1*z2;
  cRecord.q[1] = w1*x2 - x1*w2 - y1*z2 + z1*y2;
  cRecord.q[2] = w1*y2 + x1*z2 - y1*w2 - z1*x2;
  cRecord.q[3] = w1*z2 - x1*y2 + y1*x2 - z1*w2;           

  // several steps required to calculate new translation:

  // distance between tools
  x = aRecord.x - bRecord->x;
  y = aRecord.y - bRecord->y;
  z = aRecord.z - bRecord->z;

  // q = q1\q*q1 (apply inverse of reference tranformation to distance)

  // first:  qtmp = q1\q
  w2 = x1*x + y1*y + z1*z;
  x2 = w1*x - y1*z + z1*y;
  y2 = w1*y + x1*z - z1*x;
  z2 = w1*z - x1*y + y1*x;

  // next:  q = qtmp*q1
  x = w2*x1 + x2*w1 + y2*z1 - z2*y1;
  y = w2*y1 - x2*z1 + y2*w1 + z2*x1;
  z = w2*z1 + x2*y1 - y2*x1 + z2*w1;

  // find the normalization factor for q1
  f = 1.0f/(w1*w1 + x1*x1 + y1*y1 + z1*z1);
  cRecord.x = x*f;
  cRecord.y = y*f;
  cRecord.z = z*f;

  cRecord.quality = aRecord.quality+bRecord->quality;
  cRecord.time = aRecord.time;
}

void vtkAscension3DGTracker::TransformToMatrixd(const DOUBLE_ALL_TIME_STAMP_Q_RECORD trans, double matrix[16])
{
#if USE_EULER
  double sinRoll, sinPitch, sinYaw, cosRoll, cosPitch, cosYaw;

  sinRoll  = sin(vtkMath::RadiansFromDegrees(trans.r));
  sinPitch = sin(vtkMath::RadiansFromDegrees(trans.e));
  sinYaw   = sin(vtkMath::RadiansFromDegrees(trans.a));
  cosRoll  = cos(vtkMath::RadiansFromDegrees(trans.r));
  cosPitch = cos(vtkMath::RadiansFromDegrees(trans.e));
  cosYaw   = cos(vtkMath::RadiansFromDegrees(trans.a));


  // Fill in the matrix using the description from pg. 128 of Ascension guide.
  matrix[0]  = cosPitch * cosYaw;  // rotMatrix[0][0]
  matrix[1]  = cosPitch * sinYaw; // rotMatrix[0][1]
  matrix[2]  = - sinPitch; // rotMatrix[0][2]
  matrix[3]  = 0; 
  matrix[4]  = sinRoll * sinPitch * cosYaw - cosRoll * sinYaw; // rotMatrix[1][0]
  matrix[5]  = sinRoll * sinPitch * sinYaw + cosRoll * cosYaw; // rotMatrix[1][1]
  matrix[6]  = sinRoll * cosPitch; // rotMatrix[1][2]
  matrix[7]  = 0; 
  matrix[8]  = cosRoll * sinPitch * cosYaw + sinRoll * sinYaw; // rotMatrix[2][0] 
  matrix[9]  = cosRoll * sinPitch * sinYaw - sinRoll * cosYaw; // rotMatrix[2][1]
  matrix[10] = cosRoll * cosPitch; // rotMatrix[2][2]
  matrix[11] = 0;
  matrix[12] = trans.x;
  matrix[13] = trans.y;
  matrix[14] = trans.z;
  matrix[15] = 1;
#else
  double ww, xx, yy, zz, wx, wy, wz, xy, xz, yz, ss, rr, f;

  // Note: There is a bug in the Ascension quat code.  The quaternion returned is the 
  //       of the orientation returned via euler angles.  i.e. the vector portion of 
  //       the quat is multiplied by -1.  We fix this here:
  double q[4];
  q[0] = trans.q[0];
  q[1] = trans.q[1] * -1;
  q[2] = trans.q[2] * -1;
  q[3] = trans.q[3] * -1;


  // Determine some calculations done more than once.
  /*ww = trans.q[0] * trans.q[0];
  xx = trans.q[1] * trans.q[1];
  yy = trans.q[2] * trans.q[2];
  zz = trans.q[3] * trans.q[3];
  wx = trans.q[0] * trans.q[1];
  wy = trans.q[0] * trans.q[2];
  wz = trans.q[0] * trans.q[3];
  xy = trans.q[1] * trans.q[2];
  xz = trans.q[1] * trans.q[3];
  yz = trans.q[2] * trans.q[3];*/

  ww = q[0] * q[0];
  xx = q[1] * q[1];
  yy = q[2] * q[2];
  zz = q[3] * q[3];
  wx = q[0] * q[1];
  wy = q[0] * q[2];
  wz = q[0] * q[3];
  xy = q[1] * q[2];
  xz = q[1] * q[3];
  yz = q[2] * q[3];

  rr = xx + yy + zz;
  ss = (ww - rr)*0.5;
  // Normalization factor
  f = 2.0/(ww + rr);

  // Fill in the matrix.
  matrix[0]  = ( ss + xx)*f;
  matrix[1]  = ( wz + xy)*f;
  matrix[2]  = (-wy + xz)*f;
  matrix[3]  = 0;
  matrix[4]  = (-wz + xy)*f;
  matrix[5]  = ( ss + yy)*f;
  matrix[6]  = ( wx + yz)*f;
  matrix[7]  = 0;
  matrix[8]  = ( wy + xz)*f;
  matrix[9]  = (-wx + yz)*f;
  matrix[10] = ( ss + zz)*f;
  matrix[11] = 0;
  matrix[12] = trans.x;
  matrix[13] = trans.y;
  matrix[14] = trans.z;
  matrix[15] = 1;
#endif
}

void vtkAscension3DGTracker::EnableToolPorts()
{
  char temp[20];
  for(sensorID=0;sensorID<System.numberSensors;sensorID++)
  {
    if(pSensor[sensorID].attached)
    {
#if defined(macintosh)
      _itoa(pSensor[sensorID].serialNumber,temp,15);
#else
      snprintf(temp, sizeof(temp), "%d",pSensor[sensorID].serialNumber);
#endif
      temp[19] = '\0';
      this->Tools[sensorID]->SetToolSerialNumber(temp);
      this->Tools[sensorID]->SetToolManufacturer("Ascension 3DG");
      this->Tools[sensorID]->SetToolPartNumber(temp);
      this->Tools[sensorID]->SetToolType(temp);
      PortEnabled[sensorID] = 1;
    }
    else {
      //this->Tools[sensorID]->SetToolSerialNumber("");
      //this->Tools[sensorID]->SetToolManufacturer("");
      //this->Tools[sensorID]->SetToolPartNumber("");
      PortEnabled[sensorID] = 0;
    }
  }
}
void vtkAscension3DGTracker::DisableToolPorts()
{ 
  for(sensorID=0;sensorID<System.numberSensors;sensorID++) {
    this->Tools[sensorID]->SetToolSerialNumber("");
    this->Tools[sensorID]->SetToolManufacturer("");
    this->Tools[sensorID]->SetToolPartNumber("");
    PortEnabled[sensorID] = 0;
  }
}

void vtkAscension3DGTracker::EnableTransmitter()
{
  for(short id=0;id<System.numberTransmitters;id++)
  {
    if((pXmtr+id)->attached)
    {
      errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
      if(errorCode!=BIRD_ERROR_SUCCESS) 
      {
        errorHandler(errorCode, "vtkAscension3DGTracker::EnableTransmitter() - SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id))");
      }
      break; // only enable first tool found
    }
  }
}

void vtkAscension3DGTracker::DisableTransmitter()
{ 
  short id = -1;
  errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
  if(errorCode!=BIRD_ERROR_SUCCESS) 
  {
    errorHandler(errorCode, "vtkAscension3DGTracker::DisableTransmitter() - SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id))");
  }
}

int  vtkAscension3DGTracker::RestoreConfiguration(char * filename)
{
  errorCode = RestoreSystemConfiguration(filename);
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SaveConfiguration(char * filename)
{
  errorCode = SaveSystemConfiguration(filename);
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetPowerLineFrequency(double pl)
{
  errorCode = SetSystemParameter(POWER_LINE_FREQUENCY, &pl, sizeof(pl));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int vtkAscension3DGTracker::GetPowerLineFrequency()
{
  double pl, *pBuffer = &pl;
  errorCode = GetSystemParameter(POWER_LINE_FREQUENCY, pBuffer, sizeof(pl));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0;}
  return pl;
}

int vtkAscension3DGTracker::SetAGCMode(AGC_MODE_TYPE agc)
{
  errorCode = SetSystemParameter(AGC_MODE, &agc, sizeof(agc));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

AGC_MODE_TYPE vtkAscension3DGTracker::GetAGCMode()
{
  AGC_MODE_TYPE agc, *pBuffer = &agc;
  errorCode = GetSystemParameter(AGC_MODE, pBuffer, sizeof(agc));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode);}
  return agc;
}

int vtkAscension3DGTracker::SetMeasurementRate(double rate)
{
  errorCode = SetSystemParameter(MEASUREMENT_RATE, &rate, sizeof(rate));
  if(errorCode != BIRD_ERROR_SUCCESS) 
  { 
    errorHandler(errorCode, "vtkAscension3DGTracker::SetMeasurementRate(double rate) - SetSystemParameter(MEASUREMENT_RATE, &rate, sizeof(rate))"); 
    return 0; 
  }
  Sleep(2000);
  return 1;
}

double vtkAscension3DGTracker::GetMeasurementRate()
{
  double rate, *pBuffer = &rate;
  errorCode = GetSystemParameter(MEASUREMENT_RATE, pBuffer, sizeof(rate));
  if(errorCode != BIRD_ERROR_SUCCESS) 
  { 
    errorHandler(errorCode, "vtkAscension3DGTracker::GetMeasurementRate() - GetSystemParameter(MEASUREMENT_RATE, pBuffer, sizeof(rate))"); 
    return 0; 
  }
  Sleep(5);
  return rate;
}

int  vtkAscension3DGTracker::SetMaximumRange(double range)
{
  errorCode = SetSystemParameter(MAXIMUM_RANGE, &range, sizeof(range));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

double  vtkAscension3DGTracker::GetMaximumRange()
{
  double range, *pBuffer = &range;
  errorCode = GetSystemParameter(MAXIMUM_RANGE, pBuffer, sizeof(range));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return range;
}

int  vtkAscension3DGTracker::SetMetric(bool metric)
{
  BOOL metricBOOL = 0;
  if (metric == true) {
    metricBOOL = 1;
  }
  errorCode = SetSystemParameter(METRIC, &metricBOOL, sizeof(metricBOOL));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

bool vtkAscension3DGTracker::GetMetric()
{
  BOOL metric, *pBuffer = &metric;
  errorCode = GetSystemParameter(METRIC, pBuffer, sizeof(metric));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  if (metric == 1)
    return true;
  else
    return false;
}

int  vtkAscension3DGTracker::SetTransmitter(short tx)
{
  errorCode = SetSystemParameter(SELECT_TRANSMITTER, &tx, sizeof(tx));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

short  vtkAscension3DGTracker::GetTransmitter( )
{
  short tx, *pBuffer = &tx;
  errorCode = GetSystemParameter(SELECT_TRANSMITTER, pBuffer, sizeof(tx));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return tx;
}

char * vtkAscension3DGTracker::GetBoardModel() {
  return pBoard->modelString;
}

unsigned short vtkAscension3DGTracker::GetBoardFirmwareRevision() {
  return pBoard->firmwareRevision;
}

unsigned short vtkAscension3DGTracker::GetBoardRevision() {
  return pBoard->revision;
}

unsigned short vtkAscension3DGTracker::GetBoardFirmwareNumber() {
  return pBoard->firmwareNumber;
}

DEVICE_TYPES vtkAscension3DGTracker::GetSensorType() {
  return pSensor->type;
}

// SET SENSOR CONFIGURATION

int  vtkAscension3DGTracker::SetSensorDataFormat(int sensorID, DATA_FORMAT_TYPE buffer)
{
  errorCode = SetSensorParameter(sensorID, DATA_FORMAT, &buffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorAngleAlign(int sensorID, DOUBLE_ANGLES_RECORD buffer)
{
  DOUBLE_ANGLES_RECORD *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, ANGLE_ALIGN, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorHemisphere(int sensorID,HEMISPHERE_TYPE buffer)
{
  HEMISPHERE_TYPE *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, HEMISPHERE, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorFilterACWideNotch(int sensorID, BOOL buffer)
{
  BOOL *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, FILTER_AC_WIDE_NOTCH, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorFilterACNarrowNotch(int sensorID, BOOL buffer)
{
  BOOL  *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, FILTER_AC_NARROW_NOTCH, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorFilterDCAdaptive(int sensorID,double buffer)
{
  double *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, FILTER_DC_ADAPTIVE, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorFilterAlphaParamaters(int sensorID,ADAPTIVE_PARAMETERS buffer)
{
  ADAPTIVE_PARAMETERS  *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, FILTER_ALPHA_PARAMETERS, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorFilterLargeChange(int sensorID, BOOL buffer)
{
  BOOL *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, FILTER_LARGE_CHANGE, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int  vtkAscension3DGTracker::SetSensorQuality(int sensorID,QUALITY_PARAMETERS buffer)
{
  QUALITY_PARAMETERS  *pBuffer = &buffer;
  errorCode = SetSensorParameter(sensorID, QUALITY, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

// GET SENSOR CONFIGURATION

DATA_FORMAT_TYPE  vtkAscension3DGTracker::GetSensorDataFormat(int sensorID)
{
  DATA_FORMAT_TYPE buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, DATA_FORMAT, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return buffer;
}

DOUBLE_ANGLES_RECORD  vtkAscension3DGTracker::GetSensorAngleAlign(int sensorID)
{
  DOUBLE_ANGLES_RECORD buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, ANGLE_ALIGN, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return buffer;
}

HEMISPHERE_TYPE  vtkAscension3DGTracker::GetSensorHemisphere(int sensorID)
{
  HEMISPHERE_TYPE buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, HEMISPHERE, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return buffer;
}


BOOL  vtkAscension3DGTracker::GetSensorFilterACWideNotch(int sensorID)
{
  BOOL buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, FILTER_AC_WIDE_NOTCH, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return buffer;
}

BOOL  vtkAscension3DGTracker::GetSensorFilterACNarrowNotch(int sensorID)
{
  BOOL buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, FILTER_AC_NARROW_NOTCH, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return buffer;
}

double  vtkAscension3DGTracker::GetSensorFilterDCAdaptive(int sensorID)
{
  double buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, FILTER_DC_ADAPTIVE, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return buffer;
}

ADAPTIVE_PARAMETERS  vtkAscension3DGTracker::GetSensorFilterAlphaParamaters(int sensorID)
{

  ADAPTIVE_PARAMETERS buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, FILTER_ALPHA_PARAMETERS, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return buffer;
}

BOOL vtkAscension3DGTracker::GetSensorFilterLargeChange(int sensorID)
{
  BOOL buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, FILTER_LARGE_CHANGE, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return buffer;
}

QUALITY_PARAMETERS vtkAscension3DGTracker::GetSensorQuality(int sensorID)
{
  QUALITY_PARAMETERS buffer, *pBuffer = &buffer;
  errorCode = GetSensorParameter(sensorID, QUALITY, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return buffer;
}

// GET TRANSMITTOR CONFIGURATION

DOUBLE_ANGLES_RECORD vtkAscension3DGTracker::GetTransmitterReferenceFrame(int transmitterID)
{
  DOUBLE_ANGLES_RECORD buffer, *pBuffer = &buffer;
  errorCode = GetTransmitterParameter(transmitterID, REFERENCE_FRAME, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return buffer;
}

BOOL vtkAscension3DGTracker::GetTransmitterXYZReferenceFrame(int transmitterID)
{
  BOOL buffer, *pBuffer = &buffer;
  errorCode = GetTransmitterParameter(transmitterID, XYZ_REFERENCE_FRAME, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return buffer;
}

// SET TRANSMITTOR CONFIGURATION

int vtkAscension3DGTracker::SetTransmitterReferenceFrame(int transmitterID, DOUBLE_ANGLES_RECORD buffer)
{
  DOUBLE_ANGLES_RECORD *pBuffer = &buffer;
  errorCode = SetTransmitterParameter(transmitterID, REFERENCE_FRAME, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int vtkAscension3DGTracker::SetTransmitterXYZReferenceFrame(int transmitterID,BOOL buffer)
{
  BOOL *pBuffer = &buffer;
  errorCode = SetTransmitterParameter(transmitterID, XYZ_REFERENCE_FRAME, pBuffer, sizeof(buffer));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }
  return 1;
}



int vtkAscension3DGTracker::SetSensorOnly(){
  AGC_MODE_TYPE agc = SENSOR_AGC_ONLY;
  errorCode = SetSystemParameter(AGC_MODE, &agc, sizeof(agc));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}
int vtkAscension3DGTracker::SetSensorAndTransmitter(){
  AGC_MODE_TYPE agc = TRANSMITTER_AND_SENSOR_AGC;
  errorCode = SetSystemParameter(AGC_MODE, &agc, sizeof(agc));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); return 0; }
  return 1;
}

int vtkAscension3DGTracker::GetAGCType() {
  AGC_MODE_TYPE agc, *pBuffer = &agc;
  errorCode = GetSystemParameter(AGC_MODE, pBuffer, sizeof(agc));
  if(errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode);}
  if (agc == TRANSMITTER_AND_SENSOR_AGC) {
    return 2;
  }
  else if (agc == SENSOR_AGC_ONLY) {
    return 1;
  }
  else
    return 0;
}


int  vtkAscension3DGTracker::SetSensorHemisphere(int sensorID, int hemisphere)
{
  HEMISPHERE_TYPE buffer;
  switch(hemisphere)
  {
  case 1:
    buffer = FRONT;
    break;
  case 2:
    buffer = BACK;
    break;
  case 3:
    buffer = TOP;
    break;
  case 4:
    buffer = BOTTOM;
    break;
  case 5:
    buffer = LEFT;
    break;
  case 6:
    buffer = RIGHT;
    break;
  default:
    buffer = FRONT;
    break;
  }
  return SetSensorHemisphere(sensorID, buffer);
}

int  vtkAscension3DGTracker::SetSensorFilterACWideNotch(int sensorID, bool buffer)
{
  BOOL Buff = 0;
  if (buffer == true) {
    Buff = 1;
  }
  return SetSensorFilterACWideNotch(sensorID, Buff);
}

int  vtkAscension3DGTracker::SetSensorAngleAlign(int sensorID, double angles[3])
{
  DOUBLE_ANGLES_RECORD buffer;
  buffer.a = angles[0];
  buffer.e = angles[1];
  buffer.r = angles[2];
  return SetSensorAngleAlign(sensorID,buffer);
}


int  vtkAscension3DGTracker::SetSensorFilterACNarrowNotch(int sensorID,  bool buffer)
{
  BOOL Buff = 0;
  if (buffer == true) {
    Buff = 1;
  }
  return SetSensorFilterACNarrowNotch(sensorID, Buff);
}

int  vtkAscension3DGTracker::SetSensorFilterAlphaParamaters(int sensorID,unsigned short alphaMin[7], unsigned short alphaMax[7], unsigned short vm[7], bool alphaOn)
{
  ADAPTIVE_PARAMETERS	buffer;
  int i;
  for (i = 0; i < 7; i++)
    buffer.alphaMax[i] = alphaMax[i];
  for (i = 0; i < 7; i++)
    buffer.alphaMin[i] = alphaMin[i];
  for (i = 0; i < 7; i++)
    buffer.vm[i] = vm[i];
  if (alphaOn == true)
    buffer.alphaOn = 1;
  else
    buffer.alphaOn = 0;
  return SetSensorFilterAlphaParamaters(sensorID, buffer);
}

int vtkAscension3DGTracker::SetSensorFilterLargeChange(int sensorID, bool buffer)
{
  BOOL Buff = 0;
  if (buffer == true)
  {
    Buff = 1;
  }
  return SetSensorFilterLargeChange(sensorID,Buff);
}

int  vtkAscension3DGTracker::SetSensorQuality(int sensorID, unsigned short slope, unsigned short offset, unsigned short sensitivity, unsigned short filter_alpha)
{
  QUALITY_PARAMETERS buffer;
  buffer.error_slope = slope;
  buffer.error_offset = offset;
  buffer.error_sensitivity = sensitivity;
  buffer.filter_alpha = filter_alpha;
  return SetSensorQuality(sensorID, buffer);
}

// 3 doubles
double * vtkAscension3DGTracker::GetSensorAngleAlignment(int sensorID) {
  static double ReturnValue[3];
  DOUBLE_ANGLES_RECORD buffer = GetSensorAngleAlign(sensorID);
  ReturnValue[0] = buffer.a;
  ReturnValue[1] = buffer.e;
  ReturnValue[2] = buffer.r;
  return ReturnValue;
}

char * vtkAscension3DGTracker::GetSensorHemisphereName(int sensorID) {
  HEMISPHERE_TYPE buffer = GetSensorHemisphere(sensorID);
  if (buffer == FRONT)
    strcpy(this->CommandReply, "FRONT");
  else if (buffer == BACK)
    strcpy(this->CommandReply, "BACK");
  else if (buffer == TOP)
    strcpy(this->CommandReply, "TOP");
  else if (buffer == BOTTOM)
    strcpy(this->CommandReply, "BOTTOM");
  else if (buffer == LEFT)
    strcpy(this->CommandReply, "LEFT");
  else if (buffer == RIGHT)
    strcpy(this->CommandReply, "RIGHT");
  else
    strcpy(this->CommandReply, "Error");
  return this->CommandReply;
}

// 7 unsigned short
unsigned short * vtkAscension3DGTracker::GetSensorFilterAlphaMax(int sensorID) {
  ADAPTIVE_PARAMETERS buffer = GetSensorFilterAlphaParamaters(sensorID);
  return buffer.alphaMax;
}
// 7 unsigned short
unsigned short * vtkAscension3DGTracker::GetSensorFilterAlphaMin(int sensorID) {
  ADAPTIVE_PARAMETERS buffer = GetSensorFilterAlphaParamaters(sensorID);
  return buffer.alphaMin;
}

// 7 unsigned short
unsigned short * vtkAscension3DGTracker::GetSensorFilterVM(int sensorID) {
  ADAPTIVE_PARAMETERS buffer = GetSensorFilterAlphaParamaters(sensorID);
  return buffer.vm;
}

int vtkAscension3DGTracker::GetSensorFilterAlphaOn(int sensorID) {
  ADAPTIVE_PARAMETERS buffer = GetSensorFilterAlphaParamaters(sensorID);
  return buffer.alphaOn;
}
// 4 unsigned shorts
unsigned short * vtkAscension3DGTracker::GetSensorQualityValues(int sensorID) {
  static unsigned short ReturnValue[4];
  QUALITY_PARAMETERS buffer = GetSensorQuality(sensorID);
  ReturnValue[0] = buffer.error_slope;
  ReturnValue[1] = buffer.error_offset;
  ReturnValue[2] = buffer.error_sensitivity;
  ReturnValue[3] = buffer.filter_alpha;
  return ReturnValue;
}

unsigned short vtkAscension3DGTracker::GetSensorQualityErrorSlope(int sensorID) {
  QUALITY_PARAMETERS buffer = GetSensorQuality(sensorID);
  return buffer.error_slope;
}

unsigned short vtkAscension3DGTracker::GetSensorQualityErrorOffset(int sensorID) {
  QUALITY_PARAMETERS buffer = GetSensorQuality(sensorID);
  return buffer.error_offset;
}

unsigned short vtkAscension3DGTracker::GetSensorQualityErrorSensitivity(int sensorID) {
  QUALITY_PARAMETERS buffer = GetSensorQuality(sensorID);
  return buffer.error_sensitivity;
}

unsigned short vtkAscension3DGTracker::GetSensorQualityFilterAlpha(int sensorID) {
  QUALITY_PARAMETERS buffer = GetSensorQuality(sensorID);
  return buffer.filter_alpha;
}

double * vtkAscension3DGTracker::GetTransmitterReferenceFrameAngle(int transmitterID) {
  static double ReturnValue[3];
  DOUBLE_ANGLES_RECORD buffer = GetTransmitterReferenceFrame(transmitterID);
  ReturnValue[0] = buffer.a;
  ReturnValue[1] = buffer.e;
  ReturnValue[2] = buffer.r;
  return ReturnValue;
}

int vtkAscension3DGTracker::SetTransmitterReferenceFrame(int transmitterID, double azimuth, double elevation, double roll)
{
  DOUBLE_ANGLES_RECORD buffer;
  buffer.a = azimuth;
  buffer.e = elevation;
  buffer.r = roll;
  return SetTransmitterReferenceFrame(transmitterID, buffer);
}

int vtkAscension3DGTracker::SetTransmitterXYZReferenceFrame(int transmitterID, bool buffer)
{
  BOOL Buff = 0;
  if (buffer == true)
  {
    Buff = 1;
  }
  return SetTransmitterXYZReferenceFrame(transmitterID, Buff);
}