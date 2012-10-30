#define VTK_3DG_NTOOLS 12

#include "vtkObject.h"

#ifdef Ascension3DG_MedSafe
#include "ATC3DGm.h"
#else /* Ascension3DG_MedSafe */
#include "ATC3DG.h"
#endif /* Ascension3DG_MedSafe */

typedef struct ATCPartInfoStruct
{
  char modelString[11];
  char partNumber[16];
} ATCPartInfo;

class VTK_EXPORT vtkAscension3DGConfig : public vtkObject
{
public:
  static vtkAscension3DGConfig *New();
  vtkTypeMacro(vtkAscension3DGConfig,vtkObject);
  //TODO:void PrintSelf(ostream& os, vtkIndent indent);
protected:
  vtkAscension3DGConfig();
  ~vtkAscension3DGConfig();
public:
  void Copy(vtkAscension3DGConfig *src);

  // System configuration - no part numbers or model strings.
	SYSTEM_CONFIGURATION	*m_SystemConfig;
	
  // Sensor configuration and part numbers and model strings.
  SENSOR_CONFIGURATION	*m_SensorConfig;
  ATCPartInfo *m_SensorPartInfo;
	
  // Transmitter configuration and part numbers and model strings.
  TRANSMITTER_CONFIGURATION	*m_XmtrConfig;
  ATCPartInfo *m_XmtrPartInfo;
  
  // Board configuration and part numbers and model strings.
  BOARD_CONFIGURATION *m_BoardConfig;
  ATCPartInfo *m_BoardPartInfo;
};