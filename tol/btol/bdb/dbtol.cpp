/* dbtol.cpp: TOL database interface. TOL Language.

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

#if defined(_MSC_VER)
  #include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h>
#include <tol/tol_bdspool.h>
#include <tol/tol_barray.h>
#include <tol/tol_bout.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>

#ifdef UNIX
#  include <dlfcn.h>
#  define GET_FUNC dlsym
#  define UNLOAD_MODULE dlclose
#  define CHECK_ERROR(f) dlerror()
#  define LOAD_MODULE(f) dlopen(file,RTLD_NOW|RTLD_GLOBAL)
#  define EXTENSION ".so"
   typedef void* hdl_type;
#else // Windows
char *checkF(void *f) 
{
  if (f==NULL) 
    return "Could not find call.";
  else return NULL;
}
#  include <windows.h>
#  define GET_FUNC GetProcAddress
#  define UNLOAD_MODULE FreeLibrary
#  define CHECK_ERROR(f) checkF(f)
#  define LOAD_MODULE(f) LoadLibrary(f)
#  define EXTENSION ".dll"
   typedef HINSTANCE hdl_type;
#endif // UNIX

#ifdef __USE_DBTIME_MONITOR__
static BDat dbTotalCPU_ = 0;
static double dbInitPointCPUClock_; //!< Auxiliar CPU clock annotation
static void updateDBTotalCPUTime(double ticks) {
  static double dbTotalCPUClock_ = 0;
  dbTotalCPUClock_ += ticks;
  dbTotalCPU_ = BTimer::ClockToSecond((clock_t) dbTotalCPUClock_);
}
BDat& DBTotalCPUTime() { return dbTotalCPU_; }
#endif // __USE_DBTIME_MONITOR__

BTraceInit("dbtol.cpp");

/*! used to read and load Timestamp from databases
 */
struct dateStruct
{
  unsigned short int year;
  unsigned short int month;
  unsigned short int day;
  unsigned short int hour;
  unsigned short int minute;
  unsigned short int second;
  unsigned short int msecond;
};

typedef int (*dbPutHCIWriter_fpt)(void**);
typedef void* (*dbOpen_fpt)(void**);
typedef int (*dbClose_fpt)(void*);
typedef int (*dbOpenQuery_fpt)(void *, const char *);
typedef int (*dbExecQuery_fpt)(void *, const char *);
typedef int (*dbCloseQuery_fpt)(void *);
typedef int (*dbGetFirst_fpt)(void *);
typedef int (*dbGetNext_fpt)(void *);
typedef int (*dbGetFieldsNumber_fpt)(void *);
typedef int (*dbGetFieldType_fpt)(void *, int);
typedef int (*dbGetFieldName_fpt)(void *, int, char *);
typedef int (*dbGetAsReal_fpt)(void *, int, long double *);
typedef int (*dbGetAsText_fpt)(void *, int, unsigned char**);
typedef int (*dbFreeText_fpt)(void *, unsigned char**);
typedef int (*dbGetAsDate_fpt)(void *, int, struct dateStruct **);

/*! Each variable of type struct dbm_hdl represents a handler of a particular
 * database manager, what means, several connections to different databases
 * of the same db manager have the same dbm_hdl. 
 */
typedef struct dbm_hdl 
{
  hdl_type handler; //!< the connection with each loaded module
  dbPutHCIWriter_fpt dbPutHCIWriter_fp;
  dbOpen_fpt dbOpen_fp;
  dbClose_fpt dbClose_fp;
  dbOpenQuery_fpt dbOpenQuery_fp;
  dbExecQuery_fpt dbExecQuery_fp;
  dbCloseQuery_fpt dbCloseQuery_fp;
  dbGetFirst_fpt dbGetFirst_fp;
  dbGetNext_fpt dbGetNext_fp;
  dbGetFieldsNumber_fpt dbGetFieldsNumber_fp;
  dbGetFieldType_fpt dbGetFieldType_fp;
  dbGetFieldName_fpt dbGetFieldName_fp;
  dbGetAsReal_fpt dbGetAsReal_fp;
  dbGetAsText_fpt dbGetAsText_fp;
  dbFreeText_fpt dbFreeText_fp;
  dbGetAsDate_fpt dbGetAsDate_fp;
} DBM_hdl;

// char array of DBM module aliases
#define DBM_ALIASES { "odbc", "mysql", "postgres" }
// char array of DBM module file names
#define DBM_MODULES { "tolodbc%d%s\0", "tolmysql%d%s\0", "tolpgsql%d%s\0" }
// char array of DB Manager names
#define DBM_NAMES { "ODBC", "MySQL", "PostgreSQL" }
// indexes for files string in array above
#define ODBC  0 
#define MYSQL 1 
#define PGSQL 2 
#define MAX_DBM_HANDLERS 3

const char *dbm_aliases_[] = DBM_ALIASES;
const char *dbm_modules_[] = DBM_MODULES;
const char *dbm_names_[] = DBM_NAMES;
DBM_hdl *dbm_handlers_[3] = { NULL, NULL, NULL };

typedef struct db_list //!< struct to List opened databases
{
  char *alias; //!< name under which tol will handle this db connection
  void *dbd; //!< database descriptor loaded from dynamic modules at dbOpen time
  //  DBM_hdl *dbm_hdl; //dl handler
  int hdl_idx; //!< handler index to get it from 'dbm_handlers_'
  struct db_list *prev, *next;
} DB_list;

DB_list *opened_db_ = NULL; //!< Static list of opened databases
DB_list *active_db_ = NULL; //!< Pointer to active database

static BDat DBRTrim_ = 0; //!< Include DBRTrim option
BDat& DBRTrim() { return(DBRTrim_); }

//-------------------------------------------------------------------
#define FUNCTION_LOAD_DECLARATION(SUFFIX) \
  sprintf(function, "%s_%s", dbm_aliases_[index], #SUFFIX); \
  dbm_hdl->db##SUFFIX##_fp = (db##SUFFIX##_fpt) GET_FUNC(dbm_hdl->handler, function); \
  if((module_error = CHECK_ERROR(dbm_hdl->db##SUFFIX##_fp)) != NULL) { \
    loadFunctionsError(index, function, module_error); \
    return 0; \
  }
//-------------------------------------------------------------------

//-------------------------------------------------------------------
void loadFunctionsError(int index, const char* fname, const char* error)
{
  char file[24], *module_error = NULL;

  memset(file, '\0', 24); // clean buffer
  module_error = strdup(error);
  sprintf(file, dbm_modules_[index], DB_MODULES_SONAME, EXTENSION);
  Error(I2("Function " + fname + " not available in " + file + " module file\n",
	   "La función " + fname + " no esta disponible en el módulo " + file + ".\n"));
  Dump(module_error);
  free(module_error);
}

//-------------------------------------------------------------------
int loadFunctions(int index)
{
  char function[30], *module_error = NULL;
  DBM_hdl *dbm_hdl = dbm_handlers_[index];

  memset(function, '\0', 30); // clean buffer

  FUNCTION_LOAD_DECLARATION(PutHCIWriter);
  FUNCTION_LOAD_DECLARATION(Open);
  FUNCTION_LOAD_DECLARATION(Close);
  FUNCTION_LOAD_DECLARATION(OpenQuery);
  FUNCTION_LOAD_DECLARATION(ExecQuery);
  FUNCTION_LOAD_DECLARATION(CloseQuery);
  FUNCTION_LOAD_DECLARATION(GetFirst);
  FUNCTION_LOAD_DECLARATION(GetNext);
  FUNCTION_LOAD_DECLARATION(GetFieldsNumber);
  FUNCTION_LOAD_DECLARATION(GetFieldType);
  FUNCTION_LOAD_DECLARATION(GetFieldName);
  FUNCTION_LOAD_DECLARATION(GetAsReal);
  FUNCTION_LOAD_DECLARATION(GetAsText);
  FUNCTION_LOAD_DECLARATION(FreeText);
  FUNCTION_LOAD_DECLARATION(GetAsDate);

  return 1;
}

//-------------------------------------------------------------------
/*! Try to load the file module indexed in position \a index from
 * global variable \a db_modules_ and if success, creates the 
 * dbm_handler entry and load functions pointers of that module.
 * \return Returns 1 if the load was successful, or 0 if it failed.
 */
int loadModule(int index)
{
  int rc=0;
  char file[24], *module_error;
  hdl_type handler = NULL;

  memset(file, '\0', 24); // clean buffer
  // Load module
  sprintf(file, dbm_modules_[index], DB_MODULES_SONAME, EXTENSION);
  handler = LOAD_MODULE(file);
  module_error = CHECK_ERROR(handler);

  // check for errors
  if(module_error) {
    module_error = strdup(module_error);
    Error(I2("Module "+file+" for " + dbm_names_[index] + " DB access is not available.\n",
	   "El módulo "+file+" de acceso a " + dbm_names_[index] + " no está disponible.\n"));
    Dump(module_error);
    free(module_error);
    return 0;
  } 

  // create a DBM_hdl entry and loads handler symbols
  if(handler) {
    dbm_handlers_[index] = (DBM_hdl*) malloc(sizeof(DBM_hdl));
    if(dbm_handlers_[index]==NULL) {
      Error("Out of memory\n");
      return 0;
    }
    dbm_handlers_[index]->handler = handler;
    rc = loadFunctions(index); // load symbols from handler
    if(rc) // set function to output messages from DB driver to Tol
      rc = dbm_handlers_[index]->dbPutHCIWriter_fp((void**) HCIWriteString);
  } 

  return rc;
}

//-------------------------------------------------------------------
int dbOpenODBC(const char* alias, const char* user, const char* pwd)
{
  DB_list *new_entry=NULL, *last_entry=NULL, *db_list=opened_db_;
  DBM_hdl *dbm_hdl;
  void **args=NULL;

#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif

  // Verify whether the ODBC handler is created
  if(!dbm_handlers_[ODBC] && !loadModule(ODBC)) return 0;

  // Verify whether alias parameter is already connected
  while(db_list) {
    if(!strcmp(alias, db_list->alias)) {
      Warning(I2("Database alias '" + alias + "' already available.\n",
		 "El alias '" + alias + "' ya estaba disponible.\n"));
      return 0;
    }
    last_entry = db_list;
    db_list = db_list->next;
  }

  // Create new DB Connection
  args = (void **) calloc(3, sizeof(void*));
  if(args==NULL) { Error("Out of memory\n"); return 0; }

  args[0] = (void *) strdup(user);
  args[1] = (void *) strdup(pwd);
  args[2] = (void *) strdup(alias);

  new_entry = (DB_list*) malloc(sizeof(struct db_list));
  if(new_entry==NULL) { Error("Out of memory\n"); return 0; }

  new_entry->alias = strdup(alias);
  //  new_entry->dbm_hdl = dbm_handlers_[ODBC];
  new_entry->hdl_idx = ODBC;
  dbm_hdl = dbm_handlers_[ODBC];
  new_entry->dbd = dbm_hdl->dbOpen_fp(args);
  if(new_entry->dbd==NULL) {
    Error(I2("Cannot connect with alias '" + alias + "'.\n",
	     "No se pudo conectar con el alias '" + alias + "'.\n"));
    free(new_entry);
    free(args);
    return 0;
  }
  new_entry->next = NULL;
  
  // append 'new_entry' into 'opened_db_'
  if(last_entry) {
    last_entry->next = new_entry;
    new_entry->prev = last_entry;
  } else {
    opened_db_ = new_entry;
    new_entry->prev = NULL;
  }

  active_db_ = new_entry;
  free(args);

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif

  return 1;
}

//-------------------------------------------------------------------
int dbOpenDirect(const char* alias, const char* user, const char* pwd, BSet* dbset)
{
  DB_list *new_entry=NULL, *last_entry=NULL, *db_list=opened_db_;
  DBM_hdl *dbm_hdl;
  void **args=NULL;
  char *dbset_alias=Text( (*dbset)[1] );
  int index=0, i=0;

#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif

  // Verify which DBM_ALIASES comes in BSet *dbset
  for(i=0; dbm_aliases_[i] && i<MAX_DBM_HANDLERS; i++)
    if(!strcmp(dbset_alias, dbm_aliases_[i])) 
      { index=i; break; }

  // Verify whether the DBM_hdl pointed by index is created
  if(!dbm_handlers_[index] && !loadModule(index)) return 0;

  // Verify whether alias parameter is already connected
  while(db_list) {
    if(!strcmp(alias, db_list->alias)) {
      Warning(I2("Database alias '" + alias + "' already available.\n",
		 "El alias '" + alias + "' ya estaba disponible.\n"));
      return 0;
    }
    last_entry = db_list;
    db_list = db_list->next;
  }
  
  // Create new DB Connection
  args = (void **) calloc(5, sizeof(void*));
  if(args==NULL) { Error("Out of memory\n"); return 0; }

  args[0] = (void *) strdup(user);
  args[1] = (void *) strdup(pwd);
  args[2] = (void *) Text( (*dbset)[2] ); // database
  args[3] = (void *) Text( (*dbset)[3] ); // host
  args[4] = (dbset->Card()>=4) ? (void *) Text( (*dbset)[4] ) : NULL; // port
  
  new_entry = (DB_list*) malloc(sizeof(struct db_list));
  if(new_entry==NULL) { Error("Out of memory\n"); return 0; }

  new_entry->alias = strdup(alias);
  new_entry->hdl_idx = index;
  dbm_hdl = dbm_handlers_[index];
  new_entry->dbd = dbm_hdl->dbOpen_fp(args);
  if(new_entry->dbd==NULL) {
    Error(I2("DB Alias '" + alias + "' not created.\n",
	     "Alias de Base de Datos '" + alias + "' no creado.\n"));
    free(new_entry);
    free(args);
    return 0;
  }
  new_entry->next = NULL;
  
  // append 'new_entry' into 'opened_db_'
  if(last_entry) {
    last_entry->next = new_entry;
    new_entry->prev = last_entry;
  } else {
    opened_db_ = new_entry;
    new_entry->prev = NULL;
  }

  active_db_ = new_entry;
  free(args);
#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return 1;
}

//-------------------------------------------------------------------
int dbClose(const char* alias)
{
  int rc;
  DB_list *entry=opened_db_;
  DBM_hdl *dbm_hdl = NULL;

#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif

  if(!opened_db_) { // there are no more connections
    Std(I2("DBConns is empty. No more DB connections.\n",
	   "DBConns esta vacío. No quedan conexiones a BB.DD.\n"));
    return 0;
  }

  if(strlen(alias)==0)      // close active_db_ connection
    entry = active_db_;
  else 
  {                         // close 'alias' connection
    while(entry)
      if(!strcmp(alias, entry->alias))
	break;
      else entry = entry->next;
    
    if(!entry) 
      Error(I2("Alias '" + alias + "' does not exist.\n",
	       "El alias '" + alias + "' no existe.\n"));
  }
  
  if(!entry) return 0;      // there's no 'alias' or no connections
  
  // Close DB, and release resources
  dbm_hdl = dbm_handlers_[entry->hdl_idx];
  rc = dbm_hdl->dbClose_fp(entry->dbd);
  if(!rc) {
    Error(I2("Cannot close '" + alias + "' db connection.\n",
	     "No se pudo cerrar la conexión '" + alias + "'\n"));
  }
  entry->dbd = NULL;
  free(entry->alias);
  entry->alias = NULL;

  // rearrange 'opened_db_'
  if(entry->prev==NULL) { // ==> it is the opened_db_ 1st entry
    opened_db_ = entry->next;
    if(opened_db_) opened_db_->prev = NULL;
  } 
  else entry->prev->next = entry->next;
  
  // rearrange 'active_db_'
  if(entry==active_db_) active_db_ = opened_db_;

  free(entry);

  if(active_db_) {
    Std(I2("Active DB Connection: '" + active_db_->alias + "'.\n",
	   "Conexión con BB.DD. activa: '" + active_db_->alias + "'.\n"));
  } else {
    Std(I2("DBConns is empty. No more DB connections.\n",
	   "DBConns esta vacío. No quedan conexiones a BB.DD.\n"));
  }

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbActivate(const char *alias)
{
  int rc=0;
  DB_list *db_list=opened_db_;

#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif

  if(!opened_db_)
  {
    Std(I2("DBConns is empty. No more DB connections.\n",
	   "DBConns esta vacío. No quedan conexiones a BB.DD.\n"));
    return 0;
  }
  else
    while(db_list) {
      if(!strcmp(alias, db_list->alias)) {
	active_db_ = db_list;
	rc = 1;
	break;
      }
      else db_list = db_list->next;
    }

  if(!rc)
    Error(I2("Alias '" + alias + "' does not exist.\n",
	     "El alias '" + alias + "' no existe.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
BList *dbGetOpened(const char *type)
{
  DB_list *db_list=opened_db_;
  BList *set_list=NULL, *set_list_aux=NULL;
  BSyntaxObject *bso_alias=NULL, *bso_type=NULL, *bso_status=NULL;
  
  while(db_list) 
  {
    if((strlen(type)==0) || (!strcmp(dbm_names_[db_list->hdl_idx], type)))
    {
      BList *row_list=NULL, *row_list_aux=NULL;

      bso_alias = BContensText::New("", db_list->alias, "");
      bso_alias->PutName("alias");
      
      LstFastAppend(row_list, row_list_aux, bso_alias);
    
      bso_type = BContensText::New("", dbm_names_[db_list->hdl_idx], "");
      bso_type->PutName("type");
      
      LstFastAppend(row_list, row_list_aux, bso_type);
    
      if(active_db_==db_list)
	bso_status = BContensDat::New("", 1, "");
      else bso_status = BContensDat::New("", 0, "");
      bso_status->PutName("status");
      
      LstFastAppend(row_list, row_list_aux, bso_status);
    
      BSet row;
      row.RobStruct(row_list, NULL, BSet::Structured);
      BUserSet *db_conn = BContensSet::New("", row, db_list->alias);
      db_conn->PutName(db_list->alias);
      
      LstFastAppend(set_list, set_list_aux, db_conn);
    }
    
    db_list = db_list->next;
  }

  return set_list;
}

//-------------------------------------------------------------------
/*! Opens a query and execute it.
 * \return Returns 1 if success, 0 if there is any error
 */
int dbOpenQuery(const char* query)
{
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbOpenQuery_fp(active_db_->dbd, query);
  if(!rc) {
#if defined(_DEBUG)
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_OpenQuery' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_OpenQuery' en el módulo "+ 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));
#endif
    Error(I2("SQL query failed:\n" + query,
	     "La consulta SQL ha fallado:\n" + query));
  }

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
/*! Prepares the SQL string for execution and executes it.
 * \return Returns number of rows affected if success, or -1 if there 
 * is any error.
 * \sa odbc_OpenQuery
 */
int dbExecQuery(const char *query) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbExecQuery_fp(active_db_->dbd, query);
  if( rc == -1 ) {
#if defined(_DEBUG)
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_ExecQuery' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_ExecQuery' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));
#endif
    Error(I2("SQL query failed:\n" + query,
	     "La consulta SQL ha fallado:\n" + query));
  }

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbCloseQuery(void) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbCloseQuery_fp(active_db_->dbd);
  if(!rc)
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_CloseQuery' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_CloseQuery' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));
  
#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
/*! Fetches the 1st. rowset of data from the result set.
 * \return Returns  1 in case of success operation 
 *                  0 in case of End of Cursor
 *                 -1 in case of error
 */
int dbGetFirst(void) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetFirst_fp(active_db_->dbd);
  if( rc == -1 )
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFirst' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFirst' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbGetNext(void) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetNext_fp(active_db_->dbd);
  if( rc == -1 )
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetNext' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetNext' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbGetFieldsNumber(void) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetFieldsNumber_fp(active_db_->dbd);
  if( rc == -1 )
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFieldsNumber' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFieldsNumber' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbGetFieldType(int nfield) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;
  
  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetFieldType_fp(active_db_->dbd, nfield);
  if( rc == -1 )
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFieldType' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFieldType' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));  
  
#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbGetFieldName(int nfield, char *fieldName) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetFieldName_fp(active_db_->dbd, nfield, fieldName);
  if(!rc)
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFieldName' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetFieldName' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbGetAsReal(int nfield, long double &value) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetAsReal_fp(active_db_->dbd, nfield, &value);
  if(!rc)
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetAsReal' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetAsReal' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbGetAsText(int nfield, unsigned char **value) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetAsText_fp(active_db_->dbd, nfield, value);

  if(rc && *value && (DBRTrim().Value()))
  {
    int len = 0;
    while((*value)[len]!='\0') { len++; }
    do len--; while((*value)[len]==' ');
    len++;
    (*value)[len]='\0';
  } 
  else if(!rc)
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetAsText' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetAsText' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbFreeText(unsigned char **value)
{
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbFreeText_fp(active_db_->dbd, value);
  if(!rc)
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_FreeText' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_FreeText' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));

#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}

//-------------------------------------------------------------------
int dbGetAsDate(int nfield, 
		unsigned short& year, unsigned short& month, 
		unsigned short& day, unsigned short& hour, 
		unsigned short& minute, unsigned short& second, 
		unsigned short& msecond) 
{ 
#ifdef __USE_DBTIME_MONITOR__
  double dbInitPointCPUClock_ = clock();
#endif
  if(!active_db_) {
    Error(I2("There is no database connection.\n",
	     "No hay conexiones a BB.DD.\n"));
    return 0;
  }

  int rc;
  DBM_hdl *dbm_hdl = NULL;
  struct dateStruct *result = NULL;

  result = (struct dateStruct*)malloc(sizeof(struct dateStruct));
  if(result==NULL) {
    Error("Out of memory\n");
    return 0;
  }
  result->year = 0; result->month = 0;  result->day = 0;
  result->hour = 0; result->minute = 0; result->second = 0; 
  result->msecond = 0;

  dbm_hdl = dbm_handlers_[active_db_->hdl_idx];
  rc = dbm_hdl->dbGetAsDate_fp(active_db_->dbd, nfield, &result);

  if(!rc) {
    Error(I2("function '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetAsDate' from " + dbm_names_[active_db_->hdl_idx] + 
	     " DB module failed.\n", 
	     "la función '" + dbm_aliases_[active_db_->hdl_idx] + 
	     "_GetAsDate' en el módulo " + 
	     dbm_names_[active_db_->hdl_idx] + " ha fallado.\n"));
  } else {
    year    = result->year;
    month   = result->month;
    day     = result->day;
    hour    = result->hour;
    minute  = result->minute;
    second  = result->second;
    msecond = result->msecond;
  }

  free(result);
#ifdef __USE_DBTIME_MONITOR__
  updateDBTotalCPUTime(clock()-dbInitPointCPUClock_);
#endif
  return rc;
}
