/* datatype.cpp: This file implements an interface between objects in tol
                  and "objects" in Visual Basic
                  This file define a set of utils functions.
                  GNU/VBTol Library

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

#if defined(_MSC_VER)
  #include <config.h>
  #include <tol_init.h>
  #include <tol_bgrammar.h> 
#else
  #include <tol/config.h>
  #include <tol/tol_init.h>
  #include <tol/tol_bgrammar.h>
#endif

#include <tol/tol_bdat.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bmatgra.h>

#include "vbutils.h"

/*
 *  Obtains the value of an object TOL of type Text
 *
 *  lvalue - Name of the object TOL
 *  retvalue - Variable where the value of the object will be returned
 *
 *  returns 1 if the object was found and its value returned, otherwise 0
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolGetText(BSTR lvalue, BSTR * retvalue)
//---------------------------------------------------------------------------
{
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;

  BSyntaxObject * so = GraText()->EvaluateExpr(str_lvalue);
  if (so) {
    BText & d = Text(so);
    char * st = (char *)(d.String());
	  SysFreeString(*retvalue);
	  *retvalue = SysAllocStringByteLen(st, strlen(st));
	  DESTROY(so);
	  ret = 1;
  }
  return ret;
}

/*
 *  Creates an object TOL of type Text
 *
 *  name - Name of the object TOL
 *  value - Value of the object
 *
 *  returns 1 if the object was created, otherwise returns 0
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolCreateText(BSTR name, BSTR value)
//---------------------------------------------------------------------------
{
  LPSTR str_name = (LPSTR)name;
  LPSTR str_value = (LPSTR)value;
  short ret = 0;

  BSyntaxObject * so = new BContensText(str_name, str_value, "");
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    VB_concat(&VB_stack, result);
	  ret = 1;
  }
  return ret;
}

//---------------------------------------------------------------------------

#define Known 0
#define Unknown 1
#define Pos_Inf 2
#define Neg_Inf 3

// Structure for the Real Tol object.
struct stolreal
{
	short kind;      // support for special values
	double value;
};

/*
 *  Obtains the value of an object TOL of type Real
 *
 *  lvalue - Name of the object TOL
 *  retvalue - Variable where the value of the object will be returned
 *
 *  returns 1 if the object was found and its value returned, otherwise 0
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolGetReal(BSTR lvalue, stolreal * retvalue)
//---------------------------------------------------------------------------
{
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;

  BSyntaxObject * so = GraReal()->EvaluateExpr(str_lvalue);
  if (so) {
    BDat & d = Dat(so);
    retvalue->value = d.Value();
    if (d.IsKnown()) {
      if (d.IsFinite())
        retvalue->kind = Known;
      else
        if (d.IsPosInf())
          retvalue->kind = Pos_Inf;
        else
          retvalue->kind = Neg_Inf;
    }
    else
      retvalue->kind = Unknown;
    DESTROY(so);
	  ret = 1;
  }
  return ret;
}

/*
 *  Creates an object TOL of type Real
 *
 *  name - Name of the object TOL
 *  value - Value of the object
 *
 *  returns 1 if the object was created, otherwise returns 0
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolCreateReal(BSTR name, stolreal * value)
//---------------------------------------------------------------------------
{
  LPSTR str_name = (LPSTR)name;
  short ret = 0;
  BDat d;

  if (value->kind==Known)
    d.PutValue(value->value);
  else
    if (value->kind==Unknown)
      d.PutKnown(BUNKNOWN);
    else
      if (value->kind==Pos_Inf)
        d.PutValue(BDat::PosInf());
      else
        if (value->kind==Neg_Inf)
          d.PutValue(BDat::NegInf());
        else
          return 0;
  BSyntaxObject * so = new BContensDat(str_name, d, "");
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    VB_concat(&VB_stack, result);
	  ret = 1;
  }
  return ret;
}

//---------------------------------------------------------------------------

// Structure of the masks cells with special values
struct smaskcell
{
	long r, c;       // index row and column
	short kind;      // special value
};

// List of cells of the mask
struct listcell
{
	smaskcell cell;
	listcell * next;
};

/*
 *  Gets the value of a range of an object TOL of type Matrix
 *
 *  See functions TolGetRange, TolGetMatrix, TolGetRow,
 *   TolGetColumn and TolGetCell below
 */
//---------------------------------------------------------------------------
short GetMatrixRange(BSTR lvalue,
                     long frow, long lrow,
                     long fcol, long lcol,
                     LPSAFEARRAY FAR * retvalue,
                     LPSAFEARRAY FAR * retmask)
//---------------------------------------------------------------------------
{
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;

  if (!retmask || !(*retmask))          // check parameter retmask
    return -1;
  if ((*retmask)->cDims != 1)
    return -1;
  if ((*retmask)->cbElements != sizeof(smaskcell))
    return -1;
  if (!((*retmask)->rgsabound))
    return -1;
  unsigned short fFeatures = (*retmask)->fFeatures;
  if (fFeatures & FADF_STATIC || fFeatures & FADF_FIXEDSIZE)  // not sized with Basic´s ReDim 
    return -1;

  if (SafeArrayDestroy(*retvalue))    // Free previous array
    return -1;
  *retvalue = NULL;
                                      // Get Tol Object
  BSyntaxObject * so = GraMatrix()->EvaluateExpr(str_lvalue);
  if (so) {
    BMat & m = Mat(so);       // Tol Matrix
    BInt cols = m.Columns();
    BInt rows = m.Rows();

    if (lrow>rows || lcol>cols) {   // check the validity of the range
      DESTROY(so);
      return 0;
    }
    if (!frow)               // update values of the range without definition
      frow = 1;
    if (!fcol)
      fcol = 1;
    if (!lrow)
      lrow = rows;
    if (!lcol)
      lcol = cols;
    SAFEARRAYBOUND aDims[2];
               // Create a new 2-D array of Doubles
               // Equivalent to: Dim retvalue(1 To rows, 1 To cols) As Double
    aDims[0].lLbound = 1;
    aDims[0].cElements = lrow-frow+1;
    aDims[1].lLbound = 1;
    aDims[1].cElements = lcol-fcol+1;
    *retvalue = SafeArrayCreate(VT_R8, 2, aDims);
	  if (*retvalue==NULL) {         // ERROR_NOT_ENOUGH_MEMORY
      DESTROY(so);
	    return -1;
	  }
                                      // Get values
    // Modify 2-D array with SafeArrayPtrOfIndex, SafeArrayLock and SafeArrayUnlock
	  long ai[2];
    double iVal;
	  double * pInOut = NIL;
    HRESULT hres_SafeArrayLock,
            hres_SafeArrayUnlock,
            hres_SafeArrayPtrOfIndex;
    long cells = 0;
    listcell * fcell = NULL, * pcell;

    hres_SafeArrayLock = SafeArrayLock(*retvalue);
    assert(!hres_SafeArrayLock);
    for (int r=1; r<=lrow-frow+1; ++r) {
      ai[0] = r;
      for (int c=1; c<=lcol-fcol+1; ++c) {
        ai[1] = c;
        BDat & d = m(frow+r-2, fcol+c-2);
        iVal = d.Value();
        hres_SafeArrayPtrOfIndex = SafeArrayPtrOfIndex(*retvalue, ai, (void **)&pInOut);
        assert (!hres_SafeArrayPtrOfIndex);
		    *pInOut = iVal;
        if (d.IsUnknown() || !d.IsFinite()) {
          ++cells;
          pcell = new listcell;
          pcell->cell.c = c;
          pcell->cell.r = r;
          pcell->cell.kind = d.IsUnknown() ? Unknown :
                             d.IsPosInf() ? Pos_Inf : Neg_Inf;
          pcell->next = fcell;
          fcell = pcell;
        }
      }
    }
    hres_SafeArrayUnlock = SafeArrayUnlock(*retvalue);
    assert(!hres_SafeArrayUnlock);
                                       // Redim a 1-D array of MaskCell
    SAFEARRAYBOUND maskaDims;
    maskaDims.lLbound = 1;
    maskaDims.cElements = cells;
    HRESULT hres_SafeArrayRedim;
    hres_SafeArrayRedim = SafeArrayRedim(*retmask, &maskaDims);
    if (hres_SafeArrayRedim) {
      DESTROY(so);
      while (pcell = fcell) {
        fcell = fcell->next;
        delete pcell;
      }
      return -1;
    }
                                      // Get mask for special values
    if (retmask && *retmask && cells) {
      hres_SafeArrayLock = SafeArrayLock(*retmask);
      assert(!hres_SafeArrayLock);
  	  smaskcell * maskpInOut = (smaskcell *)((*retmask)->pvData);
      for (int i=0; i<cells; ++i) {
        maskpInOut[i] = fcell->cell;
        pcell = fcell;
        fcell = fcell->next;
        delete pcell;
      }
      hres_SafeArrayUnlock = SafeArrayUnlock(*retmask);
      assert(!hres_SafeArrayUnlock);
    }

    DESTROY(so);
	  ret = 1;
  }
  return ret;
}

/*
 *  Gets the value of a range of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  retvalue - Variable where the values of the range will be returned
 *  retmask - Variable where the cells with special values will be returned
 *  frow, lrow, fcol, lcol - first row, last row, first column, last column
 *                           that define the range
 *
 *  returns 1 if the object was found, the specified range was correct and
 *            its value returned; otherwise 0,
 *  also it returns -1 when a critical error has happened like
 *  a memory allocation error or
 *  an incorrect type of the array retmask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolGetRange(BSTR lvalue,
                                                 long frow, long lrow,
                                                 long fcol, long lcol,
                                                 LPSAFEARRAY FAR * retvalue,
                                                 LPSAFEARRAY FAR * retmask)
//---------------------------------------------------------------------------
{
  if (frow<1 || fcol<1 || frow>lrow || fcol>lcol) // check the definition of range
    return 0;
  return GetMatrixRange(lvalue, frow, lrow, fcol, lcol, retvalue, retmask);
}

/*
 *  Obtains the value of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  retvalue - Variable where the value of the object will be returned
 *  retmask - Variable where the cells with special values will be returned
 *
 *  returns 1 if the object was found and its value returned, otherwise 0,
 *  also it returns -1 when a critical error has happened like
 *  a memory allocation error or
 *  an incorrect type of the array retmask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolGetMatrix(BSTR lvalue,
                                                  LPSAFEARRAY FAR * retvalue,
                                                  LPSAFEARRAY FAR * retmask)
//---------------------------------------------------------------------------
{
  return GetMatrixRange(lvalue, 0, 0, 0, 0, retvalue, retmask);
}

/*
 *  Gets the value of a row of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  retvalue - Variable where the values of the row will be returned
 *  retmask - Variable where the cells with special values will be returned
 *  row - index of the row to obtain
 *
 *  returns 1 if the object was found, the specified row was correct and
 *            its value returned; otherwise 0,
 *  also it returns -1 when a critical error has happened like
 *  a memory allocation error or
 *  an incorrect type of the array retmask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolGetRow(BSTR lvalue, long row,
                                               LPSAFEARRAY FAR * retvalue,
                                               LPSAFEARRAY FAR * retmask)
//---------------------------------------------------------------------------
{
  if (row<1)      // check the definition of the row
    return 0;
  return GetMatrixRange(lvalue, row, row, 0, 0, retvalue, retmask);
}

/*
 *  Gets the value of a column of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  retvalue - Variable where the values of the column will be returned
 *  retmask - Variable where the cells with special values will be returned
 *  col - index of the column to obtain
 *
 *  returns 1 if the object was found, the specified column was correct and
 *            its value returned; otherwise 0,
 *  also it returns -1 when a critical error has happened like
 *  a memory allocation error or
 *  an incorrect type of the array retmask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolGetColumn(BSTR lvalue, long col,
                                                  LPSAFEARRAY FAR * retvalue,
                                                  LPSAFEARRAY FAR * retmask)
//---------------------------------------------------------------------------
{
  if (col<1)      // check the definition of the column
    return 0;
  return GetMatrixRange(lvalue, 0, 0, col, col, retvalue, retmask);
}

/*
 *  Gets the value of a cell of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  retvalue - Variable where the value of the cell will be returned
 *  row, col - position of the cell to obtain
 *
 *  returns 1 if the object was found, the specified position of the cell
 *            was correct and its value returned; otherwise returns 0.
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolGetCell(BSTR lvalue, long row, long col,
                                                stolreal * retvalue)
//---------------------------------------------------------------------------
{
  if (row<1 || col<1)      // check the position of the cell
    return 0;
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;
                                      // Get Tol Matrix Object
  BSyntaxObject * so = GraMatrix()->EvaluateExpr(str_lvalue);
  if (so) {
    BMat & m = Mat(so);       // Tol Matrix
    if (row>m.Rows() || col>m.Columns()) {   // check the validity of the position
      DESTROY(so);
      return 0;
    }
    BDat & d = m(row-1, col-1);       // Tol Real (Cell)
    retvalue->value = d.Value();   // Get value
    if (d.IsKnown()) {
      if (d.IsFinite())
        retvalue->kind = Known;
      else
        if (d.IsPosInf())
          retvalue->kind = Pos_Inf;
        else
          retvalue->kind = Neg_Inf;
    }
    else
      retvalue->kind = Unknown;
    DESTROY(so);
	  ret = 1;
  }
  return ret;
}

/*
 *  Puts the values of a range of an object TOL of type Matrix
 *
 *  See functions TolPutRange, TolPutMatrix, TolPutRow,
 *   TolPutColumn and TolPutCell below
 */
//---------------------------------------------------------------------------
short PutMatrixRange(BSTR lvalue, long row, long col,
                     LPSAFEARRAY FAR * value,
                     LPSAFEARRAY FAR * mask)
//---------------------------------------------------------------------------
{
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;

  if (!value || !(*value))          // check parameter value
    return -1;
  if ((*value)->cDims != 2)
    return -1;
  SAFEARRAYBOUND * aDims = (*value)->rgsabound;
  if (!aDims)
    return -1;
  int cMin = aDims[0].lLbound;
  int rMin = aDims[1].lLbound; 
  BInt v_cols = aDims[0].cElements;
  BInt v_rows = aDims[1].cElements;
  if (v_cols<=0 || v_rows<=0)
    return -1;

  int cells;                        // check parameter mask
  if (!mask || !(*mask))
    cells = 0;
  else {
    if ((*mask)->cDims != 1)
      return -1;
    if ((*mask)->cbElements != sizeof(smaskcell))
      return -1;
    SAFEARRAYBOUND * maskaDims = (*mask)->rgsabound;
    if (!maskaDims)
      return -1;
    cells = maskaDims->cElements;
    if (cells<0)
      return -1;
  }
                                      // Get Tol Object
  BSyntaxObject * so = GraMatrix()->EvaluateExpr(str_lvalue);
  if (so) {
    BMat & d = Mat(so);       // Tol Matrix
    BInt m_cols = d.Columns();
    BInt m_rows = d.Rows();

    if (!row)               // update values of the range without definition
      row = 1;
    if (!col)
      col = 1;
    if (row+v_rows-1 > m_rows || col+v_cols-1 > m_cols) {   // check the validity of the range
      DESTROY(so);
      return 0;
    }
                               // Put values
	// Use 2-D array with SafeArrayPtrOfIndex, SafeArrayLock and SafeArrayUnlock
    long ai[2];
    double * pInOut = NULL;
    HRESULT hres_SafeArrayLock,
            hres_SafeArrayUnlock,
            hres_SafeArrayPtrOfIndex;
   
    hres_SafeArrayLock = SafeArrayLock(*value);
    assert(!hres_SafeArrayLock);
    for (int r=0; r<v_rows; ++r) {
      ai[0] = r + rMin;
      for (int c=0; c<v_cols; ++c) {
        ai[1] = c + cMin;
        hres_SafeArrayPtrOfIndex = SafeArrayPtrOfIndex(*value, ai, (void **)(&pInOut));
        assert(!hres_SafeArrayPtrOfIndex);
		    d(r+row-1, c+col-1) = *pInOut;
      }
    }
    hres_SafeArrayUnlock = SafeArrayUnlock(*value);
    assert(!hres_SafeArrayUnlock);
                               // Put special values
    if (cells) {
      hres_SafeArrayLock = SafeArrayLock(*mask);
      assert(!hres_SafeArrayLock);
      smaskcell * maskpInOut = (smaskcell *)((*mask)->pvData);
      for (int i=0; i<cells; ++i) {
        long p_row = maskpInOut[i].r - rMin;
        long p_col = maskpInOut[i].c - cMin;
        if (p_row<0 || p_row>=v_rows || p_col<0 || p_col>=v_cols ) {
          hres_SafeArrayUnlock = SafeArrayUnlock(*mask);
          assert(!hres_SafeArrayUnlock);
          DESTROY(so);
          return 0;
        }
        short kind = maskpInOut[i].kind;
        if (kind==Unknown)
          d(p_row+row-1, p_col+col-1).PutKnown(BUNKNOWN);
        else
          if (kind==Pos_Inf)
            d(p_row+row-1, p_col+col-1).PutValue(BDat::PosInf());
          else
            if (kind==Neg_Inf)
              d(p_row+row-1, p_col+col-1).PutValue(BDat::NegInf());
            else
              if (kind==Known)
                ;
              else {
                hres_SafeArrayUnlock = SafeArrayUnlock(*mask);
                assert(!hres_SafeArrayUnlock);
                DESTROY(so);
                return 0;
              }
      }
      hres_SafeArrayUnlock = SafeArrayUnlock(*mask);
      assert(!hres_SafeArrayUnlock);
    }

    DESTROY(so);
	  ret = 1;
  }
  return ret;
}

/*
 *  Puts the values of a range of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  value - Values of the range
 *  mask - Array of mask cells with special values
 *  row, col - position of the begining of the range
 *
 *  returns 1 if the object was found, the specified range was correct and
 *            its values updated; otherwise 0,
 *  also it returns -1 when a critical error has happened like
 *  incorrect dimensions of the array given as a parameter or
 *  an incorrect type of the array mask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolPutRange(BSTR lvalue, long row, long col,
                                                 LPSAFEARRAY FAR * value,
                                                 LPSAFEARRAY FAR * mask)
//---------------------------------------------------------------------------
{
  if (row<1 || col<1)             // check the begining of the range
    return 0;
  return PutMatrixRange(lvalue, row, col, value, mask);
}

/*
 *  Puts the values of a row of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  value - Values of the row
 *  mask - Array of mask cells with special values
 *  row - index of the row
 *
 *  returns 1 if the object was found, the specified row was correct and
 *            its values updated; otherwise 0,
 *  also it returns -1 when a critical error has happened like
 *  incorrect dimensions of the array given as a parameter or
 *  an incorrect type of the array mask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolPutRow(BSTR lvalue, long row,
                                               LPSAFEARRAY FAR * value,
                                               LPSAFEARRAY FAR * mask)
//---------------------------------------------------------------------------
{
  if (row<1)             // check the index of the row
    return 0;
  return PutMatrixRange(lvalue, row, 0, value, mask);
}

/*
 *  Puts the values of a column of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  value - Values of the column
 *  mask - Array of mask cells with special values
 *  col - index of the column
 *
 *  returns 1 if the object was found, the specified column was correct and
 *            its values updated; otherwise 0,
 *  also it returns -1 when a critical error has happened like
 *  incorrect dimensions of the array given as a parameter or
 *  an incorrect type of the array mask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolPutColumn(BSTR lvalue, long col,
                                                  LPSAFEARRAY FAR * value,
                                                  LPSAFEARRAY FAR * mask)
//---------------------------------------------------------------------------
{
  if (col<1)             // check the index of the column
    return 0;
  return PutMatrixRange(lvalue, 0, col, value, mask);
}

/*
 *  Puts the value of a cell of an object TOL of type Matrix
 *
 *  lvalue - Name of the object TOL
 *  value - Value of the cell
 *  row, col - position of the cell
 *
 *  returns 1 if the object was found, the specified position of the cell
 *            was correct and its value updated; otherwise returns 0.
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolPutCell(BSTR lvalue, long row, long col,
                                                stolreal * value)
//---------------------------------------------------------------------------
{
  if (row<1 || col<1)      // check the position of the cell
    return 0;
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;
                                      // Get Tol Matrix Object
  BSyntaxObject * so = GraMatrix()->EvaluateExpr(str_lvalue);
  if (so) {
    BMat & m = Mat(so);       // Tol Matrix
    if (row>m.Rows() || col>m.Columns()) {   // check the validity of the position
      DESTROY(so);
      return 0;
    }
    BDat & d = m(row-1, col-1);       // Tol Real (Cell)
    if (value->kind==Known)
      d.PutValue(value->value);
    else
      if (value->kind==Unknown)
        d.PutKnown(BUNKNOWN);
      else
        if (value->kind==Pos_Inf)
          d.PutValue(BDat::PosInf());
        else
          if (value->kind==Neg_Inf)
            d.PutValue(BDat::NegInf());
          else {
            DESTROY(so);
            return 0;
          }
    DESTROY(so);
	  ret = 1;
  }
  return ret;
}

/*
 *  Creates an object TOL of type Matrix
 *
 *  name - Name of the object TOL
 *  value - Value of the object
 *  mask - Array of mask cells with special values
 *
 *  returns 1 if the object was created, otherwise returns 0,
 *  also it returns -1 when a critical error has happened like
 *  incorrect dimensions of the array given as a parameter or
 *  an incorrect type of the array retmask given as a parameter
 */
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) short __stdcall TolCreateMatrix(BSTR name,
                                                     LPSAFEARRAY FAR * value,
                                                     LPSAFEARRAY FAR * mask)
//---------------------------------------------------------------------------
{
  LPSTR str_name = (LPSTR)name;
  short ret = 0;
  
  if (!value || !(*value))          // check parameter value
    return -1;
  if ((*value)->cDims != 2)
    return -1;
  SAFEARRAYBOUND * aDims = (*value)->rgsabound;
  if (!aDims)
    return -1;
  int cMin = aDims[0].lLbound;
  int rMin = aDims[1].lLbound; 
  BInt cols = aDims[0].cElements;
  BInt rows = aDims[1].cElements;
  if (cols<=0 || rows<=0)
    return -1;

  int cells;                        // check parameter mask
  if (!mask || !(*mask))
    cells = 0;
  else {
    if ((*mask)->cDims != 1)
      return -1;
    if ((*mask)->cbElements != sizeof(smaskcell))
      return -1;
    SAFEARRAYBOUND * maskaDims = (*mask)->rgsabound;
    if (!maskaDims)
      return -1;
    cells = maskaDims->cElements;
    if (cells<0)
      return -1;
  }

  BMat d(rows, cols);   // Tol Matrix
                               // Put values
	// Use 2-D array with SafeArrayPtrOfIndex, SafeArrayLock and SafeArrayUnlock
  long ai[2];
  double * pInOut = NULL;
  HRESULT hres_SafeArrayLock,
          hres_SafeArrayUnlock,
          hres_SafeArrayPtrOfIndex;
   
  hres_SafeArrayLock = SafeArrayLock(*value);
  assert(!hres_SafeArrayLock);
  for (int r=0; r<rows; ++r) {
    ai[0] = r + rMin;
    for (int c=0; c<cols; ++c) {
      ai[1] = c + cMin;
      hres_SafeArrayPtrOfIndex = SafeArrayPtrOfIndex(*value, ai, (void **)(&pInOut));
      assert(!hres_SafeArrayPtrOfIndex);
		  d(r, c) = *pInOut;
    }
  }
  hres_SafeArrayUnlock = SafeArrayUnlock(*value);
  assert(!hres_SafeArrayUnlock);
                               // Put special values
  if (cells) {
    hres_SafeArrayLock = SafeArrayLock(*mask);
    assert(!hres_SafeArrayLock);
    smaskcell * maskpInOut = (smaskcell *)((*mask)->pvData);
    for (int i=0; i<cells; ++i) {
      long row = maskpInOut[i].r - rMin;
      long col = maskpInOut[i].c - cMin;
      if (row<0 || row>=rows || col<0 || col>=cols ) {
        hres_SafeArrayUnlock = SafeArrayUnlock(*mask);
        assert(!hres_SafeArrayUnlock);
        return 0;
      }
      short kind = maskpInOut[i].kind;
      if (kind==Unknown)
        d(row, col).PutKnown(BUNKNOWN);
      else
        if (kind==Pos_Inf)
          d(row, col).PutValue(BDat::PosInf());
        else
          if (kind==Neg_Inf)
            d(row, col).PutValue(BDat::NegInf());
          else
            if (kind==Known)
              ;
            else {
              hres_SafeArrayUnlock = SafeArrayUnlock(*mask);
              assert(!hres_SafeArrayUnlock);
              return 0;
            }
    }
    hres_SafeArrayUnlock = SafeArrayUnlock(*mask);
    assert(!hres_SafeArrayUnlock);
  }
                               // Create Tol Object
  BSyntaxObject * so = new BContensMat(str_name, d, "");
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    VB_concat(&VB_stack, result);
	  ret = 1;
  }
  return ret;
}

