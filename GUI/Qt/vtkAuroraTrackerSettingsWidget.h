#ifndef __vtkAuroraTrackerSettingsWidget_h
#define __vtkAuroraTrackerSettingsWidget_h

//#include "vtkTrackerWidget_global.h"

#include <QWidget>

#include "ui_AuroraTrackerSettingsWidget.h"

class vtkAuroraTrackerSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  vtkAuroraTrackerSettingsWidget( QWidget *parent);
  ~vtkAuroraTrackerSettingsWidget();

  inline Ui::AuroraTrackerSettingsWidget getWidget() {return m_Widget;}

public slots:
  
private:
  // this.
  QWidget *m_Parent;
  // member variables.
  Ui::AuroraTrackerSettingsWidget m_Widget;
};

#endif //__vtkAuroraTrackerSettingsWidget_h