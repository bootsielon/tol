include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library( optimal_bw OBJECT
  api_tol_optimal_bw.cpp  FastUnivariateDensityDerivative.cpp )
