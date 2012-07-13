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
  this->m_GUI->TrackerWidget->Initialize();

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
  connect(m_GUI->TrackerWidget, SIGNAL(ToolTransformUpdated(int,ndQuatTransformation)), this, SLOT(OnToolTransformUpdated(int,ndQuatTransformation)) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolEffectiveFrequencyUpdated(int,double)), this, SLOT(OnToolEffectiveFrequencyUpdated(int,double)) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolQualityNumberUpdated(int,double)), this, SLOT(OnToolQualityUpdated(int,double)) );
}

void NDIQtTrackGUI::About()
{
  QString tmpStr;
  tmpStr = "<b>NDIQtTrack</b><br><br>" 
    "NDIQtTrack is a sample application that demonstrates " 
    "how to use the Qt GUI as part of the AIGS library.<br><br>" 
    "Copyright (C) 2011-2012, Northern Digital Inc. All rights reserved. <br><br>"
    "Built using VTK" + QString(vtkVersion::GetVTKVersion()) + "<br><br>"
    "All Northern Digital Inc. (“NDI”) Media and/or Sample Code and/or Sample Code "
    "Documentation (collectively referred to as “Sample Code”) is licensed and provided \"as "
    "is\" without warranty of any kind. The licensee, by use of the Sample Code, warrants to "
    "NDI that the Sample Code is fit for the use and purpose for which the licensee intends to "
    "use the Sample Code. NDI makes no warranties, express or implied, that the functions "
    "contained in the Sample Code will meet the licensee’s requirements or that the operation "
    "of the programs contained therein will be error free. This warranty as expressed herein is "
    "exclusive and NDI expressly disclaims any and all express and/or implied, in fact or in "
    "law, warranties, representations, and conditions of every kind pertaining in any way to "
    "the Sample Code licensed and provided by NDI hereunder, including without limitation, "
    "each warranty and/or condition of quality, merchantability, description, operation, "
    "adequacy, suitability, fitness for particular purpose, title, interference with use or "
    "enjoyment, and/or non infringement, whether express or implied by statute, common law, "
    "usage of trade, course of dealing, custom, or otherwise. No NDI dealer, distributor, agent "
    "or employee is authorized to make any modification or addition to this warranty. <br><br>"
    "In no event shall NDI nor any of its employees be liable for any direct, indirect, "
    "incidental, special, exemplary, or consequential damages, sundry damages or any "
    "damages whatsoever, including, but not limited to, procurement of substitute goods or "
    "services, loss of use, data or profits, or business interruption, however caused. In no "
    "event shall NDI’s liability to the licensee exceed the amount paid by the licensee for the "
    "Sample Code or any NDI products that accompany the Sample Code. The said limitations "
    "and exclusions of liability shall apply whether or not any such damages are construed as "
    "arising from a breach of a representation, warranty, guarantee, covenant, obligation,"
    "condition or fundamental term or on any theory of liability, whether in contract, strict"
    "liability, or tort (including negligence or otherwise) arising in any way out of the use of "
    "the Sample Code even if advised of the possibility of such damage. In no event shall "
    "NDI be liable for any claims, losses, damages, judgments, costs, awards, expenses or "
    "liabilities of any kind whatsoever arising directly or indirectly from any injury to person "
    "or property, arising from the Sample Code or any use thereof.";
  QMessageBox::about(this, tr("About NDIQtTrack"), tmpStr);
}

void NDIQtTrackGUI::AboutQt()
{
  QMessageBox::aboutQt(this, tr("About NDIQtTrack"));
}

void NDIQtTrackGUI::closeEvent(QCloseEvent *event)
{
  // if tracker exists.
  if( this->m_GUI->TrackerWidget->getTracker() )
  {
    // if it is tracking, stop it.
    if( this->m_GUI->TrackerWidget->getTracker()->IsTracking() )
    {
      this->m_GUI->TrackerWidget->getTracker()->StopTracking();
    }
  }
  event->accept();
}

void NDIQtTrackGUI::OnTrackerConfigured(QString systemInfo)
{
  this->m_GUI->trackerInfoDataLabel->setText( systemInfo );
}

void NDIQtTrackGUI::OnTrackerStarted()
{
   QString toolInfo;
   // update the tool info - port 1.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(0)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(0)->GetToolSerialNumber());
   this->m_GUI->port01DataLabel->setText( toolInfo );
   // update the tool info - port 2.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(1)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(1)->GetToolSerialNumber());
   this->m_GUI->port02DataLabel->setText( toolInfo );
   // update the tool info - port 3.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(2)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(2)->GetToolSerialNumber());
   this->m_GUI->port03DataLabel->setText( toolInfo );
   // update the tool info - port 4/A.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(3)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(3)->GetToolSerialNumber());
   this->m_GUI->port04DataLabel->setText( toolInfo );
   // update the tool info - port 5/B.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(4)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(4)->GetToolSerialNumber());
   this->m_GUI->port05DataLabel->setText( toolInfo );
   // update the tool info - port 6/C.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(5)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(5)->GetToolSerialNumber());
   this->m_GUI->port06DataLabel->setText( toolInfo );
   // update the tool info - port 7/D.
   toolInfo = QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(6)->GetToolPartNumber())
      + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(6)->GetToolSerialNumber());
   this->m_GUI->port07DataLabel->setText( toolInfo );

}

void NDIQtTrackGUI::OnToolTransformUpdated(int port, QString status) 
{
  switch(port){
  case 0:
    // Port 1
    this->m_GUI->port01XfrmLabel->setText( status );
    ndSetXfrmMissing(&this->m_Port01Xfrm);
    break;
  case 1:
    // Port 2
    this->m_GUI->port02XfrmLabel->setText( status );
    ndSetXfrmMissing(&this->m_Port02Xfrm);
    break;
  case 2:
    // Port 3
    this->m_GUI->port03XfrmLabel->setText(status);
    ndSetXfrmMissing(&this->m_Port03Xfrm);
    break;
  case 3:
    // Port 4
    this->m_GUI->port04XfrmLabel->setText(status);
    ndSetXfrmMissing(&this->m_Port04Xfrm);
    break;
  default:
    //do nothing for now.
    break;
  }
}

void NDIQtTrackGUI::OnToolTransformUpdated(int port, ndQuatTransformation xfrm)
{
  switch(port){
  case 0:
    // Port 1
    this->m_GUI->port01XfrmLabel->setText( this->GetXfrmString(xfrm) );
    ndCopyTransform(&xfrm, &this->m_Port01Xfrm);
    break;
  case 1:
    // Port 2
    this->m_GUI->port02XfrmLabel->setText( this->GetXfrmString(xfrm) );
    ndCopyTransform(&xfrm, &this->m_Port02Xfrm);
    break;
  case 2:
    // Port 3
    this->m_GUI->port03XfrmLabel->setText(this->GetXfrmString(xfrm));
    ndCopyTransform(&xfrm, &this->m_Port03Xfrm);
    break;
  case 3:
    // Port 4
    this->m_GUI->port04XfrmLabel->setText(this->GetXfrmString(xfrm));
    ndCopyTransform(&xfrm, &this->m_Port04Xfrm);
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
  case 4:
    // Port 5
    this->m_GUI->port05EffFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    break;
  case 5:
    // Port 6
    this->m_GUI->port06EffFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    break;
  case 6:
    // Port 7
    this->m_GUI->port07EffFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    break;
  default:
    //do nothing for now.
    break;
  }
}

void NDIQtTrackGUI::OnToolQualityUpdated(int port, double quality)
{
    switch(port){
    case 0:
      // Port 1
      this->m_GUI->port01QualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', 4));
      break;
    case 1:
      // Port 2
      this->m_GUI->port02QualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', 4));
      break;
    case 2:
      // Port 3
      this->m_GUI->port03QualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', 4));
      break;
    case 3:
      // Port 4
      this->m_GUI->port04QualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', 4));
      break;
    case 4:
      // Port 5
      this->m_GUI->port05QualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', 4));
      break;
    case 5:
      // Port 6
      this->m_GUI->port06QualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', 4));
      break;
    case 6:
      // Port 7
      this->m_GUI->port07QualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', 4));
      break;
    default:
      //do nothing for now.
      break;
    }
}

QString NDIQtTrackGUI::GetXfrmString(ndQuatTransformation xfrm)
{
  QString xfrmString;

  if( !ndIsXfrmMissing( &xfrm ) )
  {
  xfrmString.sprintf("%+ 1.6f %+ 1.6f %+ 1.6f %+ 1.6f %+ 6.2f  %+ 6.2f  %+ 6.2f", 
    xfrm.rotation.q0, xfrm.rotation.qx, xfrm.rotation.qy, xfrm.rotation.qz, 
    xfrm.translation.x, xfrm.translation.y, xfrm.translation.z);
  }
  else
  {
    xfrmString = "Missing.";
  }

  return xfrmString;
}
