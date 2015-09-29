foreach( dir
    kmlocal clusterlib alglib optimal_bw ANN tpp )
  add_subdirectory( ${dir} )
endforeach( )

if( CINT_HEADERS_FOUND )
  add_subdirectory( cint )
endif( )
