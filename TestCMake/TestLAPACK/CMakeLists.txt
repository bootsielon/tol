cmake_minimum_required(VERSION 2.8)
project( TestDownload )

set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/../../tol/cmake/modules"
  ${CMAKE_MODULE_PATH} )

find_package( CLAPACK REQUIRED )

if( CLAPACK_FOUND )
  message("CLAPACK_INCLUDE_DIR = ${CLAPACK_INCLUDE_DIR}")
  message("CLAPACK_LIBRARY = ${CLAPACK_LIBRARY}")
  message("CLAPACK_LIBRARIES = ${CLAPACK_LIBRARIES}")
endif( )
