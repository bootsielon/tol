include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_tol_sublibrary( clusterlib cluster.c  com.c  linpack.c  ranlib.c )
