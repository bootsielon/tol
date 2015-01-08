create_header_links( )

add_tol_sublibrary( PackArchive PackArchive.cpp StoreZipArchive.cpp )
target_include_directories( PackArchive PRIVATE ${ZIPARCHIVE_INCLUDE_DIR} )
 