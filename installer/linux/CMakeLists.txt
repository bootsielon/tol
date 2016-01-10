macro( subdirlist result dir )
  set( ExtraMacroArgs ${ARGN} )
  message( "GLOB children RELATIVE ${dir} ${dir}/*" )
  file( GLOB children RELATIVE ${dir} ${dir}/* )
  set( dirlist "" )
  foreach( child ${children} )
    if( IS_DIRECTORY ${dir}/${child} )
      if( ExtraMacroArgs STREQUAL RELATIVE )
	list(APPEND dirlist ${child})
      else( ExtraMacroArgs STREQUAL RELATIVE )
	list(APPEND dirlist ${dir}/${child} )
      endif( ExtraMacroArgs STREQUAL RELATIVE )
    endif( IS_DIRECTORY ${dir}/${child} )
  endforeach( )
  set( ${result} ${dirlist} )
endmacro()

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH} )
include( cpack_generator )

message( STATUS "LINUX_NAME = ${LINUX_NAME}" )

if( CPACK_GENERATOR )
  message( STATUS "requested CPACK_GENERATOR = ${CPACK_GENERATOR}" )
else( CPACK_GENERATOR )
  if( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR )
    message( STATUS "Setting CPACK_GENERATOR to ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR}" )
    set( CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR} CACHE STRING "" )
  endif( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR )
endif( CPACK_GENERATOR )

set( CPACK_TGZ_COMPONENT_INSTALL 1 PARENT_SCOPE )
set( CPACK_DEB_COMPONENT_INSTALL 1 PARENT_SCOPE )
set( CPACK_RPM_COMPONENT_INSTALL 1 PARENT_SCOPE )
set( CPACK_PACKAGE_NAME tol PARENT_SCOPE )

#cpack --help-variable CPACK_PACKAGE_FILE_NAME
message( STATUS "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}" )

#set(SPECIFIC_COMPILER_NAME "")
  
set(CPACK_DEBIAN_PACKAGE_SECTION "Mathematics")

if ( CPACK_GENERATOR STREQUAL "RPM" )
  execute_process(COMMAND uname "-m"
    OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE )
elseif( CPACK_GENERATOR STREQUAL "DEB" )
  find_program(DPKG_CMD dpkg)
  if(NOT DPKG_CMD)
    message(STATUS "Can not find dpkg in your path, default to i386.")
    set( PACKAGE_ARCHITECTURE i386 )
  endif()
  execute_process(COMMAND "${DPKG_CMD}" --print-architecture
    OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE )
else( CPACK_GENERATOR STREQUAL "RPM" )
  set( PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} )
endif( CPACK_GENERATOR STREQUAL "RPM" )

string( TOLOWER "${LINUX_NAME}.${PACKAGE_ARQUITECTURE}" _CPACK_SYSTEM_NAME )
set( CPACK_SYSTEM_NAME ${_CPACK_SYSTEM_NAME} CACHE STRING "" )

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if( "${LIB64}" STREQUAL "TRUE" )
  set( LIBSUFFIX 64 )
else( "${LIB64}" STREQUAL "TRUE" )
  set( LIBSUFFIX "" )
endif( "${LIB64}" STREQUAL "TRUE" )

# RPM
set( CPACK_RPM_PACKAGE_URL "www.tol-project.org" PARENT_SCOPE )
set( CPACK_RPM_PACKAGE_VENDOR "TOL Project" PARENT_SCOPE )
set( CPACK_RPM_PACKAGE_RELOCATABLE OFF PAREENT_SCOPE )
set( CPACK_RPM_PACKAGE_AUTOREQ " no" PARENT_SCOPE )
set( CPACK_RPM_PACKAGE_GROUP "Applications/Engineering" PARENT_SCOPE )

set( CPACK_RPM_core_PACKAGE_SUMMARY "The minimal TOL components necessary for a functional runtime" PARENT_SCOPE )
set( CPACK_RPM_core_PACKAGE_DESCRIPTION 
"
TOL = Time Oriented Language is a language and environment for statistical 
computing. It provides a natural way to handle time series data and model it.

There is a TOL package called TolRInside based on Rcpp/RInside wich R which
expose all R funcionality whithin TOL.

" PARENT_SCOPE )
set( CPACK_RPM_core_PACKAGE_REQUIRES " atlas, tcl >= 8.5, boost-filesystem >= 1.41, boost-thread >= 1.41, suitesparse >= 3, fftw >= 3, gsl >= 1.13, bzip2-libs, zlib, libtool-ltdl, libcurl >= 7.19, postgresql-libs >= 8.4, mysql-libs, unixODBC " PARENT_SCOPE )
set( CPACK_RPM_headers_PACKAGE_REQUIRES " tol-core = ${CPACK_PACKAGE_VERSION} "  PARENT_SCOPE )
set( CPACK_RPM_gui_PACKAGE_REQUIRES " tol-core = ${CPACK_PACKAGE_VERSION}, tk >= 8.5, bwidget, tcllib, blt, tcl-tktreectrl, tkimg "  PARENT_SCOPE )

set( CPACK_RPM_core_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} PARENT_SCOPE )
set( CPACK_RPM_headers_PACKAGE_ARCHITECTURE noarch PARENT_SCOPE )
set( CPACK_RPM_gui_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} PARENT_SCOPE )

# DEB
set( CPACK_DEBIAN_core_PACKAGE_DEPENDS "libboost-filesystem (>= 1.41), libboost-thread (>= 1.41), libsuitesparse, libfftw3, libgsl0 (>= 1.13), bzip2, zlib1g, libltdl, libcurl3 (>=7.19), tcl (>= 8.5)" PARENT_SCOPE )
set( CPACK_DEBIAN_headers_PACKAGE_DEPENDS "tol-core (= ${CPACK_PACKAGE_VERSION})")
set( CPACK_DEBIAN_gui_PACKAGE_DEPENDS "tol-core (= ${CPACK_PACKAGE_VERSION}), tk (>= 8.5)")

# BIN: TOL-CORE
#file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/tol*" exe_pattern)
#file(GLOB RT_EXE  ${exe_pattern} "${TOL_RUNTIME_DIR}/bin/confenv.sh" )
set( PREFIX_TOL "lib${LIBSUFFIX}/tol${CPACK_PACKAGE_VERSION}" ) 
install(FILES 
  "${TOL_RUNTIME_DIR}/bin/tolcon"
  "${TOL_RUNTIME_DIR}/bin/tolsh"
  DESTINATION "${PREFIX_TOL}/bin"
  COMPONENT core
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ 
  GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE
  WORLD_EXECUTE)

file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/stdlib" stdlib_dir)
install(DIRECTORY ${stdlib_dir} DESTINATION "${PREFIX_TOL}/bin"
  COMPONENT core )

# LIB
file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/lib" lib_dir)
file( GLOB TOL_SH_LIBS  "${TOL_RUNTIME_DIR}/lib/*.so" )
install( FILES ${TOL_SH_LIBS} DESTINATION "${PREFIX_TOL}/lib"
  COMPONENT core)
file( GLOB toltcl_dir  "${TOL_RUNTIME_DIR}/lib/toltcl*" )
install( DIRECTORY ${toltcl_dir} DESTINATION "${PREFIX_TOL}/lib"
  COMPONENT core )

# INCLUDE
file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/include" include_dir)
install( DIRECTORY ${include_dir} DESTINATION "${PREFIX_TOL}"
  COMPONENT headers )

# TOLBASE = gui
subdirlist( tclExtDirs "${TOL_RUNTIME_DIR}/lib" )
install(FILES 
  "${TOL_RUNTIME_DIR}/bin/tolbase.tcl"
  "${TOL_RUNTIME_DIR}/bin/tolbase"
  DESTINATION "${PREFIX_TOL}/bin"
  COMPONENT gui
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ 
  GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE
  WORLD_EXECUTE)
install( DIRECTORY ${tclExtDirs} DESTINATION "${PREFIX_TOL}/lib"
  COMPONENT gui
  PATTERN *toltcl* EXCLUDE )

# SCRIPTS (/usr/bin)
## !!
## file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/building/Linux_32/scripts/tol*" sh_pattern)
## file(GLOB RT_SH  ${sh_pattern})
## install(FILES ${RT_SH} DESTINATION "bin" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)

# nombre del paquete instalador
#if( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR STREQUAL "RPM" )
#  configure_file(
#    ${CMAKE_CURRENT_SOURCE_DIR}/rpm_post_install.sh.in 
#    ${CMAKE_CURRENT_BINARY_DIR}/rpm_post_install.sh )
#  set( CPACK_RPM_core_POST_INSTALL_SCRIPT_FILE
#    "${CMAKE_CURRENT_BINARY_DIR}/rpm_post_install.sh" PARENT_SCOPE )
#endif( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR STREQUAL "RPM" )
# Estudiar como hacer lo mismo con CPACK_DEBIAN_core_PACKAGE_CONTROL_EXTRA