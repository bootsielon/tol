create_header_links( )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_tol_sublibrary( cint tolCint.cpp  )
target_include_directories( cint PRIVATE 
  ${CINT_INCLUDE_DIR} ${LTDL_INCLUDE_DIR} )
