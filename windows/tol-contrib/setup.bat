@echo ==============================================================================
@echo =                                                                            = 
@echo =                 TOL - GNU/GPL Time Oriented Language                       =
@echo =                                                                            = 
@echo =            Complementary External Resources for MS Windows                 =
@echo =                                                                            = 
@echo =                     http://www.tol-project.org                             =
@echo =                                                                            = 
@echo ==============================================================================
@set  _TOL_CONTRIB_ROOT_PATH_=%CD%
@set  CATLAS_INC="%_TOL_CONTRIB_ROOT_PATH_%\atlas\include"       
@set  CATLAS_LIB="%_TOL_CONTRIB_ROOT_PATH_%\atlas\lib"           
@set  GSL_INC="%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\include"       
@set  GSL_LIB="%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\lib"           
@set  CHOLMOD_INC="%_TOL_CONTRIB_ROOT_PATH_%\cholmod\include"   
@set  CHOLMOD_LIB="%_TOL_CONTRIB_ROOT_PATH_%\cholmod\lib"       
@set  BZIP2_INC="%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\include" 
@set  BZIP2_LIB="%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\lib"     
@set  ZipArchive_INC="%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\include" 
@set  ZipArchive_LIB="%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\lib"     
@set  MYSQL_INCLUDE="%_TOL_CONTRIB_ROOT_PATH_%\mysql\include"   
@set  MYSQL_LIB="%_TOL_CONTRIB_ROOT_PATH_%\mysql\lib"            
@set  PGSQL_INCLUDE="%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\include"   
@set  PGSQL_LIB="%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\lib\ms"            
@set  GOOGLE_SPARSEHASH="%_TOL_CONTRIB_ROOT_PATH_%\google\sparsehash-0.6\src"
@echo Environment variables needed to compile TOL 1.1.7 in windows:
@set  _TOL_CONTRIB_ROOT_PATH_
@set  ATLAS_INC
@set  ATLAS_LIB
@set  GSL_INC
@set  GSL_LIB
@set  CHOLMOD_INC
@set  CHOLMOD_LIB
@set  BZIP2_INC
@set  BZIP2_LIB
@set  ZipArchive_INC
@set  ZipArchive_LIB
@set  MYSQL_INCLUDE
@set  MYSQL_LIB
@set  PGSQL_INCLUDE
@set  PGSQL_LIB
@set  DUMA_INC
@set  DUMA_LIB
@set  GOOGLE_SPARSEHASH
@echo Exporting environment variables for all users ...
@setx ATLAS_INC "%_TOL_CONTRIB_ROOT_PATH_%\ATLAS\include"                     -m
@setx ATLAS_LIB "%_TOL_CONTRIB_ROOT_PATH_%\ATLAS\lib"                         -m
@setx GSL_INC "%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\include"                     -m
@setx GSL_LIB "%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\lib"                         -m
@setx CHOLMOD_INC "%_TOL_CONTRIB_ROOT_PATH_%\cholmod\include"                 -m
@setx CHOLMOD_LIB "%_TOL_CONTRIB_ROOT_PATH_%\cholmod\lib"                     -m
@setx BZIP2_INC "%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\include"               -m
@setx BZIP2_LIB "%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\lib"                   -m
@setx ZipArchive_INC "%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\include"           -m
@setx ZipArchive_LIB "%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\lib"               -m
@setx MYSQL_INCLUDE "%_TOL_CONTRIB_ROOT_PATH_%\mysql\include"                 -m
@setx MYSQL_LIB "%_TOL_CONTRIB_ROOT_PATH_%\mysql\lib"                         -m
@setx PGSQL_INCLUDE "%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\include"        -m
@setx PGSQL_LIB "%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\lib\ms"             -m  
@setx GOOGLE_SPARSEHASH "%_TOL_CONTRIB_ROOT_PATH_%\google\sparsehash-0.6\src" -m
@pause