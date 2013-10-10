#include <QtGui>

#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkMatrix3x3.h>
#include <vtkMath.h>
#include <vtkAxes.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkAxesActor.h>

#include "ndXfrms.h"

#include "vtkTrackedObject.h"
//*********************************************************
// Constructors/Destructors
//*********************************************************

vtkTrackedObject* vtkTrackedObject::New()
{
  return new vtkTrackedObject();
}

vtkTrackedObject::vtkTrackedObject()
{
  this->m_Xfrm = new ndQuatTransformation;

  // initialize the objects.
  //this->m_ObjectLocal = vtkSmartPointer<vtkAxes>::New();
  this->m_SphereObject = vtkSmartPointer<vtkSphereSource>::New();
  this->m_ObjectXfrm = vtkSmartPointer<vtkTransform>::New();
  this->m_Object = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->m_ObjectMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->m_ObjectActor = vtkSmartPointer<vtkActor>::New();
  this->m_ObjectAxesActor = vtkSmartPointer<vtkAxesActor>::New();

  // initialize the colours.
  this->m_ModelColor = "darkviolet";
  this->m_ModelMissingColor = "red";

  // hook them up.
  //this->m_ObjectLocal->SetScaleFactor(50.0);
  this->m_SphereObject->SetRadius(10.0);
  this->m_ObjectXfrm->Identity();
  this->m_Object->SetTransform(this->m_ObjectXfrm);
  //this->m_Object->SetInputConnection(this->m_ObjectLocal->GetOutputPort());
  this->m_Object->SetInputConnection(this->m_SphereObject->GetOutputPort());
  this->m_ObjectMapper->SetInputConnection(this->m_Object->GetOutputPort());
  this->m_ObjectActor->SetMapper(this->m_ObjectMapper);
  this->m_ObjectAxesActor->SetTotalLength(50.0,50.0,50.0);
  // hide the actor to start.
  this->m_ObjectActor->SetVisibility(false);
  this->m_ObjectAxesActor->SetVisibility(false);
  this->m_ObjectAxesActor->SetAxisLabels(false);
}

vtkTrackedObject::~vtkTrackedObject()
{
  delete this->m_Xfrm;
}

void vtkTrackedObject::UpdateXfrm(ndQuatTransformation *xfrm)
{
  // copy to local object.
  ndCopyTransform(xfrm, this->m_Xfrm);

  // use the 4x4 to set the xfrm.
  vtkSmartPointer<vtkMatrix4x4> xfrm4x4 = vtkSmartPointer<vtkMatrix4x4>::New();
  float rotMatrix[3][3];
  float quat[4];

  // initialize the 4x4.
  xfrm4x4->Identity();

  // convert quat to rotation matrix.
  quat[0] = xfrm->rotation.q0;
  quat[1] = xfrm->rotation.qx;
  quat[2] = xfrm->rotation.qy;
  quat[3] = xfrm->rotation.qz;
  vtkMath::QuaternionToMatrix3x3(quat, rotMatrix);

  // copy xfrm into 4x4 format.
  for(int i=0; i<3; i++)
  {
    for(int j=0; j<3; j++)
    {
      xfrm4x4->SetElement(i,j,rotMatrix[i][j]);
    }
  }
  xfrm4x4->SetElement(0,3,xfrm->translation.x);
  xfrm4x4->SetElement(1,3,xfrm->translation.y);
  xfrm4x4->SetElement(2,3,xfrm->translation.z);

  //debug:
  //xfrm4x4->PrintSelf(std::cout, vtkIndent());
     
  this->m_ObjectXfrm->SetMatrix(xfrm4x4);
  this->m_Object->Update();
  this->m_ObjectMapper->Update();
  this->m_ObjectAxesActor->SetUserTransform(this->m_ObjectXfrm);
  this->ChangeModelToTracking();
  emit this->TrackedObjectUpdated();
}

void vtkTrackedObject::ChangeModelToMissing()
{
  QColor color(this->m_ModelMissingColor);  
  this->m_ObjectActor->GetProperty()->SetColor(color.red(),color.green(),color.blue());
  emit this->TrackedObjectUpdated();
}

void vtkTrackedObject::ChangeModelToTracking()
{
  QColor color(this->m_ModelColor);  
  this->m_ObjectActor->GetProperty()->SetColor(color.red(),color.green(),color.blue());
  //emit this->TrackedObjectUpdated();
}