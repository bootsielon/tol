mysqladmin -u%1 -p%2 create toltests
mysql -u%1 -p%2 < create_database_rights_mysql.sql
mysql -utoltester -ppwdtt -Dtoltests
