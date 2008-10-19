/* toltcl.h - This header file contains the function declarations 
              needed for  all of the source files in this package.
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

#ifndef TOLTCL_H
#define TOLTCL_H 1

#include <tt_common.h>
#include <tcl.h>

/*
 * Windows needs to know which symbols to export. Unix does not.
 * BUILD_TOLTCL should be undefined for Unix.
 */

#define TOLTCL_MAJOR_VERSION 	1
#define TOLTCL_MINOR_VERSION 	1
#define TOLTCL_RELEASE_SERIAL	7
#ifdef PACKAGE_VERSION
 #define TOLTCL_VERSION PACKAGE_VERSION
#else
 #define TOLTCL_VERSION          "1.1.7"
#endif

#ifdef BUILD_TOLTCL
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#endif /* BUILD_TOLTCL */

/*
 * Only the _Init function is exported.
 */

EXTERN int	Toltcl_Init _ANSI_ARGS_((Tcl_Interp * interp));

#endif /* TOLTCL_H */
