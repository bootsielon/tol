create_header_links( )

add_tol_sublibrary( set_type
  kmeans_tolapi.cpp  set.cpp  setgra.cpp  setgrainc.cpp clustergra.cpp )
target_include_directories( set_type PRIVATE 
  "${PROJECT_SOURCE_DIR}/contrib/kmlocal" 
  "${PROJECT_SOURCE_DIR}/btol/real_type" )
