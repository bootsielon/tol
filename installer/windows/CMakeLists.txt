
# la variable SOURCE_DIR apunta al directorio principal del código (trunk)
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# directorio con archivos adicionales para el instalador
set(NSIS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/NSIS")
# módulo adicional para construir el instalados con NSIS
set(CMAKE_MODULE_PATH "${NSIS_DIR}" ${CMAKE_MODULE_PATH})

# => http://www.cmake.org/Wiki/CMake:CPackPackageGenerators

if(${TOL_PLATFORM} EQUAL "64")
  message(STATUS "Installer for TOL 64-bits")
  # la version de 64 bits se instala en el "program files" de 64
  # el instalador es sin embargo un ejecutable de 32 bits
  set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
  set(win_system_name win64)
  set(TOL_ENV_VAR TOLGNU_ROOT_64)
  set(CPACK_PACKAGE_NAME "TOL (GNU-64)")
else()
  message(STATUS "Installer for TOL 32-bits")
  set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES32")
  set(win_system_name win32)
  set(TOL_ENV_VAR TOLGNU_ROOT)
  set(CPACK_PACKAGE_NAME "TOL (GNU)")
endif()

if(INCLUDE_TOLBASE)
  message(STATUS "Installer includes tolbase")
  set(CPACK_PACKAGE_NAMEV_INSTALL "tol-gnu-v${CPACK_PACKAGE_VERSION}")
  set(CPACK_PACKAGE_NAMEV "tol-gnu-v${CPACK_PACKAGE_VERSION}")
else()
  message(STATUS "Installer includes only tolsh")
  set(CPACK_PACKAGE_NAMEV_INSTALL "tol-gnu-v${CPACK_PACKAGE_VERSION}")  
  # se utiliza un nombre diferente para el instalador sin tolbase
  set(CPACK_PACKAGE_NAMEV "tolsh-gnu-v${CPACK_PACKAGE_VERSION}")
endif()

#! Debería comprobarse que fueron compilados e instalados en el Runtime_Base
#! los proyectos tol, toltcl y rtol, y también tolbase si corresponde.

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAMEV_INSTALL}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAMEV_INSTALL}")

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

# => http://stackoverflow.com/questions/21144655/customizing-nsis-installer-using-cpack
configure_file(${SOURCE_DIR}/CPackOptions.cmake.in ${PROJECT_BINARY_DIR}/CPackOptions.cmake)

set(CPACK_PROPERTIES_FILE "${PROJECT_BINARY_DIR}/CPackOptions.cmake")

set(CPACK_RESOURCE_FILE_LICENSE "${SOURCE_DIR}/tol/COPYING")
string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})

# => http://www.mantidproject.org/NSIS_CPACK_Customisations
# => http://nsis.sourceforge.net/Environmental_Variables:_append,_prepend,_and_remove_entries
# => http://www.cmake.org/Wiki/CMake:CPackNSISAdvancedTips 

set(CPACK_PACKAGE_ICON "${NSIS_DIR}/nsis_tol.bmp") 
string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
set(CPACK_NSIS_MUI_ICON "${NSIS_DIR}/tolbase-install.ico")
string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
set(CPACK_NSIS_MUI_UNIICON "${NSIS_DIR}/tolbase-uninstall.ico")
string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})
set(CPACK_NSIS_HELP_LINK "www.tol-project.org")

# nombre del paquete en el instalador
set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
# nombre del instalador:
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAMEV}-${win_system_name}")
message(STATUS "Instaler Name: ${CPACK_PACKAGE_FILE_NAME}")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  DeleteRegValue HKCU \\\"Environment\\\" ${TOL_ENV_VAR}
  WriteRegExpandStr HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" ${TOL_ENV_VAR} \\\$INSTDIR
  SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
") 
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  DeleteRegValue HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" ${TOL_ENV_VAR}
  SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
") 

if(INCLUDE_TOLBASE) 
  # añade el icono en el panel para desinstalar programas
  set(CPACK_NSIS_INSTALLED_ICON_NAME "${INSTDIR}/lib/TolbaseApp${CPACK_PACKAGE_VERSION}/toltk/images/tolbase-gnu.ico")
    string(REPLACE "/" "\\\\" CPACK_NSIS_INSTALLED_ICON_NAME ${CPACK_NSIS_INSTALLED_ICON_NAME})
  
  # pregunta tras instalar si abrir tolbase
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "tolbase.exe")
  
  # crea acceso directos a estos ejecutables (pares nombre_archivo nombre_acceso)
  set(CPACK_PACKAGE_EXECUTABLES tolbase "tolbase-gnu-v${CPACK_PACKAGE_VERSION}")
endif()

# crea otros accesos directos
set(CPACK_NSIS_MENU_LINKS "http://www.tol-project.org" "Tol-Project Site")

# include(CPack) ha de hacerse en este mismo archivo para que funcione adecuadamente
include(CPack)
