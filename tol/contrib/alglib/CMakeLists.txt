include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_tol_sublibrary( alglib 
  alglib_interp.cpp  ap.cpp  bdsvd.cpp  bidiagonal.cpp  blas.cpp
  leastsquares.cpp  levenbergmarquardt.cpp  lq.cpp  polinterpolation.cpp
  qr.cpp  ratinterpolation.cpp  reflections.cpp  rotations.cpp  spline3.cpp
  svd.cpp alglib_statistics.cpp mannwhitneyu.cpp normaldistr.cpp
  binomialdistr.cpp chisquaredistr.cpp correlation.cpp correlationtests.cpp
  fdistr.cpp gammaf.cpp ibetaf.cpp igammaf.cpp jarquebera.cpp 
  nearunityunit.cpp spline2d.cpp stest.cpp studenttdistr.cpp
  studentttests.cpp variancetests.cpp wsr.cpp )
