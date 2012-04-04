/*
Copyright (C) 2011-2012, Northern Digital Inc. All rights reserved.
 
All Northern Digital Inc. (“NDI”) Media and/or Sample Code and/or Sample Code
Documentation (collectively referred to as “Sample Code”) is licensed and provided "as
is” without warranty of any kind. The licensee, by use of the Sample Code, warrants to
NDI that the Sample Code is fit for the use and purpose for which the licensee intends to
use the Sample Code. NDI makes no warranties, express or implied, that the functions
contained in the Sample Code will meet the licensee’s requirements or that the operation
of the programs contained therein will be error free. This warranty as expressed herein is
exclusive and NDI expressly disclaims any and all express and/or implied, in fact or in
law, warranties, representations, and conditions of every kind pertaining in any way to
the Sample Code licensed and provided by NDI hereunder, including without limitation,
each warranty and/or condition of quality, merchantability, description, operation,
adequacy, suitability, fitness for particular purpose, title, interference with use or
enjoyment, and/or non infringement, whether express or implied by statute, common law,
usage of trade, course of dealing, custom, or otherwise. No NDI dealer, distributor, agent
or employee is authorized to make any modification or addition to this warranty.

In no event shall NDI nor any of its employees be liable for any direct, indirect,
incidental, special, exemplary, or consequential damages, sundry damages or any
damages whatsoever, including, but not limited to, procurement of substitute goods or
services, loss of use, data or profits, or business interruption, however caused. In no
event shall NDI’s liability to the licensee exceed the amount paid by the licensee for the
Sample Code or any NDI products that accompany the Sample Code. The said limitations
and exclusions of liability shall apply whether or not any such damages are construed as
arising from a breach of a representation, warranty, guarantee, covenant, obligation,
condition or fundamental term or on any theory of liability, whether in contract, strict
liability, or tort (including negligence or otherwise) arising in any way out of the use of
the Sample Code even if advised of the possibility of such damage. In no event shall
NDI be liable for any claims, losses, damages, judgments, costs, awards, expenses or
liabilities of any kind whatsoever arising directly or indirectly from any injury to person
or property, arising from the Sample Code or any use thereof.
*/

#include <time.h>
#include <string>
#include <iostream>
#include <fstream>

/* =========== windows includes */
#if defined(WIN32) || defined(_WIN32)
#include <windows.h>

/* =========== unix includes */
#elif defined(unix) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>

/* =========== mac includes */
#elif defined(macintosh)
//TODO: add appropriate headers for sleep here.
#endif


#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkTrackerTool.h>
#include <vtkNDITracker.h>
#include <vtkTimerLog.h>

#define NUM_SAMPLES_FREQUENCY 20

class vtkTransformUpdateCallback : public vtkCommand
{
public:
  static vtkTransformUpdateCallback *New()
  {
    vtkTransformUpdateCallback *cb = new vtkTransformUpdateCallback;
    cb->m_nXfrms = 0;
    return cb;
  }

  virtual void Execute(vtkObject *caller, unsigned long eventId,
    void *vtkNotUsed(callData))
  {
    if( vtkCommand::ModifiedEvent == eventId)
    {
      vtkTrackerTool *tool = static_cast<vtkTrackerTool*>(caller);
      this->m_nXfrms++;
      double newtime = vtkTimerLog::GetUniversalTime();
      double difftime = newtime - this->m_currTime[this->m_nXfrms%NUM_SAMPLES_FREQUENCY];
      this->m_currTime[this->m_nXfrms%NUM_SAMPLES_FREQUENCY] = newtime;
      if( m_nXfrms > NUM_SAMPLES_FREQUENCY )
      {
        this->m_effectiveFrequency = NUM_SAMPLES_FREQUENCY/difftime;
      }
      else
      {
        this->m_effectiveFrequency = -1.0;
      }
      if( !tool->IsMissing() && !tool->IsOutOfView() && !tool->IsOutOfVolume() && !tool->IsBrokenSensor() )
      {
      double pos[3], quat[4];
      tool->GetTransform()->GetOrientationWXYZ(quat);
      tool->GetTransform()->GetPosition(pos);

      std::cout.setf(std::ios_base::fixed);
      std::cout << this->m_nXfrms << ". " << tool->GetToolPartNumber()
        << " updated (t = " << tool->GetTimeStamp() << ").  Effective frequency: "
        << this->m_effectiveFrequency << ". Xfrm: "
        << quat[0] << "," << quat[1] << "," << quat[2] << "," << quat[3] << ","
        << pos[0] << "," << pos[1] << "," << pos[2] << std::endl;
      }
      else if( tool->IsBrokenSensor() )
      {
        std::cout.setf(std::ios_base::fixed);
        std::cout << this->m_nXfrms << ". " << tool->GetToolPartNumber()
        << " updated (t = " << tool->GetTimeStamp() << ").  Effective frequency: "
        << this->m_effectiveFrequency << ". Xfrm: Broken Sensor." << std::endl;
      }
      else
      {
        std::cout.setf(std::ios_base::fixed);
        std::cout << this->m_nXfrms << ". " << tool->GetToolPartNumber()
        << " updated (t = " << tool->GetTimeStamp() << ").  Effective frequency: "
        << this->m_effectiveFrequency << ". Xfrm: Missing." << std::endl;
      }
    }
  }
private:
  int m_nXfrms;
  double m_currTime[NUM_SAMPLES_FREQUENCY];
  double m_effectiveFrequency;
};


int main( int argc, char *argv[] )
{
  int milliseconds = int(25/2);

  vtkNDITracker* tracker = vtkNDITracker::New();
  vtkSmartPointer<vtkTransformUpdateCallback> cb = vtkSmartPointer<vtkTransformUpdateCallback>::New();
  double pos[3];
  double quat[4];

  tracker->SetBaudRate(115200);
  std::cout << "Probe tracker." << std::endl;
  if( tracker->Probe() )
  {
    std::cout << "Tracker is connected." << std::endl;
  }
  else
  {
    std::cout << "Tracker NOT connected - Quitting program." << std::endl;
    return -1;
  }

  std::cout << "Start Tracking." << std::endl;
  tracker->StartTracking();
  tracker->GetTool(0)->AddObserver(vtkCommand::ModifiedEvent, cb);


  ofstream outFile;
  outFile.open("xfrms.csv");
  outFile.setf(std::ios_base::fixed);
  clock_t start, curr;
  double tElapsed = 0;
  start = clock();
  for(int i=0; i < 500; i++ )
  {
    tracker->Update();
    curr = clock();
    tElapsed = ((double)(curr - start)) / CLOCKS_PER_SEC;
    if( tracker->GetTool(0)->IsMissing() )
    {
      outFile << i << " Tool is missing.(t=" << tElapsed << "): Timestamp "
              << tracker->GetTool(0)->GetTimeStamp() << ", Last Update Time: "
              << tracker->GetUpdateTimeStamp() << std::endl;
    }
    else
    {
      tracker->GetTool(0)->GetTransform()->GetOrientationWXYZ(quat);
      tracker->GetTool(0)->GetTransform()->GetPosition(pos);
      outFile << i << " Xfrm (t=" << tElapsed << "): Timestamp " << tracker->GetTool(0)->GetTimeStamp() << ","
              << quat[0] << "," << quat[1] << "," << quat[2] << "," << quat[3] << ","
              << pos[0] << "," << pos[1] << "," << pos[2] << ", Last Update Time: "
              << tracker->GetUpdateTimeStamp() << std::endl;

#if defined(WIN32) || defined(_WIND32)
      Sleep(milliseconds);
#elif defined(unix) || defined(__unix__) || defined(__APPLE__)
#ifdef USE_NANOSLEEP
      struct timespec sleep_time, dummy;
      sleep_time.tv_sec = milliseconds/1000;
      sleep_time.tv_nsec = (milliseconds - sleep_time.tv_sec*1000)*1000000;
      nanosleep(&sleep_time,&dummy);
#else /* use usleep instead */
      /* some unices like IRIX can't usleep for more than 1 second,
     so break usleep into 500 millisecond chunks */
      while (milliseconds > 500) {
        usleep(500000);
        milliseconds -= 500;
      }
      usleep(milliseconds*1000);
#endif
#elif defined(macintosh)
      //TODO: figure this out.
#endif

    }
  }
  outFile.close();

  tracker->StopTracking();
  std::cout << "Tracking stopped." << std::endl;
  std::cout << "Internal Update Rate: " << tracker->GetInternalUpdateRate() << std::endl;
  tracker->Delete();
}
