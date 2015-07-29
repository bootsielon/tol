# - Find CLAPACK
# Find the native CLAPACK headers and libraries.
#
# CLAPACK_LIBRARIES - List of libraries when using cblas.
# CLAPACK_FOUND - True if cblas found.
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

if( NOT CLAPACK_NAME )
  set( CLAPACK_NAME clapack )
endif( NOT CLAPACK_NAME )

find_library( CLAPACK_LIBRARY NAMES ${CLAPACK_NAME}
  HINTS ${CLAPACK_DIR}/lib ${CLAPACK_DIR}/lib64 
  PATH_SUFFIXES "atlas" "atlas-sse2" )

set(CLAPACK_LIBRARIES ${CLAPACK_LIBRARY} )

find_path( CLAPACK_INCLUDE_DIR
  NAMES "${CLAPACK_NAME}.h" 
  PATHS ${CLAPACK_DIR}
  PATH_SUFFIXES "include" "include/${CLAPACK_NAME}")


include( FindPackageHandleStandardArgs )

# handle the QUIETLY and REQUIRED arguments and set CLAPACK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CLAPACK DEFAULT_MSG CLAPACK_LIBRARY CLAPACK_INCLUDE_DIR )

if ( CLAPACK_FOUND )
  include( CheckLibraryExists )
  check_library_exists( "${CLAPACK_LIBRARY}" ${CLAPACK_NAME}_dpotrf "" FOUND_CLAPACK_DPOTRF )
  if( NOT FOUND_CLAPACK_DPOTRF )
    message( FATAL_ERROR "Could not find cblas_dtrsm in ${CLAPACK_LIBRARY}, take a look at the error message in ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log to find out what was going wrong. You most likely have to set CLAPACK_LIBRARY by hand (i.e. -DLAPACK_LIBRARY='/path/to/libclapack.so') !" )
  endif( NOT FOUND_CLAPACK_DPOTRF )
endif( CLAPACK_FOUND )

mark_as_advanced( CLAPACK_LIBRARY )