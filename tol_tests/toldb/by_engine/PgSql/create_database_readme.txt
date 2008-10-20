This file describes how to create a PostgreSQL database for Tol Tests Framework.
The following instructions are for the command line only, but should works in
every PostgreSQL installation.

1. Create "toltests" User

  The following command creates a new user named "toltests" and
  asks for a password for that user. The password agreed for toltests
  is "pwdtt":

  createuser --pwprompt --encrypted --no-adduser --no-createdb toltests

  If everything works correctly, you'll see a "CREATE USER" notice.

2. Create "toltests" Database

  The following command creates a new database named "toltests", which is
  owned by previously created "toltests":

    createdb --encoding=UNICODE --owner=toltests toltests

  If everything works correctly, you'll see a "CREATE DATABASE" notice.
  Note that the database will be created with UTF-8 (Unicode) encoding.

3. Give network access:

  In order to be accessed from other hosts in your network, you must
  add a row of permissions to your PostgreSQL pg_hba.conf, located in:

  Linux: /etc/postgresql/{version}/main/
  Windows: {hd_letter}:\{Postgres installation dir}\{version}\data\

  note for Linux users:
      Before editing pg_hba.conf, don't forget to give "write"
      permissions to this file, as root you must execute:

        chmod u+w pg_hba.conf

  Before editing the file, you must know the IP address that fits to
  your network. If your network address is 192.168.2.0 and you want to
  give access to every host in your network, you should use 192.168.2.0/24
  as the CIDR-ADDRESS during pg_hba.conf edition.

  Note that the previous "/24" means that you are in a class C IP network.
  If you were in a class B or class A network you should use "/16" or "/8"
  respectively. To know what type of IP network class you have, run (under
  Linux) "ifconfig", or (under Windows) "ipconfig" and look at the output
  parameter "Mask":

     255.255.255.0 is for a C class IP network -> 24 bits for net addr.
     255.255.0.0   is for a B class IP network -> 16 bits for net addr.
     255.0.0.0     is for a A class IP network ->  8 bits for net addr.

     If the mask is something else, ask your network administrator
     for the correct suffix number you have to use.

  Edit pg_hba.conf file and add a row like this:

  host    all         all         IP/IP-mask        md5

  where
    IP      -> is your network IP address (i.e.: 192.168.2.0)
    IP-mask -> is the number of significative bits used for your
               network IP class: 24 or 16 or 8...

  save pg_hba.conf file and exit.

  Also edit your postgresql.conf file, in the same directory, and look
  for parameters "listen_addresses" and "port". They are together.
  You must change listen_addreess:

     listen_address = '*' # to listen on your real IP

  For the next section remember the value of parameter "port".

  Stop your PostgreSQL server and start it again.

4. Check whether it works:

  With the following command we will try to get access to database
  "toltests" with user "toltests" and password "pwdtt". If you
  configured pg_hba.conf and postgresql.conf to give network access,
  it's important to try the command also in one of those other hosts:

  psql -h IP -p port -d toltests -U toltests

  where
    -h IP       -> tells the IP address where you have installed PostgreSQL
    -p port     -> tells the TCP/IP port where PostgreSQL is listening for
                   connections. By default is 5432, in such case it's an
                   optional parameter.
    -d database -> tells the database to connect to.
    -d user     -> tells the user to connect with.

  PostgreSQL will prompt for the 'toltests' database password and you must
  answer with: pwdtt

  PostgreSQL will reply getting you into Postgres welcome message and
  it will wait for your commands with its prompt:

  toltests=>

  Just write "\q" to quit.

  The database is ready.