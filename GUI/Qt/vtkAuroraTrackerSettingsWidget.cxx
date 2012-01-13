#include <QtGui>

#include "vtkAuroraTrackerSettingsWidget.h"

vtkAuroraTrackerSettingsWidget::vtkAuroraTrackerSettingsWidget( QWidget *parent)
{
  this->m_Parent = parent;
  // set up the GUI.
  m_Widget.setupUi(this);
}

vtkAuroraTrackerSettingsWidget::~vtkAuroraTrackerSettingsWidget()
{
}