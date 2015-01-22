create_header_links( )

add_tol_sublibrary( bprdist multvar.cpp  prd.cpp slice_sampler.cpp )
target_include_directories( bprdist PRIVATE 
  "${PROJECT_SOURCE_DIR}/contrib"
  "${PROJECT_SOURCE_DIR}/bmath/gsl_ext" 
  ${CBLAS_INCLUDE_DIR} ${CLAPACK_INCLUDE_DIR} )
  