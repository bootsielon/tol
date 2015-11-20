cmake_minimum_required(VERSION 2.8)
project(TOL-Installer)

if(WIN32)
  # módulo adicional para construir el instalados con NSIS
  set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/NSIS" ${CMAKE_MODULE_PATH})
endif(WIN32)

if(NOT TOL_RUNTIME_DIR)
  message(FATAL_ERROR "You must specify the variable TOL_RUNTIME_DIR with the path to binary instalation of TOL. TOLTCL and TOLBASE should also be installed on that location.")
endif(NOT TOL_RUNTIME_DIR)

# => http://www.cmake.org/Wiki/CMake:CPackPackageGenerators

if(WIN32)
  # usando MinGW
  set(CPACK_PACKAGE_NAME "TOL (GNU)")
else(WIN32)
  set(CPACK_PACKAGE_NAME "TOL")
endif(WIN32)

set(CPACK_PACKAGE_VENDOR "Tol-Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Time Oriented Language")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

if(WIN32)
  # usando MinGW (ej. tol-gnu-v3.2)
  set(CPACK_PACKAGE_NAMEV "tol-gnu-v${CPACK_PACKAGE_VERSION}")
endif(WIN32)
if(UNIX)
  # (ej. tol_3.2)
  set(CPACK_PACKAGE_NAMEV "tol-${CPACK_PACKAGE_VERSION}")
endif(UNIX)

# System specific information: https://cmake.org/pipermail/cmake/attachments/20100722/04976a73/attachment-0001.obj
if(UNIX)
  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(SPECIFIC_SYSTEM_VERSION_NAME "${CMAKE_SYSTEM_NAME}")
    set(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "TGZ")
    if(EXISTS "/etc/issue")
      set(LINUX_NAME "")
      file(READ "/etc/issue" LINUX_ISSUE)
      message( STATUS "LINUX_ISSUE = ${LINUX_ISSUE}" )
      # CentOS case
      if(LINUX_ISSUE MATCHES "CentOS")
        string(REGEX MATCH "release ([0-9]+)" CENTOS "${LINUX_ISSUE}")
        set(LINUX_NAME "CentOS_${CMAKE_MATCH_1}")  
        set(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "RPM")      
      endif(LINUX_ISSUE MATCHES "CentOS")
      # Fedora case
      if(LINUX_ISSUE MATCHES "Fedora")
        string(REGEX MATCH "release ([0-9]+)" FEDORA "${LINUX_ISSUE}")
        set(LINUX_NAME "FC${CMAKE_MATCH_1}")  
        set(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "RPM")      
      endif(LINUX_ISSUE MATCHES "Fedora")
      # Ubuntu case
      if(LINUX_ISSUE MATCHES "Ubuntu")
        string(REGEX MATCH "buntu ([0-9]+\\.[0-9]+)" UBUNTU "${LINUX_ISSUE}")
        set(LINUX_NAME "Ubuntu_${CMAKE_MATCH_1}")        
        set(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "DEB")
      endif(LINUX_ISSUE MATCHES "Ubuntu")
      # Debian case
      if(LINUX_ISSUE MATCHES "Debian")
        string(REGEX MATCH "Debian .*ux ([a-zA-Z]*/?[a-zA-Z]*) .*" DEBIAN "${LINUX_ISSUE}")
        set(LINUX_NAME "Debian_${CMAKE_MATCH_1}")
        string(REPLACE "/" "_" LINUX_NAME ${LINUX_NAME}) 
        set(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "DEB")       
      endif(LINUX_ISSUE MATCHES "Debian")      
      # Open SuSE case
      if(LINUX_ISSUE MATCHES "SUSE")
        string(REGEX MATCH "SUSE  ([0-9]+\\.[0-9]+)" SUSE "${LINUX_ISSUE}")
        set(LINUX_NAME "openSUSE_${CMAKE_MATCH_1}")
        string(REPLACE "/" "_" LINUX_NAME ${LINUX_NAME})   
        set(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "RPM")     
      endif(LINUX_ISSUE MATCHES "SUSE")
      # Mandriva case
      # TODO      
      if(LINUX_NAME) 
         set(SPECIFIC_SYSTEM_VERSION_NAME "${CMAKE_SYSTEM_NAME}-${LINUX_NAME}")
      endif(LINUX_NAME)    
    endif(EXISTS "/etc/issue")      
  endif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(SPECIFIC_SYSTEM_VERSION_NAME "${SPECIFIC_SYSTEM_VERSION_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
  set(SPECIFIC_COMPILER_NAME "")
endif(UNIX)

# obligatorio para DEB
set(CPACK_PACKAGE_CONTACT "Pedro Gea <pgea@bayesforecast.com>")

if(UNIX)
  set(CPACK_DEBIAN_PACKAGE_SECTION "Mathematics")
endif(UNIX)

if( UNIX )
  get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
  if ("${LIB64}" STREQUAL "TRUE")
    set(LIBSUFFIX 64)
  else()
    set(LIBSUFFIX "")
  endif()

  EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
  message( STATUS "Architecture: ${ARCHITECTURE}" )
  message( STATUS "SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR = ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR}" )
  if( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR STREQUAL "RPM" )
    EXECUTE_PROCESS( 
      COMMAND rpmbuild --eval "%_arch" 
      OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE
      ERROR_VARIABLE IGNORE_ERROR
      OUTPUT_STRIP_TRAILING_WHITESPACE )

    EXECUTE_PROCESS( 
      COMMAND rpmbuild --eval "${CPACK_PACKAGE_VERSION_PATCH}%dist" 
      OUTPUT_VARIABLE CPACK_RPM_PACKAGE_RELEASE 
      ERROR_VARIABLE IGNORE_ERROR
      OUTPUT_STRIP_TRAILING_WHITESPACE )

    set( RPM_FILE_NAME "${CPACK_PACKAGE_NAMEV}-${CPACK_RPM_PACKAGE_RELEASE}.${CPACK_RPM_PACKAGE_ARCHITECTURE}" )
    message( STATUS "RPM_FILE_NAME = '${RPM_FILE_NAME}'" )
  endif()

  # RPM
  set( CPACK_RPM_PACKAGE_URL "www.tol-project.org" )
  set( CPACK_RPM_PACKAGE_RELOCATABLE ON )
  set( CPACK_RPM_PACKAGE_AUTOREQ " no" )
  #set( CPACK_RPM_PACKAGE_REQUIRES "suitesparse >= 3, boost-filesystem >= 1.41, boost-thread >= 1.41, fftw-devel >= 3")
  set( CPACK_RPM_PACKAGE_REQUIRES " boost-filesystem >= 1.41, boost-thread >= 1.41, suitesparse >= 3, fftw >= 3, gsl >= 1.13, bzip2-libs, zlib, libtool-ltdl, libcurl >= 7.19 " )
  # if the user wants to use the dbdrivers he must install
  # postgresql-libs >= 8.4, mysql-libs, unixODBC
  set( CPACK_RPM_PACKAGE_ARCHITECTURE ${ARCHITECTURE} )

  # DEB
  #set(CPACK_DEBIAN_PACKAGE_DEPENDS "liblapacke (>= 3.5.0-2), libblas3 (>= 1.2)")
  set( CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-filesystem-dev (>= 1.41), libboost-thread-dev (>= 1.41), libsuitesparse-dev, libfftw3-dev, libgsl0-dev (>= 1.13), bzip2, zlib1g-dev, libltdl-dev, libcurl3 (>=7.19)" )
# ya estaban libcurl3 y bzip2
endif( UNIX )

if(WIN32)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAMEV}")
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAMEV}")

  # => http://www.cmake.org/Wiki/CMake:Component_Install_With_CPack

  # BIN (APPLICATION)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.dll" dll_pattern)
  file(GLOB RT_DLL  ${dll_pattern})
  install(FILES ${RT_DLL} DESTINATION "bin" COMPONENT application)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.exe" exe_pattern)
  file(GLOB RT_EXE  ${exe_pattern})
  install(FILES ${RT_EXE} DESTINATION "bin" COMPONENT application)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/stdlib" stdlib_dir)
  install(DIRECTORY ${stdlib_dir} DESTINATION "bin" COMPONENT application 
    PATTERN ".svn" EXCLUDE)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/language.txt" language_txt)
  file(GLOB RT_TXT  ${language_txt})
  install(FILES ${RT_TXT} DESTINATION "bin" COMPONENT application)
  # BIN (DEVELOPMENT)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.lib" lib_pattern)
  file(GLOB RT_LIB  ${lib_pattern})
  install(FILES ${RT_LIB} DESTINATION "bin" COMPONENT development)
  # LIB (APPLICATION)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/lib" lib_dir)
  install(DIRECTORY ${lib_dir} DESTINATION "." COMPONENT application
    PATTERN "*.a" EXCLUDE PATTERN ".svn" EXCLUDE)
  # INCLUDE (DEVELOPMENT)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/include" include_dir)
  install(DIRECTORY ${include_dir} DESTINATION "." COMPONENT development)

  # Components
  set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")
  set(CPACK_COMPONENTS_ALL application development)
  set(CPACK_COMPONENT_APPLICATION_REQUIRED 1)
  set(CPACK_COMPONENT_DEVELOPMENT_DISABLED 1)
  set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "TOL Application")
  set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "C++ Headers & Libraries")
  set(CPACK_COMPONENT_APPLICATION_DESCRIPTION "Main components of TOL Application")
  set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "C/C++ header files and libraries for development purposes")
endif(WIN32)

if(UNIX)
  # BIN
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/tol*" exe_pattern)
  file(GLOB RT_EXE  ${exe_pattern} "${TOL_RUNTIME_DIR}/bin/confenv.sh" )
  set( PREFIX_TOL "lib${LIBSUFFIX}/tol${CPACK_PACKAGE_VERSION}" ) 
  install(FILES ${RT_EXE} DESTINATION "${PREFIX_TOL}/bin"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ 
                GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE
                WORLD_EXECUTE)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/stdlib" stdlib_dir)
  install(DIRECTORY ${stdlib_dir} DESTINATION "${PREFIX_TOL}/bin")
  # LIB
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/lib" lib_dir)
  install(DIRECTORY ${lib_dir} DESTINATION "${PREFIX_TOL}")
  # INCLUDE
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/include" include_dir)
  install(DIRECTORY ${include_dir} DESTINATION "${PREFIX_TOL}")
  # SCRIPTS (/usr/bin)
  ## !!
  ## file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/building/Linux_32/scripts/tol*" sh_pattern)
  ## file(GLOB RT_SH  ${sh_pattern})
  ## install(FILES ${RT_SH} DESTINATION "bin" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
endif(UNIX)

# => http://stackoverflow.com/questions/21144655/customizing-nsis-installer-using-cpack
configure_file(${CMAKE_SOURCE_DIR}/CPackOptions.cmake.in ${PROJECT_BINARY_DIR}/CPackOptions.cmake)

set(CPACK_PROPERTIES_FILE "${PROJECT_BINARY_DIR}/CPackOptions.cmake")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/tol/COPYING")
if(WIN32)
  string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})
endif(WIN32)

if(WIN32)
  # => http://www.mantidproject.org/NSIS_CPACK_Customisations
  # => http://nsis.sourceforge.net/Environmental_Variables:_append,_prepend,_and_remove_entries
  # => http://www.cmake.org/Wiki/CMake:CPackNSISAdvancedTips 

  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/NSIS/nsis_tol.bmp") 
  string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON}) 
  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/NSIS/tolbase-install.ico")
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/NSIS/tolbase-uninstall.ico")
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})
  set(CPACK_NSIS_HELP_LINK "www.tol-project.org")

  # nombre del paquete en el instalador
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
  # nombre del instalador:
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAMEV}-win32")

  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    DeleteRegValue HKCU \\\"Environment\\\" TOLGNU_ROOT
    WriteRegExpandStr HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" TOLGNU_ROOT \\\$INSTDIR
    SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
  ") 
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    DeleteRegValue HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" TOLGNU_ROOT
    SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
  ") 

  # añade el icono en el panel para desinstalar programas
  set(CPACK_NSIS_INSTALLED_ICON_NAME "${INSTDIR}/lib/TolbaseApp${CPACK_PACKAGE_VERSION}/toltk/images/tolbase-gnu.ico")
    string(REPLACE "/" "\\\\" CPACK_NSIS_INSTALLED_ICON_NAME ${CPACK_NSIS_INSTALLED_ICON_NAME})

  # pregunta tras instalar si abrir tolbase
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "tolbase.exe")

  # crea acceso directos a estos ejecutables (pares nombre_archivo nombre_acceso)
  set(CPACK_PACKAGE_EXECUTABLES tolbase "tolbase-gnu-v${CPACK_PACKAGE_VERSION}")
  # crea otros accesos directos
  set(CPACK_NSIS_MENU_LINKS "http://www.tol-project.org" "Tol-Project Site")
endif(WIN32)

if(UNIX)
  # nombre del paquete instalador
  if( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR STREQUAL "RPM" )
    configure_file( ${CMAKE_SOURCE_DIR}/cpack/rpm_post_install.sh.in ${CMAKE_CURRENT_BINARY_DIR}/rpm_post_install.sh )
    set( CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_BINARY_DIR}/rpm_post_install.sh" )
    
    set(CPACK_PACKAGE_FILE_NAME "${RPM_FILE_NAME}")
  else()
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAMEV}")
  endif()
  message( STATUS "CPACK_PACKAGE_FILE_NAME = ${CPACK_PACKAGE_FILE_NAME}" )
endif(UNIX)

include(CPack)
