/* tolCint.h: API for CINT, a interpreter to C and C++.

   http://root.cern.ch/drupal/content/cint
   http://root.cern.ch/viewvc/trunk/cint/doc/ref.txt

   Copyright (C) 2010 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_CINT_H
#define TOL_CINT_H

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_btext.h>

//#pragma pack(16)
#include <Api.h>
#include <G__ci.h>
//#pragma pack(4)


//hooks to dynamic linked functions
void Cint_scratch_all (void);
char* Cint_lasterror_filename (void);
int Cint_lasterror_linenum (void);
int Cint_init_cint (G__CONST char* command);
void Cint_set_errmsgcallback (void* p);
void Cint_setautoconsole (int autoconsole);
void Cint_SetCINTSYSDIR (char* cintsysdir);
int Cint_loadfile (G__CONST char* filename);
int Cint_unloadfile (G__CONST char* filename);
G__value Cint_exec_tempfile (G__CONST char *file);
G__value Cint_exec_text (G__CONST char *unnamedmacro);
long Cint_int (G__value buf);
double Cint_double (G__value buf);
G__value Cint_calc (G__CONST char *expr);
int Cint_deletevariable (G__CONST char* varname);

//TOL-CINT API initialization
int Cint_initialize(const BText& filePath);

//#define DO_TRACE_CINT_DECIMAL;
#ifdef DO_TRACE_CINT_DECIMAL 
#define TRACE_CINT_DECIMAL \
  Cint_exec_text(BText(" double x = 2.1112; printf(\"")+__FILE__+":"+__LINE__+" [x=%.15lg]\n\",x); ");
#else
#define TRACE_CINT_DECIMAL
#endif

#endif /*TOL_CINT_H*/
