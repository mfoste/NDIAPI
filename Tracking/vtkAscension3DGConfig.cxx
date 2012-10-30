#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkAscension3DGConfig.h"


vtkAscension3DGConfig* vtkAscension3DGConfig::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkAscension3DGConfig");
  if(ret)
  {
    return (vtkAscension3DGConfig*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkAscension3DGConfig;
}

vtkAscension3DGConfig::vtkAscension3DGConfig()
{
  this->m_SystemConfig = new SYSTEM_CONFIGURATION;
  this->m_SensorConfig = 0;
  this->m_SensorPartInfo = 0;
  this->m_XmtrConfig = 0;
  this->m_XmtrPartInfo = 0;
  this->m_BoardConfig = 0;
  this->m_BoardPartInfo = 0;
}

vtkAscension3DGConfig::~vtkAscension3DGConfig()
{
  delete this->m_SystemConfig;
  
  if( this->m_SensorConfig )
    delete [] this->m_SensorConfig;
  if( this->m_SensorPartInfo )
    delete [] this->m_SensorPartInfo;
  
  if( this->m_XmtrConfig )
    delete [] this->m_XmtrConfig;
  if( this->m_XmtrPartInfo )
    delete [] this->m_XmtrPartInfo;

  if( this->m_BoardConfig )
    delete [] this->m_BoardConfig;
  if( this->m_BoardPartInfo )
    delete [] this->m_BoardPartInfo;
}

void vtkAscension3DGConfig::Copy(vtkAscension3DGConfig *src)
{
  // System parms.
  this->m_SystemConfig->measurementRate = src->m_SystemConfig->measurementRate;
  this->m_SystemConfig->powerLineFrequency = src->m_SystemConfig->powerLineFrequency;
  this->m_SystemConfig->maximumRange = src->m_SystemConfig->maximumRange;
  this->m_SystemConfig->agcMode = src->m_SystemConfig->agcMode;
  this->m_SystemConfig->numberBoards = src->m_SystemConfig->numberBoards;
  this->m_SystemConfig->numberSensors = src->m_SystemConfig->numberSensors;
  this->m_SystemConfig->numberTransmitters = src->m_SystemConfig->numberTransmitters;


}