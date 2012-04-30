PROJECT( NDITrack )

SET( NDITrack_SRCS
NDITrack.cxx )

INCLUDE_DIRECTORIES( ${AIGS_INCLUDE_DIRS} 
                     ${VTKNDICAPI_SOURCE_DIR}
                     ${VTKNDICAPI_BINARY_DIR})

ADD_EXECUTABLE( NDITrack ${NDITrack_SRCS} )
TARGET_LINK_LIBRARIES( NDITrack vtkTracking vtkndicapi)

# install the executable.
INSTALL(TARGETS NDITrack 
        RUNTIME DESTINATION bin 
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib/static 
        COMPONENT Examples )