cmake_minimum_required(VERSION 2.8)
project(TOL-Installer)

if(LIB64)
  message( STATUS "LIB64" )
endif()

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
endif(WIN32)
if(UNIX)
  set(CPACK_PACKAGE_NAME "TOL")
endif(UNIX)

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

# obligatorio para DEB
set(CPACK_PACKAGE_CONTACT "Pedro Gea <pgea@bayesforecast.com>")

if(UNIX)
  set(CPACK_DEBIAN_PACKAGE_SECTION "Mathematics")
endif(UNIX)

if( UNIX )
  EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
  message( STATUS "Architecture: ${ARCHITECTURE}" )
  set( CPACK_RPM_PACKAGE_AUTOREQ " no" )
  #set( CPACK_RPM_PACKAGE_REQUIRES "suitesparse >= 3, boost-filesystem >= 1.41, boost-thread >= 1.41, fftw-devel >= 3")
  set( CPACK_RPM_PACKAGE_REQUIRES " boost-filesystem >= 1.41, boost-thread >= 1.41, suitesparse >= 3, fftw >= 3, gsl >= 1.13, bzip2-libs, zlib, libtool-ltdl, libcurl >= 7.19 " )
  set( CPACK_RPM_PACKAGE_ARCHITECTURE ${ARCHITECTURE} )
endif( UNIX )

#set( CPACK_RPM_PACKAGE_ARCHITECTURE 

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
  file(GLOB RT_EXE  ${exe_pattern})
  install(FILES ${RT_EXE} DESTINATION "lib/tol${CPACK_PACKAGE_VERSION}/bin" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/stdlib" stdlib_dir)
  install(DIRECTORY ${stdlib_dir} DESTINATION "lib/tol${CPACK_PACKAGE_VERSION}/bin")
  # LIB
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/lib" lib_dir)
  install(DIRECTORY ${lib_dir} DESTINATION "lib/tol${CPACK_PACKAGE_VERSION}")
  # INCLUDE
  file(TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/include" include_dir)
  install(DIRECTORY ${include_dir} DESTINATION "lib/tol${CPACK_PACKAGE_VERSION}")
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
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAMEV}")
endif(UNIX)

include(CPack)
