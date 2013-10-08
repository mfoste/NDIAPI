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

//#include "vtkTracker.h"
//#include "vtkFakeTracker.h"
//#include "vtkNDITracker.h"
//
//#if defined (Ascension3DG_MedSafe)
//#include "ATC3DGm.h"
//#include "vtkAscension3DGTracker.h"
//#else
//#include "ATC3DG.h"
//#include "vtkAscension3DGTracker.h"
//#endif

#include "QTracker.h"
#include "vtkTrackerSettingsStructures.h"
#include "vtkTrackerSettingsDialog.h"

#include "vtkTrackerWidget.h"

vtkTrackerWidget::vtkTrackerWidget(QWidget *parent ) : QWidget(parent)
{
  m_TrackerObject = 0;
  this->m_bUsePreStart = false;
  
  this->m_TrackerSettingsDialog = vtkTrackerSettingsDialog::New(this);

  // initialize the thread and tracker object here.
  this->m_TrackerThread = new QThread;
  this->m_TrackerObject = new QTracker();
  this->m_TrackerObject->moveToThread(this->m_TrackerThread);
  
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
  this->m_VolumeSelectionComboBox->setVisible(false);

  this->m_PreStartTrackingButton = new QPushButton(this);
  this->m_PreStartTrackingButton->setText("Pre-Start Configure");
  this->m_PreStartTrackingButton->setEnabled(false);
  this->m_PreStartTrackingButton->setVisible(false);

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
  mainLayout->addWidget(this->m_PreStartTrackingButton);
  mainLayout->addWidget(this->m_StartTrackingButton);
  mainLayout->addWidget(this->m_StopTrackingButton);

  setLayout( mainLayout );
}

void vtkTrackerWidget::closeEvent(QCloseEvent *event)
{
  QWidget::closeEvent(event);
  emit this->CloseTrackerWidget();
  event->accept();
}


void vtkTrackerWidget::CreateActions()
{
  // connect the buttons and dialogs.
  connect(m_ConfigureTrackerButton, SIGNAL(clicked()), this, SLOT(OnConfigureTracker()));
  connect(m_TrackerSettingsDialog, SIGNAL(accepted()), this, SLOT(OnConfigureTrackerAccepted()));
  connect(m_TrackerSettingsDialog, SIGNAL(rejected()), this, SLOT(OnConfigureTrackerCanceled()));
  connect(m_VolumeSelectionComboBox, SIGNAL(activated(int)), this, SLOT(OnVolumeSelected(int)));
  connect(m_PreStartTrackingButton, SIGNAL(clicked()), this, SLOT(OnPreStartTracker()));
  connect(m_StartTrackingButton, SIGNAL(clicked()), this, SLOT(OnStartTracker()));
  connect(m_StopTrackingButton, SIGNAL(clicked()), this, SLOT(OnStopTracker()));

  // connect this widget to the tracker object.
  connect(this, SIGNAL(TrackerConfigurationDialogOpened()), this->m_TrackerObject, SLOT(OnConfigureTracker()) );
  connect(this, SIGNAL(ConfigureFakeTracker(int,vtkFakeTrackerSettings*)), this->m_TrackerObject, SLOT(OnConfigureFakeTracker(int,vtkFakeTrackerSettings*)) );
  connect(this, SIGNAL(ConfigureAuroraTracker(int,ndiAuroraSettings*)), this->m_TrackerObject, SLOT(OnConfigureAuroraTracker(int,ndiAuroraSettings*)) );
#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
  connect(this, SIGNAL(ConfigureAscension3DGTracker(int,ascension3DGSettings*)), this->m_TrackerObject, SLOT(OnConfigureAscension3DGTracker(int,ascension3DGSettings*)) );
#endif
  connect(this, SIGNAL(ConfigureSpectraVicraTracker(int,ndiSpectraVicraSettings*)), this->m_TrackerObject, SLOT(OnConfigureSpectraVicraTracker(int,ndiSpectraVicraSettings*)) );
  connect(this, SIGNAL(VolumeSelected(int)), this->m_TrackerObject, SLOT(OnVolumeSelected(int)) );
  connect(this, SIGNAL(PreStartTracking()), this->m_TrackerObject, SLOT(OnPreConfigureTracker()) );
  connect(this, SIGNAL(StartTracking()), this->m_TrackerObject, SLOT(OnStartTracker()) );
  connect(this, SIGNAL(StopTracking()), this->m_TrackerObject, SLOT(OnStopTracker()) );
  connect(this, SIGNAL(CloseTrackerWidget()), this->m_TrackerObject, SLOT(OnCloseTracker()) );
  // thread connections -- for details see 
  //http://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
  connect(this->m_TrackerThread, SIGNAL(started()), this->m_TrackerObject, SLOT(OnInitialize()));
  connect(this->m_TrackerObject, SIGNAL(finished()), this->m_TrackerThread, SLOT(quit()));
  connect(this->m_TrackerObject, SIGNAL(finished()), this->m_TrackerObject, SLOT(deleteLater()));
  connect(this->m_TrackerThread, SIGNAL(finished()), this->m_TrackerThread, SLOT(deleteLater()));
  this->m_TrackerThread->start();
}

void vtkTrackerWidget::OnConfigureTracker()
{
  // tell the tracker to stop tracking, if it is.
  emit this->TrackerConfigurationDialogOpened();

  // clean up the button functionality and visibility when configuring.
  this->m_StartTrackingButton->setEnabled(false);
  this->m_StopTrackingButton->setEnabled(false);
  this->m_VolumeSelectionComboBox->setEnabled(false);
  this->m_VolumeSelectionComboBox->setVisible(false);

  /* launch tracker settings dialog. */
  this->m_TrackerSettingsDialog->UpdateAndShow();
}

void vtkTrackerWidget::OnConfigureTrackerAccepted()
{
  this->ConfigureTracker();
}

void vtkTrackerWidget::OnConfigureTrackerCanceled()
{
  if( this->m_TrackerObject )
  {
    this->m_StartTrackingButton->setEnabled(true);
    if(this->m_TrackerObject->GetTrackerType() == NDI_AURORA 
      || this->m_TrackerObject->GetTrackerType() == NDI_SPECTRA 
      || this->m_TrackerObject->GetTrackerType() == NDI_SPECTRA_HYBRID )
    {
      // update the volume information.
      this->m_VolumeSelectionComboBox->setEnabled(true);
      this->m_VolumeSelectionComboBox->setVisible(true);
    }
  }
}

void vtkTrackerWidget::OnVolumeListUpdated(QStringList volumeList)
{
  // update the volume information.
    this->m_VolumeSelectionComboBox->setEnabled(true);
    this->m_VolumeSelectionComboBox->setVisible(true);
    this->m_VolumeSelectionComboBox->clear();
    // get the volumes list.
    int nVolumes = volumeList.size();
    for( int i=0; i < nVolumes; i++ )
    {
      this->m_VolumeSelectionComboBox->insertItem(i, volumeList[1]);
    }
    this->m_VolumeSelectionComboBox->setCurrentIndex(0);
    this->OnVolumeSelected(0);
}

void vtkTrackerWidget::ConfigureTracker()
{
  QString errorString;

  //TODO: Fix
  //QMessageBox configMessage("NDI Aurora Map", "Configuring Tracker, please wait.");
  //configMessage.setWindowModality(Qt::WindowModal);
  //configMessage.show();
  
  switch( this->m_TrackerSettingsDialog->getSystem() )
  {
  case FAKE_TRACKER:
    emit this->ConfigureFakeTracker(this->m_TrackerSettingsDialog->getSystem(), 
      this->m_TrackerSettingsDialog->getFakeTrackerSettings());
    break;
  case NDI_AURORA:
    emit this->ConfigureAuroraTracker(this->m_TrackerSettingsDialog->getSystem(),
      this->m_TrackerSettingsDialog->getAuroraSettings());
    break;
// Ascension Tracker Settings.
#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
  case ASCENSION_3DG:
    emit this->ConfigureAscension3DGTracker(this->m_TrackerSettingsDialog->getSystem(),
      this->m_TrackerSettingsDialog->getAscension3DGSettings());
    // note: volume selection not needed for Ascension.
    this->m_VolumeSelectionComboBox->setEnabled(false);
    this->m_VolumeSelectionComboBox->setVisible(false);
    break;
#endif
  case NDI_SPECTRA:
  case NDI_SPECTRA_HYBRID:
  case NDI_VICRA:
    emit this->ConfigureSpectraVicraTracker(this->m_TrackerSettingsDialog->getSystem(),
      this->m_TrackerSettingsDialog->getSpectraVicraSettings());
    break;
  default:
    this->PopUpError("Invalid tracker system type given.  Check your tracker settings.");
    return;
  }

  if( this->m_bUsePreStart )
  {
    this->m_PreStartTrackingButton->setVisible(true);
    this->m_PreStartTrackingButton->setEnabled(true);
  }
     
  m_StartTrackingButton->setEnabled(true);
    
  //configMessage.hide();
}

void vtkTrackerWidget::OnVolumeSelected(int volume)
{
  emit this->VolumeSelected(volume);
}

void vtkTrackerWidget::OnPreStartTracker()
{
  emit this->PreStartTracking();
}

void vtkTrackerWidget::OnStartTracker()
{
  this->m_VolumeSelectionComboBox->setEnabled(false);
  m_StartTrackingButton->setEnabled(false);
  m_PreStartTrackingButton->setEnabled(false);
  m_StopTrackingButton->setEnabled(true);

  emit this->StartTracking();

  this->m_VolumeSelectionComboBox->setEnabled(false);
}

void vtkTrackerWidget::OnStopTracker()
{
  emit this->StopTracking();

  m_PreStartTrackingButton->setEnabled(true);
  m_StartTrackingButton->setEnabled(true);
  m_StopTrackingButton->setEnabled(false);

  if( (this->m_TrackerObject->GetTrackerType() == NDI_AURORA) 
    || (this->m_TrackerObject->GetTrackerType() == NDI_SPECTRA) )
  {
    this->m_VolumeSelectionComboBox->setEnabled(true);
  }
}

int vtkTrackerWidget::getTrackerSystemType()
{
  return this->m_TrackerObject->GetTrackerType();
}

void vtkTrackerWidget::PopUpError(QString str)
{
  QMessageBox::critical(this, tr("Tracker Configuration Error"), str);
}