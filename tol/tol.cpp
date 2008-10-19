/* tol.cpp: Utility, evaluate a set of TOL files.
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

#ifdef HAVE_CONFIG_H
#  if (defined(_MSC_VER) || defined(WIN32))
#    include <win_tolinc.h>
#  else
#    include <tol/config.h>
#  endif
#endif

#include <tol/tol_init.h>

//--------------------------------------------------------------------
  void bye(void)
//--------------------------------------------------------------------
{
//printf("\nBYE");
  SetTOLEnd();
}

//--------------------------------------------------------------------
int main(int argc, char *argv[], char *env[])

/*! Evaluates the TOL files receives as parameters.
 */
//--------------------------------------------------------------------
{
//FPEControl();
  atexit(bye);
  InitializeFromMainArgs(argc, argv, env);
  return(0);
}



