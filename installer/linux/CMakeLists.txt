
macro(subdirlist result dir)
  set(ExtraMacroArgs ${ARGN})
  message("GLOB children RELATIVE ${dir} ${dir}/*")
  file(GLOB children RELATIVE ${dir} ${dir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${dir}/${child})
      if(ExtraMacroArgs STREQUAL RELATIVE)
	list(APPEND dirlist ${child})
      else(ExtraMacroArgs STREQUAL RELATIVE)
	list(APPEND dirlist ${dir}/${child})
      endif(ExtraMacroArgs STREQUAL RELATIVE)
    endif(IS_DIRECTORY ${dir}/${child})
  endforeach()
  set(${result} ${dirlist})
endmacro()

#=============================================================================

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
#include(cpack_generator)
include(cpack_generator)

if(CPACK_GENERATOR)
  message(STATUS "Requested CPACK_GENERATOR = ${CPACK_GENERATOR}")
else(CPACK_GENERATOR)
  if(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR)
    message(STATUS "Setting CPACK_GENERATOR to ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR}")
    set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR} CACHE STRING "")
  endif(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR)
endif(CPACK_GENERATOR )

#set(SPECIFIC_COMPILER_NAME "")

#=============================================================================
# SETTINGS
#=============================================================================

#=== Installer filename ======================================================

# Variable PACKAGE_ARCHITECTURE
# if(CPACK_GENERATOR STREQUAL "RPM")
#   execute_process(COMMAND uname "-m"
#     OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
#     OUTPUT_STRIP_TRAILING_WHITESPACE )
# elseif(CPACK_GENERATOR STREQUAL "DEB")
#   message(STATUS "CPACK_DEBIAN_PACKAGE_ARCHITECTURE = ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
#   find_program(DPKG_CMD dpkg)
#   if(NOT DPKG_CMD)
#     message(STATUS "Can not find dpkg in your path, default to i386.")
#     set(PACKAGE_ARCHITECTURE i386)
#   endif(NOT DPKG_CMD)
#   execute_process(COMMAND "${DPKG_CMD}" --print-architecture
#     OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
#     OUTPUT_STRIP_TRAILING_WHITESPACE) 
# else(CPACK_GENERATOR STREQUAL "RPM")
#   set(PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
# endif(CPACK_GENERATOR STREQUAL "RPM")
set(PACKAGE_ARCHITECTURE ${SPECIFIC_SYSTEM_ARCHITECTURE})
message(STATUS "PACKAGE_ARCHITECTURE=${PACKAGE_ARCHITECTURE}")

# Variable CPACK_SYSTEM_NAME
string(TOLOWER "${SPECIFIC_SYSTEM_NAME}.${SPECIFIC_SYSTEM_RELEASE}.${PACKAGE_ARCHITECTURE}" _CPACK_SYSTEM_NAME)
set(CPACK_SYSTEM_NAME ${_CPACK_SYSTEM_NAME} CACHE STRING "" FORCE)
message(STATUS "CPACK_SYSTEM_NAME=${CPACK_SYSTEM_NAME}")

# Variable CPACK_PACKAGE_FILE_NAME
# Default value for ${CPACK_PACKAGE_FILE_NAME} is:
#   ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}
# See: cpack --help-variable CPACK_PACKAGE_FILE_NAME
#message(STATUS "CPACK_PACKAGE_FILE_NAME=${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")

#=== Components ==============================================================

set(CPACK_TGZ_COMPONENT_INSTALL 1 PARENT_SCOPE)
set(CPACK_DEB_COMPONENT_INSTALL 1 PARENT_SCOPE)
set(CPACK_RPM_COMPONENT_INSTALL 1 PARENT_SCOPE)

#set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE" PARENT_SCOPE)
set(CPACK_COMPONENTS_GROUPING "IGNORE" PARENT_SCOPE)
set(CPACK_COMPONENTS_ALL core headers gui)

set(CPACK_COMPONENT_CORE_DESCRIPTION "TOL Core (Time Oriented Language)" PARENT_SCOPE)
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "C++ headers for TOL (Time Oriented Language)" PARENT_SCOPE)
set(CPACK_COMPONENT_GUI_DESCRIPTION "GUI for TOL (Time Oriented Language)" PARENT_SCOPE)

#=== RPM =====================================================================
#!! Mirar si algunas configuraciones pueden obviarse porque por defecto usan 
#!! una configuración general, por ejemplo: CPACK_RPM_PACKAGE_VENDOR quizá
#!! ya usa: CPACK_PACKAGE_VENDOR
#!! Comprobar si los nombres de las componentes pueden ponerse en mayúsculas:
#!! CPACK_RPM_CORE_PACKAGE_DESCRIPTION en lugar de: CPACK_RPM_core_PACKAGE_DESCRIPTION

set(CPACK_RPM_PACKAGE_URL "www.tol-project.org" PARENT_SCOPE)
set(CPACK_RPM_PACKAGE_VENDOR "TOL Project" PARENT_SCOPE)
set(CPACK_RPM_PACKAGE_RELOCATABLE OFF PAREENT_SCOPE)
set(CPACK_RPM_PACKAGE_AUTOREQ " no" PARENT_SCOPE)
set(CPACK_RPM_PACKAGE_GROUP "Applications/Engineering" PARENT_SCOPE)

set(CPACK_RPM_core_PACKAGE_SUMMARY "The minimal TOL components necessary for a functional runtime" PARENT_SCOPE)
set(CPACK_RPM_core_PACKAGE_DESCRIPTION 
"
TOL = Time Oriented Language is a language and environment for statistical 
computing. It provides a natural way to handle time series data and model it.

There is a TOL package called TolRInside based on Rcpp/RInside wich R which
expose all R funcionality whithin TOL.

" PARENT_SCOPE)

set(FEDORA_core_REQUIRES "atlas, tcl >= 8.5, tcllib, boost-filesystem >= 1.41, boost-thread >= 1.41, suitesparse >= 3, fftw >= 3, gsl >= 1.13, bzip2-libs, zlib, libtool-ltdl, libcurl >= 7.19, postgresql-libs >= 8.4, mysql-libs, unixODBC")

set(CENTOS_core_REQUIRES "atlas, blas, lapack, tcl >= 8.5, tcllib, boost-atomic >= 1.41, boost-date-time >= 1.41, boost-chrono >= 1.41, boost-filesystem >= 1.41, boost-thread >= 1.41, suitesparse >= 3, fftw >= 3, gsl >= 1.13, bzip2-libs, zlib, libtool-ltdl, libcurl >= 7.19, postgresql-libs >= 8.4, mysql-libs, unixODBC")

set(CENTOS_gui_REQUIRES "dejavu-sans-fonts")

set(CPACK_RPM_core_PACKAGE_REQUIRES ${CENTOS_core_REQUIRES} PARENT_SCOPE)
set(CPACK_RPM_headers_PACKAGE_REQUIRES "tol-core = ${CPACK_PACKAGE_VERSION}"  PARENT_SCOPE)
set(CPACK_RPM_gui_PACKAGE_REQUIRES "tol-core = ${CPACK_PACKAGE_VERSION}, tk >= 8.5, bwidget"  PARENT_SCOPE)

set(CPACK_RPM_core_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} PARENT_SCOPE)
set(CPACK_RPM_headers_PACKAGE_ARCHITECTURE noarch PARENT_SCOPE)
set(CPACK_RPM_gui_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} PARENT_SCOPE)

#=== DEB =====================================================================

# + CPACK_DEBIAN_PACKAGE_NAME = CPACK_PACKAGE_NAME (lower case)
# + CPACK_DEBIAN_PACKAGE_VERSION = CPACK_PACKAGE_VERSION
# + CPACK_DEBIAN_PACKAGE_ARCHITECTURE
# - CPACK_DEBIAN_PACKAGE_DEPENDS
# + CPACK_DEBIAN_PACKAGE_MAINTAINER = CPACK_PACKAGE_CONTACT
# + CPACK_DEBIAN_PACKAGE_DESCRIPTION = PACK_PACKAGE_DESCRIPTION_SUMMARY
# + CPACK_DEBIAN_PACKAGE_SECTION = 'devel'
# + CPACK_DEBIAN_PACKAGE_PRIORITY = 'optional'
# - CPACK_DEBIAN_PACKAGE_HOMEPAGE
set(CPACK_DEBIAN_PACKAGE_SECTION "Mathematics" PARENT_SCOPE)
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "www.tol-project.org" PARENT_SCOPE)

set(CPACK_DEBIAN_CORE_PACKAGE_DEPENDS "libatlas3-base, tcl (>= 8.5), libboost-filesystem-dev (>= 1.41), libboost-thread-dev (>= 1.41), libsuitesparse-dev, libfftw3-bin, libgsl0ldbl (>= 1.13), bzip2, zlib1g, libltdl7, libcurl3 (>=7.19), libpq5, libmysqlclient18, unixodbc" PARENT_SCOPE) # tclcurl
set(CPACK_DEBIAN_HEADERS_PACKAGE_DEPENDS "tol-core (= ${CPACK_PACKAGE_VERSION})" PARENT_SCOPE)
set(CPACK_DEBIAN_GUI_PACKAGE_DEPENDS "tol-core (= ${CPACK_PACKAGE_VERSION}), tk (>= 8.5), bwidget, tcllib, blt, tktreectrl, libtk-img" PARENT_SCOPE) # tclodbc, tk-table

#=============================================================================
# INSTALLATION FILES
#=============================================================================

#=== Installation Directory ==================================================

# lib or lib64 directory
if(CPACK_GENERATOR STREQUAL "RPM")
  get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
  if(LIB64 STREQUAL "TRUE")
    set(LIBSUFFIX "64")
  else(LIB64 STREQUAL "TRUE")
    set(LIBSUFFIX "")
  endif(LIB64 STREQUAL "TRUE")
else(CPACK_GENERATOR STREQUAL "RPM")
  set(LIBSUFFIX "")
endif(CPACK_GENERATOR STREQUAL "RPM")

set(PREFIX_TOL "lib${LIBSUFFIX}/tol${CPACK_PACKAGE_VERSION}")

#=== [core] ==================================================================

# core executables [BIN]
install(FILES 
  "${TOL_RUNTIME_DIR}/bin/tolcon"
  "${TOL_RUNTIME_DIR}/bin/tolsh"
  DESTINATION "${PREFIX_TOL}/bin"
  COMPONENT core
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ 
  GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE
  WORLD_EXECUTE)

# TolCore (stdlib directory)
file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/stdlib" stdlib_dir)
install(DIRECTORY ${stdlib_dir} DESTINATION "${PREFIX_TOL}/bin"
  COMPONENT core)

# libraries [LIB]
file(GLOB tol_libs  "${TOL_RUNTIME_DIR}/lib/*.so")
install(FILES ${tol_libs} DESTINATION "${PREFIX_TOL}/lib"
  COMPONENT core)

# core tcltk libraries (toltcl & TclCurl)
file(GLOB toltcl_dirs  
  "${TOL_RUNTIME_DIR}/lib/toltcl*"
  "${TOL_RUNTIME_DIR}/lib/TclCurl*")
install(DIRECTORY ${toltcl_dirs} DESTINATION "${PREFIX_TOL}/lib"
  COMPONENT core)

#=== [headers] ===============================================================

# headers [INCLUDE]
file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/include" include_dir)
install( DIRECTORY ${include_dir} DESTINATION "${PREFIX_TOL}"
  COMPONENT headers)
  
#=== [gui] ===================================================================

# gui executables [BIN]
install(FILES 
  "${TOL_RUNTIME_DIR}/bin/tolbase.tcl"
  "${TOL_RUNTIME_DIR}/bin/tolbase"
  DESTINATION "${PREFIX_TOL}/bin"
  COMPONENT gui
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ 
  GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE
  WORLD_EXECUTE)

# gui tcltk libraries  
subdirlist(tclExtDirs "${TOL_RUNTIME_DIR}/lib")
install(DIRECTORY ${tclExtDirs} DESTINATION "${PREFIX_TOL}/lib"
  COMPONENT gui
  PATTERN toltcl* EXCLUDE
  PATTERN TclCurl* EXCLUDE)
##!!! cuidado que excluye bftoltcl.tcl quitar *
  
#=============================================================================

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
