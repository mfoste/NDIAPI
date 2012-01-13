#include <QtGui>

#include "vtkFakeTrackerSettingsWidget.h"
#include "vtkAuroraTrackerSettingsWidget.h"
//#include "vtkTrackerSettingsDialog.moc"
#include "vtkTrackerSettingsDialog.h"

vtkTrackerSettingsDialog::vtkTrackerSettingsDialog( QWidget *parent )
{
  this->m_Parent = parent;
  // set up the GUI.
  m_GUI.setupUi(this);

  // initialize the settings.
  this->m_Settings = new QSettings("./tracker.ini", QSettings::IniFormat);

  // add the FakeTrackerSettingsWidget to the stackedWidget
  this->m_FakeTrackerSettingsWidget = new vtkFakeTrackerSettingsWidget( this->m_GUI.stackedWidget, this->m_Settings );
  this->m_GUI.stackedWidget->insertWidget(FAKE_TRACKER, this->m_FakeTrackerSettingsWidget );
  // add the AuroraTrackerSettingsWidget to the stackedWidget
  this->m_AuroraSettingsWidget = new vtkAuroraTrackerSettingsWidget( this->m_GUI.stackedWidget, this->m_Settings );
  this->m_GUI.stackedWidget->insertWidget(NDI_AURORA, this->m_AuroraSettingsWidget );
  
  // set up the connections in the GUI.
  this->CreateActions();
  
  // update the tracking system comboBox.
  this->m_GUI.trackingSystemComboBox->addItem("Fake Tracker", FAKE_TRACKER);
  this->m_GUI.trackingSystemComboBox->addItem("Aurora", NDI_AURORA);

  // read the tracker settings in. 
  this->ReadTrackerSettings();
}

vtkTrackerSettingsDialog::~vtkTrackerSettingsDialog()
{
}

void vtkTrackerSettingsDialog::CreateActions()
{
  connect(this->m_GUI.trackingSystemComboBox, SIGNAL(currentIndexChanged(int)), this->m_GUI.stackedWidget, SLOT(setCurrentIndex(int)) );
  
}

void vtkTrackerSettingsDialog::UpdateAndShow()
{
  this->ReadTrackerSettings();
  this->show();
}

void vtkTrackerSettingsDialog::ReadTrackerSettings()
{
  // which tracking system? NDI_AURORA is default
  this->m_System = this->m_Settings->value("TrackingSystem", NDI_AURORA).toInt();
  // update the GUI
  this->m_GUI.trackingSystemComboBox->setCurrentIndex(this->m_GUI.trackingSystemComboBox->findData(this->m_System));

  // Fake Tracker Settings.
  this->m_FakeTrackerSettingsWidget->ReadTrackerSettings();
  // Aurora Tracker Settings.
  this->m_AuroraSettingsWidget->ReadTrackerSettings();
}

void vtkTrackerSettingsDialog::WriteTrackerSettings()
{
  this->m_System = this->m_GUI.trackingSystemComboBox->itemData(this->m_GUI.trackingSystemComboBox->currentIndex()).toInt();
  this->m_Settings->setValue("TrackingSystem", this->m_System );
  
  // Fake Tracker Settings.
  this->m_FakeTrackerSettingsWidget->WriteTrackerSettings();
  // Aurora Tracker Settings.
  this->m_AuroraSettingsWidget->WriteTrackerSettings();  
}



void vtkTrackerSettingsDialog::OnTrackingSystemChanged(int index)
{
  this->m_System = this->m_GUI.trackingSystemComboBox->itemData(index).toInt();
}

void vtkTrackerSettingsDialog::accept()
{
  this->WriteTrackerSettings();
  QDialog::accept();
}

void vtkTrackerSettingsDialog::reject()
{
  QDialog::reject();
}