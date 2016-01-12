cmake_minimum_required( VERSION 2.8 )
project( VBTOL )

set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  ${CMAKE_MODULE_PATH} )

include( LibFindMacros )

# The version number.
set (RTOL_VERSION_MAJOR 3)
set (RTOL_VERSION_MINOR 3)

# look for tol
find_package( TOL REQUIRED )
if( TOL_FOUND )
  message( STATUS "TOL_INCLUDE_DIR = ${TOL_INCLUDE_DIR}" )
  message( STATUS "TOL_LIBRARIES = ${TOL_LIBRARIES}" )
else( TOL_FOUND )
endif( TOL_FOUND )

if( DEFINED TOL_PREFIX_PATH )
  set( CMAKE_INSTALL_PREFIX ${TOL_PREFIX_PATH} )
  if( WIN32 )
    string( REPLACE "\\" "/" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} ) 
  endif( WIN32 )
  message( STATUS "Intalling at CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
else( DEFINED TOL_PREFIX_PATH )
  message( SEND_ERROR "TOL_PREFIX_PATH is needed." )
endif( DEFINED TOL_PREFIX_PATH )

add_subdirectory( source )
