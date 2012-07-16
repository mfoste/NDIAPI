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

#include "vtkFakeTrackerSettingsWidget.h"
#include "vtkAuroraTrackerSettingsWidget.h"
#if defined (Ascension3DG_DriveBay) || defined (Ascension3DG_MedSafe) || defined (Ascension3DG_TrakStar) || defined (Ascension3DG_TrakStar2)
#include "vtkAscension3DGTrackerSettingsWidget.h"
#endif
#include "vtkSpectraTrackerSettingsWidget.h"

#include "vtkTrackerSettingsDialog.h"

vtkTrackerSettingsDialog* vtkTrackerSettingsDialog::New()
{
  return vtkTrackerSettingsDialog::New(0);
}

vtkTrackerSettingsDialog* vtkTrackerSettingsDialog::New(QWidget *parent)
{
  return new vtkTrackerSettingsDialog(parent);
}

vtkTrackerSettingsDialog::vtkTrackerSettingsDialog( QWidget *parent )
  : QDialog(parent), m_GUI(new Ui::TrackerSettingsDialog)
{
  this->m_Parent = parent;
  // set up the GUI.
  m_GUI->setupUi(this);

  
}

vtkTrackerSettingsDialog::~vtkTrackerSettingsDialog()
{
}

void vtkTrackerSettingsDialog::CreateActions()
{
  connect(this->m_GUI->trackingSystemComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTrackingSystemChanged(int)) );
  
}

void vtkTrackerSettingsDialog::Initialize(QString settingsFile)
{
  if( settingsFile.isEmpty() )
  {
    settingsFile = "./tracker.ini";
  }
  // initialize the settings.
  this->m_Settings = new QSettings(settingsFile, QSettings::IniFormat);

  // add the FakeTrackerSettingsWidget to the stackedWidget
  this->m_FakeTrackerSettingsWidget = new vtkFakeTrackerSettingsWidget( this, this->m_Settings );
  this->m_GUI->stackedWidget->insertWidget(FAKE_TRACKER, this->m_FakeTrackerSettingsWidget );
  // add the AuroraTrackerSettingsWidget to the stackedWidget
  this->m_AuroraSettingsWidget = new vtkAuroraTrackerSettingsWidget( this, this->m_Settings );
  this->m_GUI->stackedWidget->insertWidget(NDI_AURORA, this->m_AuroraSettingsWidget );
#if defined (Ascension3DG_DriveBay) || defined (Ascension3DG_MedSafe) || defined (Ascension3DG_TrakStar) || defined (Ascension3DG_TrakStar2)
  this->m_Ascension3DGSettingsWidget = new vtkAscension3DGTrackerSettingsWidget( this, this->m_Settings );
  this->m_GUI->stackedWidget->insertWidget(ASCENSION_3DG, this->m_Ascension3DGSettingsWidget );
#endif
  // add the SpectraTrackerSettingsWidget to the stackedWidget
  this->m_SpectraSettingsWidget = new vtkSpectraTrackerSettingsWidget(this, this->m_Settings);
  this->m_GUI->stackedWidget->insertWidget(NDI_SPECTRA, this->m_SpectraSettingsWidget );
  
  // set up the connections in the GUI.
  this->CreateActions();
  
  // update the tracking system comboBox.
  this->m_GUI->trackingSystemComboBox->addItem("Fake Tracker", FAKE_TRACKER);
  this->m_GUI->trackingSystemComboBox->addItem("Aurora", NDI_AURORA);
#if defined (Ascension3DG_DriveBay)
  this->m_GUI->trackingSystemComboBox->addItem("Ascension 3DG driveBAY", ASCENSION_3DG );
#elif defined (Ascension3DG_MedSafe)
  this->m_GUI->trackingSystemComboBox->addItem("Ascension 3DG medSAFE", ASCENSION_3DG );
#elif defined (Ascension3DG_TrakStar)
  this->m_GUI->trackingSystemComboBox->addItem("Ascension 3DG trakSTAR", ASCENSION_3DG );
#elif defined (Ascension3DG_TrakStar2)
  this->m_GUI->trackingSystemComboBox->addItem("Ascension 3DG trakSTAR2", ASCENSION_3DG );
#endif
  this->m_GUI->trackingSystemComboBox->addItem("Spectra", NDI_SPECTRA);

  // read the tracker settings in. 
  this->ReadTrackerSettings();
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
  this->m_GUI->trackingSystemComboBox->setCurrentIndex(this->m_GUI->trackingSystemComboBox->findData(this->m_System));
  this->m_GUI->stackedWidget->setCurrentIndex(this->m_System);

  // Fake Tracker Settings.
  this->m_FakeTrackerSettingsWidget->ReadTrackerSettings();
  // Aurora Tracker Settings.
  this->m_AuroraSettingsWidget->ReadTrackerSettings();
  // Ascension Tracker Settings.
#if defined (Ascension3DG_DriveBay) || defined (Ascension3DG_MedSafe) || defined (Ascension3DG_TrakStar) || defined (Ascension3DG_TrakStar2)
  this->m_Ascension3DGSettingsWidget->ReadTrackerSettings();
#endif
  // Spectra Tracker Settings.
  this->m_SpectraSettingsWidget->ReadTrackerSettings();
}

void vtkTrackerSettingsDialog::WriteTrackerSettings()
{
  this->m_System = this->m_GUI->trackingSystemComboBox->itemData(this->m_GUI->trackingSystemComboBox->currentIndex()).toInt();
  this->m_Settings->setValue("TrackingSystem", this->m_System );
  
  // Fake Tracker Settings.
  this->m_FakeTrackerSettingsWidget->WriteTrackerSettings();
  // Aurora Tracker Settings.
  this->m_AuroraSettingsWidget->WriteTrackerSettings();  
  // Ascension Tracker Settings.
#if defined (Ascension3DG_DriveBay) || defined (Ascension3DG_MedSafe) || defined (Ascension3DG_TrakStar) || defined (Ascension3DG_TrakStar2)
  this->m_Ascension3DGSettingsWidget->WriteTrackerSettings();
#endif
  // Spectra Tracker Settings.
  this->m_SpectraSettingsWidget->WriteTrackerSettings();
}

void vtkTrackerSettingsDialog::OnTrackingSystemChanged(int index)
{
  
  // change the system ID.
  this->m_System = this->m_GUI->trackingSystemComboBox->itemData(index).toInt();
  this->m_GUI->stackedWidget->setCurrentIndex(this->m_System);
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