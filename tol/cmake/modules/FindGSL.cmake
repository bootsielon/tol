# - Try to find GSL
# Once done, this will define
#
#  GSL_FOUND - system has gsl
#  GSL_INCLUDE_DIRS - the GSL include directories
#  GSL_LIBRARIES - link these to use GSL

include( LibFindMacros )
include( MSysMacros )

# Use pkg-config to get hints about paths
libfind_pkg_check_modules( GSL_PKGCONF gsl )
message( "PKG_CONFIG_FOUND = ${PKG_CONFIG_FOUND}" )
if( PKG_CONFIG_FOUND )
  set( GSL_INCLUDE_SEARCH ${GSL_PKGCONF_INCLUDE_DIRS} )
  set( GSL_LIBS_SEARCH ${GSL_PKGCONF_INCLUDE_DIRS} )
else( PKG_CONFIG_FOUND )
  find_program( GSL_CONFIG_EXECUTABLE
    gsl-config
	/usr/bin/
	/usr/local/bin )
  if( GSL_CONFIG_EXECUTABLE )
    message( "GSL_CONFIG_EXECUTABLE = ${GSL_CONFIG_EXECUTABLE}" )
    # run the gsl-config program to get the libs
    execute_process(
      COMMAND sh "${GSL_CONFIG_EXECUTABLE}" --cflags
      OUTPUT_VARIABLE GSL_CONFIG_CFLAGS
      RESULT_VARIABLE RET
      ERROR_QUIET )
    message( "--cflags RET = ${RET}" )
    if( RET EQUAL 0 )
      string(STRIP "${GSL_CONFIG_CFLAGS}" GSL_CONFIG_CFLAGS )
      # extract include directory
      string( REGEX MATCHALL "-I[^;]+" GSL_CONFIG_INCLUDE_DIR "${GSL_CONFIG_CFLAGS}" )
      string( REPLACE "-I" "" GSL_INCLUDE_SEARCH "${GSL_CONFIG_INCLUDE_DIR}" )
    endif( RET EQUAL 0 )
    # run the gsl-config program to get the libs
    execute_process(
      COMMAND sh "${GSL_CONFIG_EXECUTABLE}" --libs
      OUTPUT_VARIABLE GSL_CONFIG_LIBS
      RESULT_VARIABLE RET
      ERROR_QUIET )
    if( RET EQUAL 0 )
      string(STRIP "${GSL_CONFIG_LIBS}" GSL_CONFIG_LIBS )
      separate_arguments( GSL_CONFIG_LIBS )

      # extract library directory
      string( REGEX MATCHALL "-L[^;]+" GSL_CONFIG_LIBS_DIR "${GSL_CONFIG_LIBS}" )
      string( REPLACE "-L" "" GSL_LIBS_SEARCH "${GSL_CONFIG_LIBS_DIR}" )
    endif( RET EQUAL 0 )
  endif( GSL_CONFIG_EXECUTABLE )
endif( PKG_CONFIG_FOUND )

if( MSYS )
  msys_convert_path( GSL_INCLUDE_SEARCH ${GSL_INCLUDE_SEARCH} )
  msys_convert_path( GSL_LIBS_SEARCH ${GSL_LIBS_SEARCH} )
endif( MSYS )

message( "GSL_INCLUDE_SEARCH ${GSL_INCLUDE_SEARCH}" )
message( "GSL_LIBS_SEARCH ${GSL_LIBS_SEARCH}" )
# Include dir
find_path( GSL_INCLUDE_DIR
  NAMES gsl/gsl_sys.h
  PATHS ${GSL_INCLUDE_SEARCH} )

# Finally the library itself
 find_library( GSL_LIBRARY
  NAMES gsl
  PATHS ${GSL_LIBS_SEARCH} )

find_library( GSLCBLAS_LIBRARY
  NAMES gslcblas
  PATHS ${GSL_LIBS_SEARCH} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set( GSL_PROCESS_INCLUDES GSL_INCLUDE_DIR )
set( GSL_PROCESS_LIBS GSL_LIBRARY GSLCBLAS_LIBRARY )
libfind_process( GSL )