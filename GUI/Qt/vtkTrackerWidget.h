#ifndef __vtkTrackerWidget_h
#define __vtkTrackerWidget_h

#include "vtkTrackerWidget_global.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "ndXfrms.h"

class vtkTracker;
class vtkTrackerSettingsDialog;
class vtkTrackerWidgetXfrmCallback;

class VTKTRACKERWIDGET_EXPORT vtkTrackerWidget : public QWidget
{
  Q_OBJECT

public:
  vtkTrackerWidget( QWidget *parent = 0);
  ~vtkTrackerWidget();

  QSize sizeHint() const;

  inline vtkTracker* getTracker() {return m_Tracker;}
  void UpdateToolTransform(int port, QString status);
  void UpdateToolTransform(int port, QuatTransformation xfrm);
  void UpdateToolTransform(int port, QuatTransformation xfrm, double effFreq, double quality);

public slots:
  virtual void OnConfigureTracker();
  virtual void OnConfigureTrackerAccepted();
  virtual void OnStartTracker();
  virtual void OnStopTracker();
  virtual void UpdateData();

signals:
  void TrackerConfigured(QString SerialNumber);
  void TrackerStarted();
  void TrackerStopped();
  void ToolTransformUpdated(int port, QString status);
  void ToolTransformUpdated(int port, QuatTransformation xfrm);
  void ToolEffectiveFrequencyUpdated(int port, double freq);
  void ToolQualityNumberUpdated(int port, double quality);

private:
  // some helper functions.
  void setupUi();
	void setupUiLayout();
  void CreateActions();
  void ConfigureTracker();
  void PopUpError(QString str);

  // the owner of this widget.
  QWidget *m_Parent;
  
  // GUI variables.
  //Ui::vtkTrackerWidget m_GUI;
  QPushButton *m_ConfigureTrackerButton;
  QPushButton *m_StartTrackingButton;
  QPushButton *m_StopTrackingButton;
  //QVBoxLayout *m_VerticalLayout;

  
  // actual tracker object we are controlling.
  vtkTracker *m_Tracker;
  vtkTrackerSettingsDialog *m_TrackerSettingsDialog;

  // keep track of the tools.
  std::vector < QuatTransformation > m_xfrms;
  std::vector < double > m_effectiveFrequencies;
  std::vector < double > m_quality;
  std::vector < vtkTrackerWidgetXfrmCallback* > m_xfrmCallbacks;

  //timer.
  QTimer *m_Timer; // need a timer to update the tracker.
  double m_TrackerUpdateFrequency;
};

#endif