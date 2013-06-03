/* libtolodbc.c: TOL-ODBC Interface. TOL Language.

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

#ifdef UNIX
#  ifdef _OPENLINK_
#    include <isql.h>
#    include <isqlext.h>
#  else
#    include <sql.h>
#    include <sqlext.h>
#  endif
#  define SQLFLOAT double
#  define DLLEXPORT(f) f
#else
#  include <windows.h>
#  include <sql.h>
#  include <sqlext.h>
#  ifndef _MSC_VER
#    define SQLFLOAT double
#  endif
#  if defined (_MSC_VER) || ((__BORLANDC__ >= 0x0550) || (defined(__GNUC__) && defined(__declspec)) || (defined(MAC_TCL) && FUNCTION_DECLSPEC))
#    define DLLEXPORT(f) __declspec(dllexport) f
#  else
#    define DLLEXPORT
#  endif
#endif

#if !defined(TOLSQL_BIGINT)
#  if defined(WIN32)
#    define TOLSQL_BIGINT _int64
#  else
#    define TOLSQL_BIGINT long long int
#  endif
#endif
#define SQLMAXWORD 128
#define MAX_TEXT_FIELD_LENGTH 2048

#define TRACE_DEEP 1

#ifdef _MSC_VER

#define snprintf c99_snprintf

int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

int c99_snprintf(char* str, size_t size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}

#endif // _MSC_VER


typedef struct odbcdx {
  //  char *alias;
  //  char *user;
  //  char *pwd;
  SQLHENV henv;           //!< Enviromental handle
  SQLHDBC hdbc;           //!< Connection handle
  SQLHSTMT hstmt;         //!< Statement handle
  int num_cols;           //!< Number of columns in current query
  SQLSMALLINT *typeList;  //!< Array of type columns. Will be <num_cols> size.
} odbcd;

#define NO_DIAG       0
#define GET_DIAG_ENV  1
#define GET_DIAG_DBC  2
#define GET_DIAG_STMT 3
#define GET_DIAG_DESC 4

static DBOutWriter stdOutWriter = 0;

void extract_error(
    char *fn,
    SQLHANDLE handle,
    SQLSMALLINT type)
{
    SQLSMALLINT   i = 0;
    SQLINTEGER   native;
    SQLCHAR      state[ 7 ];
    SQLCHAR      text[256];
    SQLSMALLINT  len;
    SQLRETURN    ret;

    fprintf(stderr,
            "\n"
            "The driver reported the following diagnostics whilst running "
            "%s\n\n",
            fn);

    do
    {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
                            sizeof(text), &len );
        if (SQL_SUCCEEDED(ret))
            printf("%s:%ld:%ld:%s\n", state, i, native, text);
    }
    while( ret == SQL_SUCCESS );
}


//-------------------------------------------------------------------
void getDiagRec(odbcd *dbd, int diagType)
{
  SQLCHAR     SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
  SQLINTEGER  NativeError;
  SQLSMALLINT handleType, i, MsgLen;  
  SQLHANDLE   handle;
  SQLRETURN   rc;

  if(diagType==GET_DIAG_STMT) {
    handleType = SQL_HANDLE_STMT;
    handle = dbd->hstmt;
  } else if(diagType==GET_DIAG_DBC) {
#ifdef TRACE_DEEP
  printf( "Diagnostic for SQL_HANDLE_DBC" );
#endif
    handleType = SQL_HANDLE_DBC;
    handle = dbd->hdbc;
  }

#ifdef TRACE_DEEP
  extract_error( "TRACE_DEEP", handle, handleType );
#endif
  i = 1;
  while ((rc = SQLGetDiagRec(handleType, handle, i, SqlState, 
			     &NativeError, Msg, strlen((char*)Msg), 
			     &MsgLen)) != SQL_NO_DATA) 
  {
      if(rc==SQL_SUCCESS) {
	char outmsg[SQL_MAX_MESSAGE_LENGTH + 50];
	sprintf(outmsg, 
		"[SQLState:%s][NativeError:%d]%s\n", 
		SqlState, NativeError, Msg);
	stdOutWriter(outmsg);
	i++;
      } 
      else if(rc==SQL_SUCCESS_WITH_INFO) {
#if defined(_DEBUG) || defined(TRACE_DEEP)
	if(MsgLen>SQL_MAX_MESSAGE_LENGTH)
	  stdOutWriter("The *MessageText buffer was too small to hold the "
		       "requested diagnostic message.\n");
#endif

	break;
      } 
      else if(rc==SQL_INVALID_HANDLE) {
#if defined(_DEBUG) || defined(TRACE_DEEP)
	stdOutWriter("The handle indicated by HandleType and Handle was not "
		     "a valid handle.\n");
#endif
	break;
      } 
      else if(rc==SQL_ERROR) {
	stdOutWriter("SQL_ERROR\n");
	break;
      }
    }
}

//-------------------------------------------------------------------
int checkSQLRet(SQLRETURN status, odbcd *dbd, int line, int diagType)
{
  if((status==SQL_SUCCESS_WITH_INFO) || (status==SQL_ERROR)) {
#   if defined(_DEBUG)
      char outmsg[100];
      sprintf(outmsg,"Tol-ODBC: SQL status %d, line %d\n", (int) status, line);
      stdOutWriter(outmsg);
#   endif
    getDiagRec(dbd, diagType);
    if(status==SQL_ERROR) return 0;
  }
  return 1;
}

//-------------------------------------------------------------------
int closeODBC(odbcd *dbd, int step)
{
  switch(step) 
  {
  case 3: 
    if(!checkSQLRet( SQLDisconnect(dbd->hdbc),
		     dbd, __LINE__, NO_DIAG)) { return 0; }
    
  case 2: 
    if(!checkSQLRet( SQLFreeHandle(SQL_HANDLE_DBC, dbd->hdbc),
		     dbd, __LINE__, NO_DIAG)) { return 0; }
    
  case 1: 
    if(!checkSQLRet( SQLFreeHandle(SQL_HANDLE_ENV, dbd->henv),
		     dbd, __LINE__, NO_DIAG)) { return 0; }
  }
  return 1;
}

//--------------------------------------------------------------------
/*! Evaluates the TOL files received as arguments.
 */
DLLEXPORT(int) odbc_PutHCIWriter(void (* pF)(const char *))
{
  stdOutWriter = (DBOutWriter) (*pF);
  return 1;
}

//--------------------------------------------------------------------
/*! Open the database allocating the environment handle and the
 *  connection handle.
 *  RETURN:  Pointer to odbcd struct.
 *           NULL if there is any error.
 */
DLLEXPORT(odbcd *) odbc_Open(void **args)
{
  SQLRETURN status;
  odbcd *dbd;
  char *alias, *user, *pwd;
#ifdef TRACE_DEEP
  printf( "Entering odbc_Open\n");
#endif

  dbd = (odbcd*)malloc(sizeof(odbcd));
  if(dbd==NULL) {
    stdOutWriter("Out of memory\n");
    return NULL;
  }

  alias = (char*) args[2];
  user = (char*) args[0];
  pwd = (char*) args[1];

  // http://windowssdk.msdn.microsoft.com/en-us/library/ms714078.aspx
  //-------------------------------------------------------------------
  // 1st step: Connect to the Data Source

  // 1.1: SQLAllocHandle(ENV)
#ifdef TRACE_DEEP
  printf( "1.1: SQLAllocHandle(ENV)\n");
#endif
  status =  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &(dbd->henv));
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_ENV)) {
    return NULL;
  }

  // 1.2: SQLSetEnvAttr
#ifdef TRACE_DEEP
  printf( "1.2: SQLSetEnvAttr\n");
#endif
  //SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
  status = SQLSetEnvAttr(dbd->henv, SQL_ATTR_ODBC_VERSION, 
			 (void *)SQL_OV_ODBC3, 0);
  if(!checkSQLRet(status, dbd, __LINE__, NO_DIAG)) {
    closeODBC(dbd, 1); //"1" -> last step done: SQLAllocHandle SQLHENV
    return NULL;
  }

  // 1.3: SQLAllocHandle(DBC)
#ifdef TRACE_DEEP
  printf( "1.3: SQLAllocHandle(DBC)\n");
#endif
   status = SQLAllocHandle(SQL_HANDLE_DBC, dbd->henv, &(dbd->hdbc));
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_DBC)) {
    closeODBC(dbd, 1); //"1" -> last step done: SQLAllocHandle SQLHENV
    return NULL;
  }

  // 1.4: SQLConnect
#ifdef TRACE_DEEP
  printf( "1.4: SQLConnect\n");
  printf( "(%s,%s,%s)\n", alias, user, pwd );
#endif
  status = SQLConnect(dbd->hdbc, 
		      (SQLCHAR*) alias, SQL_NTS,
		      (SQLCHAR*) user, SQL_NTS,
		      (SQLCHAR*) pwd, SQL_NTS);
#ifdef TRACE_DEEP
  printf( "Status from SQLConnect: %d\n", status );
#endif
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_DBC)) {
    closeODBC(dbd, 2); //"2"-> last step done: SQLAllocHandle of SQLHDBC
    return NULL;
  }

  dbd->hstmt = 0;
  dbd->num_cols = 0;
  return dbd;
}

DLLEXPORT(int) odbc_GetDBMSName(odbcd *dbd, char *dbmsName, size_t size)
{
  SQLSMALLINT BufferLength = (SQLSMALLINT)size;
  SQLSMALLINT ActualLength;
  
  SQLRETURN status = SQLGetInfo(dbd->hdbc, SQL_DBMS_NAME,
                                dbmsName, BufferLength, &ActualLength);
  return checkSQLRet(status, dbd->hdbc, __LINE__, GET_DIAG_DBC);
}

DLLEXPORT(int) odbc_GetDBMSVersion(odbcd *dbd, char *dbmsVersion, size_t size)
{
  SQLSMALLINT BufferLength = (SQLSMALLINT)size;
  SQLSMALLINT ActualLength;
  
  SQLRETURN status = SQLGetInfo(dbd->hdbc, SQL_DBMS_VER,
                                dbmsVersion, BufferLength, &ActualLength);
  return checkSQLRet(status, dbd->hdbc, __LINE__, GET_DIAG_DBC);
}

DLLEXPORT(int) odbc_GetDataBaseName(odbcd *dbd, char *database, size_t size)
{
  SQLSMALLINT BufferLength = (SQLSMALLINT)size;
  SQLSMALLINT ActualLength;
  
  SQLRETURN status = SQLGetInfo(dbd->hdbc, SQL_DATABASE_NAME,
                                database, BufferLength, &ActualLength);
  return checkSQLRet(status, dbd->hdbc, __LINE__, GET_DIAG_DBC);
}

//-------------------------------------------------------------------
/*! \return Returns 1 if the connection has been successfully 
 * closed, or 0 upon failure.
 */
DLLEXPORT(int) odbc_Close(odbcd *dbd)
{
  int rc = closeODBC(dbd, 3); // "3" -> last step done was SQLConnect
  if(rc) {
    //    free(dbd->alias);
    //    free(dbd->user);
    //    free(dbd->pwd);
    free(dbd);
  }
  return rc;
}

//-------------------------------------------------------------------
DLLEXPORT(int) odbc_CloseQuery(odbcd *dbd)
{
  SQLRETURN status;
  
  status = SQLFreeHandle(SQL_HANDLE_STMT, dbd->hstmt);
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) {
    return 0; // error
  } 
  else 
  {
    dbd->hstmt = 0;
    if(dbd->num_cols) 
    {
      dbd->num_cols = 0;
      free(dbd->typeList);
    }
    dbd->typeList = NULL;
    return 1; // it works
  }
}

//-------------------------------------------------------------------
/*! Opens a query (executing it)
 * RETURN:  1 if success, 0 in case of error.
 */
DLLEXPORT(int) odbc_OpenQuery(odbcd *dbd, const char *query)
{
  SQLRETURN status;

  status = SQLAllocHandle(SQL_HANDLE_STMT, dbd->hdbc, &(dbd->hstmt));
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) {
    return 0;
  }

  status = SQLExecDirect(dbd->hstmt, (SQLCHAR *) query, strlen(query));
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) {
    return 0;
  }

  return 1;
}

//-------------------------------------------------------------------
/*! Prepares the SQL string for execution and executes it.
 * \return Number of rows affected if success, or -1 in case of error.
 * \sa odbc_OpenQuery
 */
DLLEXPORT(int) odbc_ExecQuery(odbcd *dbd, const char *query)
{
  SQLRETURN status;
  SQLINTEGER row_count=-1;
  
  if(odbc_OpenQuery(dbd, query))
  {
    status = SQLRowCount(dbd->hstmt, &row_count);
    if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) {
      return -1;
    }
    if(!odbc_CloseQuery(dbd)) return -1;
    return (row_count<0)? 0 : row_count;
  } 
  else return -1;
}

//-------------------------------------------------------------------
/*! returns the number of columns
 */
DLLEXPORT(int) odbc_GetFieldsNumber(odbcd *dbd)
{
  SQLSMALLINT num_cols = -1;
  SQLRETURN status;

  status = SQLNumResultCols(dbd->hstmt, &num_cols);
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) {
    return -1;
  }

  if(num_cols>0) {
    dbd->num_cols = num_cols;
    dbd->typeList = (short int *)malloc(sizeof(short int) * num_cols);
    if(dbd->typeList==NULL) {
      stdOutWriter("Out of memory\n");
      return -1;
    }
  }

  return (int) num_cols;
}

//-------------------------------------------------------------------
/*! returns the type of the field indicated by nfield (starting at 0)
 *  -1 -> Error, 0 -> Unknown, 1 -> Text, 2 -> Date/Time, 3 -> Real
 */
DLLEXPORT(int) odbc_GetFieldType(odbcd *dbd, int nfield)
{
  SQLCHAR szColName [SQLMAXWORD+1];
  SQLSMALLINT pfSqlType, pibScale, pfNullable;
  SQLUINTEGER pcbColDef;
  SQLRETURN status;
  int ret = TypeDBUnknown;
 
  status = SQLDescribeCol(dbd->hstmt, nfield+1, szColName, SQLMAXWORD, NULL,
			  &pfSqlType, &pcbColDef, &pibScale, &pfNullable);
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) {
    return -1;
  }

  dbd->typeList[nfield] = pfSqlType;
  //printf( "pfSqlType=%d\n", pfSqlType ); 
  switch (pfSqlType)
    {
    case SQL_VARCHAR: case SQL_CHAR: case SQL_LONGVARCHAR: 
      { 
	ret = TypeDBText;
	break;
      }
    case SQL_BIT:     case SQL_TINYINT: case SQL_SMALLINT: case SQL_BIGINT:  
    case SQL_INTEGER: case SQL_DOUBLE:  case SQL_DECIMAL:  case SQL_NUMERIC:
    case SQL_FLOAT:   case SQL_REAL:
      {
	ret = TypeDBReal;
	break;
      }
    case SQL_DATE: case SQL_TIME: case SQL_TIMESTAMP:
#if (ODBCVER >= 0x0300)
    case SQL_TYPE_DATE: case SQL_TYPE_TIME: case SQL_TYPE_TIMESTAMP:
#endif
      {
	ret = TypeDBDate;
	break;
      }
    default:
      { ret = TypeDBUnknown; }
    };

  return ret;
}

//-------------------------------------------------------------------
/*! returns the name of the field indicated by nfield (starting at 0)
 * 0-index is the bookmark column, 1-index is the first field.
 * \return returns 0 in case of error, or 1 if success.
 */
DLLEXPORT(int) odbc_GetFieldName(odbcd *dbd, int nfield, char *fieldName)
{
  SQLRETURN status;

  SQLSMALLINT pfSqlType;
  SQLUINTEGER pcbColDef;
  SQLSMALLINT pibScale;
  SQLSMALLINT pfNullable;
  
  status = SQLDescribeCol(dbd->hstmt, nfield+1,
			  (SQLCHAR*)fieldName, SQLMAXWORD, NULL,
			  &pfSqlType, &pcbColDef,
			  &pibScale, &pfNullable);

  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT))
    return 0;
  else 
    return 1;  
}

//-------------------------------------------------------------------
/*! Fetches the next rowset of data from the result set.
 * RETURN:  1 in case of success operation
 *          0 in case of End of Cursor
 *         -1 in case of error
 */
DLLEXPORT(int) odbc_GetNext(odbcd *dbd)
{
  SQLRETURN status;

  status = SQLFetch(dbd->hstmt);
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) {
    return -1;
  } 

  if(status==SQL_NO_DATA) 
    return 0; // end of cursor, no more data
  else return 1;
}

DLLEXPORT(int) odbc_GetFirst(odbcd *dbd)
{
  return odbc_GetNext(dbd);
}

//-------------------------------------------------------------------
/*! Converts numeric data from SQL to Real Tol Data Type.
 * Visit http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcsql_data_types.asp
 *
 * The identifiers for the numeric ODBC SQL data types are:
 *   SQL_DECIMAL   SQL_NUMERIC   SQL_TINYINT   SQL_SMALLINT   SQL_INTEGER
 *   SQL_BIGINT    SQL_REAL      SQL_FLOAT     SQL_DOUBLE
 *
 * \return 0 in case of error, 1 to indicate a Real value will be returned
 *           in \p realval, 2 to indicate a NULL value was red.
 */
DLLEXPORT(int) odbc_GetAsReal(odbcd *dbd, int nfield, long double *realval)
{
  SQLRETURN status;
    
  //--------------------------------------------------
  /* visit next URL to get knowledge on C-SQL conversions:
   * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcc_data_types.asp
   * A Tol Real value could read from next SQL types:
   */
  SQLCHAR _sqlchar;            // for SQL_BIT, SQL_TINYINT
  SQLSMALLINT _smallint;       // for SQL_SMALLINT
  TOLSQL_BIGINT _bigint;       // for SQL_BIGINT
  SQLINTEGER _integer;         // for SQL_INTEGER
  //SQLCHAR *_decimal;           // for SQL_DECIMAL SQL_NUMERIC
  SQLCHAR   szNumeric[256];    // for SQL_DECIMAL SQL_NUMERIC SQL_BIGINT
  SQLDOUBLE _double;           // for SQL_DOUBLE, SQL_FLOAT
  SQLREAL _real;               // for SQL_REAL

  // variables for SQLGetData function
  SQLPOINTER pointer;
  SQLINTEGER pcbValue;
  int size;
  char szErrMsg[256];
  SQLSMALLINT targetType = SQL_C_DEFAULT;
  SQLSMALLINT type = dbd->typeList[nfield];

  // returned data initilization
  *realval = 0.0;

  // allocate space depending on the field type
  switch(type)
    {
    case SQL_BIT: case SQL_TINYINT:
      size=sizeof(SQLCHAR); pointer=&_sqlchar; 
      break;
    case SQL_SMALLINT:
      size=sizeof(SQLSMALLINT); pointer=&_smallint; 
      break;
/*
    case SQL_BIGINT:
printf( "case SQL_BIGINT:\n" );

      size=sizeof(TOLSQL_BIGINT); pointer=&_bigint; 
      targetType = SQL_C_SBIGINT;
      break;
*/
    case SQL_INTEGER:
      size=sizeof(SQLINTEGER); pointer=&_integer; 
      break;
    case SQL_DECIMAL: case SQL_NUMERIC: case SQL_BIGINT:
      /*size=64; 
      _decimal=(SQLCHAR *)calloc(size, sizeof(SQLCHAR)); 
      if(_decimal==NULL) { 
	stdOutWriter("Out of memory\n"); 
	return 0; 
      }
      pointer=_decimal;*/
      pointer = (SQLPOINTER)szNumeric;
      size = sizeof( szNumeric );
      targetType = SQL_C_CHAR;
      break;
    case SQL_DOUBLE: case SQL_FLOAT:
      size=sizeof(SQLDOUBLE); pointer=&_double;
      break;
    case SQL_REAL:
      size=sizeof(SQLREAL); pointer=&_real;
      break;
    default:
      snprintf( szErrMsg, sizeof( szErrMsg ) - 1,
                "Wrong numeric data type at SQL field: %i\n", nfield + 1 );
      stdOutWriter( szErrMsg );
      return 0;
    }

  // retrieve data from DDBB
  status = SQLGetData( dbd->hstmt, nfield+1, targetType, pointer, size,
                       &pcbValue );
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) { return 0; }

  /* pcbValue must be the length of the data available to return.
   * But also can be: 
   *  SQL_NO_TOTAL (there's still data to retrieve), or
   *  SQL_NULL_DATA (the data is a NULL data value)
   */
  if(pcbValue == SQL_NULL_DATA) return 2; // column data value is acceptable as NULL

  /* otherwise, fill the corresponding field and give it back */
  switch(type)
    {
    case SQL_BIT: case SQL_TINYINT:
      *realval = (long double)(_sqlchar); break;
    case SQL_SMALLINT:
      *realval = (long double)(_smallint); break;
    /*
    case SQL_BIGINT:
      *realval = (long double)_bigint; break; */
    case SQL_INTEGER:
      *realval = (long double)(_integer); break;
    case SQL_DECIMAL: case SQL_NUMERIC: case SQL_BIGINT:
      //sscanf((char *)_decimal, "%Lf", realval); break;
      sscanf( szNumeric, "%Lf", realval); break;
    case SQL_DOUBLE: case SQL_FLOAT:
      *realval = (long double)(_double); break;
    case SQL_REAL:
      *realval = (long double)(_real);
    }

  return 1;
}

//-------------------------------------------------------------------
DLLEXPORT(int) odbc_GetAsText(odbcd *dbd, int n_field, UCHAR **txt_val)
{
  SQLRETURN status;
  SQLINTEGER num_available=0, num_required=MAX_TEXT_FIELD_LENGTH;
  UCHAR *str, *long_str;

  str = (UCHAR*) calloc(num_required, sizeof(UCHAR));
  if(str==NULL) {
    stdOutWriter("Out of memory retrieving text ODBC.\n");
    return 0;
  }
  
  status = SQLGetData(dbd->hstmt,n_field+1,SQL_CHAR,str,num_required,&num_available);
  if((status!=SQL_SUCCESS) && (status!=SQL_SUCCESS_WITH_INFO)) { return 0; }
  if(num_available==SQL_NULL_DATA) { return 1; }

  if(status==SQL_SUCCESS_WITH_INFO)
  {
    long_str = (UCHAR*) calloc(num_available+1, sizeof(UCHAR));
    if(long_str==NULL) {
      stdOutWriter("Out of memory retrieving text from ODBC.\n");
      return 0;
    }
    long_str = (unsigned char*) strncpy((char*)long_str,(char*)str,MAX_TEXT_FIELD_LENGTH-1);
    free(str);
    str = (UCHAR*) calloc((num_available+2)-num_required, sizeof(UCHAR));
    if(str==NULL) {
      stdOutWriter("Out of memory retrieving text from ODBC.\n");
      return 0;
    }
    status = SQLGetData(dbd->hstmt,n_field+1,SQL_CHAR,str,num_available,&num_available);
    long_str = (unsigned char*) strncat((char*)long_str, (char*)str, num_available);
    free(str);
    str = long_str;
  }
    
  *txt_val = str;
  return 1;
}

DLLEXPORT(int) odbc_FreeText(odbcd *dbd, UCHAR **txtval)
{
  free(*txtval);
  return 1;
}

//-------------------------------------------------------------------
DLLEXPORT(int) odbc_GetAsDate(odbcd *dbd, int nfield, struct dateStruct **dateval)
{
  SQLRETURN status;

  SQL_DATE_STRUCT      *x_date;
  SQL_TIME_STRUCT      *x_time;
  SQL_TIMESTAMP_STRUCT *x_timestamp;

  SQLPOINTER pointer;
  SQLINTEGER pcbValue;
  int size;
  char *errmsg;
  SQLSMALLINT type = dbd->typeList[nfield];
  
  // allocate space depending on the field type
  switch(type)
    {
    case SQL_DATE:
#if (ODBCVER >= 0x0300)
    case SQL_TYPE_DATE:
#endif
      size   = sizeof(struct tagDATE_STRUCT);
      x_date = (struct tagDATE_STRUCT *)malloc(size);
      if(x_date==NULL) { stdOutWriter("Out of memory\n"); return 0; }
      pointer = x_date;
      break;
    case SQL_TIME:
#if (ODBCVER >= 0x0300)
    case SQL_TYPE_TIME:
#endif
      size   = sizeof(struct tagTIME_STRUCT);
      x_time = (struct tagTIME_STRUCT *)malloc(size);
      if(x_time==NULL) { stdOutWriter("Out of memory\n"); return 0; }
      pointer = x_time;
      break;
    case SQL_TIMESTAMP:
#if (ODBCVER >= 0x0300)
    case SQL_TYPE_TIMESTAMP:
#endif
      size        = sizeof(struct tagTIMESTAMP_STRUCT);
      x_timestamp = (struct tagTIMESTAMP_STRUCT *)malloc(size);
      if(x_timestamp==NULL) { stdOutWriter("Out of memory\n"); return 0; }
      pointer     = x_timestamp;
      break;
    default:
      errmsg = (char *)calloc(35, sizeof(char));
      if(errmsg!=NULL) {
	sprintf(errmsg, "Wrong data type at SQL field:%i\n", nfield+1);
	stdOutWriter(errmsg);
      } else {
	stdOutWriter("Out of memory\n");
      }
      return 0;
    }

  // retrieve data from DDBB
  status = SQLGetData(dbd->hstmt, nfield+1, type, pointer, size, &pcbValue);
  if(!checkSQLRet(status, dbd, __LINE__, GET_DIAG_STMT)) { 
    return 0; 
  }

  /* pcbValue must be the length of the data available to return.
   * But also can be: 
   *  SQL_NO_TOTAL (there's still data to retrieve), or
   *  SQL_NULL_DATA (the data is a NULL data value)
   */
  if((pcbValue == SQL_NULL_DATA) || (pcbValue == SQL_NO_TOTAL)) {
    return 1;
  }

  /* otherwise, fill the corresponding field and give it back */
  switch(type)
    {
    case SQL_DATE:
#if (ODBCVER >= 0x0300)
    case SQL_TYPE_DATE:
#endif
      (*dateval)->year  = (unsigned short)(x_date->year);
      (*dateval)->month = (unsigned short)(x_date->month);
      (*dateval)->day   = (unsigned short)(x_date->day);
      break;
    case SQL_TIME:
#if (ODBCVER >= 0x0300)
    case SQL_TYPE_TIME:
#endif
      (*dateval)->hour    = (unsigned short)(x_time->hour);
      (*dateval)->minute  = (unsigned short)(x_time->minute);
      (*dateval)->second  = (unsigned short)(x_time->second);
      break;
    case SQL_TIMESTAMP:
#if (ODBCVER >= 0x0300)
    case SQL_TYPE_TIMESTAMP:
#endif
      (*dateval)->year    = (unsigned short)(x_timestamp->year);
      (*dateval)->month   = (unsigned short)(x_timestamp->month);
      (*dateval)->day     = (unsigned short)(x_timestamp->day);
      (*dateval)->hour    = (unsigned short)(x_timestamp->hour);
      (*dateval)->minute  = (unsigned short)(x_timestamp->minute);
      (*dateval)->second  = (unsigned short)(x_timestamp->second);
      (*dateval)->msecond = (unsigned short)(x_timestamp->fraction);
    }
  
  return 1;
}
