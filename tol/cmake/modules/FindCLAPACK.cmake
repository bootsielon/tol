# - Find CLAPACK
# Find the native CLAPACK headers and libraries.
#
# CLAPACK_LIBRARIES - List of libraries when using CLAPACK.
# CLAPACK_FOUND - True if CLAPACK is found.
#
# Copyright 2009-2011 The TOL Development Team (http://www.tol-project.org)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
# USA.
#

find_package( LAPACK REQUIRED )
if( LAPACK_FOUND )
  message( STATUS "LAPACK_LIBRARIES = ${LAPACK_LIBRARIES}")
endif()

if( NOT CLAPACK_DIR )
  if( UNIX )
    set( CLAPACK_DIR /usr )
  else( UNIX )
    message( STATUS "Unkown CLAPACK_DIR for system ${CMAKE_SYSTEM_NAME}" )
  endif( UNIX )
endif( NOT CLAPACK_DIR )

if( NOT CLAPACK_NAME )
  set( CLAPACK_NAME clapack )
endif( NOT CLAPACK_NAME )

find_path( CLAPACK_INCLUDE_DIR
  NAMES "${CLAPACK_NAME}.h" 
  PATHS ${CLAPACK_DIR}
  PATH_SUFFIXES "include" "include/${CLAPACK_NAME}" atlas )

# check if the lapack found already provide the clapack API
include( CheckLibraryExists )
if( CLAPACK_NAME STREQUAL "lapacke" )
  set( NameFunction "LAPACKE_dpotrf" )
else( CLAPACK_NAME STREQUAL "lapacke" )
  set( NameFunction "clapack_dpotrf" )
endif( CLAPACK_NAME STREQUAL "lapacke" )

set(CMAKE_REQUIRED_LIBRARIES ${LAPACK_LIBRARIES})
list( GET LAPACK_LIBRARIES 0 first_lib )
check_library_exists( ${first_lib} ${NameFunction} "" FOUND_CLAPACK_DPOTRF )
if( FOUND_CLAPACK_DPOTRF  )
  message( STATUS "${CLAPACK_NAME} API found in LAPACK" )
  set( CLAPACK_LIBRARIES ${LAPACK_LIBRARIES} )
  set( CLAPACK_LIBRARY ${first_lib} CACHE PATH "" FORCE )
  set( CLAPACK_FOUND TRUE )
else( FOUND_CLAPACK_DPOTRF )
  find_library( CLAPACK_LIBRARY NAMES ${CLAPACK_NAME}
    HINTS ${CLAPACK_DIR}/lib ${CLAPACK_DIR}/lib64 
    PATH_SUFFIXES "atlas" "atlas-sse2" )

  set(CLAPACK_LIBRARIES ${CLAPACK_LIBRARY} )
  include( FindPackageHandleStandardArgs )

  # handle the QUIETLY and REQUIRED arguments and set CLAPACK_FOUND to TRUE
  # if all listed variables are TRUE
  find_package_handle_standard_args(CLAPACK DEFAULT_MSG CLAPACK_LIBRARY CLAPACK_INCLUDE_DIR )

  if ( CLAPACK_FOUND )
    set(CMAKE_REQUIRED_LIBRARIES ${LAPACK_LIBRARIES})
    check_library_exists( "${CLAPACK_LIBRARY}" ${NameFunction} "" FOUND_CLAPACK_DPOTRF_OTHER )
    if( NOT FOUND_CLAPACK_DPOTRF_OTHER )
      message( WARNING "Could not find ${NameFunction} in ${CLAPACK_LIBRARY}, take a look at the error message in ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log to find out what was going wrong. You most likely have to set CLAPACK_LIBRARY by hand (i.e. -DCLAPACK_LIBRARY='/path/to/libclapack.so') !" )
    endif( NOT FOUND_CLAPACK_DPOTRF_OTHER )
    set(CMAKE_REQUIRED_LIBRARIES )
  endif( CLAPACK_FOUND )
endif( FOUND_CLAPACK_DPOTRF )
mark_as_advanced( CLAPACK_LIBRARY )
