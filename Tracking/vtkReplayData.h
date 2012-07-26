/* ============================================================================

  File: vtkReplayData.cxx
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

#ifndef __vtkReplayData_h
#define __vtkReplayData_h

#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include <vector>

#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

struct toolTransforms {
	vtkMatrix4x4 * tool1;
	vtkMatrix4x4 * tool2;
	vtkMatrix4x4 * tool3;
	vtkMatrix4x4 * tool4;
};

class VTK_EXPORT vtkReplayData : public vtkObject 
{
public:
	static vtkReplayData *New();

	vtkReplayData(vtkReplayData, vtkObject);

	void LoadData(char * filename, int tool);

	vtkMatrix4x4 * GetCurrentData(int tool);

	int GetFrameCount();

	void GetDataSet(int frame);

	void AdvanceFrame();

protected:
	bool InsertDataSet(vtkMatrix4x4 *matrix, int tool);

	bool AppendDataSet(vtkMatrix4x4 *tool1, vtkMatrix4x4 *tool2, vtkMatrix4x4 *tool3, vtkMatrix4x4 *tool4);
	bool UpdateDataSet(vtkMatrix4x4 *matrix, int frame, int tool);
	void DeleteDataSet(int frame);

	vtkReplayData();

	~vtkReplayData();

private:
	int frame;

	std::vector<toolTransforms > data;
};

#endif