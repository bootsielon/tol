create_header_links( )

add_tol_sublibrary( matrix_type 
  matgra.cpp gibbssampler.cpp multimin.cpp tol_innerpoint.cpp )
target_include_directories( matrix_type PRIVATE 
  "${FFTW_INCLUDES}" "${PROJECT_SOURCE_DIR}/bmath/gsl_ext" 
  ${CMAKE_CURRENT_SOURCE_DIR} ${CBLAS_INCLUDE_DIR} ${CLAPACK_INCLUDE_DIR} )
