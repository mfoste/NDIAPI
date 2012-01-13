#ifndef __vtkFakeTrackerSettingsWidget_h
#define __vtkFakeTrackerSettingsWidget_h

//#include "vtkTrackerWidget_global.h"

#include <QWidget>
#include <QSettings>

#include "vtkTrackerSettingsStructures.h"

#include "ui_FakeTrackerSettingsWidget.h"

class vtkFakeTrackerSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  vtkFakeTrackerSettingsWidget( QWidget *parent, QSettings *settings);
  vtkFakeTrackerSettingsWidget( QWidget *parent);
  ~vtkFakeTrackerSettingsWidget();

  inline Ui::FakeTrackerSettingsWidget getWidget() {return m_Widget;}

  inline void SetSettings(QSettings *settings) {m_Settings = settings;}
  inline vtkFakeTrackerSettings GetFakeTrackerSettings() {return this->m_FakeTrackerSettings;}
  void ReadTrackerSettings();
  void WriteTrackerSettings();

public slots:
  
private:
  // this.
  QWidget *m_Parent;
  // member variables.
  Ui::FakeTrackerSettingsWidget m_Widget;

  // settings.
  QSettings *m_Settings;
  vtkFakeTrackerSettings m_FakeTrackerSettings;
  

};

#endif //__vtkFakeTrackerSettingsWidget_h