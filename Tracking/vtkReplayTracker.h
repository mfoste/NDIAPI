/* ============================================================================

  File: vtkReplayTracker.cxx
  Author: Chris Wedlake <cwedlake@robarts.ca>
  Language: C++
  Description: 
    This class represents a fake tracking system with tools that have
    behaviour determined by inputted transforms. This allows someonew who doesn't have 
	access current to a tracking system to test code that relies on having one active.

==========================================================================

  Copyright (c) Chris Wedlake, cwedlake@robarts.ca

  Use, modification and redistribution of the software, in source or
  binary forms, are permitted provided that the following terms and
  conditions are met:

  1) Redistribution of the source code, in verbatim or modified
  form, must retain the above copyright notice, this license,
  the following disclaimer, and any notices that refer to this
  license and/or the following disclaimer.  

  2) Redistribution in binary form must include the above copyright
  notice, a copy of this license and the following disclaimer
  in the documentation or with other materials provided with the
  distribution.

  3) Modified copies of the source code must be clearly marked as such,
  and must not be misrepresented as verbatim copies of the source code.

  THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
  WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
  MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
  OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
  THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/

#ifndef __vtkReplayTracker_h
#define __vtkReplayTracker_h

#include "vtkTracker.h"
#include "vtkReplayData.h"
#include <vector>

class vtkTransform;


class VTK_EXPORT vtkReplayTracker: public vtkTracker
{
public:
  static vtkReplayTracker *New();
  vtkTypeMacro(vtkReplayTracker,vtkObject);

  int Probe();
  int InternalStartTracking();
  int InternalStopTracking();
  void InternalUpdate();

  vtkSetMacro(SerialPort, int);
  vtkGetMacro(SerialPort, int);

  void LoadData(char * filename, int tool);

protected:
  vtkReplayTracker();
  ~vtkReplayTracker();

  int currentFrame;
  double TimeStamp;
  vtkTransform *InternalTransform;
  double updateRate;
  double NextMTime;
  int SerialPort;
  vtkSmartPointer<vtkReplayData> replayData;
};


#endif