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

#ifndef __vtkTrackerWidget_h
#define __vtkTrackerWidget_h

#include "vtkTrackerWidget_global.h"

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QTime>

#include <vtkMatrix4x4.h>

#include "ndXfrms.h"
#include "vtkTrackerSettingsStructures.h"
#include "QTracker.h"

class vtkTrackerSettingsDialog;

class VTKTRACKERWIDGET_EXPORT vtkTrackerWidget : public QWidget
{
  Q_OBJECT

public:
  vtkTrackerWidget( QWidget *parent = 0);
  ~vtkTrackerWidget();

  QSize sizeHint() const;

  inline void SetSettingsFileName( QString settingsFile ) {this->m_SettingsFileName=settingsFile;};
  inline QTracker* GetTrackerObject() {return m_TrackerObject;}
  int getTrackerSystemType();
  
  // for hardware synch.
 void SetSlaveTracker (vtkTrackerWidget *slave);
 void SetMasterTracker (vtkTrackerWidget *master);

public slots:
  virtual void Initialize();
  virtual void Initialize(QString settingsFile);
  virtual void OnConfigureTracker();
  virtual void OnConfigureTrackerAccepted();
  virtual void OnConfigureTrackerCanceled();
  virtual void OnVolumeListUpdated(QStringList volumeList);
  virtual void OnVolumeSelected(int volume);
  virtual void OnStartTracker();
  virtual void OnStopTracker();
  
signals:
  void TrackerWidgetInitialized();
  void TrackerConfigurationDialogOpened();
  void ConfigureFakeTracker(int trackerType, vtkFakeTrackerSettings *settings);
  void ConfigureAuroraTracker(int trackerType, ndiAuroraSettings *settings);
  void ConfigureSpectraVicraTracker(int trackerType, ndiSpectraVicraSettings *settings);
#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
  void ConfigureAscension3DGTracker(int trackerType, ascension3DGSettings *settings);
#endif
  void VolumeSelected(int volume);
  void StartTracking();
  void StopTracking();
  void CloseTrackerWidget();
  /*void TrackerStarted();
  void TrackerStopped();
  void ToolInfoUpdated(int port);
  void ToolTransformUpdated(int port, int frame, QString status);
  void ToolTransformUpdated(int port, int frame, ndQuatTransformation xfrm);
  void ToolEffectiveFrequencyUpdated(int port, int frame, double freq);
  void ToolQualityNumberUpdated(int port, int frame, double quality);
  void ToolTransformUpdated(int port, int frame, ndQuatTransformation xfrm, double freq, double quality);
  void PrePivotStarted(QString label, int maxtime);
  void ElapsedPrePivotTime(int elapsedTime);
  void PivotStarted(QString label, int maxtime);
  void ElapsedPivotTime(int elapsedTime);
  void PivotFinished(double error, vtkMatrix4x4 *mat);
  void finished();*/
  //void PivotError(double error);
  //void PivotCalibrationMatrix(int port, vtkMatrix4x4 *mat);

protected:
  virtual void closeEvent(QCloseEvent* event);

private:
  // some helper functions.
  void setupUi();
	void setupUiLayout();
  void CreateActions();
  void ConfigureTracker();
  void PopUpError(QString str);

  // the owner of this widget.
  QWidget *m_Parent;

  // settings file.
  QString m_SettingsFileName;
  
  // GUI variables.
  //Ui::vtkTrackerWidget m_GUI;
  QPushButton *m_ConfigureTrackerButton;
  QComboBox   *m_VolumeSelectionComboBox;
  QPushButton *m_StartTrackingButton;
  QPushButton *m_StopTrackingButton;
  //QVBoxLayout *m_VerticalLayout;
  
  // actual tracker object we are controlling.
  QTracker *m_TrackerObject;
  vtkTrackerSettingsDialog *m_TrackerSettingsDialog;

  // use a separate thread for the QTracker object.
  QThread *m_TrackerThread;
    
  // for hardware sync.
  vtkTrackerWidget *m_SlaveTracker;
  vtkTrackerWidget *m_MasterTracker;
};

#endif