include( LibFindMacros )

set( CINT_INCLUDE_DIR_MESSAGE
  "Set the variable CINT_ROOT_DIR to the root dir where cint is installed" )
 
find_path( CINT_INCLUDE_DIR
  NAMES "Api.h" "G__ci.h"
  PATHS ${CINT_ROOT_DIR}
  PATH_SUFFIXES "inc" "root"
  # Help the user find it if we cannot.
  DOC "The ${CINT_INCLUDE_DIR_MESSAGE}" )

if( CINT_INCLUDE_DIR )
  set( CINT_HEADERS_FOUND 1 )
else( )
  set( CINT_HEADERS_FOUND 0 )
endif( )

set( CINT_HEADERS_PROCESS_INCLUDES  CINT_INCLUDE_DIR )
libfind_process( CINT_HEADERS )