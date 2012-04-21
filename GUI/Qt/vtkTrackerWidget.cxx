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

#include <vector>

#include <QtGui>

#include "ndXfrms.h"

#include "vtkTracker.h"
#include "vtkFakeTracker.h"
#include "vtkNDITracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerSettingsStructures.h"
#include "vtkTrackerSettingsDialog.h"
#include "vtkTrackerWidgetXfrmCallback.h"
#include "vtkTrackerWidget.h"

vtkTrackerWidget::vtkTrackerWidget(QWidget *parent ) : QWidget(parent)
{
  //this->m_Parent = parent;
  m_Tracker = 0;
  //m_GUI.setupUi(this);

  this->m_TrackerSettingsDialog = vtkTrackerSettingsDialog::New(this);

  // set up the timer.
  m_Timer = new QTimer(this);
  this->m_TrackerUpdateFrequency = 20; // arbitrary default of 20.

  this->setupUi();
  this->setupUiLayout();
  this->CreateActions();  

  m_StartTrackingButton->setEnabled(false);
  m_StopTrackingButton->setEnabled(false);
  this->m_VolumeSelectionComboBox->setEnabled(false);
  this->m_VolumeSelectionComboBox->setVisible(false);
}

vtkTrackerWidget::~vtkTrackerWidget()
{
  // If tracker object exists.
  if(m_Tracker)
  {
    // if tracker is tracking.
    if(m_Tracker->IsTracking())
    {
      this->OnStopTracker();
    }
  }
  //delete m_Tracker;
  delete m_Timer;
}

void vtkTrackerWidget::Initialize()
{
  this->Initialize("");
}

void vtkTrackerWidget::Initialize(QString settingsFile)
{
  this->m_TrackerSettingsDialog->Initialize(settingsFile);
}

QSize vtkTrackerWidget::sizeHint() const
{
  return QSize(170, 110);
}

void vtkTrackerWidget::setupUi()
{
  this->m_ConfigureTrackerButton = new QPushButton(this);
  this->m_ConfigureTrackerButton->setText("Configure Tracker");
  this->m_ConfigureTrackerButton->setMaximumWidth(110);

  this->m_VolumeSelectionComboBox = new QComboBox(this);
  //this->m_ConfigureTrackerButton->setMaximumWidth(110);
  this->m_VolumeSelectionComboBox->setEnabled(false);
  //this->m_VolumeSelectionComboBox->setVisible(false);

  this->m_StartTrackingButton = new QPushButton(this);
  this->m_StartTrackingButton->setText("Start Tracking");
  this->m_StartTrackingButton->setMaximumWidth(110);

  this->m_StopTrackingButton = new QPushButton(this);
  this->m_StopTrackingButton->setText("Stop Tracking");
  this->m_StopTrackingButton->setMaximumWidth(110);
}

void vtkTrackerWidget::setupUiLayout()
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  mainLayout->addWidget(this->m_ConfigureTrackerButton);
  mainLayout->addWidget(this->m_VolumeSelectionComboBox);
  mainLayout->addWidget(this->m_StartTrackingButton);
  mainLayout->addWidget(this->m_StopTrackingButton);

  setLayout( mainLayout );
}


void vtkTrackerWidget::CreateActions()
{
  connect(m_ConfigureTrackerButton, SIGNAL(clicked()), this, SLOT(OnConfigureTracker()));
  connect(m_TrackerSettingsDialog, SIGNAL(accepted()), this, SLOT(OnConfigureTrackerAccepted()));
  connect(m_VolumeSelectionComboBox, SIGNAL(activated(int)), this, SLOT(OnVolumeSelected(int)));
  connect(m_StartTrackingButton, SIGNAL(clicked()), this, SLOT(OnStartTracker()));
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(UpdateData()));
  connect(m_StopTrackingButton, SIGNAL(clicked()), this, SLOT(OnStopTracker()));
}

void vtkTrackerWidget::OnConfigureTracker()
{
  if( this->m_Tracker )
  {
    if( this->m_Tracker->IsTracking() )
    {
      this->m_Tracker->StopTracking();
    }
  }
  /* launch tracker settings dialog. */
  this->m_TrackerSettingsDialog->UpdateAndShow();
}

void vtkTrackerWidget::OnConfigureTrackerAccepted()
{
  this->ConfigureTracker();
}

void vtkTrackerWidget::ConfigureTracker()
{
  QString errorString;
  int nVolumes;
  
  // if a tracker exists, delete it.
  if( m_Tracker )
  {
    for( int i=0; i < m_Tracker->GetNumberOfTools(); i++ )
    {
      m_Tracker->GetTool(i)->RemoveAllObservers();
    }
    m_Tracker->RemoveAllObservers();
    m_Tracker->Delete();
    m_Tracker = 0;
  }

  switch( this->m_TrackerSettingsDialog->getSystem() )
  {
  case FAKE_TRACKER:
    m_Tracker = vtkFakeTracker::New();
    this->m_TrackerUpdateFrequency = this->m_TrackerSettingsDialog->getFakeTrackerSettings().updateFrequency;
    this->m_VolumeSelectionComboBox->setVisible(false);
    break;
  case NDI_AURORA:
    m_Tracker = vtkNDITracker::New();
    if( this->m_TrackerSettingsDialog->getAuroraSettings().bUseManual )
    {
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetBaudRate(this->m_TrackerSettingsDialog->getAuroraSettings().baudRate); // this needs to be changed to Auto when it is implemented.
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetSerialPort(this->m_TrackerSettingsDialog->getAuroraSettings().commPort);
    }
    else
    {
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetBaudRate(115200); // this needs to be changed to Auto when it is implemented.
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetSerialPort(-1);
    }
    this->m_TrackerUpdateFrequency = this->m_TrackerSettingsDialog->getAuroraSettings().updateFrequency;
    // load virtual roms if needed.
    for(int i = 0; i < 4; i++ )
    {
      if( !this->m_TrackerSettingsDialog->getAuroraSettings().romFiles[i].isEmpty() )
      {
        dynamic_cast<vtkNDITracker*>(m_Tracker)->LoadVirtualSROM(i, 
          this->m_TrackerSettingsDialog->getAuroraSettings().romFiles[i].toLatin1());
      }
    }
    break;
  case NDI_SPECTRA:
    m_Tracker = vtkNDITracker::New();
    if( this->m_TrackerSettingsDialog->getSpectraSettings().bUseManual )
    {
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetBaudRate(this->m_TrackerSettingsDialog->getSpectraSettings().baudRate); // this needs to be changed to Auto when it is implemented.
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetSerialPort(this->m_TrackerSettingsDialog->getSpectraSettings().commPort);
    }
    else
    {
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetBaudRate(115200); // this needs to be changed to Auto when it is implemented.
      dynamic_cast<vtkNDITracker*>(m_Tracker)->SetSerialPort(-1);
    }
    this->m_TrackerUpdateFrequency = this->m_TrackerSettingsDialog->getSpectraSettings().updateFrequency;
    // load virtual roms if needed.
    for(int i = 0; i < 12; i++ )
    {
      if( !this->m_TrackerSettingsDialog->getSpectraSettings().romFiles[i].isEmpty() )
      {
        dynamic_cast<vtkNDITracker*>(m_Tracker)->LoadVirtualSROM(i, 
          this->m_TrackerSettingsDialog->getSpectraSettings().romFiles[i].toLatin1());
      }
    }
    break;
  default:
    this->PopUpError("Invalid tracker system type given.  Check your tracker settings.");
    return;
  }
  // set up the event observers.
  this->m_xfrms.resize(m_Tracker->GetNumberOfTools());
  this->m_effectiveFrequencies.resize(m_Tracker->GetNumberOfTools());
  this->m_quality.resize(m_Tracker->GetNumberOfTools());
  this->m_xfrmCallbacks.resize(m_Tracker->GetNumberOfTools());
  for( int i = 0; i < m_Tracker->GetNumberOfTools(); i++ )
  {
    m_xfrmCallbacks[i] = vtkTrackerWidgetXfrmCallback::New(this, i);
    m_Tracker->GetTool(i)->AddObserver(vtkCommand::ModifiedEvent, this->m_xfrmCallbacks[i]);
  }
  //m_Tracker->GetTool(m_RefPlugPort)->AddObserver(vtkCommand::ModifiedEvent, this->m_RefPlugCallbackObserver);
  //m_Tracker->GetTool(m_TestPlugPort)->AddObserver(vtkCommand::ModifiedEvent, this->m_TestPlugCallbackObserver);

  if( !m_Tracker->Probe() )
  {
    this->PopUpError("The tracking system you specified is not attached. Please check your connections and retry." );
    return;
  }

  if(this->m_TrackerSettingsDialog->getSystem() == NDI_AURORA 
    || this->m_TrackerSettingsDialog->getSystem() == NDI_SPECTRA )
  {
    // update the volume information.
    this->m_VolumeSelectionComboBox->setEnabled(true);
    this->m_VolumeSelectionComboBox->setVisible(true);
    this->m_VolumeSelectionComboBox->clear();
    nVolumes = dynamic_cast<vtkNDITracker*>(m_Tracker)->GetNumTrackingVolumes();
    for( int i=0; i < nVolumes; i++ )
    {
      this->m_VolumeSelectionComboBox->insertItem(i,
        QString::fromStdString(dynamic_cast<vtkNDITracker*>(m_Tracker)->GetTrackingVolumeShapeType(i)));
    }
  }
  
  m_StartTrackingButton->setEnabled(true);
  emit TrackerConfigured(QString(m_Tracker->GetSerialNumber()));
}

void vtkTrackerWidget::OnVolumeSelected(int volume)
{
  // TODO: make this work with Polaris as well.
  if( this->m_TrackerSettingsDialog->getSystem() == NDI_AURORA )
  {
    dynamic_cast<vtkNDITracker*>(m_Tracker)->SetVolume(volume);
  }
}

void vtkTrackerWidget::OnStartTracker()
{
  this->m_VolumeSelectionComboBox->setEnabled(false);
  m_StartTrackingButton->setEnabled(false);
  m_StopTrackingButton->setEnabled(true);

  m_Tracker->StartTracking();

  // check the serial number.
  //TODO: emit the serial number. this->setSerialNumber( QString(m_Tracker->GetSerialNumber()) );
  m_Timer->start((int)(1000/(this->m_TrackerUpdateFrequency*2)));
  
  emit TrackerStarted();
}

void vtkTrackerWidget::OnStopTracker()
{
  m_Timer->stop();
  m_Tracker->StopTracking();

  m_StartTrackingButton->setEnabled(true);
  m_StopTrackingButton->setEnabled(false);
  this->m_VolumeSelectionComboBox->setEnabled(true);

  emit TrackerStopped();
}
void vtkTrackerWidget::UpdateToolTransform(int port, QString status)
{
  ndSetXfrmMissing(&this->m_xfrms[port]);
  emit ToolTransformUpdated(port, status);
}

void vtkTrackerWidget::UpdateToolTransform(int port, ndQuatTransformation xfrm)
{
  ndCopyTransform(&xfrm, &this->m_xfrms[port]);
  emit ToolTransformUpdated(port, xfrm);
}

void vtkTrackerWidget::UpdateToolTransform(int port, ndQuatTransformation xfrm, double effFreq, double quality)
{
  this->UpdateToolTransform(port, xfrm);
  this->m_effectiveFrequencies[port] = effFreq;
  emit this->ToolEffectiveFrequencyUpdated(port, effFreq);
  this->m_quality[port] = quality;
  emit this->ToolQualityNumberUpdated(port, quality);
}

void vtkTrackerWidget::UpdateData ()
{
  if (m_Tracker)
  {
    if (m_Tracker->IsTracking())
    {
      this->m_Tracker->Update();
    }
  }
}

void vtkTrackerWidget::PopUpError(QString str)
{
  QMessageBox::critical(this, tr("Tracker Configuration Error"), str);
}