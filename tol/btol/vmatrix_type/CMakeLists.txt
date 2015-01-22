create_header_links( )

add_tol_sublibrary( vmatrix_type
  vmat.cpp vmat_convert.cpp vmat_stats.cpp vmat_arith.cpp vmat_logic.cpp
  vmat_chol.cpp vmat_subtypes.cpp vmat_io.cpp 
  vmat_bsr_struct.cpp vmat_bsr.cpp 
  vmat_bsr_primary.cpp vmat_bsr_joint.cpp vmat_bsr_master.cpp
  vmat_iterative.cpp vmat_iterator.cpp vmatgra.cpp mmio.c )
target_include_directories( vmatrix_type PRIVATE
  "${CHOLMOD_INCLUDE_DIR}"
  "${PROJECT_SOURCE_DIR}/bmath/gsl_ext" 
  "${CBLAS_INCLUDE_DIR}" )
