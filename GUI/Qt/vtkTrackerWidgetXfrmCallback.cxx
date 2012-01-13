#include <QtGui>

#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkMath.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkTimerLog.h>

#include <vtkTrackerTool.h>

#include "ndXfrms.h"

#include "vtkTrackerWidget.h"
#include "vtkTrackerWidgetXfrmCallback.h"


vtkTrackerWidgetXfrmCallback* vtkTrackerWidgetXfrmCallback::New()
{
  return vtkTrackerWidgetXfrmCallback::New(0, false);
}

vtkTrackerWidgetXfrmCallback* vtkTrackerWidgetXfrmCallback::New(vtkTrackerWidget *parent, int port)
{
  return new vtkTrackerWidgetXfrmCallback(parent, port);
}

vtkTrackerWidgetXfrmCallback::vtkTrackerWidgetXfrmCallback(vtkTrackerWidget *parent, int port)
{
  this->m_parent = parent;
  this->m_port = port;
}

vtkTrackerWidgetXfrmCallback::~vtkTrackerWidgetXfrmCallback()
{
}

void vtkTrackerWidgetXfrmCallback::Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData))
{
  QuatTransformation xfrm;
  if( vtkCommand::ModifiedEvent == eventId )
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

    if( tool->IsMissing() )
    {
      // do nothing do not send.
    }
    else if( tool->IsOutOfView() )
    {
      m_parent->UpdateToolTransform(this->m_port, QString("Tool is out of view.") );
    }
    else if( tool->IsOutOfVolume() )
    {
      m_parent->UpdateToolTransform(this->m_port, QString("Tool is out of volume.") );
    }
    else //if( !tool->IsMissing() && !tool->IsOutOfView() && !tool->IsOutOfVolume() )
    {
      double pos[3], quat[4];
      double rotMat[3][3];

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

      // send out.
      m_parent->UpdateToolTransform(this->m_port, xfrm, this->m_effectiveFrequency, BAD_FLOAT);
    }  
  }
}