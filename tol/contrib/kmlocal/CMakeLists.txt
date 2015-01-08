include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_tol_sublibrary( kmlocal
  KCtree.cpp  KM_ANN.cpp     KMdata.cpp  KMfilterCenters.cpp  KMrand.cpp
  KCutil.cpp  KMcenters.cpp  KMeans.cpp  KMlocal.cpp          KMterm.cpp )
