PROJECT(AIGS)

# CMake version requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# For CMake 2.6
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# default build type
IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
ENDIF( NOT CMAKE_BUILD_TYPE )

SET(AIGS_CMAKE_DIR "${AIGS_SOURCE_DIR}/CMake")
SET(CMAKE_MODULE_PATH "${AIGS_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

# Look for a VTK build directory
FIND_PACKAGE(VTK)
IF (VTK_FOUND)
  INCLUDE (${VTK_USE_FILE})
ENDIF (VTK_FOUND)

# Choose static or shared libraries.
OPTION(BUILD_SHARED_LIBS "Build shared libraries." OFF)
SET (AIGS_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
IF (BUILD_SHARED_LIBS)
  SET (BUILD_SHARED_LIBS_CONFIG 1)
ELSE (BUILD_SHARED_LIBS)
  SET (BUILD_SHARED_LIBS_CONFIG 0)
ENDIF (BUILD_SHARED_LIBS)

# Build documentation
OPTION(BUILD_DOCUMENTATION "Build Documentation" OFF)
IF(BUILD_DOCUMENTATION)
  FIND_PACKAGE(VTK)
  IF (USE_VTK_FILE)
    INCLUDE (${USE_VTK_FILE})
  ELSE (USE_VTK_FILE)
    MESSAGE(FATAL_ERROR "This application requires VTK.")
  ENDIF (USE_VTK_FILE)
  ADD_SUBDIRECTORY(Documentation/Doxygen)
ENDIF(BUILD_DOCUMENTATION)

# Output directories.
SET(LIBRARY_OUTPUT_PATH ${AIGS_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${AIGS_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
SET(AIGS_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH})
SET(AIGS_EXECUTABLE_DIR ${EXECUTABLE_OUTPUT_PATH})
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# Configure Dart testing support.
INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)

# ---------------------------------------------------------------
# Initialize configuration

# Kits
SET(AIGS_KITS)

# Libraries
SET(AIGS_LIBRARIES)

# Include directories
SET(AIGS_INCLUDE_DIRS)

# ---------------------------------------------------------------
# Include the Tcl and Python wrappers
IF(VTK_WRAP_TCL AND ${VTK_MAJOR_VERSION} GREATER 4)
  INCLUDE(${VTK_CMAKE_DIR}/vtkWrapTcl.cmake)
  INCLUDE_DIRECTORIES("${VTK_TCL_INCLUDE_DIR}" "${VTK_TK_INCLUDE_DIR}")
ENDIF(VTK_WRAP_TCL AND ${VTK_MAJOR_VERSION} GREATER 4)

IF(VTK_WRAP_PYTHON AND ${VTK_MAJOR_VERSION} GREATER 4)
  IF(VTK_PYTHON_INCLUDE_DIR)
    INCLUDE_DIRECTORIES("${VTK_PYTHON_INCLUDE_DIR}")
  ENDIF(VTK_PYTHON_INCLUDE_DIR)
  INCLUDE(${VTK_CMAKE_DIR}/vtkWrapPython.cmake)
ENDIF(VTK_WRAP_PYTHON AND ${VTK_MAJOR_VERSION} GREATER 4)

IF(VTK_WRAP_JAVA AND ${VTK_MAJOR_VERSION} GREATER 4)
  SET(VTK_WRAP_JAVA 0)
ENDIF(VTK_WRAP_JAVA AND ${VTK_MAJOR_VERSION} GREATER 4)

# Include AIGSThirdParty
INCLUDE(${AIGS_SOURCE_DIR}/CMake/AIGSThirdParty.cmake)

# ---------------------------------------------------------------
# Start listing the AIGS kits

# Tracking
OPTION (AIGS_USE_TRACKING "Build the Tracking module" ON)
IF (AIGS_USE_TRACKING)
  SET(AIGS_INCLUDE_DIRS ${AIGS_INCLUDE_DIRS} ${AIGS_SOURCE_DIR}/Tracking)
  OPTION (AIGS_USE_NDI "Provide support for the NDI POLARIS and AURORA" ON)
  IF( AIGS_USE_NDI )
	SET(AIGS_INCLUDE_DIRS ${AIGS_INCLUDE_DIRS} ${AIGS_SOURCE_DIR}/Utilities/ndicapi)
  ENDIF( AIGS_USE_NDI )
  OPTION (AIGS_USE_CERTUS "Provide support for the NDI Certus" OFF)
  OPTION (AIGS_USE_FLOCK "Provide support for the Ascension Flock of Birds" OFF)
  OPTION (AIGS_USE_LOGITECH "Provide support for the Logitech 3D mouse" OFF)
  OPTION (AIGS_USE_POLHEMUS "Provide support for the Polhemus FasTrak" OFF)
  OPTION (AIGS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
  OPTION (AIGS_USE_REPLAYTRACKER "Provide support for a tracker that loads a csv file to set as tool transforms.  Similar to vtkFakeTracker but provides ability to set custom tool transforms" OFF)

  # ascension microbird and 3DG libraries are only available for windows.
  IF(WIN32)
	OPTION (AIGS_USE_MICROBIRD "Provide support for the Ascension MicroBird" OFF)
	OPTION (AIGS_USE_3DG "Provide support for the Ascension 3D Guidance trakStar, medSafe or driveBay" OFF)
	IF( AIGS_USE_3DG )
		OPTION (AIGS_USE_3DG_MEDSAFE "Provide support for the Ascension 3D Guidance medSafe" OFF)
		OPTION (AIGS_USE_3DG_TRAKSTAR_DRIVEBAY "Provide support for the Ascension 3D Guidance trakSTAR, trakSTAR2, driveBAY, driveBAY2 (works with all versions of firmware)" ON)
		# check to make sure only one is selected, we cannot use more than one as they link to the same named dll.
		IF( AIGS_USE_3DG_MEDSAFE AND NOT AIGS_USE_3DG_TRAKSTAR_DRIVEBAY )
			SET( ATC_INCLUDE_DIRECTORY "${AIGS_SOURCE_DIR}/Utilities/ascension3DGapi/medSafe/3D Guidance API/" 
			CACHE PATH "Directory where the ATC3DGm.h file is located for the medSAFE" FORCE)
			SET( ATC_LIBRARY "${AIGS_SOURCE_DIR}/Utilities/ascension3DGapi/medSAFE/3D Guidance API/ATC3DGm.lib" 
			CACHE FILEPATH "Full path to the ATC3DGm.lib file for the medSAFE" FORCE)
			ADD_DEFINITIONS( /DAscension3DG_MedSafe)
			SET(AIGS_INCLUDE_DIRS ${AIGS_INCLUDE_DIRS} "${AIGS_SOURCE_DIR}/Utilities/ascension3DGapi/medSafe/3D Guidance API")
	    ELSEIF( AIGS_USE_3DG_TRAKSTAR_DRIVEBAY AND NOT AIGS_USE_3DG_MEDSAFE )
			SET( ATC_INCLUDE_DIRECTORY "${AIGS_SOURCE_DIR}/Utilities/ascension3DGapi/trakStar_driveBay/3DG API/" 
			CACHE PATH "Directory where the ATC3DG.h file is located for the trakSTAR, trakSTAR2, driveBAY and driveBAY2" FORCE )
			SET( ATC_LIBRARY "${AIGS_SOURCE_DIR}/Utilities/ascension3DGapi/trakStar_driveBay/3DG API/ATC3DG.lib" 
			CACHE FILEPATH "Full path to the ATC3DG.lib file for the trakSTAR, trakSTAR2, driveBAY and driveBAY2" FORCE)
			ADD_DEFINITIONS( /DAscension3DG_TrakStar_DriveBay)
			SET(AIGS_INCLUDE_DIRS ${AIGS_INCLUDE_DIRS} "${AIGS_SOURCE_DIR}/Utilities/ascension3DGapi/trakStar_driveBay/3DG API")
		ELSE()
			MESSAGE( FATAL_ERROR "Can only use one of the 3DG tracker types.  Please only select one 3DG tracker type." )
		ENDIF()
	ENDIF()
  ENDIF(WIN32)
  MESSAGE( STATUS "ATC_LIBRARY: ${ATC_LIBRARY}" )
  ADD_SUBDIRECTORY(Utilities)
  ADD_SUBDIRECTORY(Tracking)

  SET(AIGS_LIBRARIES ${AIGS_LIBRARIES} Tracking)
ENDIF (AIGS_USE_TRACKING)

# Ultrasound
IF (AIGS_USE_ULTRASOUND)
  SET(AIGS_INCLUDE_DIRS ${AIGS_INCLUDE_DIRS}
      ${AIGS_SOURCE_DIR}/Ultrasound)
  ADD_SUBDIRECTORY(Ultrasound)

  SET(AIGS_LIBRARIES ${AIGS_LIBRARIES} Ultrasound)
ENDIF (AIGS_USE_ULTRASOUND)

# Build Testing
OPTION(BUILD_TESTING "Build the tests" OFF)
IF(BUILD_TESTING)
   ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

# Build Examples
OPTION(BUILD_EXAMPLES "Build the examples" OFF)
IF (BUILD_EXAMPLES)
   ADD_SUBDIRECTORY(Examples)
ENDIF (BUILD_EXAMPLES)

# Build GUI.
OPTION(AIGS_BUILD_GUI "Build the GUI" OFF)
IF (AIGS_BUILD_GUI)
  OPTION( AIGS_BUILD_QT_GUI "Build the Qt GUI components" OFF )
  ADD_SUBDIRECTORY(GUI)
  IF(AIGS_BUILD_QT_GUI)
    SET(AIGS_INCLUDE_DIRS 
      ${AIGS_INCLUDE_DIRS} 
      ${AIGS_SOURCE_DIR}/GUI/Qt 
      ${AIGS_SOURCE_DIR}/GUI/Qt/utilities
      ${AIGS_BINARY_DIR}/GUI/Qt
      ${AIGS_BINARY_DIR}/GUI/Qt/utilities)
  ENDIF(AIGS_BUILD_QT_GUI)
ENDIF (AIGS_BUILD_GUI)

# Add aigs.py file.  On Win32, this has to go into all config dirs
IF(VTK_WRAP_PYTHON)
  IF(WIN32 AND CMAKE_CONFIGURATION_TYPES)
    FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
      CONFIGURE_FILE(
        ${AIGS_SOURCE_DIR}/AIGS.py.in
        ${LIBRARY_OUTPUT_PATH}/${config}/aigs.py)
    ENDFOREACH(config)
  ELSE(WIN32 AND CMAKE_CONFIGURATION_TYPES)
    CONFIGURE_FILE(
      ${AIGS_SOURCE_DIR}/aigs.py.in
      ${LIBRARY_OUTPUT_PATH}/aigs.py)
  ENDIF(WIN32 AND CMAKE_CONFIGURATION_TYPES)
ENDIF(VTK_WRAP_PYTHON)

#-----------------------------------------------------------------------------
# The commands in this directory are intended to be executed as
# the end of the whole configuration process, as a "last step".
# This directory is typically the last SUBDIRS in the main CMakeLists.txt.
ADD_SUBDIRECTORY(CMake/LastConfigureStep)

