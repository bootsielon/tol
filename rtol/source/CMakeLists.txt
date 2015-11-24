
add_definitions( -DRTOL_VERSION=\"${RTOL_VERSION_MAJOR}.${RTOL_VERSION_MINOR}\")

include_directories( ${TOL_INCLUDE_DIR} )
include_directories( ${RINSIDE_INCLUDE_DIRS} )

message( STATUS "Using TOL_INCLUDE_DIR = ${TOL_INCLUDE_DIR}" )
message( STATUS "Using RINSIDE_INCLUDE_DIRS = ${RINSIDE_INCLUDE_DIRS}" )

add_library( tolRlink SHARED tolRlink.cpp )

set_target_properties( tolRlink PROPERTIES PREFIX "")
target_link_libraries( tolRlink ${TOL_LIBRARIES} ${RINSIDE_LIBRARIES} )

# message( STATUS "RTOL_DEST_DIR = ${RTOL_DEST_DIR}" )
if( WIN32 )
  install( TARGETS tolRlink RUNTIME DESTINATION "bin" )
else( WIN32 )
  install( TARGETS tolRlink RUNTIME LIBRARY DESTINATION "lib" )
endif( WIN32 )

