#ifndef __vtkTrackerWidgetXfrmCallback_h
#define __vtkTrackerWidgetXfrmCallback_h

#include <QtGui>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>

#include "vtkTrackerWidget.h"

#define NUM_SAMPLES_FREQUENCY 100

class vtkTrackerWidgetXfrmCallback : public vtkCommand
{
public:
  static vtkTrackerWidgetXfrmCallback* New();
  static vtkTrackerWidgetXfrmCallback* New(vtkTrackerWidget *parent, int port);

protected:
  vtkTrackerWidgetXfrmCallback(vtkTrackerWidget *parent, int port);
  ~vtkTrackerWidgetXfrmCallback();

  virtual void Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData));
  
private:
  vtkTrackerWidget *m_parent;
  int m_port;

  // for computing the effective frequency
  int m_nXfrms;
  double m_currTime[NUM_SAMPLES_FREQUENCY];
  double m_effectiveFrequency;
};

#endif