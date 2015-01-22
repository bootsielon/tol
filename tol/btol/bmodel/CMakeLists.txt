create_header_links( )

add_tol_sublibrary( bmodel
  aia.cpp    maia.cpp foread.cpp   moddiag.cpp  modprev.cpp
  estim.cpp  modcalc.cpp  model.cpp    sparselinreg.cpp )
target_include_directories( bmodel PRIVATE 
  ${CBLAS_INCLUDE_DIR} ${CLAPACK_INCLUDE_DIR} )