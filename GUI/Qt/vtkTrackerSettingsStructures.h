#ifndef __vtkTrackerSettingsStructures_h
#define __vtkTrackerSettingsStructures_h

/* ============================================================================

  File: vtkTrackerSettingsStructures.h
  Author: Andrew Wiles <awiles@ndigital.com>
  Language: C++
  Description: 
    Various structures are declared here to be used to collect the settings
    for the various tracking systems used with the Qt GUI.

============================================================================ */

// tracker system type enumerators.
enum TRACKER_SYSTEM_TYPES
{
  FAKE_TRACKER = 0,
  NDI_AURORA
};

// fake tracker
struct vtkFakeTrackerSettings
{
  double updateFrequency;
};

// NDI Aurora tracker settings.
struct ndiAuroraSettings
{
  QStringList romFiles;
  double updateFrequency;
};

#endif
