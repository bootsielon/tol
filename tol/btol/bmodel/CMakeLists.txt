
add_tol_sublibrary( bmodel
  aia.cpp    maia.cpp foread.cpp   moddiag.cpp  modprev.cpp
  estim.cpp  modcalc.cpp  model.cpp    sparselinreg.cpp )
target_include_directories( bmodel PRIVATE 
  ${TOL_CBLAS_INCLUDE_DIR} ${CLAPACK_INCLUDE_DIR} ${CHOLMOD_INCLUDE_DIR} )

create_header_links( bmodel )

