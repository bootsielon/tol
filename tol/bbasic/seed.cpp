/* seed.cpp: Interface with HTML - GNU/TOL Language.

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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bseed.h>
#include <tol/tol_bout.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bdate.h>

BTraceInit("seed.cpp");

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BSeed).
//--------------------------------------------------------------------
static BSeed  seedDeclarator_(0,NIL);

BParam BParam::unknown_("__UNKNOWN_PARAMETER__");


//--------------------------------------------------------------------
BText BParam::Value() const

/*! Returns the actual value or defect value if no value is done
 */
//--------------------------------------------------------------------
{
    if(value_.HasName()) { return(value_);  }
    else 	         { return(defect_); }
}


//--------------------------------------------------------------------
BBool BParam::Replace(BText& text) const

/*! Replaces all occurrences in text of the name of this by Value()
 *  result
 */
//--------------------------------------------------------------------
{
    BBool ok = BFALSE;
    if(Name().HasName())
    {
	ok = (text.Find(Name())>=0);
//  if(ok) { Std(Out() + "\n  Replacing " + Name()); }
	text.Replace(Name(), Value());
    }
    return(ok);
}


//--------------------------------------------------------------------
BInt ParamCmp(const void* v1, const void* v2)

/*! Compairs two BParam receiving the pointers.
 */
//--------------------------------------------------------------------
{
    BParam& par1 = *((BParam*)v1);
    BParam& par2 = *((BParam*)v2);
    return(StrCmp(par1.Name(), par2.Name()));
}

//--------------------------------------------------------------------
BSeed::BSeed(BInt num, BParam* param)

/*! BSeed constructor
 */
//--------------------------------------------------------------------
    : BArray<BParam>(num, param)
{
    AddCommonParam();
}


//--------------------------------------------------------------------
BSeed::BSeed(const BArray<BParam>& param)

/*! BSeed constructor
 */
//--------------------------------------------------------------------
    : BArray<BParam>(param)
{
    AddCommonParam();
}

//--------------------------------------------------------------------
BSeed::BSeed(const BSeed& seed)

/*! BSeed constructor
 */
//--------------------------------------------------------------------
    : BArray<BParam>(seed)
{
    AddCommonParam();
}

//--------------------------------------------------------------------
BSeed::BSeed(const BText& paramPath)

/*! BSeed constructor
 */
//--------------------------------------------------------------------
    : BArray<BParam>()
{
  ifstream in(paramPath.String());
  if(!in.good())
  {
    Error(I2("Cannot open file for reading ",
	     "No se puede abrir el fichero para lectura ") + paramPath);
    return;
  }
  BText text;
  BArray< BText > cells;

  Read(in, text);
  ReadAllTokens(text, cells, 126);
  ReallocBuffer(cells.Size()/2);
  for(BInt n=0; n<Size(); n++)
  {
    (*this)[n] = BParam(Compact(cells[2*n]), cells[2*n+1]);
  }
  AddCommonParam();
}


BDateFormat BSeed::dteFmt ("%Y-%m-%d");
//--------------------------------------------------------------------
void BSeed::AddCommonParam()

/*! Adds common parameters to this
 */
//--------------------------------------------------------------------
{
  BInt n = Size();
  ReallocBuffer(n + 2);
  (*this)[n] = BParam("_NOW_",	 BTimer::Text());
  (*this)[n+1]	 = BParam("_TODAY_", BSeed::dteFmt.DateToText(DteToday()));
  Sort(ParamCmp);
  for(n=1; n<Size(); n++)
  {
    if((*this)[n-1].Name()==(*this)[n].Name())
    {
      Error(Out()+"<"+(*this)[n].Name()+"> "+
	    I2("Duplicate parameter in a seed",
	       "Parametro duplicado en una semilla"));
    }
  }
};

//--------------------------------------------------------------------
void BSeed::ReplaceText(BText& text, int loops) const

/*! Replaces all occurrences of each param in text
 */
//--------------------------------------------------------------------
{
    int num, cloops=0;
    do
    {
	num = 0;
	for(BInt i=0; i<Size(); i++) { num += (*this)[i].Replace(text); }
    }
    while((num && !loops) || (num && loops && loops>++cloops));
}

//--------------------------------------------------------------------
void BSeed::ReplaceFile(const BText& seedPath, const BText& path) const

/*! Replaces all occurrences of each param in the contents of seedPath
 */
//--------------------------------------------------------------------
{
  BText	   text;
  ifstream in(seedPath.String());
  if(!in.good())
  {
    Error(I2("Cannot open file for reading ",
	     "No se puede abrir el fichero para lectura ") + seedPath);
  }
  else
  {
    Read(in, text, 1024, '\0');
    in.close();
    ReplaceText(text);
    OverWrite(path, text);
  }
}


//--------------------------------------------------------------------
BParam& BSeed::Param(const BText& name) const

/*! Searchs a param by its name.
 */
//--------------------------------------------------------------------
{
    BParam p(name);
    BInt   f = FindSorted(p, ParamCmp);
    if(f>=0) { return((*this)[f]); }
    else     { return(BParam::Unknown()); }
}


//--------------------------------------------------------------------
void BSeed::PutValue(const BText& name, const BText& value) const

/*! Changes the value of name-param by a done value
 */
//--------------------------------------------------------------------
{
    BParam& par = Param(name);
    par.PutValue(value);
}
