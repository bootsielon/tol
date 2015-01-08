create_header_links( )

add_tol_sublibrary( bgrammar
  graacc.cpp  grastk.cpp  opr.cpp       spfuninst.cpp  symboltable.cpp
  gra.cpp     met.cpp     nameblock.cpp oprimp.cpp     str.cpp
  graimp.cpp  spfun.cpp   syn.cpp       class.cpp )

target_include_directories( bgrammar PRIVATE 
  ${PROJECT_SOURCE_DIR}
  ${CINT_INCLUDE_DIR} )
