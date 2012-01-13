#ifndef __vtkFakeTrackerSettingsWidget_h
#define __vtkFakeTrackerSettingsWidget_h

//#include "vtkTrackerWidget_global.h"

#include <QWidget>

#include "ui_FakeTrackerSettingsWidget.h"

class vtkFakeTrackerSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  vtkFakeTrackerSettingsWidget( QWidget *parent);
  ~vtkFakeTrackerSettingsWidget();

  inline Ui::FakeTrackerSettingsWidget getWidget() {return m_Widget;}

public slots:
  
private:
  // this.
  QWidget *m_Parent;
  // member variables.
  Ui::FakeTrackerSettingsWidget m_Widget;
};

#endif //__vtkFakeTrackerSettingsWidget_h