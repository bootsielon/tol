/* tol_init.h: Kernel initialization.
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_INIT_H
#define TOL_INIT_H 1

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h> 


BEGIN_DECLS

//calledProgram should be argv[0] in main function
TOL_API void  InitializeFromMainArgs(int argc, char *argv[], char *env[]);
TOL_API void  LoadInitLibrary       (char* calledProgram);
TOL_API void  InitTolKernel         (int lang, const char* vmode);

typedef void tgsl_error_handler 
  (const char * reason, const char * file, int line, int gsl_errno);

TOL_API const char * Tol_gsl_strerror (const int gsl_errno);
TOL_API tgsl_error_handler * 
  Tol_gsl_set_error_handler (tgsl_error_handler * new_handler);

TOL_API const char* TOLVersionShortName();
TOL_API const char* TOLVersion();
TOL_API const char* TOLVersionBuild();
TOL_API const char* TOLSessionPath ();
TOL_API const char* TOLCredits();
TOL_API const char* TOLContribAcknowledgements();
TOL_API char* TolFindExecutable(const char* argv0);
TOL_API const char* TOLCppRoot();

TOL_API int  TOLHasBeenInitialized();
TOL_API int  TOLHasBeenEnded();
TOL_API void SetTOLEnd();
TOL_API void UnexpectedTolEnd(void);

END_DECLS

#endif // TOL_INIT_H
