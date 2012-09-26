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
  void OnToolTransformUpdated(int port, ndQuatTransformation xfrm );
  void OnToolEffectiveFrequencyUpdated(int port, double freq);
  void OnToolQualityUpdated(int port, double freq);
  
signals:
  
protected:
  void closeEvent(QCloseEvent *event);

  // variables
  Ui::NDIQtTrackMainWindow  *m_GUI;

private:
  void CreateActions();
  QString GetXfrmString(ndQuatTransformation xfrm);

  // mutex lock for threading.
  QMutex *m_mutex;

  ndQuatTransformation m_Port01Xfrm;
  ndQuatTransformation m_Port02Xfrm;
  ndQuatTransformation m_Port03Xfrm;
  ndQuatTransformation m_Port04Xfrm;
  ndQuatTransformation m_Port05Xfrm;
  ndQuatTransformation m_Port06Xfrm;
  ndQuatTransformation m_Port07Xfrm;
  ndQuatTransformation m_Port08Xfrm;
  ndQuatTransformation m_Port09Xfrm;
  ndQuatTransformation m_Port10Xfrm;
  ndQuatTransformation m_Port11Xfrm;
  ndQuatTransformation m_Port12Xfrm;


};
#endif