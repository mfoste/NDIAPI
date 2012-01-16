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

  this->m_TrackerSettingsDialog = new vtkTrackerSettingsDialog(this);

  // set up the timer.
  m_Timer = new QTimer(this);
  this->m_TrackerUpdateFrequency = 20; // arbitrary default of 20.

  this->setupUi();
  this->setupUiLayout();
  this->CreateActions();  

  m_StartTrackingButton->setEnabled(false);
  m_StopTrackingButton->setEnabled(false);
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
  delete m_TrackerSettingsDialog;
}

QSize vtkTrackerWidget::sizeHint() const
{
  return QSize( 130, 110 );
}

void vtkTrackerWidget::setupUi()
{
  this->m_ConfigureTrackerButton = new QPushButton(this);
  this->m_ConfigureTrackerButton->setText("Configure Tracker");
  this->m_ConfigureTrackerButton->setMaximumWidth(110);

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
  mainLayout->addWidget(this->m_StartTrackingButton);
  mainLayout->addWidget(this->m_StopTrackingButton);

  setLayout( mainLayout );
}


void vtkTrackerWidget::CreateActions()
{
  connect(m_ConfigureTrackerButton, SIGNAL(clicked()), this, SLOT(OnConfigureTracker()));
  connect(m_TrackerSettingsDialog, SIGNAL(accepted()), this, SLOT(OnConfigureTrackerAccepted()));
  connect(m_StartTrackingButton, SIGNAL(clicked()), this, SLOT(OnStartTracker()));
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(UpdateData()));
  connect(m_StopTrackingButton, SIGNAL(clicked()), this, SLOT(OnStopTracker()));
}

void vtkTrackerWidget::OnConfigureTracker()
{
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
    break;
  case NDI_AURORA:
    m_Tracker = vtkNDITracker::New();
    dynamic_cast<vtkNDITracker*>(m_Tracker)->SetBaudRate(115200);
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
  
  m_StartTrackingButton->setEnabled(true);
  emit TrackerConfigured(QString(m_Tracker->GetSerialNumber()));
}

void vtkTrackerWidget::OnStartTracker()
{
  m_Tracker->StartTracking();

  // check the serial number.
  //TODO: emit the serial number. this->setSerialNumber( QString(m_Tracker->GetSerialNumber()) );
  m_Timer->start((int)(1000/(this->m_TrackerUpdateFrequency*2)));
  
  m_StartTrackingButton->setEnabled(false);
  m_StopTrackingButton->setEnabled(true);
  
  emit TrackerStarted();
}

void vtkTrackerWidget::OnStopTracker()
{
  m_Timer->stop();
  m_Tracker->StopTracking();

  m_StartTrackingButton->setEnabled(true);
  m_StopTrackingButton->setEnabled(false);

  emit TrackerStopped();
}
void vtkTrackerWidget::UpdateToolTransform(int port, QString status)
{
  SetXfrmMissing(&this->m_xfrms[port]);
  emit ToolTransformUpdated(port, status);
}

void vtkTrackerWidget::UpdateToolTransform(int port, QuatTransformation xfrm)
{
  CopyTransform(&xfrm, &this->m_xfrms[port]);
  emit ToolTransformUpdated(port, xfrm);
}

void vtkTrackerWidget::UpdateToolTransform(int port, QuatTransformation xfrm, double effFreq, double quality)
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