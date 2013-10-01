#ifndef __vtkTrackedObject_h
#define __vtkTrackedObject_h

#include "vtkTrackerWidget_global.h"

#include <QtGui>

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
//#include <vtkAxes.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkAxesActor.h>

#include "ndXfrms.h"

class VTKTRACKERWIDGET_EXPORT vtkTrackedObject : public QObject
{ 
  Q_OBJECT

public:
  static vtkTrackedObject *New();

protected:
  vtkTrackedObject();
  ~vtkTrackedObject();

public:
  void UpdateXfrm(ndQuatTransformation *xfrm);
  ndQuatTransformation* GetXfrm() {return this->m_Xfrm;}
  vtkActor* GetModelActor() {return this->m_ObjectActor;}
  vtkAxesActor* GetAxesActor() {return this->m_ObjectAxesActor;}
  void SetModelVisibility(bool bVisible) {this->m_ObjectActor->SetVisibility(bVisible);}
  void SetAxesVisibility(bool bVisible) {this->m_ObjectAxesActor->SetVisibility(bVisible);}

signals:
  void TrackedObjectUpdated();

private:
  ndQuatTransformation *m_Xfrm;
  //vtkSmartPointer<vtkAxes> m_ObjectLocal;
  vtkSmartPointer<vtkSphereSource> m_SphereObject;
  vtkSmartPointer<vtkTransform> m_ObjectXfrm;
  vtkSmartPointer<vtkTransformPolyDataFilter> m_Object;
  vtkSmartPointer<vtkPolyDataMapper> m_ObjectMapper;
  vtkSmartPointer<vtkActor> m_ObjectActor;

  // if we use axes instead of a poly data, we can do this here.
  vtkSmartPointer<vtkAxesActor> m_ObjectAxesActor;


};

#endif