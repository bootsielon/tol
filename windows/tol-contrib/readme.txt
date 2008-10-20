=================================================================================
 
                    TOL - GNU/GPL Time Oriented Language v1.1.7
   
                  Complementary External Resources for MS Windows
  
                           http://www.tol-project.org
  
=================================================================================


This file includes all external open-source complements needed to build 
TOL 1.1.5, the current development version.

-GSL-1.8: 
    GNU Scientific Library: TOL makes use of them.

-CHOLMOD: 
    Choleski for large sparse matrix.

-MySQL v5.1 header files and client library (Not mandatory):
    You will need them to get native access to MySQL DDBB servers. 

-PostgreSQL v8.1 header files and client library (Not mandatory):
    You will need them to get native access to PostgreSQL DDBB servers. 

-bzip2-1.0.3: 
    BZip2 compression software, required by OIS, the Tol module for 
    Objects Indexed Serialization.

-ZipArchive: 
    Encapsulation of files and directories and compression software, 
    required by OIS, the Tol module for Objects Indexed Serialization.

-GOOGLE_SPARSEHASH: 
    An extremely memory-efficient hash_map implementation

To use this package you must download last fresh TOL Sources from the 
Tol-Project CVS. Follow the instructions to use it here:
http://www.tol-project.org/index.php?op=s&secid=10

Before compiling Tol using gnutol.sln file (Visual C++ 2005 Solution) available 
in win-VC8 directory, you must create several environment variables.
You can create them automatically using setup.bat script available with this
package, or you can create them by yourself.

The list of environment variables to be created before running the build process
 are:

GSL_INC           -> absolute path for GSL "include" directory.
GSL_LIB           -> absolute path for GSL "lib" directory.
CHOLMOD_INC       -> absolute path for CHOLMOD "include" directory.
CHOLMOD_LIB       -> absolute path for CHOLMOD "lib" directory.
MYSQL_INCLUDE     -> absolute path for MySQL "include" directory.
MYSQL_LIB         -> absolute path for MySQL "lib" directory.
PGSQL_INCLUDE     -> absolute path for MySQL "include" directory.
PGSQL_LIB         -> absolute path for MySQL "lib" directory.
BZIP2_INC         -> absolute path for BZip2 "include" directory.
BZIP2_LIB         -> absolute path for BZip2 "lib" directory.
ZipArchive_INC    -> absolute path for ZipArchive "include" directory.
ZipArchive_LIB    -> absolute path for ZipArchive "lib" directory.
GOOGLE_SPARSEHASH -> absolute path for Google-Sparse-Hash directory.

=================================================================================
 If you get in troubles, please contact us here:
   vdebuen@tol-project.org
   danirus@tol-project.org
   josp@tol-project.org
=================================================================================
