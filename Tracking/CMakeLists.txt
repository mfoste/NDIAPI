PROJECT(TRACKING)

SET(KIT Tracking)
SET(UKIT TRACKING)
SET(KIT_LIBS vtkHybrid vtkParallel)
SET(KIT_EXTRA_LIBS)

SET ( Kit_HDRS
vtkTracker.h
vtkTrackerTool.h
vtkFakeTracker.h
vtkTrackerBuffer.h
vtkFrameToTimeConverter.h
)

SET ( Kit_SRCS
vtkTracker.cxx
vtkTrackerTool.cxx
vtkFakeTracker.cxx
vtkTrackerBuffer.cxx
vtkFrameToTimeConverter.cxx
)

SET_SOURCE_FILES_PROPERTIES(
vtkTracker
ABSTRACT
)

IF (AIGS_USE_CERTUS)
  SET ( Kit_HDRS ${Kit_HDRS}
    vtkNDICertusTracker.h)
  SET ( Kit_SRCS ${Kit_SRCS}
	vtkNDICertusTracker.cxx)
  FIND_PATH(NDIOAPI_INCLUDE_DIR ndopto.h)
  FIND_LIBRARY(NDIOAPI_LIBRARY oapi)
  INCLUDE_DIRECTORIES(${VTKNDICAPI_SOURCE_DIR}
                      ${VTKNDICAPI_BINARY_DIR}
                      ${NDIOAPI_INCLUDE_DIR})
  SET (KIT_EXTRA_LIBS ${KIT_EXTRA_LIBS} vtkndicapi ${NDIOAPI_LIBRARY})
ENDIF (AIGS_USE_CERTUS)

IF (AIGS_USE_NDI)
  SET ( Kit_HDRS ${Kit_HDRS}
	vtkNDITracker.h)
  SET ( Kit_SRCS ${Kit_SRCS}
	vtkNDITracker.cxx)
  SET (KIT_EXTRA_LIBS ${KIT_EXTRA_LIBS} vtkndicapi)
  INCLUDE_DIRECTORIES(${VTKNDICAPI_SOURCE_DIR}
                      ${VTKNDICAPI_BINARY_DIR})
ENDIF (AIGS_USE_NDI)

IF (AIGS_USE_FLOCK)
  SET ( Kit_HDRS ${Kit_HDRS}
	vtkFlockTracker.h)
  SET ( Kit_SRCS ${Kit_SRCS}
        vtkFlockTracker.cxx)
  SET (KIT_EXTRA_LIBS ${KIT_EXTRA_LIBS} vtkflock)
  INCLUDE_DIRECTORIES(${VTKFLOCK_SOURCE_DIR}
                      ${VTKFLOCK_BINARY_DIR})
ENDIF (AIGS_USE_FLOCK)

IF (AIGS_USE_3DG)
  SET ( Kit_HDRS ${Kit_HDRS}
	vtkAscension3DGTracker.h)
  SET ( Kit_SRCS ${Kit_SRCS}
    vtkAscension3DGTracker.cxx)
  INCLUDE_DIRECTORIES( ${ATC_INCLUDE_DIRECTORY} )
  SET (KIT_EXTRA_LIBS ${KIT_EXTRA_LIBS} ${ATC_LIBRARY})
ENDIF (AIGS_USE_3DG)

IF (AIGS_USE_LOGITECH)
  SET ( Kit_HDRS ${Kit_HDRS}
	vtkLogitechTracker.h)
  SET ( Kit_SRCS ${Kit_SRCS}
        vtkLogitechTracker.cxx)
  SET (KIT_EXTRA_LIBS ${KIT_EXTRA_LIBS} vtklogitech3d)
  INCLUDE_DIRECTORIES(${VTKLOGITECH_SOURCE_DIR}
                      ${VTKLOGITECH_BINARY_DIR})
ENDIF (AIGS_USE_LOGITECH)

IF (AIGS_USE_POLHEMUS)
  SET ( Kit_HDRS ${Kit_HDRS}
	vtkPolhemusTracker.h)
  SET ( Kit_SRCS ${Kit_SRCS}
        vtkPolhemusTracker.cxx)
  SET (KIT_EXTRA_LIBS ${KIT_EXTRA_LIBS} vtkpolhemus)
  INCLUDE_DIRECTORIES(${VTKPOLHEMUS_SOURCE_DIR}
                      ${VTKPOLHEMUS_BINARY_DIR})
ENDIF (AIGS_USE_POLHEMUS)

IF (AIGS_USE_MICRONTRACKER)
  SET ( Kit_HDRS ${Kit_HDRS}
	vtkMicronTracker.h)
  SET ( Tracking_SRCS ${Tracking_SRCS}
	vtkMicronTracker.cxx)
  INCLUDE_DIRECTORIES(${VTKMICRONTRACKERFILES_SOURCE_DIR}
                      ${VTKMICRONTRACKERFILES_BINARY_DIR})
  TARGET_LINK_LIBRARIES (vtkTracking vtkMicrontrackerfiles)
  INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
ENDIF (AIGS_USE_MICRONTRACKER)

SET(Kit_EXTRA_SRCS)
SET(Kit_EXTRA_CMDS)
SET(Kit_TCL_EXTRA_SRCS)
SET(Kit_PYTHON_EXTRA_SRCS)
SET(Kit_JAVA_EXTRA_SRCS)

# -----------------------------------------------------------------

# Include the common setting file
INCLUDE(${AIGS_SOURCE_DIR}/CMake/AIGSCommonBlock.cmake)
