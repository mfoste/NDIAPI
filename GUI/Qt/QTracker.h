/*
Copyright (C) 2011-2013, Northern Digital Inc. All rights reserved.
 
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

#ifndef __QTracker_h
#define __QTracker_h

#include <QtGui>

#include <vtkMatrix4x4.h>

#include "ndXfrms.h"
#include "vtkTrackerSettingsStructures.h"

class vtkTracker;
class vtkTrackerXfrmCallback;

#include "vtkTrackerWidget_global.h"

class VTKTRACKERWIDGET_EXPORT QTracker : public QObject
{
  Q_OBJECT

public:
  QTracker();
  ~QTracker();

public slots:
  virtual void OnInitialize();
  //virtual void Initialize(QString settingsFile);
  virtual void OnConfigureTracker();
  virtual void OnConfigureFakeTracker(int trackerType, vtkFakeTrackerSettings *settings);
  virtual void OnConfigureAuroraTracker(int trackerType, ndiAuroraSettings *settings);
  virtual void OnConfigureSpectraVicraTracker(int trackerType, ndiSpectraVicraSettings *settings);
#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
  virtual void OnConfigureAscension3DGTracker(int trackerType, ascension3DGSettings *settings);
#endif
  virtual void OnVolumeSelected(int volume);
  virtual void OnStartTracker();
  virtual void OnStopTracker();
  virtual void UpdateData();
  virtual void OnCloseTracker();
  // pivot functions.
  virtual void OnInitializePivot(int port, double preTime, double collectTime );
  virtual void OnStartPivot();
  virtual void OnStopPivot();

signals:
  void ErrorMessage(QString msg);
  void TrackerConfigured(QString SerialNumber);
#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
  void Ascension3DGDefaultFrequencyUsed(double freq);
#endif
  void TrackerHasNVolumes(QStringList volumeList);
  void TrackerStarted();
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
  //void PivotError(double error);
  //void PivotCalibrationMatrix(int port, vtkMatrix4x4 *mat);
  void finished();

public:
  int GetTrackerType() {return this->m_TrackerType;};
  vtkTracker* getTracker() {return m_Tracker;};
  
  // used by the TrackerXfrmCallback.
  void UpdateToolInfo(int port);
  void UpdateToolTransform(int port, int frame, QString status);
  void UpdateToolTransform(int port, int frame, ndQuatTransformation xfrm);
  void UpdateToolTransform(int port, int frame, ndQuatTransformation xfrm, double effFreq, double quality);

private:
  // some helper functions
  void CreateActions();
  void SetupObservers();
  void RemoveTracker();

  // mutex
  QMutex *m_mutex;

  // system type id.
  int m_TrackerType;

  // actual tracker object we are controlling.
  vtkTracker *m_Tracker;

  // keep track of the tools.
  std::vector < ndQuatTransformation > m_xfrms;
  std::vector < double > m_effectiveFrequencies;
  std::vector < double > m_quality;
  std::vector < vtkTrackerXfrmCallback* > m_xfrmCallbacks;

  //timer.
  QTimer *m_Timer; // need a timer to update the tracker.
  double m_TrackerUpdateFrequency;
#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
  // 3DG specific variables.
  bool m_bUseDefaultFrequency;
#endif

  // pivot.
  QTime m_PhaseTime;
  int m_PivotTool;  // tool to pivot.
  bool m_bPrePivot; // init pivot.
  double m_PrePivotTime;
  bool m_bPivot;  // whether we pivot.
  double m_PivotTime;
  QTimer *m_PrePivotTimer;
  QTimer *m_PivotTimer;
};

#endif //__QTracker_h