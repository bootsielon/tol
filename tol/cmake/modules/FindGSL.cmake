# - Try to find GSL
# Once done, this will define
#
#  GSL_FOUND - system has gsl
#  GSL_INCLUDE_DIRS - the GSL include directories
#  GSL_LIBRARIES - link these to use GSL

include( LibFindMacros )

# Use pkg-config to get hints about paths
libfind_pkg_check_modules( GSL_PKGCONF gsl )

# Include dir
find_path( GSL_INCLUDE_DIR
  NAMES gsl/gsl_sys.h
  PATHS ${GSL_PKGCONF_INCLUDE_DIRS} )

# Finally the library itself
find_library( GSL_LIBRARY
  NAMES gsl
  PATHS ${GSL_PKGCONF_LIBRARY_DIRS} )

find_library( GSLCBLAS_LIBRARY
  NAMES gslcblas
  PATHS ${GSL_PKGCONF_LIBRARY_DIRS} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set( GSL_PROCESS_INCLUDES GSL_INCLUDE_DIR )
set( GSL_PROCESS_LIBS GSL_LIBRARY GSLCBLAS_LIBRARY )
libfind_process( GSL )