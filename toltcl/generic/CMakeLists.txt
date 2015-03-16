add_definitions( -DTCL_THREADS )
include_directories( ${TCL_INCLUDE_PATH} ${TOL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
 
message( STATUS "Using TCL_INCLUDE_PATH = ${TCL_INCLUDE_PATH}" )
message( STATUS "Using TCL_STUB_LIBRARY = ${TCL_STUB_LIBRARY}" )
message( STATUS "Using TCL_LIBRARY = ${TCL_LIBRARY}" )

if ( NOT WIN32 )
	# use, i.e. don't skip the full RPATH for the build tree
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


  # the RPATH to be used when installing, but only if it's not a system directory
  LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  IF("${isSystemDir}" STREQUAL "-1")
     SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF("${isSystemDir}" STREQUAL "-1")
endif( NOT WIN32 )

add_library( toltcl SHARED
  crash.c toltcl.c pool.cpp tclutils.cpp tolaccess.cpp tolsergrp.cpp 
  toltableset.cpp toltime.cpp
)
set_target_properties( toltcl PROPERTIES COMPILE_FLAGS "-DBUILD_TOLTCL -DUSE_TCL_STUBS")

target_link_libraries( toltcl
  ${TCL_STUB_LIBRARY} ${TOL_LIBRARY} )

add_executable( tolsh tolsh.c )
target_link_libraries( tolsh ${TCL_LIBRARY} )

message( STATUS "TOLTCL_DEST_DIR = ${TOLTCL_DEST_DIR}" )
install( TARGETS tolsh RUNTIME DESTINATION "bin" )
if( WIN32 )
  install( TARGETS toltcl RUNTIME DESTINATION "${TOLTCL_DEST_DIR}" )
else( WIN32 )
  install( TARGETS toltcl RUNTIME LIBRARY DESTINATION "${TOLTCL_DEST_DIR}" )
endif( WIN32 )
