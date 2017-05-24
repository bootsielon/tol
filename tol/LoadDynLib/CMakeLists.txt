
add_tol_sublibrary( LoadDynLib LoadDynLib.cpp )
target_include_directories( LoadDynLib PRIVATE ${LTDL_INCLUDE_DIR} )

create_header_links( LoadDynLib )
