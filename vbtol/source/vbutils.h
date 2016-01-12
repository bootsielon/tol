/* tt_tclutils.h - This file define a set of utils functions.
                   GNU/tolTcl Library

   Copyright (C) 2001, 2002, 2003 - Bayes Decisión, SL

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

#ifndef VB_UTILS_H
#define VB_UTILS_H 1

#include <string.h>
#include <windows.h>

#if defined(_MSC_VER)
  #include <config.h>
  #include <tol_init.h>
  #include <tol_bmatgra.h>
#else
  #include <tol/config.h>
  #include <tol/tol_init.h>
  #include <tol/tol_bmatgra.h>
#endif

#if defined(_MSC_VER)
  #define DLLEXPORT(f) f __declspec(dllexport) CALLBACK
#else
  #define DLLEXPORT(f) extern "C" __declspec(dllexport) f __stdcall
#endif

class BList;

extern BList VB_stack;

void VB_concat(BList* head, BList* tail);


typedef void (__stdcall *FN_PTR)(BSTR str);

extern FN_PTR VB_Writer;

void VBTol_gsl_error_handler(const char * reason, const char * file, int line,
	 						 int gsl_errno);

void VBTol_HciWriter(const BText & str);


#endif
