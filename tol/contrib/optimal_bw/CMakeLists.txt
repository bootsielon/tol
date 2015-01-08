add_tol_sublibrary( optimal_bw 
  api_tol_optimal_bw.cpp  FastUnivariateDensityDerivative.cpp )
target_include_directories( optimal_bw PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR} )