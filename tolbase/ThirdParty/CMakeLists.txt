include(ExternalProject)

get_filename_component( TCL_LIB_DIR ${TCL_LIBRARY} DIRECTORY )
get_filename_component( TK_LIB_DIR ${TK_LIBRARY} DIRECTORY )

find_library( ODBC_LIBRARY 
  NAMES iodbc unixodbc odbc
)

if( ODBC_LIBRARY )
  message( STATUS "ODBC_LIBRARY = ${ODBC_LIBRARY}" )
else( ODBC_LIBRARY )
  message( ERROR "Could not find ODBC library" )
endif( ODBC_LIBRARY )

get_filename_component( ODBC_LIB_DIR ${ODBC_LIBRARY} DIRECTORY )

ExternalProject_Add( tclodbc
  URL ${CMAKE_CURRENT_SOURCE_DIR}/tclodbc2.5_bayes.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tclodbc/configure 
    CC=g++ --prefix=${CMAKE_CURRENT_BINARY_DIR}
    --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
    --with-tcl=${TCL_LIB_DIR}
    --with-odbclibrary=${ODBC_LIB_DIR}
)

ExternalProject_Add( tktable
  URL ${CMAKE_CURRENT_SOURCE_DIR}/Tktable2.10.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tktable/configure
    --prefix=${CMAKE_CURRENT_BINARY_DIR}
    --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
    --with-tcl=${TCL_LIB_DIR}
    --with-tk=${TK_LIB_DIR}
)

file( GLOB lib_children
  ${CMAKE_CURRENT_BINARY_DIR}/lib/* )

#message( STATUS "children = ${children}" )

install( DIRECTORY ${lib_children} COMPONENT RUNTIME DESTINATION lib )
