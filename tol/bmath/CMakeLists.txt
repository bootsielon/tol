foreach( d 
    barith bfunct blinalg bprdist bstat dcdflib gsl_ext mathobjects fftw )
  add_subdirectory( ${d} )
endforeach( )

add_tol_sublibrary( bmath matfun.cpp )

create_header_links( bmath )

