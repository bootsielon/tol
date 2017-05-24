
add_tol_sublibrary( blinalg
  divconq.cpp  matimp.cpp svdsep.cpp tol_lapack.cpp
  linalg.cpp mfastpro.cpp tol_blas.cpp )
  
target_include_directories( blinalg PRIVATE 
  ${TOL_CBLAS_INCLUDE_DIR} ${CLAPACK_INCLUDE_DIR} )

create_header_links( blinalg )
