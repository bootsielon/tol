# -*- mode: conf -*-
configure prefix : /usr/local/tol ;
configure lib : readline ;
configure lib : gsl ;
configure lib : gslcblas ;
configure lib : blas ;
configure lib : lapack_atlas ;
configure lib : lapack : <needs>lapack_atlas <search>/usr/lib/sse2 ;
configure lib : cholmod : <prefix>/usr/local/cholmod ;
configure lib 
          : Cint 
          :
          <name>Cint_static 
          <search>/home/jsperez/TOL/ExternalLibs/cint/lib
          <include>/home/jsperez/TOL/ExternalLibs/cint/cint/inc
configure lib : odbc : <name>odbc ;
configure lib : mysql : <name>mysqlclient <include>/usr/include/mysql ;
configure lib : pgsql : <name>pq ;
configure lib : bz2 ;
configure lib : ziparch  : <include>/usr/include/ziparchive ;
configure lib : ANN : <prefix>/usr/local/src/GEOM/ann_1.1.1 ;
configure build : tolodbc tolmysql tolpgsql ;
