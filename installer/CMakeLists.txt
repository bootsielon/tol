# => http://www.cmake.org/Wiki/CMake:CPackPackageGenerators

cmake_minimum_required(VERSION 3.0)
project(TOL-Installer)

if(NOT TOL_RUNTIME_DIR)
  message(FATAL_ERROR "You must specify the variable TOL_RUNTIME_DIR with the path to binary instalation of TOL. TOLTCL and TOLBASE should also be installed on that location.")
endif(NOT TOL_RUNTIME_DIR)

set(CPACK_PACKAGE_NAME "TOL")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "4")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Time Oriented Language")
set(CPACK_PACKAGE_VENDOR "Tol-Project")
set(CPACK_PACKAGE_CONTACT "Pedro Gea <pgea@bayesforecast.com>")

if(WIN32)
  add_subdirectory(windows)
else(WIN32)
  add_subdirectory(linux)
  #! Sacar el include del IF después de incluir los PARENT_SCOPE en Windows
  #! y comprobar que todo funciona adecuadamente.
  include(CPack)
endif(WIN32)
