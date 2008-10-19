/* tclutils.cpp: This file define a set of utils functions.
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


#include <tt_tclutils.h>

/*!
 *  EvalScript::EvalScript --
 *
 *    class constructor. Setup the script and the arguments
 *    to be used in next evaluations.
 *
 */
EvalScript::EvalScript( Tcl_Interp * interp,
                        Tcl_Obj * script, int objc, Tcl_Obj * objv[] )
{
  int itemc;
  Tcl_Obj ** itemv;
  int i;

  m_interp = interp;
  m_status = Tcl_ListObjGetElements( interp, script, &itemc, &itemv );
  if ( m_status != TCL_OK ) {
    m_objc = 0;
    m_objv = NULL;
  }
  else {
    m_xobjc = objc;
    m_xobjv = objv;
    m_objc = itemc + objc;
    m_objv = (Tcl_Obj**)Tcl_Alloc( sizeof(Tcl_Obj*) * m_objc );
    for ( i = 0; i < itemc; i++ ) {
      m_objv[i] = itemv[i];
      Tcl_IncrRefCount( m_objv[i] );
    }
    m_status = TCL_OK;
  }
}

/*!
 *  EvalScript::~EvalScript --
 *
 */
EvalScript::~EvalScript()
{
  int i;

  if ( m_objv ) {
    for ( i = 0; i < m_objc - m_xobjc; i++ )
      Tcl_DecrRefCount( m_objv[i] );
    Tcl_Free( (char*)m_objv );
    m_objc = 0;
    m_objv = NULL;
  }
}

/*!
 *  EvalScript::Eval --
 *
 *    Eval the script and arguments.
 *
 */
int EvalScript::Eval()
{
  if ( m_objc ) {
    /* Refresh the extra arguments value */
    for ( int j = 0, i = m_objc-m_xobjc; i < m_objc; i++, j++ )
      m_objv[i] = m_xobjv[j];
    m_status = Tcl_EvalObjv( m_interp, m_objc, m_objv, TCL_EVAL_GLOBAL);
  }
  return m_status;
}

Tcl_Obj * NewExternalStringObj( char * str )
{
  Tcl_DString dstr;
  Tcl_Obj * obj;

  Tcl_DStringInit(&dstr);
  Tcl_ExternalToUtfDString(NULL, str, -1, &dstr);
  obj = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);
  return obj;
}
