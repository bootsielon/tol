create_header_links( )

foreach( d 
    barith bfunct blinalg bprdist bstat dcdflib gsl_ext mathobjects fftw )
  add_subdirectory( ${d} )
  list( APPEND sublibs $<TARGET_OBJECTS:${d}> )
endforeach( )

add_library( bmath matfun.cpp ${sublibs} )