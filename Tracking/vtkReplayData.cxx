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

#include "vtkReplayData.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

#include "vtkTimerLog.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <stdlib.h>

using namespace std;

vtkReplayData* vtkReplayData::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkReplayData");
	if(ret)
	{
		return (vtkReplayData*)ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkReplayData;
}

vtkReplayData::vtkReplayData() 
{
	this->frame=0;

}

vtkReplayData::~vtkReplayData()
{
	for (unsigned int i=0; i < this->data.size(); i++) {
		if (this->data[i].tool1 != NULL) 
		{
			this->data[i].tool1->Delete();
		}

		if (this->data[i].tool2 != NULL) 
		{
			this->data[i].tool2->Delete();
		}

		if (this->data[i].tool3 != NULL) 
		{
			this->data[i].tool3->Delete();
		}

		if (this->data[i].tool4 != NULL) 
		{
			this->data[i].tool4->Delete();
		}
	}
}

bool vtkReplayData::AppendDataSet(vtkMatrix4x4 *tool1, vtkMatrix4x4 *tool2, vtkMatrix4x4 *tool3, vtkMatrix4x4 *tool4)
{
	toolTransforms tools;

	tools.tool1 = vtkMatrix4x4::New();
	tools.tool2 = vtkMatrix4x4::New();
	tools.tool3 = vtkMatrix4x4::New();
	tools.tool4 = vtkMatrix4x4::New();

	tools.tool1->DeepCopy(tool1);
	tools.tool2->DeepCopy(tool2);
	tools.tool3->DeepCopy(tool3);
	tools.tool4->DeepCopy(tool4);

	this->data.push_back(tools);
	return true;
}

bool vtkReplayData::InsertDataSet(vtkMatrix4x4 *matrix, int tool)
{
	toolTransforms tools;

	tools.tool1 = NULL;
	tools.tool2 = NULL;
	tools.tool3 = NULL;
	tools.tool4 = NULL;
	switch (tool) 
	{
		case 0:
			tools.tool1 = vtkMatrix4x4::New();
			tools.tool1->DeepCopy(matrix);
			tools.tool1->Modified();
			break;
		case 1:
			tools.tool2 = vtkMatrix4x4::New();
			tools.tool1->DeepCopy(matrix);
			break;
		case 2:
			tools.tool3 = vtkMatrix4x4::New();
			tools.tool1->DeepCopy(matrix);
			break;
		case 3:
			tools.tool4 = vtkMatrix4x4::New();
			tools.tool1->DeepCopy(matrix);
			break;
		default:
			return false;
	}
	this->data.push_back(tools);
	return true;
}


void vtkReplayData::DeleteDataSet(int frame)
{

}

vtkMatrix4x4 * vtkReplayData::GetCurrentData(int tool)
{
	switch (tool)
	{
		case 0:
			return this->data[this->frame].tool1;
		case 1:
			return this->data[this->frame].tool2;
		case 2:
			return this->data[this->frame].tool3;
		case 3:
			return this->data[this->frame].tool4;
		default:
			return NULL;
	}

}

int vtkReplayData::GetFrameCount()
{
	return this->frame;
}

void vtkReplayData::GetDataSet(int frame)
{

}

void vtkReplayData::AdvanceFrame()
{
	this->frame++;

	if (this->frame >= this->data.size()) 
	{
		this->frame=0;
	}
}

bool vtkReplayData::UpdateDataSet(vtkMatrix4x4 *matrix, int frame, int tool) 
{
	if (frame >= this->data.size() || frame < 0)
	{
		return false;
	}

	switch (tool)
	{
		case 0:
			this->data[frame].tool1->DeepCopy(matrix);
			return true;
		case 1:
			this->data[frame].tool2->DeepCopy(matrix);
			return true;
		case 2:
			this->data[frame].tool3->DeepCopy(matrix);
			return true;
		case 3:
			this->data[frame].tool4->DeepCopy(matrix);
			return true;
		default:
			return false;
	}
	
}

void vtkReplayData::LoadData(char * filename, int tool) 
{
	string line;
	string element;
	ifstream readFile;
	readFile.open(filename);

	bool insert = false;
	int counter = 0;
	if (this->data.size() == 0) 
	{
		insert = true;
	}


	if (readFile.is_open())
	{
		vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		
		while ( readFile.good() )
		{
			line.clear();
			std::getline (readFile,line);
			if (line == "" )
				break;
			istringstream liness( line );
			

			// Row 0 
			getline( liness, element, ',' );
			double item = atof(element.c_str());
			matrix->SetElement(0,0, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(0,1, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(0,2, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(0,3, item);

			// Row 1 
			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(1,0, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(1,1, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(1,2, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(1,3, item);

			// Row 2 
			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(2,0, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(2,1, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(2,2, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(2,3, item);

			// Row 3 
			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(3,0, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(3,1, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(3,2, item);

			getline( liness, element, ',' );
			item = atof(element.c_str());
			matrix->SetElement(3,3, item);
			
			if (insert)
			{
				InsertDataSet(matrix, tool);
			}
			else 
			{
				UpdateDataSet(matrix, counter, tool);
				counter++;
			}

		}
		readFile.close();
	}
	else 
	{  
		cout << "Unable to open file"; 
	}
}

void vtkReplayData::ChangeFrame(int frame) 
{
	if (frame < this->data.size() && frame >=0)
	{
		this->frame = frame;
	}
}