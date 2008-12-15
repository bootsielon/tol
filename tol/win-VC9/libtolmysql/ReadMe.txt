Windows VC2005 compilation instructions for Driver "libtolmysql":
-----------------------------------------------------------------

First of all you need to have installed some contrib files from 
MySQL, all them packaged into a file called: Windows Essentials,
that includes all header files and libraries needed to compile the
driver.

In order to get this package visit the MySQL Web Site at: 
http://www.mysql.com, go to "Downloads", and continue in "MySQL Community 
Server", where you can choose between several versions of MySQL. Choose 
the version you are interested in, and browse down to find the mentioned 
file: "Windows Essential".

Install it, and create two Environment Variables:

MYSQL_INCLUDE=path_to_include_directory
MYSQL_LIB=path_to_libmysql_directory

If you have installed the package under C:\MySQL\MySQL Server 5.1,
you will find all header files and libraries under "include" directory and
"lib/opt" directory respectively, so both environment variables should 
be:

MYSQL_INCLUDE=C:\MySQL\MySQL Server 5.1\include
MYSQL_LIB=C:\MySQL\MySQL Server 5.1\lib\opt

After create them, don't forget to relaunch your VC2005.

If you detect problems following these steps, please visit www.tol-project.org, 
subscribe yourself to <tol-devel@lists.tol-project.org>, and explain us in an
email what is happening. We will pleased to help you.

