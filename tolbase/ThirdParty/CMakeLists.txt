include(ExternalProject)

ExternalProject_Add( tclodbc
  URL ${CMAKE_CURRENT_SOURCE_DIR}/tclodbc2.5_bayes.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tclodbc/configure CC=g++ --prefix=${CMAKE_CURRENT_BINARY_DIR} --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
)

ExternalProject_Add( tktable
  URL ${CMAKE_CURRENT_SOURCE_DIR}/Tktable2.10.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/src/tktable/configure --prefix=${CMAKE_CURRENT_BINARY_DIR} --exec-prefix=${CMAKE_CURRENT_BINARY_DIR}
)

file( GLOB lib_children
  ${CMAKE_CURRENT_BINARY_DIR}/lib/* )

#message( STATUS "children = ${children}" )

install( DIRECTORY ${lib_children} COMPONENT RUNTIME DESTINATION lib )