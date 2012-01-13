#include "vtkTrackerWidgetPlugin.h"
#include "vtkTrackerWidget.h"

#include "QtCore/qplugin.h"

vtkTrackerWidgetPlugin::vtkTrackerWidgetPlugin( QObject* parent ) 
: QObject(parent) {
}

vtkTrackerWidgetPlugin::~vtkTrackerWidgetPlugin( ) {
}

QString vtkTrackerWidgetPlugin::name( ) const {
	return "vtkTrackerWidget";
}

QString vtkTrackerWidgetPlugin::includeFile() const {
	return "vtkTrackerWidget.h";
}

QString vtkTrackerWidgetPlugin::group( ) const {
	return "AIGS";
}

QIcon vtkTrackerWidgetPlugin::icon( ) const {
	QIcon i;
	return QIcon();
}

QString vtkTrackerWidgetPlugin::toolTip() const {
	return "AIGS Tracker Interface";
}

QString vtkTrackerWidgetPlugin::whatsThis() const {
	return "Widget to control a spatial tracking device.";
}

bool vtkTrackerWidgetPlugin::isContainer() const {
	return false;
}

QWidget* vtkTrackerWidgetPlugin::createWidget( QWidget* parent ) {
	//return new vtkTrackerWidget( parent );
  QWidget* temp;
  temp = new vtkTrackerWidget( parent );
  return temp;
}

Q_EXPORT_PLUGIN2(vtktrackerwidgetplugin, vtkTrackerWidgetPlugin)