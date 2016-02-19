include(ExternalProject)

get_filename_component( TCL_LIB_DIR ${TCL_LIBRARY} DIRECTORY )
get_filename_component( TK_LIB_DIR ${TK_LIBRARY} DIRECTORY )

if( NOT TCLODBC_VERSION )
  set( TCLODBC_VERSION 2.5 )
endif( )

if( BUILD_TCLODBC )
  message( STATUS "Third party tclodbc${TCLODBC_VERSION} will be built." )
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
    URL ${CMAKE_CURRENT_SOURCE_DIR}/tclodbc${TCLODBC_VERSION}_bayes.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tclodbc/configure 
    CC=g++ --prefix=${CMAKE_CURRENT_BINARY_DIR}
    --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
    --with-tcl=${TCL_LIB_DIR}
    --with-odbclibrary=${ODBC_LIB_DIR}
    )
  install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/tclodbc${TCLODBC_VERSION} COMPONENT RUNTIME DESTINATION lib )    
endif( BUILD_TCLODBC )

if( NOT TKTABLE_VERSION )
  set( TKTABLE_VERSION 2.10 )
endif( )

if( BUILD_TKTABLE )
  message( STATUS "Third party Tktable${TKTABLE_VERSION} will be built." )
  ExternalProject_Add( tktable
    URL ${CMAKE_CURRENT_SOURCE_DIR}/Tktable${TKTABLE_VERSION}.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tktable/configure
    --prefix=${CMAKE_CURRENT_BINARY_DIR}
    --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
    --with-tcl=${TCL_LIB_DIR}
    --with-tk=${TK_LIB_DIR}
    )
  install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/Tktable${TKTABLE_VERSION} COMPONENT RUNTIME DESTINATION lib )
endif( BUILD_TKTABLE )
