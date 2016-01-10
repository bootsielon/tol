include(ExternalProject)

get_filename_component( TCL_LIB_DIR ${TCL_LIBRARY} DIRECTORY )
get_filename_component( TK_LIB_DIR ${TK_LIBRARY} DIRECTORY )

if( BUILD_TCLCURL )
  if( TCLCURL_VERSION )
    set( tclcurl_tgz "TclCurl-${TCLCURL_VERSION}.tar.gz" )
  else( TCLCURL_VERSION )
    set( tclcurl_tgz "TclCurl-7.22.0.tar.gz" )
  endif( TCLCURL_VERSION )
  ExternalProject_Add( tclcurl
    URL ${CMAKE_CURRENT_SOURCE_DIR}/${tclcurl_tgz}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tclcurl/configure
    --prefix=${CMAKE_CURRENT_BINARY_DIR}
    --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
    --with-tcl=${TCL_LIB_DIR}
    BUILD_IN_SOURCE 1
    )
endif( BUILD_TCLCURL )

if( BUILD_TCLCURL )
  file( GLOB lib_children
    ${CMAKE_CURRENT_BINARY_DIR}/lib/* )
  
  #message( STATUS "lib_children = ${lib_children}" )
  
  install( DIRECTORY ${lib_children} COMPONENT RUNTIME DESTINATION lib )
endif( BUILD_TCLCURL )
