# This module defines
# TOL_FOUND.  If false, you cannot build anything that requires TOL.
# TOL_INCLUDE_DIR where to find the tol's header files
# TOL_LIBRARIES, the libraries to link against to use TOL
# also defined, but not for general use is
# TOL_LIBRARY, the path to tol library.
#
# You can provide in TOL_PREFIX_PATH the path to the prefix
# instalation of TOL.
#

include( LibFindMacros )

set( TOL_FOUND 0 )

if( EXISTS ${TOL_PREFIX_PATH} )

  #find includes
  find_path(
    TOL_INCLUDE_DIR
    NAMES "tol/tol_bcommon.h"
    PATHS ${TOL_PREFIX_PATH}
    PATH_SUFFIXES "include"
    NO_DEFAULT_PATH )
  find_library( TOL_LIBRARY
    NAMES tol
    PATHS ${TOL_PREFIX_PATH}
    PATH_SUFFIXES "lib"
    NO_DEFAULT_PATH )
else(  EXISTS ${TOL_PREFIX_PATH} )

  #find includes
  find_path(
    TOL_INCLUDE_DIR
    NAMES "tol/tol_bcommon.h" )
  find_library( TOL_LIBRARY
    NAMES tol )
endif( EXISTS ${TOL_PREFIX_PATH} )

set( TOL_PROCESS_INCLUDES TOL_INCLUDE_DIR )
set( TOL_PROCESS_LIBS TOL_LIBRARY )
libfind_process( TOL )

if( TOL_FOUND )
  add_definitions(
    -DHAVE_CONFIG_H -DUSE_DELAY_INIT -D__USE_ZIP_ARCHIVE__
    -D__USE_DYNSCOPE__
    -D__POOL_NONE__=0 -D__POOL_BFSMEM__=1 -D__USE_POOL__=__POOL_BFSMEM__
    -D__HASH_MAP_MSVC__=1 -D__HASH_MAP_GOOGLE__=2 -D__HASH_MAP_GCC__=3 
    -D__HASH_MAP_ICC__=4 -D__USE_HASH_MAP__=__HASH_MAP_GOOGLE__ )
  if( UNIX )
    add_definitions( -DUNIX )
  endif( )
endif( TOL_FOUND )

