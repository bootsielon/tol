
message( STATUS "CMAKE_SIZEOF_VOID_P = ${CMAKE_SIZEOF_VOID_P}")

add_definitions(
  -DTOLPLAT_SYSTEM_NAME=\"${CMAKE_SYSTEM_NAME}\"
  -DTOLPLAT_SYSTEM_VERSION=\"${CMAKE_SYSTEM_VERSION}\"
  -DTOLPLAT_SYSTEM_PROCESSOR=\"${CMAKE_SYSTEM_PROCESSOR}\"
  -DTOLPLAT_POINTER_SIZE=${CMAKE_SIZEOF_VOID_P}
  -DTOLPLAT_COMPILER_ID=\"${CMAKE_CXX_COMPILER_ID}\"
  )

add_tol_sublibrary( set_type
  kmeans_tolapi.cpp  set.cpp  setgra.cpp  setgrainc.cpp clustergra.cpp )
target_include_directories( set_type PRIVATE 
  "${PROJECT_SOURCE_DIR}/contrib/kmlocal" 
  "${PROJECT_SOURCE_DIR}/btol/real_type" )

create_header_links( set_type )
