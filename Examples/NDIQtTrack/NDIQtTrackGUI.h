#ifndef __NDIQtTrackGUI_h
#define __NDIQtTrackGUI_h

#include <QtGui>
#include <QSettings>

#include <vtkSmartPointer.h>

#include "ui_NDIQtTrackGUI.h"

namespace Ui {
  class NDIQtTrackMainWindow;
}


class NDIQtTrackGUI : public QMainWindow
{
  Q_OBJECT

public:
  static NDIQtTrackGUI *New();
  static NDIQtTrackGUI *New(QWidget *parent);

protected:
  NDIQtTrackGUI(QWidget *parent);
  ~NDIQtTrackGUI();

public:
  

public slots:
  virtual void About();
  virtual void AboutQt();
  void OnTrackerConfigured( QString systemInfo );
  void OnTrackerStarted();
  void OnToolTransformUpdated(int port, QString status);
  void OnToolTransformUpdated(int port, QuatTransformation xfrm );
  void OnToolEffectiveFrequencyUpdated(int port, double freq);
  void OnToolQualityUpdated(int port, double freq);
  
signals:
  
protected:
  void closeEvent(QCloseEvent *event);

  // variables
  Ui::NDIQtTrackMainWindow  *m_GUI;

private:
  void CreateActions();
  QString GetXfrmString(QuatTransformation xfrm);

  // mutex lock for threading.
  QMutex *m_mutex;

  QuatTransformation m_Port01Xfrm;
  QuatTransformation m_Port02Xfrm;
  QuatTransformation m_Port03Xfrm;
  QuatTransformation m_Port04Xfrm;

};
#endif