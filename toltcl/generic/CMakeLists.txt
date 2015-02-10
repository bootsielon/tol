add_library( toltcl SHARED
  crash.c toltcl.c pool.cpp tclutils.cpp tolaccess.cpp tolsergrp.cpp 
  toltableset.cpp toltime.cpp
)

message( "aaaa = ${CMAKE_CURRENT_SOURCE_DIR}" )

include_directories( ${TOL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries( toltcl
  ${TCL_STUB_LIBRARY} ${TOL_LIBRARY} )

install( TARGETS toltcl DESTINATION "${TOLTCL_DEST_DIR}" )