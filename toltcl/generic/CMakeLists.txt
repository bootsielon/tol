add_definitions( -DTCL_THREADS 
  -D__TOLTCL_MAJOR_VERSION__=${TOLTCL_VERSION_MAJOR}
  -D__TOLTCL_MINOR_VERSION__=${TOLTCL_VERSION_MINOR} )

include_directories( ${TCL_INCLUDE_PATH} ${TOL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
 
message( STATUS "Using TCL_INCLUDE_PATH = ${TCL_INCLUDE_PATH}" )
message( STATUS "Using TCL_STUB_LIBRARY = ${TCL_STUB_LIBRARY}" )
message( STATUS "Using TCL_LIBRARY = ${TCL_LIBRARY}" )

if( UNIX )
  SET(CMAKE_INSTALL_RPATH "$ORIGIN/..") 
endif( UNIX )
add_library( toltcl SHARED
  crash.c toltcl.c pool.cpp tclutils.cpp tolaccess.cpp tolsergrp.cpp 
  toltableset.cpp toltime.cpp
)

set_target_properties( toltcl PROPERTIES OUTPUT_NAME 
  toltcl${TOLTCL_VERSION_MAJOR}.${TOLTCL_VERSION_MINOR} )

set_target_properties( toltcl PROPERTIES COMPILE_FLAGS "-DBUILD_TOLTCL -DUSE_TCL_STUBS")

if( WIN32 )
  target_link_libraries( toltcl
    ${TCL_STUB_LIBRARY} ${TOL_LIBRARY} )
else( WIN32 )
  target_link_libraries( toltcl
    ${TCL_STUB_LIBRARY} )
endif( WIN32 )

if( UNIX )
  SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib") 
endif( UNIX )
add_executable( tolsh tolsh.c )
if( WIN32 )
  target_link_libraries( tolsh ${TCL_LIBRARY} )
else( WIN32 )
  target_link_libraries( tolsh ${TCL_LIBRARY} ${TOL_LIBRARY} )
endif( WIN32 )

message( STATUS "TOLTCL_DEST_DIR = ${TOLTCL_DEST_DIR}" )
install( TARGETS tolsh RUNTIME DESTINATION "bin" )
if( WIN32 )
  install( TARGETS toltcl RUNTIME DESTINATION "${TOLTCL_DEST_DIR}" )
else( WIN32 )
  install( TARGETS toltcl RUNTIME LIBRARY DESTINATION "${TOLTCL_DEST_DIR}" )
endif( WIN32 )
