#include <QtGui>

#include "vtkFakeTrackerSettingsWidget.h"

vtkFakeTrackerSettingsWidget::vtkFakeTrackerSettingsWidget( QWidget *parent)
{
  this->m_Parent = parent;
  // set up the GUI.
  m_Widget.setupUi(this);
}

vtkFakeTrackerSettingsWidget::~vtkFakeTrackerSettingsWidget()
{
}