cmake_minimum_required( VERSION 2.8 )
project( TOL-Installer )

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/NSIS" ${CMAKE_MODULE_PATH})

if( NOT TOL_RUNTIME_DIR )
  message( FATAL_ERROR "You must specify the variable TOL_RUNTIME_DIR with the path to binary instalation of TOL. TOLTCL and TOLBASE should also be installed on that location." )
endif( NOT TOL_RUNTIME_DIR )

if( WIN32 )
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.dll" dll_pattern )
  file( GLOB RT_DLL  ${dll_pattern} )
  install( FILES ${RT_DLL} DESTINATION "bin" )
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.exe" exe_pattern )
  file( GLOB RT_EXE  ${exe_pattern} )
  install( FILES ${RT_EXE} DESTINATION "bin" )
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/lib" lib_dir )
  install( DIRECTORY ${lib_dir} DESTINATION "." PATTERN "*.a" EXCLUDE )
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/stdlib" stdlib_dir )
  install( DIRECTORY ${stdlib_dir} DESTINATION "bin" )
endif( WIN32 )

configure_file( ${CMAKE_SOURCE_DIR}/CPackOptions.cmake.in ${PROJECT_BINARY_DIR}/CPackOptions.cmake )

# http://stackoverflow.com/questions/21144655/customizing-nsis-installer-using-cpack
# ...

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
#set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\NSIS\\\\splash-gnu.bmp")
#string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/tol/COPYING" )
  string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/NSIS/tolbase-install.ico" )
  string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/NSIS/tolbase-uninstall.ico" )
  string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})

set(CPACK_NSIS_HELP_LINK "www.tol-project.org")

# http://www.mantidproject.org/NSIS_CPACK_Customisations
# http://nsis.sourceforge.net/Environmental_Variables:_append,_prepend,_and_remove_entries

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
