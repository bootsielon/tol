cmake_minimum_required( VERSION 2.8 )
project( TOL-Installer )

if( NOT TOL_RUNTIME_DIR )
  messsage( FATAL_ERROR "You must specify the variable TOL_RUNTIME_DIR with the path to binary instalation of TOL. TOLTCL and TOLBASE should also be installed on that location." )
endif(  NOT TOL_RUNTIME_DIR )

if( WIN32 )
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.dll" dll_pattern )
  file( GLOB RT_DLL  ${dll_pattern} )
  install( FILES ${RT_DLL} DESTINATION "bin" )
  file( TO_CMAKE_PATH "${TOL_RUNTIME_DIR}/bin/*.exe" exe_pattern )
  file( GLOB RT_EXE  ${dll_pattern} )
  install( FILES ${RT_EXE} DESTINATION "bin" )
  install( DIRECTORY lib DESTINATION "." PATTERN "*.a" EXCLUDE )
endif( WIN32 )

set(CPACK_PACKAGE_NAME "TOL")
set(CPACK_PACKAGE_VENDOR "www.tol-project.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Time Oriented Language")
set(CPACK_PACKAGE_VERSION "3.2.0")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "tolbase")

# http://www.mantidproject.org/NSIS_CPACK_Customisations
# http://nsis.sourceforge.net/Environmental_Variables:_append,_prepend,_and_remove_entries
set (CPACK_NSIS_EXTRA_INSTALL_COMMANDS "Push \\\"TOLGNU_ROOT\\\"
   Push \\\"A\\\"
   Push \\\"HKCU\\\"
   Push \\\"$INSTDIR\\\"
   Call EnvVarUpdate
   Pop  \\\$0" )

set (CPACK_NSIS_EXTRA_INSTALL_COMMANDS "Push \\\"TOLGNU_ROOT\\\"
   Push \\\"R\\\"
   Push \\\"HKCU\\\"
   Push \\\"$INSTDIR\\\"
   Call un.EnvVarUpdate
   Pop  \\\$0" )

include(CPack)