
add_tol_sublibrary( fftw fftw.cpp )
target_include_directories( fftw PRIVATE ${FFTW_INCLUDES} )

create_header_links( fftw )

