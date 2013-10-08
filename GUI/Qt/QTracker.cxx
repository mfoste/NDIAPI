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

#include <vector>

#include <QtGui>

#include "ndXfrms.h"

#include "vtkTracker.h"
#include "vtkFakeTracker.h"
#include "vtkNDITracker.h"

#if defined (Ascension3DG_MedSafe)
#include "ATC3DGm.h"
#include "vtkAscension3DGTracker.h"
#else
#include "ATC3DG.h"
#include "vtkAscension3DGTracker.h"
#endif

#include "vtkTrackerTool.h"
#include "vtkTrackerSettingsStructures.h"
#include "vtkTrackerXfrmCallback.h"
#include "QTracker.h"

QTracker::QTracker()
{
  this->m_TrackerType=0;
  this->m_Tracker = 0;

  this->m_TrackerUpdateFrequency = 20; // arbitrary default of 20.

  // set up the pivot variables.
  this->m_PivotTool = -1;
  this->m_bPrePivot = false;
  this->m_PrePivotTime = 0.0;
  this->m_bPivot = false;
  this->m_PivotTime = 0.0;  
}

QTracker::~QTracker()
{
  // If tracker object exists.
  if(m_Tracker)
  {
    // if tracker is tracking.
    if(m_Tracker->IsTracking())
    {
      this->OnStopTracker();
    }
  }
  //delete m_Tracker;
  delete m_Timer;
  delete m_PivotTimer;
}

// ******  Helper Functions ******

void QTracker::CreateActions()
{
  // connect timers.
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(UpdateData()));
  //pivot connections.
  connect(this->m_PrePivotTimer, SIGNAL(timeout()), this, SLOT(OnStartPivot()));
  connect(this->m_PivotTimer, SIGNAL(timeout()), this, SLOT(OnStopPivot()));
}

void QTracker::SetupObservers()
{
  // set up the event observers.
  this->m_xfrms.resize(m_Tracker->GetNumberOfTools());
  this->m_effectiveFrequencies.resize(m_Tracker->GetNumberOfTools());
  this->m_quality.resize(m_Tracker->GetNumberOfTools());
  this->m_xfrmCallbacks.resize(m_Tracker->GetNumberOfTools());
  for( int i = 0; i < m_Tracker->GetNumberOfTools(); i++ )
  {
    m_xfrmCallbacks[i] = vtkTrackerXfrmCallback::New(this, i);
    m_Tracker->GetTool(i)->AddObserver(vtkCommand::ModifiedEvent, this->m_xfrmCallbacks[i]);
  }
  // add the hardware sync call to the first xfrm call back.
  m_Tracker->AddObserver(vtkCommand::UserEvent+1, this->m_xfrmCallbacks[0]);

  // now try to probe the tracker.
  if( !m_Tracker->Probe() )
  {
    emit this->ErrorMessage("The tracking system you specified is not attached. Please check your connections and retry." );
    this->RemoveTracker();
    return;
  }
  emit this->TrackerConfigured(this->m_Tracker->GetSerialNumber());
}

void QTracker::RemoveTracker()
{
  // if a tracker exists, delete it.
  if( m_Tracker )
  { 
    for( int i=0; i < m_Tracker->GetNumberOfTools(); i++ )
    {
      m_Tracker->GetTool(i)->RemoveAllObservers();
    }
    m_Tracker->RemoveAllObservers();
    m_Tracker->Delete();
    m_Tracker = 0;
  }
}

// ******  PUBLIC SLOTS ******

void QTracker::OnInitialize()
{
  // set up the mutex.
  this->m_mutex = new QMutex();

  // set up the timer.
  m_Timer = new QTimer(this);

  // pivot timers.
  this->m_PrePivotTimer = new QTimer(this);
  this->m_PrePivotTimer->setSingleShot(this);
  this->m_PivotTimer = new QTimer(this);
  this->m_PivotTimer->setSingleShot(true);

  // register ndQuatTransformation
  qRegisterMetaType<ndQuatTransformation>("ndQuatTransformation");

  this->CreateActions();  
}

void QTracker::OnConfigureTracker()
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  if( this->m_Tracker )
  {
    if( this->m_Tracker->IsTracking() )
    {
      this->m_Tracker->StopTracking();
    }
  }
}

void QTracker::OnConfigureFakeTracker(int trackerType, vtkFakeTrackerSettings *settings)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  m_Tracker = vtkFakeTracker::New();
  this->m_TrackerUpdateFrequency = settings->updateFrequency;

  // set-up observer calls.
  this->SetupObservers();
}

void QTracker::OnConfigureAuroraTracker(int trackerType, ndiAuroraSettings *settings)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  this->m_TrackerType = trackerType;

  m_Tracker = vtkNDITracker::New();
  vtkNDITracker *tracker = dynamic_cast<vtkNDITracker*>(m_Tracker);

  if( settings->bUseManual )
  {
    tracker->SetBaudRate(settings->baudRate); 
    tracker->SetSerialPort(settings->commPort);
  }
  else
  {
    // otherwise use auto detect.
    tracker->SetBaudRate(-1); 
    tracker->SetSerialPort(-1);
  }

  // communication log.
  if( settings->logComm )
  {
    tracker->LogCommunication(1);
  }
  else
  {
    tracker->LogCommunication(0);
  }

  this->m_TrackerUpdateFrequency = settings->updateFrequency;
  // load virtual roms if needed.
  for(int i = 0; i < 4; i++ )
  {
    if( !settings->romFiles[i].isEmpty() )
    {
      tracker->LoadVirtualSROM(i, settings->romFiles[i].toLatin1());
    }
  }

  // get volume names -- send out.
  QStringList volumeList;
  int nVolumes = tracker->GetNumTrackingVolumes();
  volumeList.reserve(nVolumes);
  // read volume shape types.
  for( int i=0; i < nVolumes; i++ )
  {
    volumeList[i] = QString::fromStdString(tracker->GetTrackingVolumeShapeType(i));
  }

  emit this->TrackerHasNVolumes(volumeList);

  // set-up observer calls.
  this->SetupObservers();
}

#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
void QTracker::OnConfigureAscension3DGTracker(int trackerType, ascension3DGSettings *settings)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  this->m_TrackerType = trackerType;

  m_Tracker = vtkAscension3DGTracker::New();
  vtkAscension3DGTracker *tracker = dynamic_cast<vtkAscension3DGTracker*>(m_Tracker);

  tracker->SetUseSynchronousRecord(settings->bUseSynchronousRecord);
  tracker->SetUseAllSensors(settings->bUseAllSensors);
  this->m_TrackerUpdateFrequency = settings->updateFrequency;

  // set-up observer calls.
  this->SetupObservers();
}
#endif

void QTracker::OnConfigureSpectraVicraTracker(int trackerType, ndiSpectraVicraSettings *settings)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  this->m_TrackerType = trackerType;

  m_Tracker = vtkNDITracker::New();
  vtkNDITracker *tracker = dynamic_cast<vtkNDITracker*>(m_Tracker);

  if( settings->bUseManual )
  {
    tracker->SetBaudRate(settings->baudRate); // this needs to be changed to Auto when it is implemented.
    tracker->SetSerialPort(settings->commPort);
  }
  else
  {
    tracker->SetBaudRate(115200); // this needs to be changed to Auto when it is implemented.
    tracker->SetSerialPort(-1);
  }
  // communication log.
  if( settings->logComm )
  {
    tracker->LogCommunication(1);
  }
  else
  {
    tracker->LogCommunication(0);
  }

  this->m_TrackerUpdateFrequency = settings->updateFrequency;
  // load virtual roms if needed.
  for(int i = 0; i < 12; i++ )
  {
    if( !settings->romFiles[i].isEmpty() )
    {
      tracker->LoadVirtualSROM(i, settings->romFiles[i].toLatin1());
    }
  }

  // get volume names -- send out.
  
  int nVolumes = tracker->GetNumTrackingVolumes();
  if (nVolumes > 0)
  {
    QStringList volumeList;
    volumeList.reserve(nVolumes);
    // read volume shape types.
    for( int i=0; i < nVolumes; i++ )
    {
      volumeList[i] = QString::fromStdString(tracker->GetTrackingVolumeShapeType(i));
    }

    emit this->TrackerHasNVolumes(volumeList);
  }
  else
  {
    fprintf(stderr, "Problem reading out the volumes from the camera.\n");
  }

  // set-up observer calls.
  this->SetupObservers();
}


void QTracker::OnVolumeSelected(int volume)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  if( this->m_TrackerType == NDI_AURORA || this->m_TrackerType == NDI_SPECTRA
    || this->m_TrackerType == NDI_SPECTRA_HYBRID )
  {
    dynamic_cast<vtkNDITracker*>(m_Tracker)->SetVolume(volume);
  }
}

void QTracker::OnPreConfigureTracker()
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  m_Tracker->PreConfigureTracking();

  emit this->TrackerPreConfigured();
}

void QTracker::OnStartTracker()
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

#if defined (Ascension3DG_TrakStar_DriveBay) || defined (Ascension3DG_MedSafe)
  double freq;
  if( this->m_TrackerType == ASCENSION_3DG )
  {
    if( this->m_bUseDefaultFrequency )
    {
      freq = dynamic_cast<vtkAscension3DGTracker*>(m_Tracker)->GetCurrentSettings()->m_SystemConfig->measurementRate;
      emit this->Ascension3DGDefaultFrequencyUsed(freq);
    }
    else
    {
      dynamic_cast<vtkAscension3DGTracker*>(m_Tracker)->SetMeasurementRate(this->m_TrackerUpdateFrequency);
    }
  }
#endif

  m_Tracker->StartTracking();

  // check the serial number.
  //TODO: emit the serial number. this->setSerialNumber( QString(m_Tracker->GetSerialNumber()) );
  m_Timer->start((int)(1000/(this->m_TrackerUpdateFrequency*2)));

  emit TrackerStarted();
}

void QTracker::OnStopTracker()
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  m_Timer->stop();
  m_Tracker->StopTracking();

  emit TrackerStopped();
}

void QTracker::UpdateData()
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  if (m_Tracker)
  {
    if (m_Tracker->IsTracking())
    {
      this->m_Tracker->Update();
      if( this->m_bPrePivot )
      {
        emit ElapsedPrePivotTime((int)this->m_PhaseTime.elapsed()/1000.0);
      }
      else if( this->m_bPivot )
      {
        //m_Tracker->GetTool(this->m_PivotTool)->InsertNextCalibrationPoint();
        emit ElapsedPivotTime((int)this->m_PhaseTime.elapsed()/1000.0);
      }
    }
  }
}

void QTracker::OnCloseTracker()
{
  if( this->m_Tracker->IsTracking() )
  {
    this->m_Tracker->StopTracking();
  }
  emit this->finished();
}

void QTracker::OnInitializePivot(int port, double preTime, double collectTime )
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  if( port > -1 && port < m_Tracker->GetNumberOfTools() )
  {
    this->m_PivotTool = port;
    this->m_PrePivotTime = preTime;
    this->m_PivotTime = collectTime;

    // initialize the pivot tool.
    this->m_Tracker->GetTool(this->m_PivotTool)->InitializeToolTipCalibration();

    // start the timer.
    this->m_bPrePivot = true;
    this->m_PhaseTime.start();
    this->m_PrePivotTimer->start(1000*this->m_PrePivotTime);

    emit this->PrePivotStarted("Get Ready to Pivot...", (int)this->m_PrePivotTime);
  }
  else
  {
    emit this->ErrorMessage("Invalid port number provided for pivoting.");
  }
}

void QTracker::OnStartPivot()
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  this->m_bPrePivot = false;
  this->m_bPivot = true;
  emit this->PivotStarted("Pivot Now...", (int)this->m_PivotTime);
  this->m_PhaseTime.start();
  m_Tracker->GetTool(this->m_PivotTool)->SetCollectToolTipCalibrationData(1);
  this->m_PivotTimer->start(1000*this->m_PivotTime);
}

void QTracker::OnStopPivot()
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  double error;
  this->m_bPivot = false;
  m_Tracker->GetTool(this->m_PivotTool)->SetCollectToolTipCalibrationData(0);
  // compute the calibration
  error = this->m_Tracker->GetTool(this->m_PivotTool)->DoToolTipCalibration();

  emit PivotFinished(error, this->m_Tracker->GetTool(this->m_PivotTool)->GetCalibrationMatrix());
  //emit PivotError(error);
  //emit PivotCalibrationMatrix(this->m_PivotTool, this->m_Tracker->GetTool(this->m_PivotTool)->GetCalibrationMatrix());
}

void QTracker::UpdateToolInfo(int port)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  emit this->ToolInfoUpdated(port);
}

void QTracker::UpdateToolTransform(int port, int frame, QString status)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  ndSetXfrmMissing(&this->m_xfrms[port]);
  emit this->ToolTransformUpdated(port, frame, status);
}

void QTracker::UpdateToolTransform(int port, int frame, ndQuatTransformation xfrm)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  ndCopyTransform(&xfrm, &this->m_xfrms[port]);
  emit this->ToolTransformUpdated(port, frame, xfrm);
}

void QTracker::UpdateToolTransform(int port, int frame, ndQuatTransformation xfrm, double effFreq, double quality)
{
  // lock while processing.
  QMutexLocker(this->m_mutex);

  this->UpdateToolTransform(port, frame, xfrm);
  this->m_effectiveFrequencies[port] = effFreq;
  emit this->ToolEffectiveFrequencyUpdated(port, frame, effFreq);
  this->m_quality[port] = quality;
  emit this->ToolQualityNumberUpdated(port, frame, quality);
  emit this->ToolTransformUpdated(port, frame, xfrm, effFreq, quality);
}
