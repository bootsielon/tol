create_header_links( )

add_tol_sublibrary( OIS 
  oisapitol.cpp    oiscreator.cpp      oisloader.cpp      oisstream_zip.cpp
  oiscompress.cpp  oisindex.cpp        oisstream.cpp      oisxml.cpp
  ois.cpp          oisinteractive.cpp  oisstream_dir.cpp )
target_include_directories( OIS PRIVATE 
  ${ZIPARCHIVE_INCLUDE_DIR} ${PROJECT_SOURCE_DIR} )
