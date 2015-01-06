cmake_minimum_required( VERSION 2.8 )
project( TOL )

set( TRACE_COMPONENTS_FOUND 1 )

set( CMAKE_MODULE_PATH 
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )

find_package(Subversion)

if( SUBVERSION_FOUND )
  Subversion_WC_INFO( ${PROJECT_SOURCE_DIR} Project )
  message( "Current revision is ${Project_WC_REVISION}" )
  Subversion_WC_LOG( ${PROJECT_SOURCE_DIR} Project )
  message( "Last changed log is ${Project_LAST_CHANGED_LOG}" )
endif( )

macro( create_header_links )
  message( "create_header_links from ${CMAKE_CURRENT_SOURCE_DIR}" )
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

include( CheckIncludeFiles )
check_include_files( "utime.h" HAVE_UTIME_H )
check_include_files( "sys/utime.h" HAVE_SYS_UTIME_H )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/tol/config.h)

add_definitions( -DHAVE_CONFIG_H -DUSE_DELAY_INIT -D__USE_ZIP_ARCHIVE__
  -D__USE_DYNSCOPE__
  -D__POOL_NONE__=0 -D__POOL_BFSMEM__=1 -D__USE_POOL__=__POOL_BFSMEM__
  -D__HASH_MAP_MSVC__=1 -D__HASH_MAP_GOOGLE__=2 -D__HASH_MAP_GCC__=3 
  -D__HASH_MAP_ICC__=4 -D__USE_HASH_MAP__=__HASH_MAP_GOOGLE__ )
  
string( TIMESTAMP build_date "%Y-%m-%d" UTC )
string( TIMESTAMP build_time "%H:%M:%S" UTC )

find_package( GSL REQUIRED )

if ( GSL_FOUND AND TRACE_COMPONENTS_FOUND )
  message("GSL_INCLUDE_DIRS = ${GSL_INCLUDE_DIRS}")
  message("GSL_LIBRARIES = ${GSL_LIBRARIES}")
endif( )

find_package( BZip2 REQUIRED )

if( BZip2_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "BZIP2_INCLUDE_DIR = ${BZIP2_INCLUDE_DIR}" )
  message( "BZIP2_LIBRARIES = ${BZIP2_LIBRARIES}" )
endif( )

find_package( Boost COMPONENTS thread system )

if( Boost_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}" )
  message( "Boost_LIBRARIES = ${Boost_LIBRARIES}" )
endif( )

create_header_links( )
foreach( d bbasic bmath bparser btol contrib lang LoadDynLib OIS PackArchive )
  add_subdirectory( ${d} )
endforeach( )
