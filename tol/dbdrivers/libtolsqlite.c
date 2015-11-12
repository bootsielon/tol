/* libtolsqlite.c: TOL SqLite interface. TOL Language.

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
#  include <libpq-fe.h>
#else
#  include <windows.h>
#  if defined (_MSC_VER) || ((__BORLANDC__ >= 0x0550) || (defined(__GNUC__) && defined(__declspec)) || (defined(MAC_TCL) && FUNCTION_DECLSPEC))
#    define DLLEXPORT(f) __declspec(dllexport) f
#  else
#    define DLLEXPORT
#  endif
#endif

#include "sqlite/sqlite3.h"

typedef struct sqlitedx {
  sqlite3 *conn;
  char version[1024];
  char database[1024];
  sqlite3_stmt *result;
  sqlite3_context *context;
  int num_cols;
  int num_rows;
  int current_tuple;
  int result_step;
} sqlited;


static DBOutWriter stdOutWriter = 0;

//--------------------------------------------------------------------
/*! Evaluates the TOL files received as arguments.
 */
DLLEXPORT(int) sqlite_PutHCIWriter(void (* pF)(const char *))
{
  stdOutWriter = (DBOutWriter) (*pF);
  return 1;
}


char out_msg[32000];


//--------------------------------------------------------------------
/*! Opens the database allocating the handle.
 *  RETURN:  sqlited struct, or NULL in case of error
 */
DLLEXPORT(sqlited *)sqlite_Open( void **args)
{
  sqlited *dbd;
  
  const char *database;
  const char *userName;
  const char *password;
  const char *host;
  int rc;

  userName = args[0];
  password = args[1];
  database = args[2];
  host =     args[3];

  dbd = (sqlited *)malloc(sizeof(sqlited));
  dbd->conn = NULL;
  dbd->version[0]='\0';
  dbd->database[0]='\0';
  dbd->result = NULL;
  dbd->context = NULL;
  dbd->num_cols = 0;
  dbd->num_rows = 0;
  dbd->current_tuple = 0;
  dbd->result_step = 0;

  rc = sqlite3_open_v2(database, &(dbd->conn),
   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if(rc!=SQLITE_OK)
  {
    sprintf(out_msg,"\n<E>\nERROR: [SQLite] Can't open database: %s\n</E>\n", 
      sqlite3_errmsg(dbd->conn));
    stdOutWriter(out_msg);   
    sqlite3_close_v2(dbd->conn);
    return(0);
  }
  strcpy(dbd->version,sqlite3_libversion());
  strcpy(dbd->database,database);
  return dbd;
}

//--------------------------------------------------------------------
DLLEXPORT(int) sqlite_CloseQuery(sqlited *dbd)
{
  int ok = 0;
  int fin = 0;
  if(dbd->result) 
  {
    //res= dbd->result;
    fin = sqlite3_finalize(dbd->result);
    if(fin == SQLITE_OK)
    {
      dbd->result=NULL;
      dbd->result = NULL;
      dbd->context = NULL;
      dbd->num_cols = 0;
      dbd->num_rows = 0;
      dbd->current_tuple = 0;
      dbd->result_step = 0;
      ok=1;
    }
  } 
  return ok;
}

//--------------------------------------------------------------------
/*! Close the direct connection.
 * \return 1 if OK, or 0 if an error ocurred.
 */
DLLEXPORT(int) sqlite_Close(sqlited *dbd)
{
  sqlite_CloseQuery(dbd);
  sqlite3_close_v2(dbd->conn);
  return 1;
}

DLLEXPORT(int) sqlite_GetDBMSName(sqlited *dbd, char *dbmsName, size_t size)
{
  const char* sqliteName = "SQLite";
  strncpy(dbmsName, sqliteName, size);
  return 1;
}

DLLEXPORT(int) sqlite_GetDBMSVersion(sqlited *dbd, char *dbmsVersion, size_t size)
{
  strncpy(dbmsVersion, dbd->version, size);
  return 1;
}

DLLEXPORT(int) sqlite_GetDataBaseName(sqlited *dbd, char *database, size_t size)
{
  strncpy(database, dbd->database, size);
  return 1;
}

//--------------------------------------------------------------------
/*! Opens a query (executing it).
 * \return 1 if success, 0 in case of error.
 */
DLLEXPORT(int) sqlite_OpenQuery(sqlited *dbd, const char *query)
{
  const char *pzTail=0;
  int ok;
  if(dbd->result) { sqlite_CloseQuery(dbd); }
  ok = sqlite3_prepare_v2(
    dbd->conn,       /* Database handle */
    query,           /* SQL statement, UTF-8 encoded */
    strlen(query),   /* Maximum length of zSql in bytes. */
    &(dbd->result),  /* OUT: Statement handle */
    &pzTail           /* OUT: Pointer to unused portion of zSql */
  );

  if(ok!=SQLITE_OK)
  { // Error
    sprintf(out_msg,"\n<E>\nERROR: [SQLite] Can't open query: \n\n%s\n</E>\n", 
      sqlite3_errmsg(dbd->conn));
    stdOutWriter(out_msg);
    return 0; 
  } 
  dbd->num_cols = sqlite3_column_count(dbd->result);
  return 1;
}

//--------------------------------------------------------------------
/*! Prepares the SQL string for execution and executes it.
 * \return Number of rows affected if success, or -1 in case of error.
 * \sa sqlite_OpenQuery
 */
DLLEXPORT(int) sqlite_ExecQuery(sqlited *dbd, const char *query)
{
  char *errmsg=0;
  int ok_exec;
  ok_exec = sqlite3_exec(
    dbd->conn,                                  /* An open database */
    query,                           /* SQL to be evaluated */
    NULL,  /* Callback function */
    NULL,                                    /* 1st argument to callback */
    &errmsg                              /* Error msg written here */
  );
  if(ok_exec!=SQLITE_OK)
  { // Error
    sprintf(out_msg,"\n<E>\nERROR: [SQLite] Can't exec query: \n\n%s\n</E>\n", 
      errmsg);
    stdOutWriter(out_msg);    
    return -1; 
  } 
  dbd->num_rows = sqlite3_changes(dbd->conn);
  return (dbd->num_rows<0) ? 0 : dbd->num_rows;
}

//--------------------------------------------------------------------
/*! fetches the next rowset of data
 */
DLLEXPORT(int) sqlite_GetNext(sqlited *dbd)
{
  if(dbd->result_step==SQLITE_DONE) { return(0); }
  dbd->result_step = sqlite3_step(dbd->result); 
  if(dbd->result_step==SQLITE_ROW) { return(1); }
  if(dbd->result_step==SQLITE_DONE) { return(0); }
  stdOutWriter(sqlite3_errmsg(dbd->conn));
  return(0);
}

//--------------------------------------------------------------------
/*! fetches the first rowset of data
 */
DLLEXPORT(int) sqlite_GetFirst(sqlited *dbd)
{
  return sqlite_GetNext(dbd);
}

//--------------------------------------------------------------------
/*! returns the number of columns
 */
DLLEXPORT(int) sqlite_GetFieldsNumber(sqlited *dbd)
{
  if (dbd->result)
  {
    return (dbd->num_cols);
  }
  return (0);
}

//--------------------------------------------------------------------
/*! returns the type of the field indicated by nfield (starting at 0)
 */
DLLEXPORT(int) sqlite_GetFieldType (sqlited *dbd, int nfield)
{
  int type;    
  int dbType;
  char name[1024];
  int n;
  dbType = TypeDBUnknown;    
  if(!dbd->result) { return(dbType); }
  type = sqlite3_column_type(dbd->result, nfield);
  strcpy(name,sqlite3_column_name(dbd->result, nfield));
  for(n=0;name[n];n++) { name[n] = tolower(name[n]); }  
  if((n>3) & (name[0]=='d') & (name[1]=='t') & (name[2]=='_')) 
  { dbType = TypeDBDate; }
  else if((n>3) & (name[0]=='d') & (name[1]=='d') & (name[2]=='_')) 
  { dbType = TypeDBDate; }
  else if((type==SQLITE_INTEGER) || (type==SQLITE_FLOAT))
  {
    dbType=TypeDBReal; 
  }
  else if(type==SQLITE_TEXT)
  {
    dbType=TypeDBText;
  }
  return dbType;
}

//--------------------------------------------------------------------
/*! returns the name of the field indicated by nfield (starting at 0)
 * 0-index is the bookmark column, 1-index is the first field
 */
DLLEXPORT(int) sqlite_GetFieldName(sqlited *dbd, int nfield, char *fieldName)
{
  if (dbd->result)
  {
    strcpy(fieldName,sqlite3_column_name(dbd->result, nfield));
    return 1;
  }
  sprintf(out_msg,"\n<E>\nERROR: [SQLite] Cannot get %d-th field name: %s\n</E>\n", 
    nfield, sqlite3_errmsg(dbd->conn));
  stdOutWriter(out_msg);
  return 0;
}

//--------------------------------------------------------------------
/*! gets the value of a field as a real
 *  PRECONDITIONS: Caller program must know it is actually a numeric value.
 *                 Otherwise, no error can be returned.
 *  TODO: Check for overflow conditions in sqlite long long data!
 *
 * \return 0 in case of error, 1 to indicate a Real value will be returned
 *           in \p realval, 2 to indicate a NULL value was red.
 */
DLLEXPORT(int) sqlite_GetAsReal(sqlited *dbd, int nfield, long double *realval)
{
  if (dbd->result) 
  {
    *realval = sqlite3_column_double(dbd->result, nfield); 
    return 1;
  }
  else
  {
    stdOutWriter("Error: sqlite_GetAsReal: Not a Numeric value.\n");
    return 0;
  }
}
//--------------------------------------------------------------------
/*! gets the value of a field as a text
  Optimized to fit the length of the data field.
  It has not been proben with rows of more than 64KB, but should work.
 */
DLLEXPORT(int) sqlite_GetAsText(sqlited *dbd, int nfield, char **txt_val)
{
  if (dbd->result) 
  {
    *txt_val = (char*)sqlite3_column_text(dbd->result, nfield);
    return 1;
  }
  else
  {
    stdOutWriter("Error: sqlite_GetAsText: Not a Text value.\n");
    return 0;
  }

  return 0;
}

DLLEXPORT(int) sqlite_FreeText(sqlited *dbd, char **txt_val) { return 1; }

//--------------------------------------------------------------------
/*! gets the value of a field as a date
 */
DLLEXPORT(int) sqlite_GetAsDate(sqlited *dbd, int nfield, struct dateStruct **dateval)
{
  int err = 0;
  
  if(!dbd->result) { err = 1; }
  else
  {
    char s1=0,s2=0,s3=0,s4=0,s5=0,s6=0;  
    int y,m,d,h,i,s,ms; 
    const char *txt_val = sqlite3_column_text(dbd->result, nfield);
    if(!txt_val) { err=2; }
    else
    {
      int len = strlen(txt_val);
      if(len==10) 
      { 
        sscanf(txt_val,"%d%c%d%c%d",
          &y, &s1, &m, &s2, &d);
        (*dateval)->year  = (unsigned short)y;
        (*dateval)->month = (unsigned short)m;
        (*dateval)->day   = (unsigned short)d;
        (*dateval)->hour = (*dateval)->minute = (*dateval)->second = (*dateval)->msecond = 0;
      }
      else if(len==19)
      {
        sscanf(txt_val,"%d%c%d%c%d%c%d%c%d%c%d",
          &y, &s1, &m, &s2, &d, &s3, &h, &s4, &i, &s5, &s);
        (*dateval)->year   = (unsigned short)y;
        (*dateval)->month  = (unsigned short)m;
        (*dateval)->day    = (unsigned short)d;
        (*dateval)->hour   = (unsigned short)h;
        (*dateval)->minute = (unsigned short)i;
        (*dateval)->second = (unsigned short)s;
        (*dateval)->msecond = 0;
      }
      else if((len==22)||(len==23))
      {
        sscanf(txt_val,"%d%c%d%c%d%c%d%c%d%c%d%c%d",
          &y, &s1, &m, &s2, &d, &s3, &h, &s4, &i, &s5, &s, &s6, &ms);
        (*dateval)->year    = (unsigned short)y;
        (*dateval)->month   = (unsigned short)m;
        (*dateval)->day     = (unsigned short)d;
        (*dateval)->hour    = (unsigned short)h;
        (*dateval)->minute  = (unsigned short)i;
        (*dateval)->second  = (unsigned short)s;
        (*dateval)->msecond = (unsigned short)ms;
      }
      else { err = 3; }
    }
  }
  if(err)
  {
    stdOutWriter("Error: sqlite_GetAsDate: Not a Date value.\n");
    return 0;
  }
  return 1;
}
