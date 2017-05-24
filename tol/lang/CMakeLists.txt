
add_tol_sublibrary( lang language.cpp exportfu.cpp )
target_include_directories( lang PRIVATE ${ZIPARCHIVE_INCLUDE_DIR} )

create_header_links( lang )
