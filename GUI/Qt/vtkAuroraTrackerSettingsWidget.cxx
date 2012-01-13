#include <QtGui>

#include "vtkAuroraTrackerSettingsWidget.h"

vtkAuroraTrackerSettingsWidget::vtkAuroraTrackerSettingsWidget( QWidget *parent, QSettings *settings )
{
  this->m_Parent = parent;
  this->m_Settings = settings;
  // set up the GUI.
  m_Widget.setupUi(this);

  // initialize the from file list.
  for(int i=0; i < 4; i++)
  {
    this->m_AuroraSettings.romFiles << "";
  }

  // set validators on port line edits.
  this->m_Widget.updateFrequencyAuroraLineEdit->setValidator(new QDoubleValidator(0.1, 40, 1, this) );
}

vtkAuroraTrackerSettingsWidget::vtkAuroraTrackerSettingsWidget( QWidget *parent)
{
  vtkAuroraTrackerSettingsWidget(parent, 0);
}

vtkAuroraTrackerSettingsWidget::~vtkAuroraTrackerSettingsWidget()
{
}

void vtkAuroraTrackerSettingsWidget::CreateActions()
{
  connect(this->m_Widget.port01AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
  connect(this->m_Widget.port02AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
  connect(this->m_Widget.port03AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
  connect(this->m_Widget.port04AuroraRomFileBrowseButton, SIGNAL(clicked()), this, SLOT(OnLoadRomFile()) );
}

void vtkAuroraTrackerSettingsWidget::ReadTrackerSettings()
{
  if( !m_Settings )
    return;

  // aurora port data.
  this->m_Widget.port01AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port01/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[0] = this->m_Settings->value("Aurora/Port01/RomFile", "").toString();
  this->m_Widget.port02AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port02/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[1] = this->m_Settings->value("Aurora/Port02/RomFile", "").toString();
  this->m_Widget.port03AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port03/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[2] = this->m_Settings->value("Aurora/Port03/RomFile", "").toString();
  this->m_Widget.port04AuroraVirtualRomCheckBox->setChecked(this->m_Settings->value("Aurora/Port04/UseVirtualRom", false).toBool());
  this->m_AuroraSettings.romFiles[3] = this->m_Settings->value("Aurora/Port04/RomFile", "").toString();
  this->m_AuroraSettings.updateFrequency = this->m_Settings->value("Aurora/updateFrequency", 40.0).toDouble();
   
  // update the GUI.
  this->m_Widget.port01AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[0]);
  this->m_Widget.port02AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[1]);
  this->m_Widget.port03AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[2]);
  this->m_Widget.port04AuroraRomFileLineEdit->setText(this->m_AuroraSettings.romFiles[3]);
  
  this->m_Widget.updateFrequencyAuroraLineEdit->setText(QString("%1").arg(this->m_AuroraSettings.updateFrequency));

}

void vtkAuroraTrackerSettingsWidget::WriteTrackerSettings()
{
  if( !m_Settings )
    return;

  // write the Aurora data.
  // port 01:
  if( this->m_Widget.port01AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[0] = this->m_Widget.port01AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[0].clear();
  }
  this->m_Settings->setValue("Aurora/Port01/UseVirtualRom",this->m_Widget.port01AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port01/RomFile", this->m_AuroraSettings.romFiles[0]);

  // port 02:
  if( this->m_Widget.port02AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[1] = this->m_Widget.port02AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[1].clear();
  }
  this->m_Settings->setValue("Aurora/Port02/UseVirtualRom", this->m_Widget.port02AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port02/RomFile", this->m_AuroraSettings.romFiles[1]);

  // port 03:
  if( this->m_Widget.port03AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[2] = this->m_Widget.port03AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[2].clear();
  }
  this->m_Settings->setValue("Aurora/Port03/UseVirtualRom", this->m_Widget.port03AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port03/RomFile", this->m_AuroraSettings.romFiles[2]);

  // port 04:
  if( this->m_Widget.port04AuroraVirtualRomCheckBox->isChecked() )
  {
    this->m_AuroraSettings.romFiles[3] = this->m_Widget.port04AuroraRomFileLineEdit->text();
  }
  else
  {
    this->m_AuroraSettings.romFiles[3].clear();
  }
  this->m_Settings->setValue("Aurora/Port04/UseVirtualRom", this->m_Widget.port04AuroraVirtualRomCheckBox->isChecked() );
  this->m_Settings->setValue("Aurora/Port04/RomFile", this->m_AuroraSettings.romFiles[3]);

  this->m_AuroraSettings.updateFrequency = this->m_Widget.updateFrequencyAuroraLineEdit->text().toDouble();
  this->m_Settings->setValue("Aurora/updateFrequency", this->m_AuroraSettings.updateFrequency);
}

void vtkAuroraTrackerSettingsWidget::OnLoadRomFile()
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
      this->m_Widget.port01AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_Widget.port01AuroraRomFileLineEdit->setText(filename);
    break;
  case 1:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 1"), 
      this->m_Widget.port02AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_Widget.port02AuroraRomFileLineEdit->setText(filename);
    break;
  case 2:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 1"), 
      this->m_Widget.port03AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_Widget.port03AuroraRomFileLineEdit->setText(filename);
    break;
  case 3:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 1"), 
      this->m_Widget.port04AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_Widget.port04AuroraRomFileLineEdit->setText(filename);
    break;
  default:
    // pop up message box invalid port given.
    break;
  }

}