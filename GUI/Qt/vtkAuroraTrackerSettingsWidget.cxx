/*
Copyright (C) 2011-2012, Northern Digital Inc. All rights reserved.
 
All Northern Digital Inc. (“NDI”) Media and/or Sample Code and/or Sample Code
Documentation (collectively referred to as “Sample Code”) is licensed and provided "as
is” without warranty of any kind. The licensee, by use of the Sample Code, warrants to
NDI that the Sample Code is fit for the use and purpose for which the licensee intends to
use the Sample Code. NDI makes no warranties, express or implied, that the functions
contained in the Sample Code will meet the licensee’s requirements or that the operation
of the programs contained therein will be error free. This warranty as expressed herein is
exclusive and NDI expressly disclaims any and all express and/or implied, in fact or in
law, warranties, representations, and conditions of every kind pertaining in any way to
the Sample Code licensed and provided by NDI hereunder, including without limitation,
each warranty and/or condition of quality, merchantability, description, operation,
adequacy, suitability, fitness for particular purpose, title, interference with use or
enjoyment, and/or non infringement, whether express or implied by statute, common law,
usage of trade, course of dealing, custom, or otherwise. No NDI dealer, distributor, agent
or employee is authorized to make any modification or addition to this warranty.

In no event shall NDI nor any of its employees be liable for any direct, indirect,
incidental, special, exemplary, or consequential damages, sundry damages or any
damages whatsoever, including, but not limited to, procurement of substitute goods or
services, loss of use, data or profits, or business interruption, however caused. In no
event shall NDI’s liability to the licensee exceed the amount paid by the licensee for the
Sample Code or any NDI products that accompany the Sample Code. The said limitations
and exclusions of liability shall apply whether or not any such damages are construed as
arising from a breach of a representation, warranty, guarantee, covenant, obligation,
condition or fundamental term or on any theory of liability, whether in contract, strict
liability, or tort (including negligence or otherwise) arising in any way out of the use of
the Sample Code even if advised of the possibility of such damage. In no event shall
NDI be liable for any claims, losses, damages, judgments, costs, awards, expenses or
liabilities of any kind whatsoever arising directly or indirectly from any injury to person
or property, arising from the Sample Code or any use thereof.
*/

#include <QtGui>

#include "vtkAuroraTrackerSettingsWidget.h"

vtkAuroraTrackerSettingsWidget::vtkAuroraTrackerSettingsWidget( QWidget *parent, QSettings *settings )
{
  this->m_Parent = parent;
  this->m_Settings = settings;
  // set up the GUI.
  m_Widget.setupUi(this);
  this->CreateActions();

  // initialize the rom file list.
  for(int i=0; i < 4; i++)
  {
    this->m_AuroraSettings.romFiles << "";
  }
  this->m_AuroraSettings.bUseManual = false;
  this->m_AuroraSettings.baudRate = -1;
  this->m_AuroraSettings.commPort = -1;
  this->m_AuroraSettings.logComm = false;

  this->m_Widget.baudRateComboBox->addItem("Auto", -1);
  this->m_Widget.baudRateComboBox->addItem("9600", 9600);
  this->m_Widget.baudRateComboBox->addItem("14400", 14400);
  this->m_Widget.baudRateComboBox->addItem("19200", 19200);
  this->m_Widget.baudRateComboBox->addItem("38400", 38400);
  this->m_Widget.baudRateComboBox->addItem("57600", 57600);
  this->m_Widget.baudRateComboBox->addItem("115200", 115200);
  this->m_Widget.baudRateComboBox->addItem("921600", 921600);

  this->m_Widget.commPortComboBox->addItem("Auto", -1);
  this->m_Widget.commPortComboBox->addItem("COM1", 1);
  this->m_Widget.commPortComboBox->addItem("COM2", 2);
  this->m_Widget.commPortComboBox->addItem("COM3", 3);
  this->m_Widget.commPortComboBox->addItem("COM4", 4);
  this->m_Widget.commPortComboBox->addItem("COM5", 5);
  this->m_Widget.commPortComboBox->addItem("COM6", 6);
  this->m_Widget.commPortComboBox->addItem("COM7", 7);
  this->m_Widget.commPortComboBox->addItem("COM8", 8);


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
  connect(this->m_Widget.useManualParmsCheckBox, SIGNAL(clicked(bool)), this, SLOT(OnUseManual(bool)) );
  connect(this->m_Widget.commPortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCommPortChanged(int)) );
  connect(this->m_Widget.baudRateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnBaudRateChanged(int)) );
  // other settings
  connect(this->m_Widget.logCommunicationsCheckBox, SIGNAL(clicked(bool)), this, SLOT(OnLogCommunicationChanged(bool)) );
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

  if( this->sender()->objectName().compare("port01AuroraRomFileBrowseButton") == 0 )
  {
    port = 0;
  }
  else if(this->sender()->objectName().compare("port02AuroraRomFileBrowseButton") == 0 )
  {
    port = 1;
  }
  else if(this->sender()->objectName().compare("port03AuroraRomFileBrowseButton") == 0 )
  {
    port = 2;
  }
  else if(this->sender()->objectName().compare("port04AuroraRomFileBrowseButton") == 0 )
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
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 2"), 
      this->m_Widget.port02AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_Widget.port02AuroraRomFileLineEdit->setText(filename);
    break;
  case 2:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 3"), 
      this->m_Widget.port03AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_Widget.port03AuroraRomFileLineEdit->setText(filename);
    break;
  case 3:
    filename = QFileDialog::getOpenFileName(this, tr("Open Aurora ROM File: Port 4"), 
      this->m_Widget.port04AuroraRomFileLineEdit->text(), tr("NDI ROM File (*.rom)") );
    if( filename.isEmpty() ) return;
    this->m_Widget.port04AuroraRomFileLineEdit->setText(filename);
    break;
  default:
    // pop up message box invalid port given.
    break;
  }
}

void vtkAuroraTrackerSettingsWidget::OnUseManual(bool bUseManual)
{
  this->m_AuroraSettings.bUseManual = bUseManual;

  if(!bUseManual)
  {
    this->m_Widget.baudRateComboBox->setCurrentIndex(0);
    this->m_Widget.commPortComboBox->setCurrentIndex(0);
  }
  else
  {
    this->m_AuroraSettings.baudRate 
      = this->m_Widget.baudRateComboBox->itemData(this->m_Widget.baudRateComboBox->currentIndex()).toInt();
    this->m_AuroraSettings.commPort 
      = this->m_Widget.commPortComboBox->itemData(this->m_Widget.commPortComboBox->currentIndex()).toInt();
  }
}

void vtkAuroraTrackerSettingsWidget::OnCommPortChanged(int index)
{
  this->m_AuroraSettings.commPort = this->m_Widget.commPortComboBox->itemData(index).toInt();
}

void vtkAuroraTrackerSettingsWidget::OnBaudRateChanged(int index)
{
  this->m_AuroraSettings.baudRate = this->m_Widget.baudRateComboBox->itemData(index).toInt();
}

void vtkAuroraTrackerSettingsWidget::OnLogCommunicationChanged(bool bLogComm)
{
  this->m_AuroraSettings.logComm = bLogComm;
}