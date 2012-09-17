PROJECT(vtkTrackerWidgetPlugin)

# CMake version requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# For CMake 2.6
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# find and setup Qt4 for this project
FIND_PACKAGE(Qt REQUIRED)
#Qt Includes:
SET (QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

include_directories(
    ${AIGS_SOURCE_DIR}/GUI/Qt
    ${AIGS_BINARY_DIR}/GUI/Qt
	${QT_INCLUDE_DIR}
    ${NDCommon_SOURCE_DIR}
    ${NDPublic_SOURCE_DIR}
)

# need an additional compiler flag for the NDLIBS
IF( WIN32 )
  ADD_DEFINITIONS( /DPLATFORM_X86CONSOLE)
ELSE( WIN32 )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive" )
  ADD_DEFINITIONS(-DPLATFORM_LINUX)
ENDIF( WIN32 )

# plugin source files.
set( PLUGIN_HDR_FILES
    vtkTrackerWidgetPlugin.h )
    
set( PLUGIN_SRC_FILES
    vtkTrackerWidgetPlugin.cxx )

    # build plugin.
#add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

INCLUDE_DIRECTORIES(${QT_QTDESIGNER_INCLUDE_DIR})
QT4_WRAP_CPP( GEN_PLUGIN_MOC_FILES ${PLUGIN_HDR_FILES} )

add_library( vtkTrackerWidgetPlugin SHARED 
	${GEN_PLUGIN_MOC_FILES}
    ${PLUGIN_SRC_FILES}
    ${PLUGIN_HDR_FILES}
)

add_dependencies( vtkTrackerWidgetPlugin vtkTrackerWidget)

target_link_libraries( vtkTrackerWidgetPlugin  
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
	${QT_QTDESIGNER_LIBRARY}
	${QT_QTDESIGNERCOMPONENTS_LIBRARY}
    vtkTrackerWidget
)
# build plugin
#SET(QT_PLUGIN_LIBS ${QT_LIBRARIES})


#  SET(QT_PLUGIN_LIBS
#     ${QT_QTCORE_LIBRARY}
#     ${QT_QTGUI_LIBRARY}
#  )


#${QT_QTCORE_LIBRARY}
#${QT_QTDESIGNER_LIBRARY}
#${QT_QTDESIGNERCOMPONENTS_LIBRARY}

install( TARGETS vtkTrackerWidgetPlugin DESTINATION ${QT_PLUGINS_DIR}/designer CONFIGURATIONS Release )
