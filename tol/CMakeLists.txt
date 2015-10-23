cmake_minimum_required( VERSION 2.8 )
project( TOL )

#-----------------------------------------------------------------------------
#
# To fix compilation problem: relocation R_X86_64_32 against `a local symbol' can not be
# used when making a shared object; recompile with -fPIC
# See http://www.cmake.org/pipermail/cmake/2007-May/014350.html
#
if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT WIN32 )
  set( NEED_FPIC "-fPIC" )
else( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT WIN32 )
  unset( NEED_FPIC )
endif( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT WIN32 )

if( TOL_EXTERNAL_DEVEL_DIR )
  if( CMAKE_BUILD_TYPE MATCHES Debug )
    set( TOL_EXTERNAL_PREFIX "${TOL_EXTERNAL_DEVEL_DIR}/debug" )
  else( CMAKE_BUILD_TYPE MATCHES DEBUG )
    set( TOL_EXTERNAL_PREFIX "${TOL_EXTERNAL_DEVEL_DIR}/release" )
  endif( CMAKE_BUILD_TYPE MATCHES Debug )
  set( CMAKE_PREFIX_PATH "${TOL_EXTERNAL_PREFIX}" ${CMAKE_PREFIX_PATH} )
  set( GSL_INCLUDE_SEARCH "${TOL_EXTERNAL_PREFIX}/include" )
  set( GSL_LIBS_SEARCH "${TOL_EXTERNAL_PREFIX}/lib" )
  set( CBLAS_DIR "${TOL_EXTERNAL_PREFIX}" )
  set( CLAPACK_DIR "${TOL_EXTERNAL_PREFIX}" )
  set( ZIPARCHIVE_AS_STATIC 0 )
  set( CINT_ROOT_DIR "${TOL_EXTERNAL_PREFIX}/include/cint" )
  set( PKG_FFTW_LIBRARY_DIRS "${TOL_EXTERNAL_PREFIX}/lib" CACHE INTERNAL "" )
  set( PKG_FFTW_INCLUDE_DIRS "${TOL_EXTERNAL_PREFIX}/include" CACHE INTERNAL "" )
  set( CMAKE_LIBRARY_PATH "${TOL_EXTERNAL_PREFIX}/lib" )
  find_library( FFTW_LIB
    NAMES "fftw3"
    PATHS ${PKG_FFTW_LIBRARY_DIRS}
    )
  #message( "aaaaaa FFTW_LIB ${FFTW_LIB}" )
  #-DFFTW_ROOT:PATH=c:/MinGW/msys/1.0/local -DFFTW_USE_STATIC_LIBS=1 
  set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Runtime_Base" )
endif( )

set( TRACE_COMPONENTS_FOUND 1 )

message( "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}" )

set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  ${CMAKE_MODULE_PATH} )
include( MSysMacros )
include( GetPrerequisites )

if( MSYS )
  msys_convert_path( msys_usrlocal "/usr/local" )
  set( CMAKE_PREFIX_PATH "${msys_usrlocal}" ${CMAKE_PREFIX_PATH} )
  include_directories( "${msys_usrlocal}/include" )
endif( MSYS )

if ( MINGW )
  #add_definitions( "-Wno-attributes" )
  add_definitions( "-Wall" )
endif( MINGW )
if( WIN32 )
  add_definitions( -DTOLDLL_EXPORTS )
endif( WIN32 )

if( CHECK_SVN )
  find_package(Subversion)
endif( CHECK_SVN )

if( SUBVERSION_FOUND )
  Subversion_WC_INFO( ${PROJECT_SOURCE_DIR} Project )
  message( "Current revision is ${Project_WC_REVISION}" )
  Subversion_WC_LOG( ${PROJECT_SOURCE_DIR} Project )
  message( "Last changed log is ${Project_LAST_CHANGED_LOG}" )
else( )
  set( Project_WC_REVISION -1 )
  set( Project_LAST_CHANGED_LOG "unknown changes" )
endif( )

string( REPLACE "\n" "\\\n" LAST_CHANGED_LOG ${Project_LAST_CHANGED_LOG} )
message( "LAST_CHANGED_LOG = \n ${LAST_CHANGED_LOG}" )

set( TOL_SUBLIBRARIES "" CACHE INTERNAL "" )
macro( add_tol_sublibrary name )
  add_library( ${name} OBJECT ${ARGN} )
  #message( STATUS "add_library( ${name} OBJECT ${ARGN} )" )
  if( CMAKE_BUILD_TYPE MATCHES Debug )
    #set_property(TARGET ${name} PROPERTY COMPILE_FLAGS "-fPIC -Wall")
    target_compile_options( ${name} PRIVATE "-Wall" )
  endif( CMAKE_BUILD_TYPE MATCHES Debug )
  if( NEED_FPIC )
    target_compile_options( ${name} PRIVATE "-fPIC" )
  endif( NEED_FPIC )
  if( TOL_SUBLIBRARIES STREQUAL "" )
    set( TOL_SUBLIBRARIES $<TARGET_OBJECTS:${name}> CACHE INTERNAL "" )
  else( )
    set( TOL_SUBLIBRARIES "${TOL_SUBLIBRARIES};$<TARGET_OBJECTS:${name}>" 
      CACHE INTERNAL "" )
  endif( )
endmacro( )

set( skip_copy_headers 0 )
macro( create_header_links__ )
  #message( "create_header_links from ${CMAKE_CURRENT_SOURCE_DIR}" )
  if( skip_copy_headers )
    message( STATUS "Skipping create_header_links for ${CMAKE_CURRENT_SOURCE_DIR}" )
  else( )
      file( GLOB headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h *.hpp )
      if( WIN32 )
        set( cmd "copy_if_different" )
      else( WIN32 )
        set( cmd "create_symlink" )
      endif( WIN32 )
      foreach( h ${headers} )
        execute_process(
          COMMAND "${CMAKE_COMMAND}" "-E" ${cmd} 
          "${CMAKE_CURRENT_SOURCE_DIR}/${h}"
          "${PROJECT_BINARY_DIR}/tol/${h}"
          RESULT_VARIABLE rc
          OUTPUT_VARIABLE output
          ERROR_VARIABLE error
          )
       endforeach( h )
  endif( )  
endmacro( create_header_links__ )

macro( create_header_links2 )
  if( WIN32 )
    set( cmd "copy" )
  else( WIN32 )
    set( cmd "create_symlink" )
  endif( WIN32 )
  file( GLOB headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h *.hpp )
  foreach( h ${headers} )
    set( srcFile "${CMAKE_CURRENT_SOURCE_DIR}/${h}" )
    set( destFile "${PROJECT_BINARY_DIR}/tol/${h}" )
    if( NOT EXISTS ${destFile} OR ( ${srcFile} IS_NEWER_THAN ${destFile} ) )
        message( STATUS "Copying from %{srcFile} to ${newFile}" )
        execute_process(
          COMMAND "${CMAKE_COMMAND}" "-E" ${cmd} ${srcFile} ${destFile}
          RESULT_VARIABLE rc
          OUTPUT_VARIABLE output
          ERROR_VARIABLE error )
    endif()
  endforeach( )
endmacro( create_header_links2 )

set( COPY_HEADERS_DEPENDS "" CACHE INTERNAL "" )
macro( create_header_links )
  if( WIN32 )
    set( cmd "copy" )
  else( WIN32 )
    set( cmd "create_symlink" )
  endif( WIN32 )
  if( skip_copy_headers )
    message( STATUS "Skipping create_header_links for ${CMAKE_CURRENT_SOURCE_DIR}" )
  else( skip_copy_headers )
    file( GLOB headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h *.hpp )
    set( lst  "" )
    foreach( h ${headers} )
      set( srcFile "${CMAKE_CURRENT_SOURCE_DIR}/${h}" )
      set( destFile "${PROJECT_BINARY_DIR}/tol/${h}" )
      set( lst ${lst} "${destFile}" )
      add_custom_command ( 
        OUTPUT ${destFile}
        COMMAND "${CMAKE_COMMAND}" "-E" ${cmd} ${srcFile} ${destFile}
        DEPENDS ${srcFile}
        COMMENT "Creating header ${destFile}" )
    endforeach( )
    string( REPLACE "${PROJECT_SOURCE_DIR}" "" COPY_TARGET_NAME "COPY_HDR${CMAKE_CURRENT_SOURCE_DIR}" )
    string( REPLACE "/" "_" COPY_TARGET_NAME "${COPY_TARGET_NAME}" )
    #message( "add_custom_target( ${COPY_TARGET_NAME} ALL DEPENDS ${lst} ) " )
    add_custom_target( ${COPY_TARGET_NAME} ALL DEPENDS ${lst} )  
    set( tmp ${COPY_HEADERS_DEPENDS} ${COPY_TARGET_NAME} )
    set( COPY_HEADERS_DEPENDS ${tmp} CACHE INTERNAL "" )
  endif( skip_copy_headers )
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
check_include_files( "values.h" HAVE_VALUES_H )
check_include_files( "varargs.h" HAVE_VARARGS_H )

set( CMAKE_REQUIRED_LIBRARIES m )
check_function_exists( round HAVE_ROUND )

string( TIMESTAMP build_date "%Y-%m-%d" UTC )
string( TIMESTAMP build_time "%H:%M:%S" UTC )

add_definitions( -DHAVE_CONFIG_H -DUSE_DELAY_INIT -D__USE_ZIP_ARCHIVE__
  -D__USE_DYNSCOPE__
  -D__POOL_NONE__=0 -D__POOL_BFSMEM__=1 -D__USE_POOL__=__POOL_BFSMEM__
  -D__HASH_MAP_MSVC__=1 -D__HASH_MAP_GOOGLE__=2 -D__HASH_MAP_GCC__=3 
  -D__HASH_MAP_ICC__=4 -D__USE_HASH_MAP__=__HASH_MAP_GOOGLE__ )

if( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
  add_definitions( -DUNIX )
endif( )

if ( MINGW )
  # http://stackoverflow.com/questions/14181351/i-got-duplicate-section-errors-when-compiling-boost-regex-with-size-optimizati
  # add_definitions( -fno-tree-vectorize )
  # add_definitions( -fno-implicit-templates )
endif( MINGW )

# GSL configuration
find_package( GSL REQUIRED )

if( GSL_FOUND AND TRACE_COMPONENTS_FOUND )
  message("GSL_INCLUDE_DIRS = ${GSL_INCLUDE_DIRS}")
  message("GSL_LIBRARY = ${GSL_LIBRARY}")
  message("GSL_LIBRARIES = ${GSL_LIBRARIES}")
endif( )

# CBLAS configuration
find_package( CBLAS )
if( CBLAS_FOUND )
  set( HAVE_CBLAS_H 1 CACHE INTERNAL "" )
  set( TOL_CBLAS_LIBRARIES ${CBLAS_LIBRARIES} )
  set( TOL_CBLAS_INCLUDE_DIR ${CBLAS_INCLUDE_DIR} )
else( CBLAS_FOUND )
  message( STATUS "CBLAS not found, using gslcblas = ${GSLCBLAS_LIBRARY}" )
  unset( HAVE_CBLAS_H CACHE )
  set( TOL_CBLAS_LIBRARIES ${GSLCBLAS_LIBRARY} CACHE INTERNAL "" )
  set( TOL_CBLAS_INCLUDE_DIR ${GSL_INCLUDE_DIRS} CACHE INTERNAL "")
endif( CBLAS_FOUND )
if( TRACE_COMPONENTS_FOUND )
  message( "TOL_CBLAS_INCLUDE_DIR = ${TOL_CBLAS_INCLUDE_DIR}" )
  message( "TOL_CBLAS_LIBRARIES = ${TOL_CBLAS_LIBRARIES}" )
endif( TRACE_COMPONENTS_FOUND )

# CLAPACK configuration
find_package( CLAPACK REQUIRED )

if( CLAPACK_FOUND AND TRACE_COMPONENTS_FOUND )
  message("CLAPACK_INCLUDE_DIR = ${CLAPACK_INCLUDE_DIR}")
  message("CLAPACK_LIBRARY = ${CLAPACK_LIBRARY}")
  message("CLAPACK_LIBRARIES = ${CLAPACK_LIBRARIES}")
  if( CLAPACK_NAME STREQUAL "lapacke" )
    set( HAVE_LAPACKE_H 1 CACHE INTERNAL "" )
  else( CLAPACK_NAME STREQUAL "lapacke" )
    unset( HAVE_LAPACKE_H CACHE )
  endif( CLAPACK_NAME STREQUAL "lapacke" )
  include_directories( "${CLAPACK_INCLUDE_DIR}" )
endif( )

# BZIP2 configuration
find_package( BZip2 REQUIRED )

if( BZip2_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "BZIP2_INCLUDE_DIR = ${BZIP2_INCLUDE_DIR}" )
  message( "BZIP2_LIBRARIES = ${BZIP2_LIBRARIES}" )
endif( )

# BOOST configuration
find_package( Boost REQUIRED COMPONENTS thread system filesystem )

if( Boost_FOUND AND TRACE_COMPONENTS_FOUND )
  message( "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}" )
  message( "Boost_LIBRARIES = ${Boost_LIBRARIES}" )
endif( )

include_directories( ${Boost_INCLUDE_DIRS} )

find_package( BLAS REQUIRED )
if ( BLAS_FOUND AND TRACE_COMPONENTS_FOUND )
  message( STATUS "BLAS_LIBRARIES = ${BLAS_LIBRARIES}")
endif()

# BLAS & LAPACK configuration
find_package( LAPACK REQUIRED )
if( LAPACK_FOUND AND TRACE_COMPONENTS_FOUND )
  message( STATUS "LAPACK_LIBRARIES = ${LAPACK_LIBRARIES}")
endif()

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
if( CINT_HEADERS_FOUND )
  if( TRACE_COMPONENTS_FOUND )
    message( "CINT_INCLUDE_DIR = ${CINT_INCLUDE_DIR}" )
  endif( )
  set( HAVE_CINT 1 CACHE INTERNAL "" )
else( )
  unset( HAVE_CINT CACHE )
endif( )

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

if( NOT DEFINED WRITE_CONFIG_H )
  set( WRITE_CONFIG_H 1 )
endif( NOT DEFINED WRITE_CONFIG_H )
  
if( ${WRITE_CONFIG_H} )
  message( STATUS "Invoking CONFIGURE_FILE" )
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
    ${CMAKE_CURRENT_BINARY_DIR}/tol/config.h )
endif( )

if( WIN32 )
  add_tol_sublibrary( tol_init toldll.cpp init.cpp )
else( WIN32 )
  add_tol_sublibrary( tol_init init.cpp )
endif( WIN32 )
target_include_directories( tol_init PRIVATE
    ${CHOLMOD_INCLUDE_DIR} )
    
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
if( WIN32 )
  set( PLATFORM_LIBRARIES ws2_32 )
endif( WIN32 )
target_link_libraries( tol 
  ${GSL_LIBRARY} ${TOL_CBLAS_LIBRARIES} ${CLAPACK_LIBRARIES}
  ${BZIP2_LIBRARIES} ${Boost_LIBRARIES}
  ${FFTW_LIBRARIES} ${ZLIB_LIBRARIES}
  ${ZIPARCHIVE_LIBRARY} ${CHOLMOD_LIBRARIES} ${LTDL_LIBRARIES}
  ${PLATFORM_LIBRARIES} ${LAPACK_LIBRARIES} )

add_executable( tolcon tol.cpp )
target_link_libraries( tolcon tol )

#add_subdirectory( zippie_src )

#message( STATUS "ALL_HEADERS_LIST ${ALL_HEADERS_LIST}" )
add_custom_target( COPY_HEADERS ALL DEPENDS ${COPY_HEADERS_DEPENDS} )

install( TARGETS tolcon RUNTIME DESTINATION bin )
install( TARGETS tol
  RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib )
install( DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tol" 
  DESTINATION "include"
  COMPONENT DEVELOPMENT )
install( DIRECTORY stdlib COMPONENT RUNTIME DESTINATION "bin" 
  PATTERN "*.tol" 
  PATTERN "*.bst"
  PATTERN "*.oza" )

if( WIN32 AND TOL_EXTERNAL_DEVEL_DIR )
  if( NOT EXISTS ${CMAKE_INSTALL_PREFIX}/bin )
    file( MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin )
  endif( )
  if( NOT EXISTS ${CMAKE_INSTALL_PREFIX}/lib )
    file( MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib )
  endif( )
  if( NOT EXISTS ${CMAKE_INSTALL_PREFIX}/include )
    file( MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include )
  endif( )  
  file( TO_CMAKE_PATH "${TOL_EXTERNAL_PREFIX}/bin/*.dll" dll_pattern )
  file( GLOB RT_DLL  ${dll_pattern} )
  foreach( dll libboost_filesystem.dll libboost_system.dll)
    list( APPEND RT_DLL "${TOL_EXTERNAL_PREFIX}/lib/${dll}" )
  endforeach( )
  list( GET RT_DLL 0 rt_dll0)
  get_filename_component( name_rt_dll0 ${rt_dll0} NAME )
  message(  "OJO ${CMAKE_INSTALL_PREFIX}/bin/${name_rt_dll0}")
  if( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/bin/${name_rt_dll0}" )
    file( INSTALL ${RT_DLL} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
  endif( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/bin/${name_rt_dll0}" )
  if( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/tkdnd" )
    file( GLOB tcl-libs "${TOL_EXTERNAL_DEVEL_DIR}/tcl-libs/*" )
    file( INSTALL ${tcl-libs} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
  endif( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/tkdnd" )
  
  set( tcldirs "")
  foreach( pdir tcl8 tk8 dde reg TclCurl )
    file( GLOB pdirx.y RELATIVE "${TOL_EXTERNAL_PREFIX}/lib" "${TOL_EXTERNAL_PREFIX}/lib/${pdir}*" )
	list( APPEND tcldirs ${pdirx.y} )
  endforeach( pdir tcl8 tk8 dde reg TclCurl )
  list( GET tcldirs 0 tcldir0)
  if( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${tcldir0}" )
    foreach( d ${tcldirs} )
	  file( INSTALL "${TOL_EXTERNAL_PREFIX}/lib/${d}" DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
	endforeach( d ${tcldirs} )
  endif( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${tcldir0}" )
  
  if( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/include/tcl.h" )
    file( GLOB tcl_h "${TOL_EXTERNAL_PREFIX}/include/tcl*.h" )
    file( GLOB tk_h "${TOL_EXTERNAL_PREFIX}/include/tk*.h" )
    file( INSTALL ${tcl_h} DESTINATION "${CMAKE_INSTALL_PREFIX}/include" )
    file( INSTALL ${tk_h} DESTINATION "${CMAKE_INSTALL_PREFIX}/include" )
    file( INSTALL "${TOL_EXTERNAL_PREFIX}/include/X11" DESTINATION "${CMAKE_INSTALL_PREFIX}/include" )
  endif( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/include/tcl.h" )
  if( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/include/google" )
    file( INSTALL
          "${TOL_EXTERNAL_PREFIX}/include/google" 
          "${TOL_EXTERNAL_PREFIX}/include/sparsehash"
          "${TOL_EXTERNAL_PREFIX}/include/libltdl"
          "${TOL_EXTERNAL_PREFIX}/include/ltdl.h"
          "${TOL_EXTERNAL_PREFIX}/include/gsl"
          DESTINATION "${CMAKE_INSTALL_PREFIX}/include" )
  endif( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/include/google" )
  message( STATUS "queee ${CMAKE_INSTALL_PREFIX}/lib/libltdl.dll.a")
  if( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/libltdl.dll.a" )
    file( INSTALL 
          "${TOL_EXTERNAL_PREFIX}/lib/libltdl.dll.a" 
          "${TOL_EXTERNAL_PREFIX}/lib/libgsl.dll.a"
          "${TOL_EXTERNAL_PREFIX}/lib/libgslcblas.dll.a"
		  "${TOL_EXTERNAL_PREFIX}/lib/libblas.dll.a"
		  "${TOL_EXTERNAL_PREFIX}/lib/liblapack.dll.a"
		  "${TOL_EXTERNAL_PREFIX}/lib/liblapacke.dll.a"
          DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
  endif( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/libltdl.dll.a" )
  file( GLOB tcl_a "${TOL_EXTERNAL_PREFIX}/lib/libtcl*.a" )
  message( "tcl_a = ${tcl_a}")
  list( GET tcl_a 0 tcl_a0 )
  get_filename_component( name_tcl_a0 ${tcl_a0} NAME )
  if( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${name_tcl_a0}" )
    file( GLOB tk_a "${TOL_EXTERNAL_PREFIX}/lib/libtk*.a" )
    file( INSTALL ${tcl_a} ${tk_a}
          DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
  endif( NOT EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${name_tcl_a0}" )  

endif( WIN32 AND TOL_EXTERNAL_DEVEL_DIR )
