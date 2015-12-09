if( POLICY CMP0026 )
  cmake_policy( SET CMP0026 OLD )
endif( POLICY CMP0026 )

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/pkgIndex.tcl.in"
  "${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl"
  )

install( FILES 
  "${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl" 
  COMPONENT RUNTIME DESTINATION "${TOLBASE_DEST_DIR}" )

install( DIRECTORY 
  autoscroll      markupparser  nbdbmanager      renderpane   wtree
  byswidget       markupviewer  notebookdb       rmtps_client
  mimetex         toltk         mkWidgets1.3     trycatch
  COMPONENT RUNTIME DESTINATION ${TOLBASE_DEST_DIR} 
  PATTERN .svn EXCLUDE )
