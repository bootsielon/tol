/* tol_bseed.h: Interface with HTML
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

#ifndef TOL_BSEED_H
#define TOL_BSEED_H 1

#include <tol/tol_barray.h>
#include <tol/tol_bdate.h>
#include <tol/tol_bobject.h>

//--------------------------------------------------------------------
class BParam : public BObject

/*! HTML variable implementation
 */
//--------------------------------------------------------------------
{
 private:
    static BParam unknown_;
    //! defect value of param
    BText defect_;
    //! value of param
    BText value_;

 public:

  BParam()
  : BObject(), defect_(), value_() {}
  BParam(const BText& name)
  : BObject(name), defect_(), value_() {}
  BParam(const BText& name, const BText& defect)
  : BObject(name), defect_(defect), value_() {}
  BParam(const BText& name, const BText& defect, const BText& value)
  : BObject(name), defect_(defect), value_(value) {}
 ~BParam() {}
  void	PutValue (const BText& value) { value_ = value; }
  BText Value	 () const;
  BBool Replace	 (BText& text) const;

  static BParam& Unknown() {return(unknown_); }
  DeclareClassNewDelete(BParam);
};

#ifdef _MSC_VER
   template class BArray<BParam>;
#endif


//--------------------------------------------------------------------
class BSeed : public BArray<BParam>

/*! Parametric text file's generation assistant class
 */
//--------------------------------------------------------------------
{
public:
  BSeed(BInt num, BParam* param);
  BSeed(const BArray<BParam>& param);
  BSeed(const BText& paramPath);
  BSeed(const BSeed& seed);
 ~BSeed() {}
  void AddCommonParam();
  void ReplaceText(BText& text, int loops=0) const;
  void Replace(BText& text, int loops=0) const { ReplaceText(text,loops); }
  void ReplaceFile(const BText& seedPath, const BText& path) const;

  BParam& Param	  (const BText& name) const;
  void	  PutValue(const BText& name, const BText& value) const;
private:
  static BDateFormat dteFmt;
};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BInt ParamCmp(const void* v1, const void* v2);

#endif // TOL_BSEED_H
