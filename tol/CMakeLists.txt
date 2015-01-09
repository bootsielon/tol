cmake_minimum_required( VERSION 2.8 )
project( TOL )

set( TRACE_COMPONENTS_FOUND 1 )

set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  ${CMAKE_MODULE_PATH} )

find_package(Subversion)

if( SUBVERSION_FOUND )
  Subversion_WC_INFO( ${PROJECT_SOURCE_DIR} Project )
  message( "Current revision is ${Project_WC_REVISION}" )
  Subversion_WC_LOG( ${PROJECT_SOURCE_DIR} Project )
  message( "Last changed log is ${Project_LAST_CHANGED_LOG}" )
endif( )

string( REPLACE "\n" "\\\n" LAST_CHANGED_LOG ${Project_LAST_CHANGED_LOG} )
message( "LAST_CHANGED_LOG = \n ${LAST_CHANGED_LOG}" )

set( TOL_SUBLIBRARIES "" CACHE INTERNAL "" )
macro( add_tol_sublibrary name )
  add_library( ${name} OBJECT ${ARGN} )
  if( TOL_SUBLIBRARIES STREQUAL "" )
    set( TOL_SUBLIBRARIES $<TARGET_OBJECTS:${name}> CACHE INTERNAL "" )
  else( )
    set( TOL_SUBLIBRARIES "${TOL_SUBLIBRARIES};$<TARGET_OBJECTS:${name}>" 
      CACHE INTERNAL "" )
  endif( )
endmacro( )

macro( create_header_links )
  #message( "create_header_links from ${CMAKE_CURRENT_SOURCE_DIR}" )
  file( GLOB headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h *.hpp )
  foreach( h ${headers} )
    execute_process(
      COMMAND "${CMAKE_COMMAND}" "-E" "create_symlink" 
      "${CMAKE_CURRENT_SOURCE_DIR}/${h}"
      "${PROJECT_BINARY_DIR}/tol/${h}"
      RESULT_VARIABLE rc
      OUTPUT_VARIABLE output
      ERROR_VARIABLE error
      )
  endforeach( h )
  
endmacro( create_header_links )

# create directory where the headers are goind to be linked.
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tol )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
#include_directories( "${PROJECT_SOURCE_DIR}/contrib" )

include( CheckIncludeFiles )
include( CheckFunctionExists )
check_include_files( "utime.h" HAVE_UTIME_H )
check_include_files( "sys/utime.h" HAVE_SYS_UTIME_H )
check_include_files( "dirent.h" HAVE_DIRENT_H )
check_include_files( "sys/ndir.h" HAVE_SYS_NDIR_H )
check_include_files( "sys/dir.h" HAVE_SYS_DIR_H )
check_include_files( "ndir.h" HAVE_NDIR_H )
check_include_files( "string.h" HAVE_STRING_H )
check_include_files( "strings.h" HAVE_STRINGS_H )
check_include_files( "unistd.h" HAVE_UNISTD_H )
check_include_files( "sys/time.h" HAVE_SYS_TIME_H )
check_include_files( "time.h" HAVE_TIME_H )
check_include_files( "stdlib.h" HAVE_STDLIB_H )
check_include_files( "limits.h" HAVE_LIMITS_H )
check_include_files( "float.h" HAVE_FLOAT_H )
check_include_files( "values.h", HAVE_VALUES_H )
check_include_files( "varargs.h", HAVE_VARARGS_H )

set( CMAKE_REQUIRED_LIBRARIES m )
check_function_exists( round HAVE_ROUND )

string( TIMESTAMP build_date "%Y-%m-%d" UTC )
string( TIMESTAMP build_time "%H:%M:%S" UTC )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/tol/config.h)

add_definitions( -DHAVE_CONFIG_H -DUSE_DELAY_INIT -D__USE_ZIP_ARCHIVE__
  -D__USE_DYNSCOPE__
  -D__POOL_NONE__=0 -D__POOL_BFSMEM__=1 -D__USE_POOL__=__POOL_BFSMEM__
  -D__HASH_MAP_MSVC__=1 -D__HASH_MAP_GOOGLE__=2 -D__HASH_MAP_GCC__=3 
  -D__HASH_MAP_ICC__=4 -D__USE_HASH_MAP__=__HASH_MAP_GOOGLE__ )

if ( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
  add_definitions( -DUNIX )
endif( )

# GSl configuration
find_package( GSL REQUIRED )

if ( GSL_FOUND AND TRACE_COMPONENTS_FOUND )
  message("GSL_INCLUDE_DIRS = ${GSL_INCLUDE_DIRS}")
  message("GSL_LIBRARIES = ${GSL_LIBRARIES}")
endif( )

# BZIP2 configuration
find_package( BZip2 REQUIRED )

if( BZip2_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "BZIP2_INCLUDE_DIR = ${BZIP2_INCLUDE_DIR}" )
  message( "BZIP2_LIBRARIES = ${BZIP2_LIBRARIES}" )
endif( )

# BOOST configuration
find_package( Boost REQUIRED COMPONENTS thread system )

if( Boost_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}" )
  message( "Boost_LIBRARIES = ${Boost_LIBRARIES}" )
endif( )

# BLAS & LAPACK configuration
find_package( LAPACK REQUIRED )

if( LAPACK_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "LAPACK_LIBRARIES = ${LAPACK_LIBRARIES}" )
endif( )

# ODBC configuration
find_package( ODBC )

if( ODBC_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "ODBC_INCLUDE_DIR = ${ODBC_INCLUDE_DIR}" )
  message( "ODBC_LIBRARIES = ${ODBC_LIBRARIES}" )
endif( )

# PostgreSQL configuration, be carefull the library path may be
# pointing to 64 bit release as it depends on pg_config output.  Note
# that we are using the FindPostgreSQL.cmake distributed by VTK
find_package( PostgreSQL )

if( PostgreSQL_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "PostgreSQL_INCLUDE_DIRS = ${PostgreSQL_INCLUDE_DIRS}" )
  message( "PostgreSQL_LIBRARY = ${PostgreSQL_LIBRARY}" )
  message( "PostgreSQL_LIBRARIES = ${PostgreSQL_LIBRARIES}" )
  message( "PostgreSQL_LIBRARY_DIRS =${PostgreSQL_LIBRARY_DIRS}" )
endif( )

# MySQL configuration
find_package( MySQL )
if( MYSQL_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "MYSQL_INCLUDE_DIRECTORIES = ${MYSQL_INCLUDE_DIRECTORIES}" )
  message( "MYSQL_LIBRARY = ${MYSQL_LIBRARY}" )
  message( "MYSQL_EXTRA_LIBRARIES = ${MYSQL_EXTRA_LIBRARIES}" )
endif( )

# FFTW configuration
find_package( FFTW REQUIRED )
if( FFTW_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "FFTW_INCLUDES = ${FFTW_INCLUDES}" )
  message( "FFTW_LIBRARIES = ${FFTW_LIBRARIES}" )
endif( )

# CINT configuration
find_package( CintHeaders )

# ZIPARCHIVE configuration
find_package( ZipArchive REQUIRED )
if( ZIPARCHIVE_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "ZIPARCHIVE_INCLUDE_DIR = ${ZIPARCHIVE_INCLUDE_DIR}" )
  message( "ZIPARCHIVE_LIBRARY = ${ZIPARCHIVE_LIBRARY}" )
endif( )

# ZLIB configuration
find_package( ZLIB REQUIRED )
if( ZLIB_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "ZLIB_INCLUDE_DIRS = ${ZLIB_INCLUDE_DIRS}" )
  message( "ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}" )
endif( )

# CHOLMOD configuration
find_package( CHOLMOD REQUIRED )
if( CHOLMOD_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "CHOLMOD_INCLUDE_DIR = ${CHOLMOD_INCLUDE_DIR}" )
  message( "CHOLMOD_LIBRARIES = ${CHOLMOD_LIBRARIES}" )
endif( )

# LTDL configuration
find_package( LTDL REQUIRED )
if( LTDL_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "LTDL_INCLUDE_DIR = ${LTDL_INCLUDE_DIR}" )
  message( "LTDL_LIBRARIES = ${LTDL_LIBRARIES}" )
endif( )

create_header_links( )

add_tol_sublibrary( tol_init init.cpp )

foreach( d 
    bbasic bmath bparser btol dbdrivers contrib lang LoadDynLib OIS 
    PackArchive )
  add_subdirectory( ${d} )
#  get_property(type TARGET ${d} PROPERTY TYPE)
#  message("type(${d}) = '${type}'") 
#  if( type STREQUAL "OBJECT_LIBRARY" )
#    list( APPEND tol_sublibs $<TARGET_OBJECTS:${d}> )
#  else( )
#    list( APPEND tol_sublibs ${d} )
#  endif( )
endforeach( )
add_library( tol SHARED ${TOL_SUBLIBRARIES} )
target_link_libraries( tol 
  ${GSL_LIBRARIES} ${BZIP2_LIBRARIES} ${Boost_LIBRARIES}
  ${LAPACK_LIBRARIES} ${FFTW_LIBRARIES} ${ZLIB_LIBRARIES}
  ${ZIPARCHIVE_LIBRARY} ${CHOLMOD_LIBRARIES} ${LTDL_LIBRARIES} )

add_executable( tolcon tol.cpp )
target_link_libraries( tolcon tol )