#ifndef __vtkTrackedObject_h
#define __vtkTrackedObject_h

#include "vtkTrackerWidget_global.h"

#include <QtGui>

#include <vtkSmartPointer.h>
#include <vtkAxes.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

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
  vtkActor* GetActor() {return this->m_ObjectActor;}
  void SetVisibility(bool bVisible) {this->m_ObjectActor->SetVisibility(bVisible);}

signals:
  void TrackedObjectUpdated();

private:
  ndQuatTransformation *m_Xfrm;
  vtkSmartPointer<vtkAxes> m_ObjectLocal;
  vtkSmartPointer<vtkTransform> m_ObjectXfrm;
  vtkSmartPointer<vtkTransformPolyDataFilter> m_Object;
  vtkSmartPointer<vtkPolyDataMapper> m_ObjectMapper;
  vtkSmartPointer<vtkActor> m_ObjectActor;


};

#endif