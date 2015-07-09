cmake_minimum_required( VERSION 2.8 )
project( TOL-Installer )

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/NSIS" ${CMAKE_MODULE_PATH})

if( NOT TOL_RUNTIME_DIR )
  message( FATAL_ERROR "You must specify the variable TOL_RUNTIME_DIR with the path to binary instalation of TOL. TOLTCL and TOLBASE should also be installed on that location." )
endif( NOT TOL_RUNTIME_DIR )

# => http://www.cmake.org/Wiki/CMake:Component_Install_With_CPack

if( WIN32 )
  # BIN (APPLICATION)
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.dll" dll_pattern )
  file( GLOB RT_DLL  ${dll_pattern} )
  install( FILES ${RT_DLL} DESTINATION "bin" COMPONENT application)
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.exe" exe_pattern )
  file( GLOB RT_EXE  ${exe_pattern} )
  install( FILES ${RT_EXE} DESTINATION "bin" COMPONENT application)
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/stdlib" stdlib_dir )
  install( DIRECTORY ${stdlib_dir} DESTINATION "bin" COMPONENT application 
    PATTERN ".svn" EXCLUDE )
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/language.txt" language_txt )
  file( GLOB RT_TXT  ${language_txt} )
  install( FILES ${RT_TXT} DESTINATION "bin" COMPONENT application)
  # BIN (DEVELOPMENT)
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.lib" lib_pattern )
  file( GLOB RT_LIB  ${lib_pattern} )
  install( FILES ${RT_LIB} DESTINATION "bin" COMPONENT development)
  # LIB (APPLICATION)
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/lib" lib_dir )
  install( DIRECTORY ${lib_dir} DESTINATION "." COMPONENT application
    PATTERN "*.a" EXCLUDE PATTERN ".svn" EXCLUDE )
  # INCLUDE (DEVELOPMENT)
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/include" include_dir )
  install( DIRECTORY ${include_dir} DESTINATION "." COMPONENT development)

  set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")
  set(CPACK_COMPONENTS_ALL application development)

  set(CPACK_COMPONENT_APPLICATION_REQUIRED 1)
  set(CPACK_COMPONENT_DEVELOPMENT_DISABLED 1)

  set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "TOL-GNU Application")
  set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "C++ Headers & Libraries")

  set(CPACK_COMPONENT_APPLICATION_DESCRIPTION 
    "Main components of TOL-GNU Application")
  set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION
    "C/C++ header files and libraries for development purposes")
endif( WIN32 )

configure_file( ${CMAKE_SOURCE_DIR}/CPackOptions.cmake.in ${PROJECT_BINARY_DIR}/CPackOptions.cmake )

# => http://stackoverflow.com/questions/21144655/customizing-nsis-installer-using-cpack
# => http://www.cmake.org/Wiki/CMake:CPackPackageGenerators

set(CPACK_PACKAGE_NAME "TOL-GNU")
set(CPACK_PACKAGE_VENDOR "Tol-Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Time Oriented Language")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

set(CPACK_PROPERTIES_FILE "${PROJECT_BINARY_DIR}/CPackOptions.cmake")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/tol/COPYING" )
  string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/NSIS/tolbase-install.ico" )
  string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/NSIS/tolbase-uninstall.ico" )
  string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})

set(CPACK_NSIS_HELP_LINK "www.tol-project.org")

# => http://www.mantidproject.org/NSIS_CPACK_Customisations
# => http://nsis.sourceforge.net/Environmental_Variables:_append,_prepend,_and_remove_entries

# => http://www.cmake.org/Wiki/CMake:CPackNSISAdvancedTips

if( WIN32 )
  # nombre del paquete en el instalador
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
  # nombre del instalador:
  set(CPACK_PACKAGE_FILE_NAME "tol-gnu-${CPACK_PACKAGE_VERSION}-win32")

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
  set(CPACK_NSIS_INSTALLED_ICON_NAME "${INSTDIR}/lib/TolbaseApp3.2/toltk/images/tolbase-gnu.ico")
    string(REPLACE "/" "\\\\" CPACK_NSIS_INSTALLED_ICON_NAME ${CPACK_NSIS_INSTALLED_ICON_NAME})

  # pregunta tras instalar si abrir tolbase
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "tolbase.exe")

  # crea acceso directos a estos ejecutables (pares nombre_archivo nombre_acceso)
  set(CPACK_PACKAGE_EXECUTABLES tolbase "tolbase-gnu-v3.2")
  # crea otros accesos directos
  set(CPACK_NSIS_MENU_LINKS "http://www.tol-project.org" "Tol-Project Site")
endif( WIN32 )

include(CPack)
