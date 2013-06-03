/* tol_bdspool.h: Class and functions header. TOL Language.

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


#ifndef TOL_BDSPOOL_H
#define TOL_BDSPOOL_H 1

#include <stdlib.h>

class BSet;
class BList;

#define MAX_TEXT_FIELD_LENGTH 2048

typedef char TDBTextField[MAX_TEXT_FIELD_LENGTH];

enum TypeDataBase
{
    TypeDBUnknown = 0,
    TypeDBText	  = 1,
    TypeDBDate	  = 2,
    TypeDBReal	  = 3
};

extern int dbOpenODBC(const char* alias, const char *user, const char *pwd);
extern int dbOpenDirect(const char* alias, const char *user, const char *pwd, BSet * dbset=0);
extern int dbClose(const char *alias);
extern int dbGetDBMSName(char *dbms, size_t size);
extern int dbGetDBMSVersion(char *version, size_t size);
extern int dbGetDataBaseName(char *database, size_t size);
extern int dbActivate(const char *alias);
extern BList *dbGetOpened(const char *dbtype);
extern int dbOpenQuery(const char *query);
extern int dbExecQuery(const char *query);
extern int dbCloseQuery(void);
extern int dbGetFirst(void);
extern int dbGetNext(void);
extern int dbGetFieldsNumber(void);
extern int dbGetFieldType(int nfield);
extern int dbGetFieldName(int nfield, char *fieldName);
extern int dbGetAsReal(int nfield, long double &valor);
extern int dbGetAsText(int nfield, unsigned char **value);
extern int dbFreeText(unsigned char **value);
extern int dbGetAsDate(int nfield,
		       unsigned short& year,
		       unsigned short& month,
		       unsigned short& day,
		       unsigned short& hour,
		       unsigned short& minute,
		       unsigned short& second,
		       unsigned short& msecond);
extern int dbGetDBMSName(char *dbmsName, size_t size);
extern int dbGetDBMSVersion(char *dbmsVersion, size_t size);
extern int dbGetDataBaseName(char *database, size_t size);

#endif // TOL_BDSPOOL_H
