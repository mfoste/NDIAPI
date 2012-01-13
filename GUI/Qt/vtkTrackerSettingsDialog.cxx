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

  // add the FakeTrackerSettingsWidget to the stackedWidget
  this->m_FakeTrackerSettingsWidget = new vtkFakeTrackerSettingsWidget( this->m_GUI.stackedWidget );
  this->m_GUI.stackedWidget->insertWidget(FAKE_TRACKER, this->m_FakeTrackerSettingsWidget );
  // add the AuroraTrackerSettingsWidget to the stackedWidget
  this->m_AuroraSettingsWidget = new vtkAuroraTrackerSettingsWidget( this->m_GUI.stackedWidget );
  this->m_GUI.stackedWidget->insertWidget(NDI_AURORA, this->m_AuroraSettingsWidget );
  
  // set up the connections in the GUI.
  this->CreateActions();
  
  // set validators on port line edits.
  this->m_AuroraSettingsWidget->getWidget().updateFrequencyAuroraLineEdit->setValidator(new QDoubleValidator(0.1, 40, 1, this) );

  // initialize the from file list.
  for(int i=0; i < 4; i++)
  {
    this->m_AuroraSettings.romFiles << "";
  }

  
  // update the tracking system comboBox.
  this->m_GUI.trackingSystemComboBox->addItem("Fake Tracker", FAKE_TRACKER);
  this->m_GUI.trackingSystemComboBox->addItem("Aurora", NDI_AURORA);

  // initialize the settings.
  this->m_Settings = new QSettings("./tracker.ini", QSettings::IniFormat); 
  this->ReadTrackerSettings();
}

vtkTrackerSettingsDialog::~vtkTrackerSettingsDialog()
{
}

void vtkTrackerSettingsDialog::CreateActions()
{
  connect(this->m_GUI.trackingSystemComboBox, SIGNAL(currentIndexChanged(int)), this->m_GUI.stackedWidget, SLOT(setCurrentIndex(int)) );
  connect(this->m_AuroraSettingsWidget->getWidget().port01AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
  connect(this->m_AuroraSettingsWidget->getWidget().port02AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
  connect(this->m_AuroraSettingsWidget->getWidget().port03AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
  connect(this->m_AuroraSettingsWidget->getWidget().port04AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
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

  // aurora port data.
  this->m_AuroraSettingsWidget->getWidget().port01AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port01/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[0] = this->m_Settings->value("Aurora/Port01/RomFile", "").toString();
  this->m_AuroraSettingsWidget->getWidget().port02AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port02/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[1] = this->m_Settings->value("Aurora/Port02/RomFile", "").toString();
  this->m_AuroraSettingsWidget->getWidget().port03AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port03/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[2] = this->m_Settings->value("Aurora/Port03/RomFile", "").toString();
  this->m_AuroraSettingsWidget->getWidget().port04AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port04/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[3] = this->m_Settings->value("Aurora/Port04/RomFile", "").toString();
  this->m_AuroraSettings.updateFrequency = this->m_Settings->value("Aurora/updateFrequency", 40.0).toDouble();

  // update the GUI - System
  this->m_GUI.trackingSystemComboBox->setCurrentIndex(this->m_GUI.trackingSystemComboBox->findData(this->m_System));
  
  this->m_AuroraSettingsWidget->getWidget().port01AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[0]);
  this->m_AuroraSettingsWidget->getWidget().port02AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[1]);
  this->m_AuroraSettingsWidget->getWidget().port03AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[2]);
  this->m_AuroraSettingsWidget->getWidget().port04AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[3]);
  
  this->m_AuroraSettingsWidget->getWidget().updateFrequencyAuroraLineEdit->setText(QString("%1").arg(this->m_AuroraSettings.updateFrequency));
}

void vtkTrackerSettingsDialog::WriteTrackerSettings()
{
  this->m_System = this->m_GUI.trackingSystemComboBox->itemData(this->m_GUI.trackingSystemComboBox->currentIndex()).toInt();

  this->m_Settings->setValue("TrackingSystem", this->m_System );

  // write the Aurora data.
  // port 01:
  if( this->m_AuroraSettingsWidget->getWidget().port01AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[0] = this->m_AuroraSettingsWidget->getWidget().port01AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[0].clear();
  }
  this->m_Settings->setValue("Aurora/Port01/UseVirtualRom",this->m_AuroraSettingsWidget->getWidget().port01AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port01/RomFile", this->m_AuroraSettings.romFiles[0]);

  // port 02:
  if( this->m_AuroraSettingsWidget->getWidget().port02AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[1] = this->m_AuroraSettingsWidget->getWidget().port02AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[1].clear();
  }
  this->m_Settings->setValue("Aurora/Port02/UseVirtualRom", this->m_AuroraSettingsWidget->getWidget().port02AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port02/RomFile", this->m_AuroraSettings.romFiles[1]);

  // port 03:
  if( this->m_AuroraSettingsWidget->getWidget().port03AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[2] = this->m_AuroraSettingsWidget->getWidget().port03AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[2].clear();
  }
  this->m_Settings->setValue("Aurora/Port03/UseVirtualRom", this->m_AuroraSettingsWidget->getWidget().port03AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port03/RomFile", this->m_AuroraSettings.romFiles[2]);

  // port 04:
  if( this->m_AuroraSettingsWidget->getWidget().port04AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[3] = this->m_AuroraSettingsWidget->getWidget().port04AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[3].clear();
  }
  this->m_Settings->setValue("Aurora/Port04/UseVirtualRom", this->m_AuroraSettingsWidget->getWidget().port04AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port04/RomFile", this->m_AuroraSettings.romFiles[3]);

  this->m_AuroraSettings.updateFrequency = this->m_AuroraSettingsWidget->getWidget().updateFrequencyAuroraLineEdit->text().toDouble();
  this->m_Settings->setValue("Aurora/updateFrequency", this->m_AuroraSettings.updateFrequency);
}

void vtkTrackerSettingsDialog::OnLoadRomFile()
{
  int port;
  QString filename;

  if( this->sender()->objectName().compare("port01AuroraRomFileBrowseButton") )
  {
    port = 0;
  }
  else if(this->sender()->objectName().compare("port02AuroraRomFileBrowseButton"))
  {
    port = 1;
  }
  else if(this->sender()->objectName().compare("port03AuroraRomFileBrowseButton"))
  {
    port = 2;
  }
  else if(this->sender()->objectName().compare("port04AuroraRomFileBrowseButton"))
  {
    port = 3;
  }
  else
  {
    //TODO: error pop-up.
  }
  
  switch( port )
  {
  case 0:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 1"), 
      this->m_AuroraSettingsWidget->getWidget().port01AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_AuroraSettingsWidget->getWidget().port01AuroraRomFileLineEdit->setText(filename);
    break;
  case 1:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 1"), 
      this->m_AuroraSettingsWidget->getWidget().port02AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_AuroraSettingsWidget->getWidget().port02AuroraRomFileLineEdit->setText(filename);
    break;
  case 2:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 1"), 
      this->m_AuroraSettingsWidget->getWidget().port03AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_AuroraSettingsWidget->getWidget().port03AuroraRomFileLineEdit->setText(filename);
    break;
  case 3:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 1"), 
      this->m_AuroraSettingsWidget->getWidget().port04AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_AuroraSettingsWidget->getWidget().port04AuroraRomFileLineEdit->setText(filename);
    break;
  default:
    // pop up message box invalid port given.
    break;
  }

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