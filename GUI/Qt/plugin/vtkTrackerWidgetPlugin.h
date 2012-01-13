#ifndef __vtkTrackerWidgetPlugin_h
#define __vtkTrackerWidgetPlugin_h

#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtGui>

class vtkTrackerWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
	Q_OBJECT
	Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
	vtkTrackerWidgetPlugin( QObject* parent = 0 );
	~vtkTrackerWidgetPlugin();

	//! Virtual functions to reimplement
	QString name() const;
	QString includeFile() const;
	QString group() const;
	QIcon icon() const;
	QString toolTip() const;
	QString whatsThis() const;
	bool isContainer() const;
	QWidget* createWidget( QWidget* parent );
};
#endif