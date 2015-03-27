create_header_links( )

set( basic_src atm.cpp bfsmem.cpp bobject.cpp cor.cpp dircomma.cpp dir.cpp
  dirtext.cpp dtealgeb.cpp dte.cpp list.cpp lstalgeb.cpp lst.cpp lsttext.cpp
  optargtx.cpp opt.cpp opttext.cpp out.cpp qfile.cpp seed.cpp sys.cpp timer.cpp
  tree.cpp txtalgeb.cpp txt.cpp classify.cpp )

if( WIN32 )
  set( basic_src ${basic_src} win_findexe.c )
else( WIN32 )
  set( basic_src ${basic_src} unix_findexe.c )
endif( )

add_tol_sublibrary( bbasic ${basic_src} )
