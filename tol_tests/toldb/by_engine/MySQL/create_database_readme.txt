This file describes how to create a MySQL database for Tol Tests Framework.
The following instructions are for the command line only, but should works in
every MySQL installation.

If your Database server Also you can try to execute in windows

> create_toltests_mysql.bat user password

or in linux

> create_toltests_mysql.sh user password


1. Create "toltests" Database

  In the following examples, 'dbuser' is an example MySQL user which has
  the CREATE and GRANT privileges. Use the appropriate user name and pwd
  for your system: Command line A.

  If you are the administrator of your own host try with mysql user "root".
  The initial root account passwords are empty, so they are still empty
  try without a root password: Command line B.

  If you have assigned a password to MySQL root account: Command line C.

  Execute one of the command line that you chose before:
   A: mysqladmin -u dbuser -p create toltests
   B: mysqladmin -u root create toltests
   C: mysqladmin -u root -p create toltests


2. Create Database rights:

  Next you must login and set the access database rights.
  Execute the right command line to get access:

  A: mysql -u dbuser mysql -p
  B: mysql -u root mysql
  C: mysql -u root mysql -p

  In the following mysql command there is an IP address that should fit
  into your host or network. If you want your MySQL server to be accessed
  from other hosts in your network, you must use a combination of your
  net address and % symbol.
  i.e. if your network address is 192.168.2.0 and you want to give access
  every host on your net, you should use:  192.168.2.% as the IP address
  in the following command:

  At the MySQL prompt, enter following command:

    GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX,
    ALTER, CREATE TEMPORARY TABLES, LOCK TABLES
    ON toltests.* TO 'toltests'@'IP' IDENTIFIED BY 'pwdtt';

  where 'pwdtt' is the password agreed for toltests

  Note: Unless your database user has all of the privileges listed
  above, you will not be able to run all Tol database user functions.

  If successful, MySQL will reply with:

    Query OK, 0 rows affected

  To activate the new permissions, enter the following command:

    FLUSH PRIVILEGES;

  Now you can exit:

    exit


3. Check whether it works:

  With the following command we will try to get access to database
  "toltests" with user "toltests" and password "pwdtt". If you are
  giving MySQL access to other hosts, it is important to try the
  command in at least one of those other hosts:

  mysql -u toltests -h IP toltests -p

  where IP is the host IP address where you have installed MySQL.

  MySQL will prompt for the 'toltests' database password and you must
  answer with: pwdtt

  MySQL will reply getting you into MySQL welcome message and waiting
  for commands with its prompt:

  mysql>

  Just write "quit" or "exit" to quit.

  The database is ready.
