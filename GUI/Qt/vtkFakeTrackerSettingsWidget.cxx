#include <QtGui>

#include "vtkFakeTrackerSettingsWidget.h"

vtkFakeTrackerSettingsWidget::vtkFakeTrackerSettingsWidget( QWidget *parent, QSettings *settings )
{
  this->m_Parent = parent;
  this->m_Settings = settings;
  // set up the GUI.
  m_Widget.setupUi(this);

  // set validators on port line edits.
  this->m_Widget.updateFrequencyFakeTrackerLineEdit->setValidator(new QDoubleValidator(0.1, 200, 1, this) );
}

vtkFakeTrackerSettingsWidget::vtkFakeTrackerSettingsWidget( QWidget * parent )
{
  vtkFakeTrackerSettingsWidget(parent, 0);
}

vtkFakeTrackerSettingsWidget::~vtkFakeTrackerSettingsWidget()
{
}

void vtkFakeTrackerSettingsWidget::ReadTrackerSettings()
{
  if( !m_Settings )
    return;
  
  // update frequency.
  this->m_FakeTrackerSettings.updateFrequency = this->m_Settings->value("FakeTracker/updateFrequency", 40.0).toDouble();
  this->m_Widget.updateFrequencyFakeTrackerLineEdit->setText(QString("%1").arg(this->m_FakeTrackerSettings.updateFrequency));
}

void vtkFakeTrackerSettingsWidget::WriteTrackerSettings()
{
  if( !m_Settings )
    return;

  // update frequency.
  this->m_FakeTrackerSettings.updateFrequency = this->m_Widget.updateFrequencyFakeTrackerLineEdit->text().toDouble();
  this->m_Settings->setValue("FakeTracker/updateFrequency", this->m_FakeTrackerSettings.updateFrequency);
}