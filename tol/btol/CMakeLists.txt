
foreach( dir
    bdb bgrammar bgsl bmodel bmonte code_type complex_type date_type 
    matrix_type vmatrix_type polynomial_type ratio_type real_type serie_type 
    set_type text_type timeset_type polmat_type )
  add_subdirectory( ${dir} )
endforeach( )

add_tol_sublibrary( btol delay_init.cpp )

create_header_links( btol )

