if( POLICY CMP0026 )
  cmake_policy( SET CMP0026 OLD )
endif( POLICY CMP0026 )

get_target_property( TOLTCL_LIB_LOCATION toltcl LOCATION )
get_filename_component( TOLTCL_LIB_NAME ${TOLTCL_LIB_LOCATION} NAME )
message( STATUS ${TOLTCL_LIB_NAME} )

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/pkgIndex.tcl.in"
  "${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl"
  )

install( FILES "${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl" toltcl.tcl tolsh.tcl
  COMPONENT RUNTIME DESTINATION "${TOLTCL_DEST_DIR}" )

