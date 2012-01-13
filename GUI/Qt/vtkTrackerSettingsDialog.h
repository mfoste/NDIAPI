#ifndef __vtkTrackerSettingsDialog_h
#define __vtkTrackerSettingsDialog_h

#include "vtkTrackerWidget_global.h"

#include <QDialog>
#include <QSettings>

#include "vtkTrackerSettingsDialog.h"
#include "vtkFakeTrackerSettingsWidget.h"
#include "vtkAuroraTrackerSettingsWidget.h"
#include "ui_vtkTrackerSettingsDialog.h"

class VTKTRACKERWIDGET_EXPORT vtkTrackerSettingsDialog : public QDialog
{
  Q_OBJECT

public:
  vtkTrackerSettingsDialog( QWidget *parent);
  ~vtkTrackerSettingsDialog();

  void UpdateAndShow();
  inline int getSystem() {return this->m_System;}
  inline vtkFakeTrackerSettings getFakeTrackerSettings() {return this->m_FakeTrackerSettingsWidget->GetFakeTrackerSettings();}
  inline ndiAuroraSettings getAuroraSettings() {return this->m_AuroraSettingsWidget->GetAuroraSettings();}

public slots:
  virtual void OnTrackingSystemChanged( int index );
  virtual void accept();
  virtual void reject();

private:
  // some helper functions.
  void CreateActions();
  
  void ReadTrackerSettings();
  void WriteTrackerSettings();
  // this.
  QWidget *m_Parent;
  // member variables.
  Ui::TrackerSettingsDialog      m_GUI;
  vtkFakeTrackerSettingsWidget *m_FakeTrackerSettingsWidget;
  vtkAuroraTrackerSettingsWidget *m_AuroraSettingsWidget;
  // settings.
  QSettings *m_Settings; 
  // stored data.
  int m_System;
};

#endif