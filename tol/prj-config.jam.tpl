# -*- mode: conf -*-
configure prefix : /usr/local/tol ;
configure lib : gsl ;
configure lib : gslcblas ;
configure lib : cholmod : <prefix>/usr/local/cholmod ;
configure lib : odbc : <name>odbc ;
configure lib : mysql : <name>mysqlclient <include>/usr/include/mysql ;
configure lib : pgsql : <name>pq ;
configure lib : bz2 ;
configure lib : ziparch  : <include>/usr/include/ziparchive ;
configure build : tolodbc tolmysql ;
