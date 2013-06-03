/* libtolmysql.c: TOL MySQL interface. TOL Language.

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

#define SQLFLOAT double

#ifdef UNIX
#  define DLLEXPORT(f) f
#  include <mysql/mysql.h>
#else
#  include <windows.h>
#  include <mysql.h>
#  if defined (_MSC_VER) || ((__BORLANDC__ >= 0x0550) || (defined(__GNUC__) && defined(__declspec)) || (defined(MAC_TCL) && FUNCTION_DECLSPEC))
#    define DLLEXPORT(f) __declspec(dllexport) f
#  else
#    define DLLEXPORT
#  endif
#endif

typedef struct mysqldx {
  MYSQL *desc;
  MYSQL_RES *result;
  int num_cols;
} mysqld;

//To return connection errors:
char *connError=NULL;
unsigned short int nullValue=0;

static DBOutWriter stdOutWriter = 0;

//--------------------------------------------------------------------
/*! Evaluates the TOL files received as arguments.
 */
DLLEXPORT(int) mysql_PutHCIWriter(void (* pF)(const char *))
{
  stdOutWriter = (DBOutWriter) (*pF);
  return 1;
}

//--------------------------------------------------------------------
/*! Opens the database allocating the handle.
 *  RETURN:  mysqld struct, or NULL in case of error
 */
DLLEXPORT(mysqld *)mysql_Open( void **args)
{
  mysqld *dbd;
  MYSQL *mysql;
  const char *database;
  const char *userName;
  const char *password;
  const char *host;

  userName = args[0];
  password = args[1];
  database = args[2];
  host =     args[3];

  mysql = (MYSQL *)malloc(sizeof(MYSQL));

  mysql_init(mysql);
  //mysql_options(mysql,MYSQL_READ_DEFAULT_GROUP,0);

  if (!mysql_real_connect(mysql,host,userName,password,database,0,NULL,0))
  {
     char *out_msg;
     connError = (char *)mysql_error(mysql);
     out_msg = calloc(strlen(connError)+30, sizeof(char));
     sprintf(out_msg, 
	           "Error: mysql_Open: %s\n", 
	           connError);
     stdOutWriter(out_msg);
     free(out_msg);
     return 0;
  }
  dbd = (mysqld *)malloc(sizeof(mysqld));
  dbd->desc = mysql;
  dbd->result = NULL;
  return dbd;
}

//--------------------------------------------------------------------
DLLEXPORT(int) mysql_CloseQuery(mysqld *dbd)
{
  //  MYSQL_RES *res;

  if(dbd->result) 
  {
    //res= dbd->result;
    mysql_free_result(dbd->result);
    dbd->result=NULL;
    return 1;
  } 
  else 
    return 0;
}

//--------------------------------------------------------------------
/*! Close the direct connection.
 * \return 1 if OK, or 0 if an error ocurred.
 */
DLLEXPORT(int) mysql_Close(mysqld *dbd)
{
  MYSQL *desc;

  mysql_CloseQuery(dbd);
  desc = dbd->desc;
  mysql_close(desc);
  dbd->desc=NULL;
  return 1;
}

DLLEXPORT(int) mysql_GetDBMSName(mysqld *dbd, char *dbmsName, size_t size)
{
  const char* mysqlName = "mysql";
  strncpy(dbmsName, mysqlName, size);
  return 1;
}

DLLEXPORT(int) mysql_GetDBMSVersion(mysqld *dbd, char *dbmsVersion, size_t size)
{
  strncpy(dbmsVersion, dbd->desc->server_version, size);
  return 1;
}

DLLEXPORT(int) mysql_GetDataBaseName(mysqld *dbd, char *database, size_t size)
{
  const char* unknownName = "unknown";
  strncpy(database, unknownName, size);
  return 1;
}

//--------------------------------------------------------------------
/*! Opens a query (executing it).
 * \return 1 if success, 0 in case of error.
 */
DLLEXPORT(int) mysql_OpenQuery(mysqld *dbd, const char *query)
{
  const char *error_msg;
  char *out_msg;

  if(mysql_real_query(dbd->desc, query, (unsigned long)strlen(query)))
  { // Error
    error_msg = mysql_error(dbd->desc);
    out_msg = calloc(strlen(error_msg)+10, sizeof(char));
    sprintf(out_msg, "Error: %s\n", error_msg);
    stdOutWriter(out_msg);
    free(out_msg);
    return 0; 
  } 
  else 
    return 1;
}

//--------------------------------------------------------------------
/*! Prepares the SQL string for execution and executes it.
 * \return Number of rows affected if success, or -1 in case of error.
 * \sa mysql_OpenQuery
 */
DLLEXPORT(int) mysql_ExecQuery(mysqld *dbd, const char *query)
{
  int num_rows = 0;

  if(mysql_OpenQuery(dbd, query))
  {
    dbd->result = mysql_store_result(dbd->desc);

    if(dbd->result) { // there are rows
      num_rows = (int) mysql_num_fields(dbd->result);
      mysql_free_result(dbd->result);  // == mysql_CloseQuery
      dbd->result = NULL;              // == mysql_CloseQuery
    }
    else  // mysql_store_result() returned nothing; should it have?
      if(mysql_field_count(dbd->desc) == 0)
	// query does not return data (it was not a SELECT)
	num_rows = (int) mysql_affected_rows(dbd->desc);
      else
	return -1;
  }
  else return -1;

  return (num_rows<0) ? 0 : num_rows;
}

//--------------------------------------------------------------------
/*! fetches the next rowset of data
 */
DLLEXPORT(int) mysql_GetNext(mysqld *dbd)
{
  if(!mysql_fetch_row(dbd->result)) {
    //The query got to the last row or an error ocurred:
    if (mysql_errno(dbd->desc) != 0)
      return -1;
    else
      return 0;
  }
  return(1);
}

//--------------------------------------------------------------------
/*! fetches the first rowset of data
 */
DLLEXPORT(int) mysql_GetFirst(mysqld *dbd)
{
  dbd->result = mysql_use_result(dbd->desc);
  if(dbd->result == NULL) {
    stdOutWriter("Error: mysql_GetFirst: mysql_use_result returns no result.\n");
    return -1;
  }
  return mysql_GetNext(dbd);
}

//--------------------------------------------------------------------
/*! returns the number of columns
 */
DLLEXPORT(int) mysql_GetFieldsNumber(mysqld *dbd)
{
  if (dbd->result)
  {
    return (dbd->result->field_count);
  }
  else
  {
    if (mysql_errno(dbd->desc))
    {
      char error[1024];
      sprintf(error, 
	      "Error: mysql_GetFieldsNumber: %s\n", 
	      mysql_error(dbd->desc));
      stdOutWriter(error);
    }
    else
    {
      stdOutWriter("Error: mysql_GetFieldsNumber.\n");
    }
  }
  return (0);
}

//--------------------------------------------------------------------
/*! returns the type of the field indicated by nfield (starting at 0)
 */
DLLEXPORT(int) mysql_GetFieldType (mysqld *dbd, int nfield)
{
  int dbType;
  int type;

  if(!dbd->result)
  {
    puts ("No result found!");
    return TypeDBUnknown;
  }
  type = dbd->result->fields[nfield].type;
  //Cuidado con esto, toma por numeros los tipos YEAR y TIMESTAMP:
  //if (IS_NUM(type)){
  //  return(TypeDBReal);
  //}
  switch(type)
    {
    case FIELD_TYPE_SHORT:    case FIELD_TYPE_LONG:    case FIELD_TYPE_INT24:
    case FIELD_TYPE_LONGLONG: case FIELD_TYPE_DECIMAL: case FIELD_TYPE_FLOAT:
    case FIELD_TYPE_DOUBLE:   
#if (defined(MYSQL_VERSION_ID) && (MYSQL_VERSION_ID>=50024))
    case MYSQL_TYPE_NEWDECIMAL:
#endif
      {
	dbType=TypeDBReal;
	break;
      }
    case FIELD_TYPE_TIMESTAMP: case FIELD_TYPE_DATE:  case FIELD_TYPE_TIME:
    case FIELD_TYPE_DATETIME:  case FIELD_TYPE_YEAR:  
      {
	dbType=TypeDBDate;
	break;
      }
    case FIELD_TYPE_TINY:        case FIELD_TYPE_STRING: 
    case FIELD_TYPE_VAR_STRING:  case FIELD_TYPE_BLOB:
      {
	dbType=TypeDBText;
	break;
      }
  case FIELD_TYPE_SET: case FIELD_TYPE_ENUM: case FIELD_TYPE_NULL:
  default:
      dbType=TypeDBUnknown;
  }
  return dbType;
}

//--------------------------------------------------------------------
/*! returns the name of the field indicated by nfield (starting at 0)
 * 0-index is the bookmark column, 1-index is the first field
 */
DLLEXPORT(int) mysql_GetFieldName(mysqld *dbd, int nfield, char *fieldName)
{
  if (dbd->result)
  {
    strcpy(fieldName, dbd->result->fields[nfield].name);
    return 1;
  }
  stdOutWriter("Error: mysql_GetFieldName: No previous result found.\n");
  return 0;
}

//--------------------------------------------------------------------
/*! gets the value of a field as a real
 *  PRECONDITIONS: Caller program must know it is actually a numeric value.
 *                 Otherwise, no error can be returned.
 *  TODO: Check for overflow conditions in mysql long long data!
 *
 * \return 0 in case of error, 1 to indicate a Real value will be returned
 *           in \p realval, 2 to indicate a NULL value was red.
 */
DLLEXPORT(int) mysql_GetAsReal(mysqld *dbd, int nfield, long double *realval)
{
  int type;

  if (dbd->result) 
  {
    type = dbd->result->fields[nfield].type;
    if (IS_NUM(type)) 
    {
      if(dbd->result->current_row[nfield]!=NULL) 
        *realval = ((double)atof(dbd->result->current_row[nfield]));
      else 
      {
	*realval = 0.0;
	return 2;
      }
      return 1;
    } 
    else 
      stdOutWriter("Error: Mysql_GetAsReal: Not a Numeric value.\n");
  }

  return 0;
}
//--------------------------------------------------------------------
/*! gets the value of a field as a text
  Optimized to fit the length of the data field.
  It has not been proben with rows of more than 64KB, but should work.
 */
DLLEXPORT(int) mysql_GetAsText(mysqld *dbd, int n_field, char **txt_val)
{
  if (dbd->result) 
  {
    if(dbd->result->current_row[n_field])
    {
      *txt_val = (char*)dbd->result->current_row[n_field];
      return 1;
    }
    else
    {
      *txt_val=NULL;
      return 2;
    }
  } 

  return 0;
}

DLLEXPORT(int) mysql_FreeText(mysqld *dbd, char **txt_val) { return 1; }

//--------------------------------------------------------------------
/*! gets the value of a field as a date
 */
DLLEXPORT(int) mysql_GetAsDate(mysqld *dbd, int nfield, struct dateStruct **dateval)
{
  int type;
  char *tmp, *auxTmp;

  if(dbd->result) 
  {
    type = dbd->result->fields[nfield].type;

    if (dbd->result->current_row[nfield]==NULL) {
    return 2;
    }

    switch (type)
    {
    case FIELD_TYPE_TIMESTAMP:
      {
	tmp=(char*)malloc(5*sizeof(char));
	auxTmp=dbd->result->current_row[nfield];
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
    case FIELD_TYPE_DATE:
      {
	tmp=(char*)malloc(5*sizeof(char));
	auxTmp=dbd->result->current_row[nfield];
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
	(*dateval)->hour = 0;
	(*dateval)->minute = 0;
	(*dateval)->second = 0;
	(*dateval)->msecond = 0;
	free(tmp);
	return 1;
      }
    case FIELD_TYPE_TIME:
      {
	(*dateval)->year = 0;
	(*dateval)->month = 0;
	(*dateval)->day = 0;
	tmp    =(char*)malloc(3*sizeof(char));
	auxTmp = dbd->result->current_row[nfield];
	tmp    = strncpy(tmp, auxTmp,2);
	tmp[2] ='\0';
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
    case FIELD_TYPE_DATETIME:
      {
	tmp=(char*)malloc(5*sizeof(char));
	auxTmp=dbd->result->current_row[nfield];
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
    case FIELD_TYPE_YEAR:
      {
	(*dateval)->year    = (unsigned short)strtol(dbd->result->current_row[nfield],
						     (char **)NULL, 10);
	(*dateval)->month   = (unsigned short)1;
	(*dateval)->day     = (unsigned short)1;
	(*dateval)->hour    = 0;
	(*dateval)->minute  = 0;
	(*dateval)->second  = 0;
	(*dateval)->msecond = 0;
	return 1;
      }
    }
  }

  return 0;
}
