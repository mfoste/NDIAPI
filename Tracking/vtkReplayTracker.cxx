/* ============================================================================

  File: vtkReplayTracker.cxx
  Author: Chris Wedlake <cwedlake@robarts.ca>
  Language: C++
  Description: 
    This class represents a fake tracking system with tools that have
    behaviour determined by inputted transforms. This allows someonew who doesn't have 
	access current to a tracking system to test code that relies on having one active.

==========================================================================

  Copyright (c) Chris Wedlake, cwedlake@robarts.ca

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

#include "vtkReplayTracker.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTrackerTool.h"

#include "vtkTimerLog.h"

vtkReplayTracker* vtkReplayTracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkReplayTracker");
  if(ret)
    {
    return (vtkReplayTracker*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkReplayTracker;
}

vtkReplayTracker::vtkReplayTracker() 
{
#if (VTK_MAJOR_VERSION <= 4)
	this->TimeStamp = vtkTimerLog::GetCurrentTime();
#else
	this->TimeStamp = vtkTimerLog::GetUniversalTime();
#endif
	this->currentFrame = 0;
	this->InternalTransform = vtkTransform::New();
	this->SerialPort = 0;
	this->SetNumberOfTools(4);
	this->updateRate = 30;

	// Setup tool info for fake tools
	this->Tools[0]->SetToolType("Pointer");
	this->Tools[0]->SetToolRevision("1.0");
	this->Tools[0]->SetToolManufacturer("Robarts");
	this->Tools[0]->SetToolPartNumber("Device1");
	this->Tools[0]->SetToolSerialNumber("RRID1");

	this->Tools[1]->SetToolType("Pointer");
	this->Tools[1]->SetToolRevision("1.0");
	this->Tools[1]->SetToolManufacturer("Robarts");
	this->Tools[1]->SetToolPartNumber("Device2");
	this->Tools[1]->SetToolSerialNumber("RRID2");

	this->Tools[2]->SetToolType("Pointer");
	this->Tools[2]->SetToolRevision("1.0");
	this->Tools[2]->SetToolManufacturer("Robarts");
	this->Tools[2]->SetToolPartNumber("Device3");
	this->Tools[2]->SetToolSerialNumber("RRID3");

	this->Tools[3]->SetToolType("Pointer");
	this->Tools[3]->SetToolRevision("1.0");
	this->Tools[3]->SetToolManufacturer("Robarts");
	this->Tools[3]->SetToolPartNumber("Device4");
	this->Tools[3]->SetToolSerialNumber("RRID4");

	this->replayData= vtkReplayData::New();

}

vtkReplayTracker::~vtkReplayTracker()
{
	this->StopTracking();
	this->InternalTransform->Delete();
	this->replayData->Delete();
}

int vtkReplayTracker::Probe()
{
	return 1;
}

int vtkReplayTracker::InternalStartTracking()
{
	this->currentFrame = 0;
	return 1;
}

int vtkReplayTracker::InternalStopTracking()
{
	return 1;
}

void vtkReplayTracker::ChangeCurrentFrame(int frame) 
{
	this->replayData->ChangeFrame(frame);
}

void vtkReplayTracker::ForceRestart() 
{
	this->replayData->ChangeFrame(0);
}

void vtkReplayTracker::LoadData(char * filename, int tool)
{
	this->replayData->LoadData(filename, tool);
}


// Spins the tools around different axis to fake movement
void vtkReplayTracker::InternalUpdate()
{

#if (VTK_MAJOR_VERSION <= 4)
    double newtime = vtkTimerLog::GetCurrentTime();
#else
    double newtime = vtkTimerLog::GetUniversalTime();
#endif

	if (newtime <= this->NextMTime)
		return;

	if (this->currentFrame++ > 355559)
	{
		this->currentFrame = 0;
	}

	for (int tool = 0; tool < 4; tool++) 
	{
		int flags = 0;
  
		if (this->replayData->GetCurrentData(tool) != NULL)
		{
			this->ToolUpdate(tool, this->replayData->GetCurrentData(tool),flags,this->GetMTime());   
		}
		else 
		{
			vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
			flags |= TR_MISSING;
			this->ToolUpdate(tool, matrix ,flags, this->GetMTime());   
		}
	}
	this->replayData->AdvanceFrame();
	this->Modified();
	this->NextMTime = newtime+(1/this->updateRate);
  }


  void vtkReplayTracker::SetUpdateRate(int rate)
  {
	  this->updateRate=rate;
  }

  int vtkReplayTracker::GetUpdateRate()
  {
	  return this->updateRate;
  }