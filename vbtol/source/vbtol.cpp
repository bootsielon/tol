/* VBTol.cpp: This file implements a Visual Basic interface to Tol.
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

#include <windows.h>
#include <config.h>
#include <tol_init.h>
#include <tol_bout.h>
#include <tol_list.h>

#include "VButils.h"

/*
 *  Decompile all Tol objects
 *
 *  returns 1 if there was some object to be decompiled, otherwise returns 0
 */
//---------------------------------------------------------------------------
short __declspec(dllexport) CALLBACK TolDecompileAll()
//---------------------------------------------------------------------------
{
  BList *P = VB_stack.Cdr();
  short ret = P!=NULL;
  
  while (P) {
    BSyntaxObject * so = (BSyntaxObject*)(P->Car());
	VB_stack.PutCdr(P->Cdr());
    delete P;
    DESTROY(so);
	P = VB_stack.Cdr();
  }
  return ret;
}

/*
 *  Decompile a Tol object
 *
 *  name - string that contains the name of the object
 *
 *  returns 1 if an object whith that name was found and decompiled, otherwise returns 0
 */
//---------------------------------------------------------------------------
short __declspec(dllexport) CALLBACK TolDecompile(BSTR name)
//---------------------------------------------------------------------------
{
  LPSTR str_name = (LPSTR)name;
  BList *P = VB_stack.Cdr(), *prev = &VB_stack;
  short found = 0;
  
  while (!found && P) {
    BSyntaxObject * so = (BSyntaxObject*)(P->Car());
    const char* object_name = so->Name().String();
    if (!strcmp(object_name, str_name)) {
	  prev->PutCdr(P->Cdr());
      delete P;
      DESTROY(so);
      found = 1;
	  break;
    }
	prev = P;
    P = P->Cdr();
  }
  return found;
}

int VBTol_EvalExpr(char * expr, short* nerror, short* nwarning)
{
  BList * tol_result;

  BDat& bderror = TOLErrorNumber();
  BDat& bdwarning = TOLWarningNumber();

  *nerror = (int)(bderror.Value());
  *nwarning = (int)(bdwarning.Value());
  tol_result = MultyEvaluate(expr);
  *nerror = (int)(bderror.Value()) - *nerror;
  *nwarning = (int)(bdwarning.Value()) - *nwarning;
  if (tol_result) {
    VB_concat(&VB_stack, tol_result);
    return 1;
  }
  return 0;
}

/*
 *  Evaluates a Tol expresion
 *
 *  expr - string that contains the expresion
 *
 *  returns 1 if an object was created by the evaluation, otherwise 0
 */
//---------------------------------------------------------------------------
short __declspec(dllexport) CALLBACK TolEval(BSTR expr, short* nerror, short* nwarning)
//---------------------------------------------------------------------------
{
  LPSTR str_expr;

  str_expr = (LPSTR)expr;    // The address is coerced into an LPSTR.

  return VBTol_EvalExpr(str_expr, nerror, nwarning);
}

void VBTol_InstallHciWriter()
{
  BOut::PutHciWriter(VBTol_HciWriter);
  BOut::PutLogHciWriter(VBTol_HciWriter);
}

/*
 * Initialize TOL
 *
 * addr - the address of a Visual Basic function that will catch all the Tol output
 * vmode - VerboseMode
 * initlib - if it's non zero the InitLibrary will be loaded
 */
//---------------------------------------------------------------------------
void __declspec(dllexport) CALLBACK TolInit(long addr, BSTR vmode, short initlib)
//---------------------------------------------------------------------------
{
  VB_Writer = (FN_PTR)addr;

  VBTol_InstallHciWriter();

  Tol_gsl_set_error_handler(&VBTol_gsl_error_handler);

  LPSTR str_vmode;
  str_vmode = (LPSTR)vmode;
  InitTolKernel(NULL, 0, str_vmode);

  if (initlib)
    LoadInitLibrary(true,true);
}

// VC++ DLL entry point routine. Called during certain system events.

//---------------------------------------------------------------------------
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
					 )
//---------------------------------------------------------------------------
{
  switch(ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
//	  ::MessageBox(NULL, "Initializing the DLL.", "C++ DLL", 0);
	  break;
	case DLL_PROCESS_DETACH:
//	  ::MessageBox(NULL, "Closing down the DLL.", "C++ DLL", 0);
	  break;
  }
  return TRUE;
}
