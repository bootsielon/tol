include( LibFindMacros )

set( ZIPARCHIVE_DIR_MESSAGE
  "Set the variable ZIPARCHIVE_ROOT_DIR to the root dir where ZipArchive is installed" )

set( CMAKE_FIND_LIBRARY_SUFFIXES_SAV ${CMAKE_FIND_LIBRARY_SUFFIXES} )
if( ${ZIPARCHIVE_AS_STATIC} )
  set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} )
else()
  #set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX} )
endif( )

message( STATUS "ZIPARCHIVE_ROOT_DIR = ${ZIPARCHIVE_ROOT_DIR}" )
find_library( ZIPARCHIVE_LIBRARY
  NAMES "ziparch" "ZipArchive"
  PATHS ${ZIPARCHIVE_ROOT_DIR}
  PATH_SUFFIXES "lib"
  # Help the user find it if we cannot.
  DOC ${ZIPARCHIVE_DIR_MESSAGE} )

set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV} )

if( ZIPARCHIVE_ROOT_DIR )
  find_path( ZIPARCHIVE_INCLUDE_DIR
    NAMES "ZipArchive.h" 
    PATHS ${ZIPARCHIVE_ROOT_DIR}
    PATH_SUFFIXES "include" "include/ziparchive" 
    # Help the user find it if we cannot.
    DOC ${ZIPARCHIVE_DIR_MESSAGE} )
else( ZIPARCHIVE_ROOT_DIR )
  find_path( ZIPARCHIVE_INCLUDE_DIR
    NAMES "ZipArchive.h"
    PATH_SUFFIXES "ZipArchive" "ziparchive" 
    # Help the user find it if we cannot.
    DOC ${ZIPARCHIVE_DIR_MESSAGE} )
endif( ZIPARCHIVE_ROOT_DIR )

#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(ZIPARCHIVE DEFAULT_MSG
#                                  ZIPARCHIVE_INCLUDE_DIR ZIPARCHIVE_LIBRARY )
				
#mark_as_advanced( ZIPARCHIVE DEFAULT_MSG 
#  ZIPARCHIVE_INCLUDE_DIR ZIPARCHIVE_LIBRARY )
set( ZIPARCHIVE_PROCESS_INCLUDES  ZIPARCHIVE_INCLUDE_DIR )
set( ZIPARCHIVE_PROCESS_LIBS ZIPARCHIVE_LIBRARY )
libfind_process( ZIPARCHIVE )