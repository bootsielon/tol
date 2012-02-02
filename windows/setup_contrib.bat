@echo ==============================================================================
@echo =                                                                            = 
@echo =                 TOL - GNU/GPL Time Oriented Language                       =
@echo =                                                                            = 
@echo =       Complementary External Resources for TOL 3.1 in MS Windows         =
@echo =                                                                            = 
@echo =                     http://www.tol-project.org                             =
@echo =                                                                            = 
@echo ==============================================================================
@set  _TOL_CONTRIB_ROOT_PATH_="%CD%\tol-contrib"
@set  ATLAS_INC="%_TOL_CONTRIB_ROOT_PATH_%\atlas\include"       
@set  ATLAS_LIB="%_TOL_CONTRIB_ROOT_PATH_%\atlas\lib"           
@set  BOOST="%_TOL_CONTRIB_ROOT_PATH_%\boost"       
@set  BZIP2_INC="%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\include" 
@set  BZIP2_LIB="%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\lib"     
@set  CHOLMOD_INC="%_TOL_CONTRIB_ROOT_PATH_%\cholmod\include"   
@set  CHOLMOD_LIB="%_TOL_CONTRIB_ROOT_PATH_%\cholmod\lib"       
@set  GOOGLE_SPARSEHASH="%_TOL_CONTRIB_ROOT_PATH_%\google\sparsehash-1.8.1\src"
@set  GSL_INC="%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\include"       
@set  GSL_LIB="%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\lib"           
@set  MYSQL_INCLUDE="%_TOL_CONTRIB_ROOT_PATH_%\mysql\include"   
@set  MYSQL_LIB="%_TOL_CONTRIB_ROOT_PATH_%\mysql\lib"            
@set  PGSQL_INCLUDE="%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\include"   
@set  PGSQL_LIB="%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\lib\ms"            
@set  ZipArchive_INC="%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\include" 
@set  ZipArchive_LIB="%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\lib" 
@set  ANN_LIB="%_TOL_CONTRIB_ROOT_PATH_%\ann_1.1.1_MS_Win32_bin"
@set  ANN_INCLUDE="%_TOL_CONTRIB_ROOT_PATH_%\ann_1.1.1_MS_Win32_bin"
@set  LTDL_LIB="%_TOL_CONTRIB_ROOT_PATH_%\libltdl\lib"
@set  LTDL_INC="%_TOL_CONTRIB_ROOT_PATH_%\libltdl\include"
@set  CINT_LIB="%_TOL_CONTRIB_ROOT_PATH_%\cint\lib"
@set  CINT_INC="%_TOL_CONTRIB_ROOT_PATH_%\cint\inc"
@echo Environment variables needed to compile TOL 3.1 in windows:
@set  _TOL_CONTRIB_ROOT_PATH_
@set  ATLAS_INC
@set  ATLAS_LIB
@set  BOOST
@set  BZIP2_INC
@set  BZIP2_LIB
@set  CHOLMOD_INC
@set  CHOLMOD_LIB
@set  GOOGLE_SPARSEHASH
@set  GSL_INC
@set  GSL_LIB
@set  MYSQL_INCLUDE
@set  MYSQL_LIB
@set  PGSQL_INCLUDE
@set  PGSQL_LIB
@set  ANN_INCLUDE
@set  ANN_LIB
@set  ZipArchive_INC
@set  ZipArchive_LIB
@set  LTDL_INC
@set  LTDL_LIB
@set  CINT_INC
@set  CINT_LIB
@echo Exporting environment variables for all users ...
@setx ATLAS_INC "%_TOL_CONTRIB_ROOT_PATH_%\ATLAS\include"                     -m
@setx ATLAS_LIB "%_TOL_CONTRIB_ROOT_PATH_%\ATLAS\lib"                         -m
@setx BOOST "%_TOL_CONTRIB_ROOT_PATH_%\boost"                                 -m
@setx BZIP2_INC "%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\include"               -m
@setx BZIP2_LIB "%_TOL_CONTRIB_ROOT_PATH_%\bzip2-1.0.3\lib"                   -m
@setx GOOGLE_SPARSEHASH "%_TOL_CONTRIB_ROOT_PATH_%\google\sparsehash-1.8.1\src" -m
@setx CHOLMOD_INC "%_TOL_CONTRIB_ROOT_PATH_%\cholmod\include"                 -m
@setx CHOLMOD_LIB "%_TOL_CONTRIB_ROOT_PATH_%\cholmod\lib"                     -m
@setx GSL_INC "%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\include"                     -m
@setx GSL_LIB "%_TOL_CONTRIB_ROOT_PATH_%\GSL-1.8\lib"                         -m
@setx MYSQL_INCLUDE "%_TOL_CONTRIB_ROOT_PATH_%\mysql\include"                 -m
@setx MYSQL_LIB "%_TOL_CONTRIB_ROOT_PATH_%\mysql\lib"                         -m
@setx PGSQL_INCLUDE "%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\include"        -m
@setx PGSQL_LIB "%_TOL_CONTRIB_ROOT_PATH_%\postgresql\8.1\lib\ms"             -m  
@setx ANN_INCLUDE "%_TOL_CONTRIB_ROOT_PATH_%\ann_1.1.1_MS_Win32_bin"          -m
@setx ANN_LIB "%_TOL_CONTRIB_ROOT_PATH_%\ann_1.1.1_MS_Win32_bin"              -m  
@setx ZipArchive_INC "%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\include"           -m
@setx ZipArchive_LIB "%_TOL_CONTRIB_ROOT_PATH_%\ZipArchive\lib"               -m
@setx LTDL_INC "%_TOL_CONTRIB_ROOT_PATH_%\libltdl\include"                    -m
@setx LTDL_LIB "%_TOL_CONTRIB_ROOT_PATH_%\libltdl\lib"                        -m
@setx CINT_INC "%_TOL_CONTRIB_ROOT_PATH_%\cint\inc"                           -m
@setx CINT_LIB "%_TOL_CONTRIB_ROOT_PATH_%\cint\lib"                           -m
@pause