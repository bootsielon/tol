create_header_links( )

foreach( dir
    bdb bgrammar bgsl bmodel bmonte code_type complex_type date_type 
    matrix_type vmatrix_type polynomial_type ratio_type real_type serie_type 
    set_type text_type timeset_type polmat_type )
  add_subdirectory( ${dir} )
  list( APPEND sublibs $<TARGET_OBJECTS:${dir}> )
endforeach( )

add_library( btol delay_init.cpp ${sublibs} )