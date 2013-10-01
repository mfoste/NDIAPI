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
#define MAX_TRACKED_PORTS 12

#include <QtGui>

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

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

  // set-up the tracked objects.
  this->m_TrackedObjects.resize(MAX_TRACKED_PORTS);
  for(int i=0; i<MAX_TRACKED_PORTS; i++)
  {
    this->m_TrackedObjects[i].object = vtkTrackedObject::New();
    this->m_TrackedObjects[i].xfrm = new ndQuatTransformation;
  }
  // add the labels manually, need to figure out a way to loop this. Maybe.
  // data labels.
  this->m_TrackedObjects[0].datalabel = this->m_GUI->port01DataLabel;
  this->m_TrackedObjects[1].datalabel = this->m_GUI->port02DataLabel;
  this->m_TrackedObjects[2].datalabel = this->m_GUI->port03DataLabel;
  this->m_TrackedObjects[3].datalabel = this->m_GUI->port04DataLabel;
  this->m_TrackedObjects[4].datalabel = this->m_GUI->port05DataLabel;
  this->m_TrackedObjects[5].datalabel = this->m_GUI->port06DataLabel;
  this->m_TrackedObjects[6].datalabel = this->m_GUI->port07DataLabel;
  this->m_TrackedObjects[7].datalabel = this->m_GUI->port08DataLabel;
  this->m_TrackedObjects[8].datalabel = this->m_GUI->port09DataLabel;
  this->m_TrackedObjects[9].datalabel = this->m_GUI->port10DataLabel;
  this->m_TrackedObjects[10].datalabel = this->m_GUI->port11DataLabel;
  this->m_TrackedObjects[11].datalabel = this->m_GUI->port12DataLabel;
  // xfrm labels.
  this->m_TrackedObjects[0].xfrmlabel = this->m_GUI->port01XfrmLabel;
  this->m_TrackedObjects[1].xfrmlabel = this->m_GUI->port02XfrmLabel;
  this->m_TrackedObjects[2].xfrmlabel = this->m_GUI->port03XfrmLabel;
  this->m_TrackedObjects[3].xfrmlabel = this->m_GUI->port04XfrmLabel;
  this->m_TrackedObjects[4].xfrmlabel = this->m_GUI->port05XfrmLabel;
  this->m_TrackedObjects[5].xfrmlabel = this->m_GUI->port06XfrmLabel;
  this->m_TrackedObjects[6].xfrmlabel = this->m_GUI->port07XfrmLabel;
  this->m_TrackedObjects[7].xfrmlabel = this->m_GUI->port08XfrmLabel;
  this->m_TrackedObjects[8].xfrmlabel = this->m_GUI->port09XfrmLabel;
  this->m_TrackedObjects[9].xfrmlabel = this->m_GUI->port10XfrmLabel;
  this->m_TrackedObjects[10].xfrmlabel = this->m_GUI->port11XfrmLabel;
  this->m_TrackedObjects[11].xfrmlabel = this->m_GUI->port12XfrmLabel;
  // effective frequency line edits.
  this->m_TrackedObjects[0].effFreqLineEdit = this->m_GUI->port01EffFreqLineEdit;
  this->m_TrackedObjects[1].effFreqLineEdit = this->m_GUI->port02EffFreqLineEdit;
  this->m_TrackedObjects[2].effFreqLineEdit = this->m_GUI->port03EffFreqLineEdit;
  this->m_TrackedObjects[3].effFreqLineEdit = this->m_GUI->port04EffFreqLineEdit;
  this->m_TrackedObjects[4].effFreqLineEdit = this->m_GUI->port05EffFreqLineEdit;
  this->m_TrackedObjects[5].effFreqLineEdit = this->m_GUI->port06EffFreqLineEdit;
  this->m_TrackedObjects[6].effFreqLineEdit = this->m_GUI->port07EffFreqLineEdit;
  this->m_TrackedObjects[7].effFreqLineEdit = this->m_GUI->port08EffFreqLineEdit;
  this->m_TrackedObjects[8].effFreqLineEdit = this->m_GUI->port09EffFreqLineEdit;
  this->m_TrackedObjects[9].effFreqLineEdit = this->m_GUI->port10EffFreqLineEdit;
  this->m_TrackedObjects[10].effFreqLineEdit = this->m_GUI->port11EffFreqLineEdit;
  this->m_TrackedObjects[11].effFreqLineEdit = this->m_GUI->port12EffFreqLineEdit;
  // effective frequency line edits.
  this->m_TrackedObjects[0].frameLineEdit = this->m_GUI->port01FrameLineEdit;
  this->m_TrackedObjects[1].frameLineEdit = this->m_GUI->port02FrameLineEdit;
  this->m_TrackedObjects[2].frameLineEdit = this->m_GUI->port03FrameLineEdit;
  this->m_TrackedObjects[3].frameLineEdit = this->m_GUI->port04FrameLineEdit;
  this->m_TrackedObjects[4].frameLineEdit = this->m_GUI->port05FrameLineEdit;
  this->m_TrackedObjects[5].frameLineEdit = this->m_GUI->port06FrameLineEdit;
  this->m_TrackedObjects[6].frameLineEdit = this->m_GUI->port07FrameLineEdit;
  this->m_TrackedObjects[7].frameLineEdit = this->m_GUI->port08FrameLineEdit;
  this->m_TrackedObjects[8].frameLineEdit = this->m_GUI->port09FrameLineEdit;
  this->m_TrackedObjects[9].frameLineEdit = this->m_GUI->port10FrameLineEdit;
  this->m_TrackedObjects[10].frameLineEdit = this->m_GUI->port11FrameLineEdit;
  this->m_TrackedObjects[11].frameLineEdit = this->m_GUI->port12FrameLineEdit;
  // quality value line edits.
  this->m_TrackedObjects[0].qualityLineEdit = this->m_GUI->port01QualityLineEdit;
  this->m_TrackedObjects[1].qualityLineEdit = this->m_GUI->port02QualityLineEdit;
  this->m_TrackedObjects[2].qualityLineEdit = this->m_GUI->port03QualityLineEdit;
  this->m_TrackedObjects[3].qualityLineEdit = this->m_GUI->port04QualityLineEdit;
  this->m_TrackedObjects[4].qualityLineEdit = this->m_GUI->port05QualityLineEdit;
  this->m_TrackedObjects[5].qualityLineEdit = this->m_GUI->port06QualityLineEdit;
  this->m_TrackedObjects[6].qualityLineEdit = this->m_GUI->port07QualityLineEdit;
  this->m_TrackedObjects[7].qualityLineEdit = this->m_GUI->port08QualityLineEdit;
  this->m_TrackedObjects[8].qualityLineEdit = this->m_GUI->port09QualityLineEdit;
  this->m_TrackedObjects[9].qualityLineEdit = this->m_GUI->port10QualityLineEdit;
  this->m_TrackedObjects[10].qualityLineEdit = this->m_GUI->port11QualityLineEdit;
  this->m_TrackedObjects[11].qualityLineEdit = this->m_GUI->port12QualityLineEdit;

  // set-up viewer items.
  this->m_Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->m_axesActor = vtkSmartPointer<vtkAxesActor>::New();

  // configure viewer items.
  m_GUI->view3DQVTKWidget->GetRenderWindow()->AddRenderer(this->m_Renderer);
  this->m_axesActor->SetTotalLength(100.0,100.0,100.0);
  this->m_Renderer->AddActor(this->m_axesActor);
  // add the tool actors.
  for(int i=0; i<12; i++)
  {
    this->m_Renderer->AddActor(this->m_TrackedObjects[i].object->GetModelActor());
    this->m_Renderer->AddActor(this->m_TrackedObjects[i].object->GetAxesActor());
  }

  // set up the connections.
  this->CreateActions();
}

NDIQtTrackGUI::~NDIQtTrackGUI()
{
  for(int i=0; i<MAX_TRACKED_PORTS; i++)
  {    
    delete this->m_TrackedObjects[i].xfrm;
  }
  this->m_TrackedObjects.clear();
  delete m_mutex;
  delete m_GUI;
}

void NDIQtTrackGUI::CreateActions()
{
  connect(m_GUI->actionAboutNDIQtTrack, SIGNAL(triggered()), this, SLOT(About()));
  connect(m_GUI->actionAboutQt, SIGNAL(triggered()), this, SLOT(AboutQt()));
  connect(m_GUI->TrackerWidget, SIGNAL(TrackerConfigured(QString)), this, SLOT(OnTrackerConfigured(QString)) );
  connect(m_GUI->TrackerWidget, SIGNAL(TrackerStarted()), this, SLOT(OnTrackerStarted()) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolInfoUpdated(int)), this, SLOT(OnToolInfoUpdated(int)) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolTransformUpdated(int,int,QString)), this, SLOT(OnToolTransformUpdated(int,int,QString)) );
  /* older versions, link into the single function below now.
  connect(m_GUI->TrackerWidget, SIGNAL(ToolTransformUpdated(int,int,ndQuatTransformation)), this, SLOT(OnToolTransformUpdated(int,int,ndQuatTransformation)) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolEffectiveFrequencyUpdated(int,int,double)), this, SLOT(OnToolEffectiveFrequencyUpdated(int,int,double)) );
  connect(m_GUI->TrackerWidget, SIGNAL(ToolQualityNumberUpdated(int,int,double)), this, SLOT(OnToolQualityUpdated(int,int,double)) );
  */
  connect(m_GUI->TrackerWidget, SIGNAL(ToolTransformUpdated(int,int,ndQuatTransformation,double,double)), this, SLOT(OnToolTransformUpdated(int,int,ndQuatTransformation,double,double)) );
  // hook up the tracked objects.
  for(int i=0; i<12; i++)
  {
    connect(this->m_TrackedObjects[i].object, SIGNAL(TrackedObjectUpdated()), this, SLOT(OnTrackedObjectUpdated()) );
  }
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
  for(int tool=0; tool < this->m_GUI->TrackerWidget->getTracker()->GetNumberOfTools(); tool++)
  {
    this->OnToolInfoUpdated(tool);    
  }
}

void NDIQtTrackGUI::OnToolInfoUpdated(int port)
{
  QString toolInfo;
  // create the string
  toolInfo = "PN-" + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(port)->GetToolPartNumber())
    + "-SN-" + QString(this->m_GUI->TrackerWidget->getTracker()->GetTool(port)->GetToolSerialNumber());

  if( port < MAX_TRACKED_PORTS)
  {
    this->m_TrackedObjects[port].datalabel->setText(toolInfo);
    // TODO: figure out how to determine if tool changes.
    this->m_TrackedObjects[port].object->SetModelVisibility(true);
    this->m_TrackedObjects[port].object->SetAxesVisibility(true);

  }
  else
  {
    //default:
    std::cout << "Port " << port << " exists but will not be updated in this app." << std::endl;
  }
}

void NDIQtTrackGUI::OnToolTransformUpdated(int port, int frame, QString status) 
{
  if( port < MAX_TRACKED_PORTS )
  {
    this->m_TrackedObjects[port].frameLineEdit->setText(QString("%1").arg(frame));
    this->m_TrackedObjects[port].xfrmlabel->setText( status );
  }
  else
  {
    // do nothing for now.
  }
}

/* old versions, link into single function now.
void NDIQtTrackGUI::OnToolTransformUpdated(int port, int frame, ndQuatTransformation xfrm)
{
  if( port < MAX_TRACKED_PORTS)
  {
    this->m_TrackedObjects[port].xfrmlabel->setText( this->GetXfrmString(xfrm) );
    ndCopyTransform(&xfrm, this->m_TrackedObjects[port].xfrm);
    this->m_TrackedObjects[port].object->UpdateXfrm(&xfrm);
  }
  else
  {
    // do nothing for now.
  }
}

void NDIQtTrackGUI::OnToolEffectiveFrequencyUpdated(int port, int frame, double freq)
{
  if( port < MAX_TRACKED_PORTS)
  {
    this->m_TrackedObjects[port].effFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
  }
  else
  {
    // do nothing for now.
  }
}



void NDIQtTrackGUI::OnToolQualityUpdated(int port, int frame, double quality)
{
  int prec;

  switch(this->m_GUI->TrackerWidget->getTrackerSystemType())
  {
  case NDI_AURORA:
  case NDI_SPECTRA:
    prec = 4;
    break;
  case ASCENSION_3DG:
    prec = 0;
    break;
  default:
    prec = 4;
    break;
  }

  if( port < MAX_TRACKED_PORTS)
  {
    this->m_TrackedObjects[port].qualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', prec));
  }
  else
  {
    // do nothing for now.
  }
} */

void NDIQtTrackGUI::OnToolTransformUpdated(int port, int frame, ndQuatTransformation xfrm, double freq, double quality)
{
  int prec;

  switch(this->m_GUI->TrackerWidget->getTrackerSystemType())
  {
  case NDI_AURORA:
  case NDI_SPECTRA:
    prec = 4;
    break;
  case ASCENSION_3DG:
    prec = 0;
    break;
  default:
    prec = 4;
    break;
  }

  if( port < MAX_TRACKED_PORTS)
  {
    this->m_TrackedObjects[port].frameLineEdit->setText(QString("%1").arg(frame));
    this->m_TrackedObjects[port].xfrmlabel->setText( this->GetXfrmString(xfrm) );
    ndCopyTransform(&xfrm, this->m_TrackedObjects[port].xfrm);
    this->m_TrackedObjects[port].object->UpdateXfrm(&xfrm);
    this->m_TrackedObjects[port].effFreqLineEdit->setText(QString("%1").arg(freq, 0, 'f', 0));
    this->m_TrackedObjects[port].qualityLineEdit->setText(QString("%1").arg(quality, 1, 'f', prec));
  }
  else
  {
    // do nothing for now.
  }
}

void NDIQtTrackGUI::OnTrackedObjectUpdated()
{
  this->m_GUI->view3DQVTKWidget->GetRenderWindow()->Render();
  //this->m_Renderer->ResetCamera();
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
