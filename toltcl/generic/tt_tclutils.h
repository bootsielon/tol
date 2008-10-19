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

#ifndef TT_TCLUTILS_H
#define TT_TCLUTILS_H 1

#include <tt_common.h>
#include <tcl.h>

class EvalScript {
public:
  EvalScript( Tcl_Interp * interp, 
	      Tcl_Obj * script, int objc, Tcl_Obj * objv[] );
  ~EvalScript();
  int Eval();
  int Status() {
    return m_status;
  }
private:
  Tcl_Interp * m_interp;
  int m_objc;
  Tcl_Obj ** m_objv;
  int m_xobjc;
  Tcl_Obj ** m_xobjv;
  int m_status;
};

Tcl_Obj * NewExternalStringObj( char * str );

#endif /* Tt_TCLUTILS_H */
