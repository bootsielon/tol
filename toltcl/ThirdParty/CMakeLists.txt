include(ExternalProject)

get_filename_component( TCL_LIB_DIR ${TCL_LIBRARY} DIRECTORY )
get_filename_component( TK_LIB_DIR ${TK_LIBRARY} DIRECTORY )

if( NOT TCLCURL_VERSION )
  set( TCLCURL_VERSION 7.22.0 )
endif( )

if( BUILD_TCLCURL )
  message( STATUS "Third party TclCurl${TCLCURL_VERSION} will be built." )
  set( tclcurl_tgz "TclCurl-${TCLCURL_VERSION}.tar.gz" )
  ExternalProject_Add( tclcurl
    URL ${CMAKE_CURRENT_SOURCE_DIR}/${tclcurl_tgz}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tclcurl/configure
    --prefix=${CMAKE_CURRENT_BINARY_DIR}
    --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
    --with-tcl=${TCL_LIB_DIR}
    BUILD_IN_SOURCE 1
    )
  install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/TclCurl${TCLCURL_VERSION} COMPONENT RUNTIME DESTINATION lib )
endif( BUILD_TCLCURL )

