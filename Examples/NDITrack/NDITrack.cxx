#include <windows.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>

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
      if( !tool->IsMissing() && !tool->IsOutOfView() && !tool->IsOutOfVolume() )
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
      Sleep(25/2);
    }
  }
  outFile.close();
  
  tracker->StopTracking();
  std::cout << "Tracking stopped." << std::endl;
  std::cout << "Internal Update Rate: " << tracker->GetInternalUpdateRate() << std::endl;
  tracker->Delete();
}