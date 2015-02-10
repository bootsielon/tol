cmake_minimum_required( VERSION 2.8 )

set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  ${CMAKE_MODULE_PATH} )

# The version number.
set (TOLTCL_VERSION_MAJOR 3)
set (TOLTCL_VERSION_MINOR 2)

# look for tcl
find_package( TclStub REQUIRED )
if( TCL_FOUND )
  add_definitions( -DUSE_TCL_STUBS )
  add_definitions( -DTCL_THREADS )
  include_directories( ${TCL_INCLUDE_PATH} )
  message( STATUS ${TCL_LIBRARY_PATH} )
  message( STATUS ${TCL_LIBRARY_PATH_PARENT} )

  set( CMAKE_INSTALL_PREFIX ${TCL_LIBRARY_PATH_PARENT} )
else( TCL_FOUND )
  message( SEND_ERROR "TCL extensions need Tcl Stubs library" )
endif( TCL_FOUND )

# look fot tol
find_package( TOL REQUIRED )
if( TOL_FOUND )
  message( STATUS "TOL_INCLUDE_DIR = ${TOL_INCLUDE_DIR}" )
  message( STATUS "TOL_LIBRARIES = ${TOL_LIBRARIES}" )
else( TOL_FOUND )
endif( TOL_FOUND )
 
set( TOLTCL_DEST_DIR "lib/toltcl${TOLTCL_VERSION_MAJOR}.${TOLTCL_VERSION_MINOR}"
  CACHE INTERNAL "" )

add_subdirectory( generic )
add_subdirectory( library )

