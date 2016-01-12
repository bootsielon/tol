
add_definitions( -DRTOL_VERSION=\"${RTOL_VERSION_MAJOR}.${RTOL_VERSION_MINOR}\")

include_directories( ${TOL_INCLUDE_DIR} )

message( STATUS "Using TOL_INCLUDE_DIR = ${TOL_INCLUDE_DIR}" )

# if( UNIX )
#   SET(CMAKE_INSTALL_RPATH "$ORIGIN")
# endif( UNIX )
add_library( VBTol SHARED datatype.cpp vbutils.cpp  vbtol.cpp vbtol.def )

set_target_properties( VBTol PROPERTIES PREFIX "")

target_link_libraries( VBTol ${TOL_LIBRARIES} )

# message( STATUS "RTOL_DEST_DIR = ${RTOL_DEST_DIR}" )
if( WIN32 )
  install( TARGETS VBTol RUNTIME DESTINATION "bin" )
else( WIN32 )
  # install( TARGETS tolRlink RUNTIME LIBRARY DESTINATION "lib" )
endif( WIN32 )

