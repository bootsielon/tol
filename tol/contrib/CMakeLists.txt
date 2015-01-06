foreach( dir
    kmlocal clusterlib alglib optimal_bw ANN cint )
  add_subdirectory( ${dir} )
  list( APPEND sublibs $<TARGET_OBJECTS:${dir}> )
endforeach( )

add_library( contrib ${sublibs} )
