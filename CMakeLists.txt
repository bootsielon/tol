cmake_minimum_required( VERSION 2.8 )
project( TOL-Installer )

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/NSIS" ${CMAKE_MODULE_PATH})

if( NOT TOL_RUNTIME_DIR )
  message( FATAL_ERROR "You must specify the variable TOL_RUNTIME_DIR with the path to binary instalation of TOL. TOLTCL and TOLBASE should also be installed on that location." )
endif(  NOT TOL_RUNTIME_DIR )

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
  file( TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/tolbase-gnu.ico" ico_tolbase )
  install( FILES ${ico_tolbase} DESTINATION "bin" )
endif( WIN32 )

configure_file( ${CMAKE_SOURCE_DIR}/CPackOptions.cmake.in ${PROJECT_BINARY_DIR}/CPackOptions.cmake)

# http://stackoverflow.com/questions/21144655/customizing-nsis-installer-using-cpack
# ...

set(CPACK_PACKAGE_NAME "TOL")
set(CPACK_PACKAGE_VENDOR "www.tol-project.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Time Oriented Language")
set(CPACK_PACKAGE_VERSION "3.2.0")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "tolbase")
set(CPACK_PROPERTIES_FILE "${PROJECT_BINARY_DIR}/CPackOptions.cmake")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/tolbase/lib/toltk/images/splash-gnu.gif")
string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/tol/COPYING" )
string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/tolbase/lib/toltk/images/tolbase-gnu.ico" )
string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/tolbase/lib/toltk/images/tolbase-gnu.ico" )
string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})

# http://www.mantidproject.org/NSIS_CPACK_Customisations
# http://nsis.sourceforge.net/Environmental_Variables:_append,_prepend,_and_remove_entries

if( WIN32 )
  set (CPACK_NSIS_EXTRA_INSTALL_COMMANDS 
    "
    Push \\\"TOLGNU_ROOT\\\"
    Push \\\"A\\\"
    Push \\\"HKCU\\\"
    Push \\\"$INSTDIR\\\"
    Call EnvVarUpdate
    Pop  \\\$0" )  
  set (CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Push \\\"TOLGNU_ROOT\\\"
    Push \\\"R\\\"
    Push \\\"HKCU\\\"
    Push \\\"$INSTDIR\\\"
    Call un.EnvVarUpdate
    Pop  \\\$0" )
  set (ICONPATH "$INSTDIR/bin/tolbase-gnu.ico")
	string(REPLACE "/" "\\\\" ICONPATH ${ICONPATH})
	set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut	'$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\tolbase.lnk'	'$INSTDIR\\\\bin\\\\tolbase.exe' ''	'${ICONPATH}'" )
	set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete	'$SMPROGRAMS\\\\$MUI_TEMP\\\\tolbase.lnk'" )

endif( WIN32 )
include(CPack)
