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

#include "vtkTrackerSettingsDialog.h"
#include "vtkAscension3DGTrackerSettingsWidget.h"

vtkAscension3DGTrackerSettingsWidget::vtkAscension3DGTrackerSettingsWidget( QWidget *parent, QSettings *settings )
{
  this->m_Parent = parent;
  this->m_Settings = settings;
  // set up the GUI.
  m_Widget.setupUi(this);
  this->CreateActions();

  // set validators on port line edits.
  this->m_Widget.updateFrequencyAscension3DGLineEdit->setValidator(new QDoubleValidator(0.1, 40, 1, this) );
}

vtkAscension3DGTrackerSettingsWidget::vtkAscension3DGTrackerSettingsWidget( QWidget *parent)
{
  vtkAscension3DGTrackerSettingsWidget(parent, 0);
}

vtkAscension3DGTrackerSettingsWidget::~vtkAscension3DGTrackerSettingsWidget()
{
}

void vtkAscension3DGTrackerSettingsWidget::CreateActions()
{
  connect( this->m_Widget.useAllSensorsAscension3DGCheckBox, SIGNAL(clicked(bool)), this, SLOT(OnUseAllSensors(bool)) );
}

void vtkAscension3DGTrackerSettingsWidget::OnUseAllSensors(bool useAllSensors)
{
  if( useAllSensors )
  {
    this->m_Widget.useSynchronousAscension3DGCheckBox->setEnabled(true);
  }
  else
  {
    this->m_Widget.useSynchronousAscension3DGCheckBox->setEnabled(false);
    this->m_Widget.useSynchronousAscension3DGCheckBox->setChecked(false);
  }
}

void vtkAscension3DGTrackerSettingsWidget::ReadTrackerSettings()
{
  if( !m_Settings )
    return;

  // read the settings.
  this->m_Ascension3DGSettings.bUseSynchronousRecord = this->m_Settings->value("Ascension3DG/useSynchronousRecord", false).toBool();
  this->m_Ascension3DGSettings.bUseAllSensors = this->m_Settings->value("Ascension3DG/useAllSensors", false).toBool();
  this->m_Ascension3DGSettings.updateFrequency = this->m_Settings->value("Ascension3DG/updateFrequency", 22.0).toDouble(); 

  // update the GUI.
  this->m_Widget.useSynchronousAscension3DGCheckBox->setChecked(this->m_Ascension3DGSettings.bUseSynchronousRecord);
  this->m_Widget.useAllSensorsAscension3DGCheckBox->setChecked(this->m_Ascension3DGSettings.bUseAllSensors);
  
  // check to make sure that correct combinations are used due known issue with Ascension.
  if( this->m_Ascension3DGSettings.bUseAllSensors )
  {
    this->m_Widget.useSynchronousAscension3DGCheckBox->setEnabled(true);
  }
  else
  {
    this->m_Widget.useSynchronousAscension3DGCheckBox->setEnabled(false);
    this->m_Widget.useSynchronousAscension3DGCheckBox->setChecked(false);
  }
  this->m_Widget.updateFrequencyAscension3DGLineEdit->setText(QString("%1").arg(this->m_Ascension3DGSettings.updateFrequency));
}

void vtkAscension3DGTrackerSettingsWidget::WriteTrackerSettings()
{
  if( !m_Settings )
    return;

  this->m_Ascension3DGSettings.bUseSynchronousRecord = this->m_Widget.useSynchronousAscension3DGCheckBox->isChecked();
  this->m_Settings->setValue("Ascension3DG/useSynchronousRecord", this->m_Ascension3DGSettings.bUseSynchronousRecord);

  this->m_Ascension3DGSettings.bUseAllSensors = this->m_Widget.useAllSensorsAscension3DGCheckBox->isChecked();
  this->m_Settings->setValue("Ascension3DG/useAllSensors", this->m_Ascension3DGSettings.bUseAllSensors);

  this->m_Ascension3DGSettings.updateFrequency = this->m_Widget.updateFrequencyAscension3DGLineEdit->text().toDouble();
  this->m_Settings->setValue("Ascension3DG/updateFrequency", this->m_Ascension3DGSettings.updateFrequency);  
}