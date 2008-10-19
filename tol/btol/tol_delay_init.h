/* tol_delay_init.h: Support for variable number of parameters in functions.
                     GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

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

#ifndef __DELAY_INIT__
#define __DELAY_INIT__ 1

#define DELAY_LIST_INCR 10

#ifndef DEFAULT_INIT_PRIORITY
#  define DEFAULT_INIT_PRIORITY 1000
#endif

void *	
__delay_init(void ** obj, 
	     void * (*clone)(), 
	     int key = DEFAULT_INIT_PRIORITY );

#endif // __DELAY_INIT__
