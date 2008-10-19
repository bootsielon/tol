/* libtolodbc.c: TOL ODBC interface.
                GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
*/

/*
  This is a SAMPLE TOL-database interface code esqueleton.
  This file is included as HELP for anyone who wants to add a database (or file or whatever) driver to TOL:
  First, replace all "_seed_" text with your driver's name, like "postgree", "tds", etc.
  Next, design the needed connection handler (Can be as simple as a file descriptor).
  Next, fill up every function to get the needed functionality.
  Last, change the file name as you did in the first step.

  Compile using:
    gcc -Wall -O -g -shared -fPIC -DUNIX -o libtol_seed_.so libtol_seed_.c -ldl
*/


#include <stdlib.h>
#include <stdio.h>
//TOL-Database type info:
#include "libtolbdb.h"

#ifdef UNIX
#  define DLLEXPORT(f) f
#else
#  include <windows.h>
#  if defined (_MSC_VER) || ((__BORLANDC__ >= 0x0550) || (defined(__GNUC__) && defined(__declspec)) || (defined(MAC_TCL) && FUNCTION_DECLSPEC))
#    define DLLEXPORT(f) __declspec(dllexport) f
#  else
#    define DLLEXPORT
#  endif
#endif

/*
SAMPLE DATA STRUCT.
  The Name notation for the struct is _seed_d, that is: odbcd, mysqld,
  oracled, etc.
  As for the rest of data, use whatever you may think that can be useful.

  It is the real ODBC struct used for TOL ODBC access.

typedef struct odbcdx
{
  SQLHDBC  hdbc;        // Connection handle
  SQLHENV  henv;        // Enviromental handle
  SQLHSTMT hstmt;       // Statement handle
  int numCols;          // Number of columns in current query
  short int *typeList;        // Array of type columns. Will be <numCols> size.
  //TypeDatabase *typeList;
} odbcd;
*/

typedef struct _seed_dx
{
  int sample;
} _seed_d;

//This is necessary to retreive connection errors.
//Else, we would have no handler to ask for trouble through DB_Errors function.
char *connError=NULL;

//--------------------------------------------------------------------
DLLEXPORT(char **) DB_Errors (_seed_d *dbd)

/*! Shows statment, connection and environmental errors.
  TODO: Expand errorText in case more than 16 errors are returned.
  (Eventhough it seems improblable)
 */
//--------------------------------------------------------------------
{
  char *aux, **errorText;
  int inx=0;

  errorText=(char **)malloc(16);

  //Get connection errors:
  if (connError)
  {
    errorText[0] = (char *)strdup(connError);
    errorText[1]=NULL;
    free(connError);
	  connError=NULL;
    return (errorText);
  }

  aux = (char *)malloc(250);
  aux = strcpy ("This is an error test, don't panic.");

  errorText[inx]=(char *)strdup(aux);
  free(aux);
  inx++;
  errorText[inx]=NULL;
  return errorText;
}

//--------------------------------------------------------------------
DLLEXPORT(int)  DBPutHCIWrite(void (* pF)(const char *))

/*! Evaluates the TOL files received as arguments.
TODO: This does not do anything yet.
 */
//--------------------------------------------------------------------
{
    return(1);
}

//--------------------------------------------------------------------
DLLEXPORT(_seed_d *)DBOpen(void **args)
{

/*! Opens the database.
 *  RETURN:  Pointer to TYPEd struct.
 *           NULL if there is any error
 */
//--------------------------------------------------------------------
  _seed_d *dbd;
  const char *database;
  const char *userName;
  const char *password;
  const char *host;

  userName = args[0];
  password = args[1];
  database = args[2];
  host =     args[3];

  dbd = (_seed_d *)malloc(sizeof(_seed_d));

  dbd->sample=0;
  return dbd;
}

//--------------------------------------------------------------------
DLLEXPORT(int)  DBCloseQuery(_seed_d *dbd)
/*! Close a query and free all resources.
 *
 * RETURN:
 *          1 if OK.
 *          0 if an error ocurred.
 */
//--------------------------------------------------------------------
{
  int result = 0;

  return result;
}

//--------------------------------------------------------------------
DLLEXPORT(int) DBClose(_seed_d *dbd)

/*! Close the database:
 *  If there is any handle (environment, connection or statment),
 *  it has to free resources associated with them
 *
 * RETURN:
 *          1 if OK.
 *          0 if an error ocurred.
 */
//--------------------------------------------------------------------
{
  int res;

  res=DBCloseQuery(dbd);
  if ( !res ) {
    puts("TODO: DBclose ERRORS");
    return 0;
  }
  free(dbd);
  return res;
}

//--------------------------------------------------------------------
DLLEXPORT(int)  DBOpenQuery(_seed_d *dbd, const char *query)

/*! Opens a query (executing it)
 * RETURN:   Numnber of affected rows if success
 *          -1 if there is any error
 */
//--------------------------------------------------------------------
{

  int ok = 1;

  return(ok);
}

//--------------------------------------------------------------------
DLLEXPORT(long)  DBExecQuery(_seed_d *dbd, const char *query)
/*! Prepares the SQL string for execution and executes it.
 * RETURN:  Number of rows affected if success
 *          -1 if there is any error
 * NOTE that there can be a 0 rows return but still beeing a correct call.
 * For instance, in create, delete or drop statements.
 */
//--------------------------------------------------------------------
{
  long int rowCount = -1;

  if (DBOpenQuery(dbd, query))
  {
    //Count rows here:
    rowCount = 0;
  }
  else {
	  return -1;
  }
  if ( !DBCloseQuery(dbd) ) {
	  return -1;
  }

  return((long)rowCount);
}

//--------------------------------------------------------------------
DLLEXPORT(int)  DBGetNext(_seed_d *dbd)
/*! fetches the next rowset of data
 */
//--------------------------------------------------------------------
{
  return (0);
}

//--------------------------------------------------------------------
DLLEXPORT(int)  DBGetFirst(_seed_d *dbd)

/*! fetches then first rowset of data
 */
//--------------------------------------------------------------------
{
  //Move to the first row, then get it:
  int res = DBGetNext(dbd);
  return(res);
}

//--------------------------------------------------------------------
DLLEXPORT(int)  DBGetFieldsNumber(_seed_d *dbd)

/*! returns the number of columns
 */
//--------------------------------------------------------------------
{
  int numCols=0;

  return((int)numCols);
}

//--------------------------------------------------------------------
DLLEXPORT(int)  DBGetFieldType (_seed_d *dbd, int nfield)

/*! returns the type of the field indicated by nfield (starting at 0)
 */
//--------------------------------------------------------------------
{
  int dbType = TypeDBUnknown;
  int returnedType;
  //Notice this sample code comes from ODBC:
  switch (returnedType)
  {
    case VARCHAR:
    case CHAR:
    case LONGVARCHAR:
      dbType = TypeDBText;
      break;
    case BIT:
    case TINYINT:
    case SMALLINT:
    case INTEGER:
    case DOUBLE:
    case DECIMAL:
    case NUMERIC:
    case FLOAT:
    case REAL:
      dbType = TypeDBReal;
      break;
    case DATE:
    case TIME:
    case TIMESTAMP:
      dbType = TypeDBDate;
      break;
    default:
      dbType = TypeDBUnknown;
  };
  return(dbType);
}

//--------------------------------------------------------------------
DLLEXPORT(int) DBGetFieldName(_seed_d *dbd,
                   int nfield,
                   char *fieldName)

/*! returns the name of the field indicated by nfield (starting at 0)
 * 0-index is the bookmark column, 1-index is the first field
 */
//--------------------------------------------------------------------
{
  strcpy(fieldName, "ColumnName");
  return 1;
}

//--------------------------------------------------------------------
DLLEXPORT(unsigned short int) DBCheckNull()

/* Checks if the last returned value was NULL
 */
//--------------------------------------------------------------------
{
  return (unsigned short int) 0;
}

//--------------------------------------------------------------------
DLLEXPORT(long double) DBGetAsReal(_seed_d *dbd, int nfield)

/*! Gets the value of a field as a real
TODO: Return correct error codes
 */
//--------------------------------------------------------------------
{
  return((long double)0);
}

//--------------------------------------------------------------------
DLLEXPORT(unsigned char *)DBGetAsText(_seed_d *dbd, int nfield, int hardMaxText)

/*! Gets the value of a field as a text
 */
//--------------------------------------------------------------------
{
  unsigned char *field;

  field=(char *)strdup("Line of data");
  return(field);
}

//--------------------------------------------------------------------
DLLEXPORT(struct dateStruct *)DBGetAsDate(_seed_d *dbd, int nfield)

/*! Gets the value of a field as a date
 */
//--------------------------------------------------------------------
{
  struct dateStruct *result;

  result = (struct dateStruct *)malloc(sizeof(struct dateStruct));
  result->year    = 0;
  result->month   = 0;
  result->day     = 0;
  result->hour    = 0;
  result->minute  = 0;
  result->second  = 0;
  result->msecond = 0;
  return result;
}

//--------------------------------------------------------------------
DLLEXPORT(struct openInfo *) DBGetOpenInfo ()
//
/*! Shows specific DBOpen arguments.
 *  returns a struct containing:
 *    - The min. argument number (Most likely to be 3)
 *    - The max. argument number (Most likely to be 3)
 *    - A TypeDatabase list for all arguments that DBOpen expects to find.
 */
//--------------------------------------------------------------------
{
  struct openInfo *info;
  info = (struct openInfo *)malloc(sizeof(struct openInfo));

  info->min = 3;
  info->max = 3;
  info->types = (enum TypeDataBase*)malloc(info->max*sizeof(enum TypeDataBase));
  info->types[0] = 1; //HostName
  info->types[1] = 1; //User
  info->types[2] = 1; //Password
  return info;
}
