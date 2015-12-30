cmake_minimum_required( VERSION 2.8 )
project( TOLTCL )

set( INSTALL_WITH_TOL 1 )

set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  ${CMAKE_MODULE_PATH} )

include( LibFindMacros )

# The version number.
set (TOLTCL_VERSION_MAJOR 3)
set (TOLTCL_VERSION_MINOR 2)

# look for tol
find_package( TOL REQUIRED )
if( TOL_FOUND )
  message( STATUS "TOL_INCLUDE_DIR = ${TOL_INCLUDE_DIR}" )
  message( STATUS "TOL_LIBRARIES = ${TOL_LIBRARIES}" )
else( TOL_FOUND )
endif( TOL_FOUND )

# look for tcl
if ( WIN32 AND TOL_PREFIX_PATH )
  if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set( g "g" )
  endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  file( TO_CMAKE_PATH "${TOL_PREFIX_PATH}" TCL_LIBRARY_PATH )
  set( TCL_LIBRARY_PATH "${TCL_LIBRARY_PATH}/lib" )
  message( STATUS "buscando tcl libs en ${TCL_LIBRARY_PATH}" )
  find_library( TCL_STUB_LIBRARY
      NAMES "libtclstub84" "libtclstub85" "libtclstub86"
      PATHS ${TCL_LIBRARY_PATH}
      NO_DEFAULT_PATH
    )
  find_library( TCL_LIBRARY
      NAMES "libtcl84" "libtcl85" "libtcl86"
      PATHS "${TCL_LIBRARY_PATH}"
      NO_DEFAULT_PATH
    )
  find_path(
    TCL_INCLUDE_PATH
    NAMES "tcl.h"
    PATHS "${TOL_PREFIX_PATH}/include"
    NO_DEFAULT_PATH
    )
  set( TCL_PROCESS_INCLUDES TCL_INCLUDE_PATH )
  set( TCL_PROCESS_LIBS TCL_STUB_LIBRARY )
  libfind_process( TCL )
else( WIN32 AND TOL_PREFIX_PATH )
  find_package( TclStub REQUIRED )
endif( WIN32 AND TOL_PREFIX_PATH )

if( TCL_FOUND )
  string(REGEX REPLACE
    "^.*tclstub([0-9])\\.([0-9]).*$" "\\1;\\2" 
    TCL_STUB_VERSION "${TCL_STUB_LIBRARY}")  
  message( STATUS "TCL_STUB_VERSION = ${TCL_STUB_VERSION}" )
  list( GET TCL_STUB_VERSION 0 TCL_MAJOR_VERSION )
  #list( GET TCL_STUB_VERSION 1 TCL_MINOR_VERSION )
  message( STATUS "TCL_STUB_VERSION = ${TCL_MAJOR_VERSION}.${TCL_MINOR_VERSION}" )
  message( STATUS ${TCL_STUB_LIBRARY} )
  message( STATUS ${TCL_INCLUDE_PATH} )
  message( STATUS ${TCL_LIBRARY_PATH} )
  if( ${INSTALL_WITH_TOL} )
    set( CMAKE_INSTALL_PREFIX ${TOL_PREFIX_PATH} )
  else( ${INSTALL_WITH_TOL} )
    get_filename_component( CMAKE_INSTALL_PREFIX ${TCL_LIBRARY_PATH} PATH )
  endif( )
  if( WIN32 )
    string( REPLACE "\\" "/" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} ) 
  endif( WIN32 )
  message( STATUS "Using CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
else( TCL_FOUND )
  message( SEND_ERROR "TCL extensions need Tcl Stubs library" )
endif( TCL_FOUND )
  
set( TOLTCL_DEST_DIR "lib/toltcl${TOLTCL_VERSION_MAJOR}.${TOLTCL_VERSION_MINOR}"
  CACHE INTERNAL "" )

if( UNIX )
  add_subdirectory( bin )
endif( UNIX )

add_subdirectory( generic )
add_subdirectory( library )

