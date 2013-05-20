/* tol_memhandler.h: BGrammar (gra) class and functions header
                   of GNU/TOL language.

   Copyright (C) 2003-2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_MEMHANDLER_H
#define TOL_MEMHANDLER_H 1

#ifdef __cplusplus
# if (__USE_POOL__==__POOL_NONE__)
#  define DeclareClassNewDelete(ANY_)
#  define RedeclareClassNewDelete(ANY_)
#  define UndeclareClassNewDelete
# elif (__USE_POOL__==__POOL_BFSMEM__)
#  include <tol/tol_bfsmem.h>
# endif
#endif


#endif  //TOL_MEMHANDLER_H
