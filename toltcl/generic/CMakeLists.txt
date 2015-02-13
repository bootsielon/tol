add_definitions( -DBUILD_TOLTCL -DUSE_TCL_STUBS -DTCL_THREADS )
include_directories( ${TCL_INCLUDE_PATH} )
  
add_library( toltcl SHARED
  crash.c toltcl.c pool.cpp tclutils.cpp tolaccess.cpp tolsergrp.cpp 
  toltableset.cpp toltime.cpp
)

include_directories( ${TCL_INCLUDE_PATH} ${TOL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

message( STATUS "Using TCL_INCLUDE_PATH = ${TCL_INCLUDE_PATH}" )
message( STATUS "Using TCL_STUB_LIBRARY = ${TCL_STUB_LIBRARY}" )
target_link_libraries( toltcl
  ${TCL_STUB_LIBRARY} ${TOL_LIBRARY} )

if( WIN32 )
  install( TARGETS toltcl RUNTIME DESTINATION "${TOLTCL_DEST_DIR}" )
else( WIN32 )
  install( TARGETS toltcl RUNTIME LIBRARY DESTINATION "${TOLTCL_DEST_DIR}" )
endif( WIN32 )
