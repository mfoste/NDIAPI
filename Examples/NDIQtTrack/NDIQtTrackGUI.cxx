#include <QtGui>

#include <vtkVersion.h>

#include "ndXfrms.h"

#include "vtkTracker.h"
#include "vtkTrackerTool.h"

#include "NDIQtTrackGUI.h"
#include "ui_NDIQtTrackGUI.h"

//*********************************************************
// Constructors/Destructors
//*********************************************************

NDIQtTrackGUI *NDIQtTrackGUI::New()
{
  return NDIQtTrackGUI::New(0);
}

NDIQtTrackGUI *NDIQtTrackGUI::New(QWidget *parent)
{
  return new NDIQtTrackGUI(parent);
}

NDIQtTrackGUI::NDIQtTrackGUI(QWidget *parent) 
: QMainWindow(parent), m_GUI(new Ui::NDIQtTrackMainWindow)
{
  // set up the mutex lock first.
  m_mutex = new QMutex();

  m_GUI->setupUi(this);

  // set up the connections.
  this->CreateActions();
}

NDIQtTrackGUI::~NDIQtTrackGUI()
{
  delete m_mutex;
  delete m_GUI;
}

void NDIQtTrackGUI::CreateActions()
{
  connect(m_GUI->actionAboutNDIQtTrack, SIGNAL(triggered()), this, SLOT(About()));
  connect(m_GUI->actionAboutQt, SIGNAL(triggered()), this, SLOT(AboutQt()));
  connect(m_GUI->TrackerWidget, SIGNAL(TrackerConfigured(QString)), this, SLOT(OnTrackerConfigured(QString)) );
  connect(m_GUI->TrackerWidget, SIGNAL(TrackerStarted()), this, SLOT(OnTrackerStarted()) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolTransformUpdated(int,QString)), this, SLOT(OnToolTransformUpdated(int,QString)) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolTransformUpdated(int,QuatTransformation)), this, SLOT(OnToolTransformUpdated(int,QuatTransformation)) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolEffectiveFrequencyUpdated(int,double)), this, SLOT(OnToolEffectiveFrequencyUpdated(int,double)) );
}

void NDIQtTrackGUI::About()
{
  QString tmpStr;
  tmpStr = "<b>NDIQtTrack</b><br><br>" 
    "NDIQtTrack is a sample application that demonstrates " 
    "how to use the Qt GUI as part of the AIGS GUI.<br><br>" 
    "Built using VTK" + QString(vtkVersion::GetVTKVersion());
  QMessageBox::about(this, tr("About NDIQtTrack"), tmpStr);
}

void NDIQtTrackGUI::AboutQt()
{
  QMessageBox::aboutQt(this, tr("About NDIQtTrack"));
}

void NDIQtTrackGUI::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void NDIQtTrackGUI::OnTrackerConfigured(QString systemInfo)
{
  this->m_GUI->trackerInfoDataLabel->setText( systemInfo );
}

void NDIQtTrackGUI::OnTrackerStarted()
{
   QString toolInfo;
   // update the tool info.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(0)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(0)->GetToolSerialNumber());
   this->m_GUI->port01DataLabel->setText( toolInfo );
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(1)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(1)->GetToolSerialNumber());
   this->m_GUI->port02DataLabel->setText( toolInfo );
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(2)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(2)->GetToolSerialNumber());
   this->m_GUI->port03DataLabel->setText( toolInfo );
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(3)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(3)->GetToolSerialNumber());
   this->m_GUI->port04DataLabel->setText( toolInfo );

}

void NDIQtTrackGUI::OnToolTransformUpdated(int port, QString status) 
{
  switch(port){
  case 0:
    // Port 1
    this->m_GUI->port01XfrmLabel->setText( status );
    SetXfrmMissing(&this->m_Port01Xfrm);
    break;
  case 1:
    // Port 2
    this->m_GUI->port02XfrmLabel->setText( status );
    SetXfrmMissing(&this->m_Port02Xfrm);
    break;
  case 2:
    // Port 3
    this->m_GUI->port03XfrmLabel->setText(status);
    SetXfrmMissing(&this->m_Port03Xfrm);
    break;
  case 3:
    // Port 4
    this->m_GUI->port04XfrmLabel->setText(status);
    SetXfrmMissing(&this->m_Port04Xfrm);
    break;
  default:
    //do nothing for now.
    break;
  }
}

void NDIQtTrackGUI::OnToolTransformUpdated(int port, QuatTransformation xfrm)
{
  switch(port){
  case 0:
    // Port 1
    this->m_GUI->port01XfrmLabel->setText( this->GetXfrmString(xfrm) );
    CopyTransform(&xfrm, &this->m_Port01Xfrm);
    break;
  case 1:
    // Port 2
    this->m_GUI->port02XfrmLabel->setText( this->GetXfrmString(xfrm) );
    CopyTransform(&xfrm, &this->m_Port02Xfrm);
    break;
  case 2:
    // Port 3
    this->m_GUI->port03XfrmLabel->setText(this->GetXfrmString(xfrm));
    CopyTransform(&xfrm, &this->m_Port03Xfrm);
    break;
  case 3:
    // Port 4
    this->m_GUI->port04XfrmLabel->setText(this->GetXfrmString(xfrm));
    CopyTransform(&xfrm, &this->m_Port04Xfrm);
    break;
  default:
    //do nothing for now.
    break;
  }
}

void NDIQtTrackGUI::OnToolEffectiveFrequencyUpdated(int port, double freq)
{
  switch(port){
  case 0:
    // Port 1
    this->m_GUI->port01EffFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    break;
  case 1:
    // Port 2
    this->m_GUI->port02EffFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    break;
  case 2:
    // Port 3
    this->m_GUI->port03EffFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    break;
  case 3:
    // Port 4
    this->m_GUI->port04EffFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    break;
  default:
    //do nothing for now.
    break;
  }
}

void NDIQtTrackGUI::OnToolQualityUpdated(int port, double quality)
{
  // to implement.
}

QString NDIQtTrackGUI::GetXfrmString(QuatTransformation xfrm)
{
  QString xfrmString;

  if( !IsXfrmMissing( &xfrm ) )
  {
  xfrmString.sprintf("%+ 1.6f %+ 1.6f %+ 1.6f %+ 1.6f %+ 6.4f  %+ 6.4f  %+ 6.4f", 
    xfrm.rotation.q0, xfrm.rotation.qx, xfrm.rotation.qy, xfrm.rotation.qz, 
    xfrm.translation.x, xfrm.translation.y, xfrm.translation.z);
  }
  else
  {
    xfrmString = "Missing.";
  }

  return xfrmString;
}
