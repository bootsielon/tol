create_header_links( )

add_tol_sublibrary( real_type
  arms.cpp      datgralg.cpp  datgrast.cpp  datgsrst.cpp
  armseval.cpp  datgra.cpp    datgrapr.cpp  datgrav.cpp   llkarma.cpp )
target_include_directories( real_type PRIVATE 
  "${PROJECT_SOURCE_DIR}/bmath/gsl_ext" )
