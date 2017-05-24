
add_tol_sublibrary( bgrammar
  graacc.cpp  grastk.cpp  opr.cpp       spfuninst.cpp  symboltable.cpp
  gra.cpp     met.cpp     nameblock.cpp oprimp.cpp     str.cpp
  graimp.cpp  spfun.cpp   syn.cpp       class.cpp )

target_include_directories( bgrammar PRIVATE 
  ${PROJECT_SOURCE_DIR} )
if( CINT_HEADERS_FOUND )
  target_include_directories( bgrammar PRIVATE 
    ${CINT_INCLUDE_DIR} )
endif( )

create_header_links( bgrammar )
