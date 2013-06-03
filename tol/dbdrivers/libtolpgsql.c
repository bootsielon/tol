/* libtolpgsql.c: TOL PostgreSQL interface. TOL Language.

   Copyright (C) 2003-2007, Bayes Decision, SL (Spain [EU])

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

#include "libtolbdb.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define SQLFLOAT double

#ifdef UNIX
#  define DLLEXPORT(f) f
#  include <libpq-fe.h>
#else
#  include <libpq-fe.h>
#  include <windows.h>
#  if defined (_MSC_VER) || ((__BORLANDC__ >= 0x0550) || (defined(__GNUC__) && defined(__declspec)) || (defined(MAC_TCL) && FUNCTION_DECLSPEC))
#    define DLLEXPORT(f) __declspec(dllexport) f
#  else
#    define DLLEXPORT
#  endif
#endif

#define PG_TYPE_BOOL 16

typedef struct pgsqldx {
  PGconn *conn;
  PGresult *result;
  int n_tuples;
  int current_tuple;
} pgsqld;

//To return connection errors:
char *connError=NULL;
unsigned short int nullValue=0;

static DBOutWriter stdOutWriter = 0;

//--------------------------------------------------------------------
/*! Evaluates the TOL files received as arguments.
 */
DLLEXPORT(int) postgres_PutHCIWriter(void (* pF)(const char *))
{
  stdOutWriter = (DBOutWriter) (*pF);
  return 1;
}

//--------------------------------------------------------------------
/*! Opens the database allocating the handle.
 *  RETURN:  postgresd struct or NULL in case of error
 */
DLLEXPORT(pgsqld *)postgres_Open( void **args)
{
  pgsqld *dbd;
  PGconn *conn;
  const char *database;
  const char *username;
  const char *password;
  const char *host;
  const char *port;
  int rc_fclose;

  username = args[0];
  password = args[1];
  database = args[2];
  host = args[3];
  port = args[4];

  conn = PQsetdbLogin(host, port,
		      "", // pgoptions
		      "", // pgtty
		      database, username, password);

  /* Check to see that the backend connection was successfully made */
  if(PQstatus(conn) != CONNECTION_OK) 
  {
    char outmsg[1024];
    sprintf(outmsg, "Connection to database failed: %s\n", 
	    PQerrorMessage(conn));
    stdOutWriter(outmsg);
    PQfinish(conn);
    return NULL;
  }

  dbd = (pgsqld *)malloc(sizeof(pgsqld));
  dbd->conn = conn;
  dbd->result = NULL;
  
  if((rc_fclose=fclose(stderr)) == EOF) {
      stdOutWriter(strerror(errno));
  }
  
  return dbd;
}

//--------------------------------------------------------------------
DLLEXPORT(int) postgres_CloseQuery(pgsqld *dbd)
{
  if(dbd->result)
  {
    PQclear(dbd->result);
    dbd->result = NULL;
    return 1;
  }
  else
    return 0;
}

//--------------------------------------------------------------------
/*! Close the direct connection.
 * \return 1 if OK, or 0 if an error ocurred.
 */
DLLEXPORT(int) postgres_Close(pgsqld *dbd)
{
  postgres_CloseQuery(dbd);
  PQfinish(dbd->conn);
  dbd->conn = NULL;
  return 1;
}

DLLEXPORT(int) postgres_GetDBMSName(pgsqld *dbd, char *dbmsName, size_t size)
{
  const char* pgsqlName = "postgres";
  strncpy(dbmsName, pgsqlName, sizeof(pgsqlName)-1);
  return 1;
}

DLLEXPORT(int) postgres_GetDBMSVersion(pgsqld *dbd, char *dbmsVersion, size_t size)
{
  int version = PQserverVersion(dbd->conn);
  int major = version / 10000;
  int min_rev = version - major*10000;
  int minor = (min_rev) / 100;
  int revision = (min_rev - minor*100);
  snprintf(dbmsVersion, size, "%d.%d.%d", major, minor, revision);
  return 1;
}

DLLEXPORT(int) postgres_GetDataBaseName(pgsqld *dbd, char *database, size_t size)
{
  strncpy(database, PQdb(dbd->conn), size);
  return 1;
}

//--------------------------------------------------------------------
/*! Opens a query (executing it)
 * \return 1 if sucess, 0 in case of error
 */
DLLEXPORT(int) postgres_OpenQuery(pgsqld *dbd, const char *query)
{
  dbd->result = PQexec(dbd->conn, query);
  if(dbd->result==NULL)
  { // Error
    char *error_msg = PQerrorMessage(dbd->conn);
    char *out_msg = calloc(strlen(error_msg)+10, sizeof(char));
    sprintf(out_msg, "Error: %s\n", error_msg);
    stdOutWriter(out_msg);
    free(out_msg);
    return 0;
  } 

  if(PQresultStatus(dbd->result) == PGRES_TUPLES_OK)
    dbd->n_tuples = PQntuples(dbd->result);
  
  return 1;
}

//--------------------------------------------------------------------
/*! Prepares the SQL string for execution and executes it.
 * \return Number of rows affected
 * \sa mysql_OpenQuery
 */
DLLEXPORT(int) postgres_ExecQuery(pgsqld *dbd, const char *query)
{
  char *error_msg, *out_msg;
  int num_rows = -1;

  if(postgres_OpenQuery(dbd, query))
  {
    ExecStatusType status_type = PQresultStatus(dbd->result);
    if(status_type == PGRES_COMMAND_OK)
      num_rows = atoi(PQcmdTuples(dbd->result));
    else if(status_type == PGRES_TUPLES_OK)
      num_rows = dbd->n_tuples;
  } 
  if(num_rows<0) 
  {
    error_msg = PQresultErrorMessage(dbd->result);
    out_msg = calloc(strlen(error_msg)+10, sizeof(char));
    sprintf(out_msg, "<E>\n%s\n</E>", error_msg);
    stdOutWriter(out_msg);
    free(out_msg);
  }

  if(!postgres_CloseQuery(dbd)) return -1;
  return (num_rows<0) ? 0 : num_rows;
}

//--------------------------------------------------------------------
/*! fetches the next rowset of data
 */
DLLEXPORT(int) postgres_GetNext(pgsqld *dbd)
{
  if( ++(dbd->current_tuple) < dbd->n_tuples )
    return 1;
  else
    return 0;
}

//--------------------------------------------------------------------
/*! fetches then first rowset of data
 */
DLLEXPORT(int) postgres_GetFirst(pgsqld *dbd)
{
  if(dbd->n_tuples<=0) 
  {
    dbd->current_tuple = -1;
    return 0;
  }
  else
  {
    dbd->current_tuple = 0;
    return 1;
  }
}

//--------------------------------------------------------------------
/*! returns the number of columns
 */
DLLEXPORT(int) postgres_GetFieldsNumber(pgsqld *dbd)
{
  return PQnfields(dbd->result);
}

//--------------------------------------------------------------------
/*! returns the type of the field indicated by nfield (starting at 0)
 */
DLLEXPORT(int) postgres_GetFieldType(pgsqld *dbd, int nfield)
{
  int dbtype;
  Oid oid; // Oid is a typedef for unsigned int (postgres_ext.h)

  oid = PQftype(dbd->result, nfield);

  switch(oid)
    {
    case 20: case 21: case 23: // int8, int2, int4
    case 1700:                 // numeric == DECIMAL
    case 700: case 701:        // float4, float8
    case 790:                  // money
      {
	dbtype = TypeDBReal;
	break;
      }
    case 1114: case 1184:            // timestamp, timestamptz
    case 1083: case 1266: case 1082: // time, timetz, date
      {
	dbtype = TypeDBDate;
	break;
      }
    case 18: case 25: case 1043: // char, text, varchar
      {
	dbtype = TypeDBText;
	break;
      }
    default:
      dbtype = TypeDBUnknown;
    }

  return dbtype;  
}

//--------------------------------------------------------------------
/*! returns the name of the field indicated by nfield (starting at 0)
 * 0-index is the bookmark column, 1-index is the first field
 */
DLLEXPORT(int) postgres_GetFieldName(pgsqld *dbd, int nfield, char *fieldName)
{
  if(strcpy(fieldName, PQfname(dbd->result, nfield)))
    return 1;
  else
    return 0;
}

//--------------------------------------------------------------------
/*! gets the value of a field as a real
 * \return 0 in case of error, 1 to indicate a Real value will be returned
 *           in \p realval, 2 to indicate a NULL value was red.
 */
DLLEXPORT(int) postgres_GetAsReal(pgsqld *dbd, int nfield, long double *realval)
{
  char *val = NULL;

  if(dbd->result)
  {
    val = PQgetvalue(dbd->result, dbd->current_tuple, nfield);
    if(!val || !strcmp(val, "")) 
    {
      *realval = 0.0;
      return 2;
    } 
    else
      *realval = (double) atof(val);

    return 1;
  }
  return 0;
}
//--------------------------------------------------------------------
/*! gets the value of a field as a text
  Optimized to fit the length of the data field.
  It has not been proben with rows of more than 64KB, but should work.
 */
DLLEXPORT(int) postgres_GetAsText(pgsqld *dbd, int nfield, char **txt_val)
{
  if(dbd->result)
  {
    *txt_val = PQgetvalue(dbd->result, dbd->current_tuple, nfield);
    return 1;
  }

  return 0;
}

DLLEXPORT(int) postgres_FreeText(pgsqld *dbd, char **txt_val) { return 1; }

//--------------------------------------------------------------------
/*! gets the value of a field as a date
 */
DLLEXPORT(int) postgres_GetAsDate(pgsqld *dbd, int nfield, 
				  struct dateStruct **dateval)
{
  Oid oid;
  char *tmp, *auxTmp;

  if(dbd->result)
  {
    oid = PQftype(dbd->result, nfield);
    
    switch(oid)
      {
      case 1114: // timestamp, timestamptz
	{
	  tmp=(char*)malloc(5*sizeof(char));
	  auxTmp=PQgetvalue(dbd->result, dbd->current_tuple, nfield);
	  tmp=strncpy(tmp, auxTmp,4);
	  tmp[4]='\0';
	  (*dateval)->year = (unsigned short)atoi(tmp);
	  auxTmp = auxTmp+(5*sizeof(char));
	  tmp=(char *)strncpy(tmp, auxTmp,2);
	  tmp[2]='\0';
	  (*dateval)->month = (unsigned short)atoi(tmp);
	  auxTmp=auxTmp+(3*sizeof(char));
	  tmp=(char *)strncpy(tmp, auxTmp, 2);
	  (*dateval)->day = (unsigned short)atoi(tmp);
	  auxTmp=auxTmp+(3*sizeof(char));
	  tmp=(char *)strncpy(tmp, auxTmp, 2);
	  (*dateval)->hour = (unsigned short)atoi(tmp);
	  auxTmp=auxTmp+(3*sizeof(char));
	  tmp=(char *)strncpy(tmp, auxTmp, 2);
	  (*dateval)->minute = (unsigned short)atoi(tmp);
	  auxTmp=auxTmp+(3*sizeof(char));
	  tmp=(char *)strncpy(tmp, auxTmp, 2);
	  (*dateval)->second = (unsigned short)atoi(tmp);
	  (*dateval)->msecond = 0;
	  free(tmp);
	  return 1;	  
	}
      }
  }

  return 0;
}
