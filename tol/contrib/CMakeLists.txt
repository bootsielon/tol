foreach( dir
    kmlocal clusterlib alglib optimal_bw ANN )
  add_subdirectory( ${dir} )
endforeach( )

if( CINT_HEADERS_FOUND )
  add_subdirectory( cint )
endif( )
