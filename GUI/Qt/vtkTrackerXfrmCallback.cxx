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

#include <QtGui>

#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkMath.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkTimerLog.h>

#include <vtkTracker.h>
#include <vtkTrackerTool.h>

#include "ndXfrms.h"

#include "QTracker.h"
#include "vtkTrackerXfrmCallback.h"


vtkTrackerXfrmCallback* vtkTrackerXfrmCallback::New()
{
  return vtkTrackerXfrmCallback::New(0, false);
}

vtkTrackerXfrmCallback* vtkTrackerXfrmCallback::New(QTracker *parent, int port)
{
  return new vtkTrackerXfrmCallback(parent, port);
}

vtkTrackerXfrmCallback::vtkTrackerXfrmCallback(QTracker *parent, int port)
{
  this->m_parent = parent;
  this->m_port = port;
  this->m_nXfrms = 0;
  for( int i =0; i < NUM_SAMPLES_FREQUENCY; i++ )
    this->m_currTime[i] = 0.0;
}

vtkTrackerXfrmCallback::~vtkTrackerXfrmCallback()
{
}

void vtkTrackerXfrmCallback::Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData))
{
  ndQuatTransformation xfrm;
  if( eventId == vtkCommand::ModifiedEvent )
  {
    vtkTrackerTool *tool = static_cast<vtkTrackerTool*>(caller);
    // compute the effective frequency.
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

    if( tool->IsToolInfoUpdated() )
    {
      // launch the update events.
      m_parent->UpdateToolInfo(this->m_port);
      // reset the flag.
      tool->SetToolInfoUpdated(0);
      // debug:
      // fprintf(stdout, "Port %d had its info updated in XfrmCallback.\n", this->m_port);
    }

    if( tool->IsBrokenSensor() )
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("Tool sensor is broken.") );
    }
    else if( tool->IsMissing() ) // at NDI, this is equivalent to occupied.
    {
      // do nothing do not send.
    }
    else if( tool->IsBrokenSensor() )
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("Tool sensor is broken.") );
    }
    else if( tool->IsProcessingException() )
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("Processing exception occurred.") );
    }
    else if (tool->IsBadFit() )
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("Tool algorithm fit is bad.") );
    }
    else if( tool->IsTooFewMarkers() )
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("Too few markers.") );
    }
    else if( tool->IsTooMuchIR() )
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("IR Interference.") );
    }
    else if( tool->IsOutOfView() ) // at NDI, this is equivalent to missing.
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("Tool is out of view.") );
    }
    else if( tool->IsOutOfVolume() )
    {
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), QString("Tool is out of volume.") );
    }    
    else //if( !tool->IsMissing() && !tool->IsOutOfView() && !tool->IsOutOfVolume() )
    {
      double pos[3], quat[4];
      double rotMat[3][3];
      double quality = 0.0;

      // extract the rotation matrix
      rotMat[0][0] = tool->GetTransform()->GetMatrix()->GetElement(0,0);
      rotMat[0][1] = tool->GetTransform()->GetMatrix()->GetElement(0,1);
      rotMat[0][2] = tool->GetTransform()->GetMatrix()->GetElement(0,2);
      rotMat[1][0] = tool->GetTransform()->GetMatrix()->GetElement(1,0);
      rotMat[1][1] = tool->GetTransform()->GetMatrix()->GetElement(1,1);
      rotMat[1][2] = tool->GetTransform()->GetMatrix()->GetElement(1,2);
      rotMat[2][0] = tool->GetTransform()->GetMatrix()->GetElement(2,0);
      rotMat[2][1] = tool->GetTransform()->GetMatrix()->GetElement(2,1);
      rotMat[2][2] = tool->GetTransform()->GetMatrix()->GetElement(2,2);
      // get the quat rotation.
      vtkMath::Matrix3x3ToQuaternion(rotMat, quat);
      xfrm.rotation.q0 = quat[0];
      xfrm.rotation.qx = quat[1];
      xfrm.rotation.qy = quat[2];
      xfrm.rotation.qz = quat[3];

      // get the translation
      tool->GetTransform()->GetPosition(pos);
      xfrm.translation.x = pos[0];
      xfrm.translation.y = pos[1];
      xfrm.translation.z = pos[2];

      quality = tool->GetErrorValue();

      // send out.
      m_parent->UpdateToolTransform(this->m_port, tool->GetFrame(), xfrm, this->m_effectiveFrequency, quality);
    }  
  }
}