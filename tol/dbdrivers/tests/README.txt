Name: DBTest
Date: July 14th, 2004.
Mandatory: No.
Description:
  This directory contains a collection of tol files to test database
  functionality.
  All tests are made for ODBC and mysql drivers unless otherwise especified.
Notes:
  Before executing any of the tests change parameters in DBTestIni.tol.
  DBTestIni.tol defines parameters such as user, password, host and aliasnames
  for ODBC.
  Since all tests run using the same table names and databases, no parallel
  execution should be done.
Returns:
  - "Ok" on success.
  - "Error" in case the test failed
  - "Error-Check DB parameters in DBTestIni.tol" if any DBOpen fails.
Test Descriptions:
  - DBTestIni.tol: Initializes all parameters needed for all but DBTestErrors
      tests.
  - DBTestErrors.tol: Tests for Seg Faults when using wrong params in DB
      functions. Ignore Warnings.
  - DBTestEdgeValues.tol: Checks for NULL, Unknown and fixed-length access in
      Real(INT), Text(VARCHAR, CHAR, TEXT) and Date(DATE) types.
  - DBTestMultipleDB.tol: Light tests for multiple database handling. Just
      DBOpen, DBClose and DBActivate are used here.
  - DBTestBigQuery.tol: Hard test, might take some time executing it. Inserts
      4000 rows of data and then reads it, comparing every value in every row.
  - DBTestBigTextODBC.tol: Hard test for Text fields. Inserts 100 rows of
      different data sizes up to 32 KB using ODBC. Reads and checks returned
      rows.
  - DBTestBigTextMySQL.tol: Hard test for Text fields. Inserts 100 rows of
      different data sizes up to 32 KB using mysql. Reads and checks returned
      rows.
  - DBTestMultipleQuery.tol: Big tests for multiple database handling: Different 
      querys are made simultaneously on various databases, and every result checked.

Credits:
  - All: Manuel Balderrábano <manuelb@tol-project.org>
