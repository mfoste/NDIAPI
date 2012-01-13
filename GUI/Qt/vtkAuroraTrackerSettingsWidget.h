#ifndef __vtkAuroraTrackerSettingsWidget_h
#define __vtkAuroraTrackerSettingsWidget_h

//#include "vtkTrackerWidget_global.h"

#include <QWidget>
#include <qsettings.h>

#include "vtkTrackerSettingsStructures.h"

#include "ui_AuroraTrackerSettingsWidget.h"

class vtkAuroraTrackerSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  vtkAuroraTrackerSettingsWidget( QWidget *parent, QSettings *settings);
  vtkAuroraTrackerSettingsWidget( QWidget *parent);
  ~vtkAuroraTrackerSettingsWidget();

  inline Ui::AuroraTrackerSettingsWidget getWidget() {return m_Widget;}

  inline void SetSettings(QSettings *settings) {m_Settings = settings;}
  inline ndiAuroraSettings GetAuroraSettings() {return this->m_AuroraSettings;}
  void ReadTrackerSettings();
  void WriteTrackerSettings();

public slots:
  virtual void OnLoadRomFile();
  
private:
  // some helper functions.
  void CreateActions();

  // this.
  QWidget *m_Parent;
  // member variables.
  Ui::AuroraTrackerSettingsWidget m_Widget;

  //settings
  QSettings *m_Settings;
  ndiAuroraSettings m_AuroraSettings;
};

#endif //__vtkAuroraTrackerSettingsWidget_h