create_header_links( )

add_tol_sublibrary( bstat 
  ar.cpp  arma.cpp  linreg.cpp  logit.cpp  stat.cpp  test.cpp unitroot.cpp )
target_include_directories( bstat PRIVATE 
  ${TOL_CBLAS_INCLUDE_DIR} ${CLAPACK_INCLUDE_DIR} )
  