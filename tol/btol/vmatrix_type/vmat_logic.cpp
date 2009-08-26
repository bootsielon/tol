/* vmat_logic.cpp: BVMat logic methods
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bvmat_impl.h>


////////////////////////////////////////////////////////////////////////////////
//Matrix algebra operators
////////////////////////////////////////////////////////////////////////////////

double fRR_not(double a) { return(Not(BDat(a)     ).Value()); }
double fRR_yes(double a) { return(Not(Not(BDat(a))).Value()); }
double fRR_isUnknown(double a) { return(BDat(a)     ).IsUnknown(); }
double fRR_isFinite (double a) { return(BDat(a)     ).IsFinite(); }
double fR2R_and(double a, double b) { return(And(BDat(a),BDat(b)).Value()); }
double fR2R_or (double a, double b) { return(Or (BDat(a),BDat(b)).Value()); }
double fR2R_eq (double a, double b) { return(Eq (BDat(a),BDat(b)).Value()); }
double fR2R_ne (double a, double b) { return(Ne (BDat(a),BDat(b)).Value()); }
double fR2R_le (double a, double b) { return(Le (BDat(a),BDat(b)).Value()); }
double fR2R_lt (double a, double b) { return(Lt (BDat(a),BDat(b)).Value()); }
double fR2R_ge (double a, double b) { return(Ge (BDat(a),BDat(b)).Value()); }
double fR2R_gt (double a, double b) { return(Gt (BDat(a),BDat(b)).Value()); }
double fR2R_max(double a, double b)
{
  return(Maximum(BDat(a),BDat(b)).Value()); 
}
double fR2R_min(double a, double b) 
{ 
  return(Minimum(BDat(a),BDat(b)).Value()); 
}

DefineMonary(Not,       fRR_not,       false, "Not");
DefineMonary(Yes,       fRR_yes,       true,  "Yes");
DefineMonary(IsUnknown, fRR_isUnknown, true,  "IsUnknown");
DefineMonary(IsFinite,  fRR_isFinite,  false, "IsFinite");

DefineBinary(And, fR2R_and,  true,  true,  "And");
DefineBinary(Or,  fR2R_or,   true,  false, "Or");
DefineBinary(EQ,  fR2R_eq,   false, false, "EQ");
DefineBinary(NE,  fR2R_ne,   true,  false, "NE");
DefineBinary(LE,  fR2R_le,   false, false, "LE");
DefineBinary(LT,  fR2R_lt,   true,  false, "LT");
DefineBinary(GE,  fR2R_ge,   false, false, "GE");
DefineBinary(GT,  fR2R_gt,   true,  false, "GT");
DefineBinary(Min, fR2R_min,  false, false, "Min");
DefineBinary(Max, fR2R_max,  false, false, "Max");


////////////////////////////////////////////////////////////////////////////////
  int BVMat::If(const BVMat& A_, const BVMat& B_, const BVMat& C_, BVMat& D)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  const char* fName = "IfVMat";
  if(!A_.CheckDefined(fName)) { return(-1); }
  if(!B_.CheckDefined(fName)) { return(-1); }
  if(!C_.CheckDefined(fName)) { return(-1); }
  int r  = A_.Rows();
  int c  = A_.Columns();
  int rB = B_.Rows();
  int cB = B_.Columns();
  int rC = C_.Rows();
  int cC = C_.Columns();
  if((r!=rB)||(c!=cB))
  {
    err_invalid_dimensions(fName,A_,B_);
    return(-1);
  }
  if((r!=rC)||(c!=cC))
  {
    err_invalid_dimensions(fName,A_,C_);
    return(-1);
  }
  D.Delete();
  BVMat* A__, *B__, *C__;
  convertIfNeeded_all2bRd(A_,A__,fName);
  convertIfNeeded_all2bRd(B_,B__,fName);
  convertIfNeeded_all2bRd(C_,C__,fName);
  BVMat &A = *A__,  &B = *B__,  &C = *C__;
  int result = 0;
  if(A.code_!=B.code_)
  {
    err_invalid_subtypes(fName,A,B);
    result = -2;
  }
  if(A.code_!=C.code_)
  {
    err_invalid_subtypes(fName,A,C);
    result = -2;
  }
  int k, n;
  const BDat* Ax;
  const BDat* Bx;
  const BDat* Cx;
        BDat* Dx;
  switch(A.code_) {
  case(ESC_blasRdense  ) :
    D.Copy(A);
    n = A.s_.blasRdense_->nzmax;
    Ax = (const BDat*)A.s_.blasRdense_->x;
    Bx = (const BDat*)B.s_.blasRdense_->x;
    Cx = (const BDat*)C.s_.blasRdense_->x;
    Dx = (      BDat*)D.s_.blasRdense_->x;
    for(k=0; k<n; k++)
    {
      if(Ax->IsKnown())
      {
        if((*Ax)!=0)
        {
          *Dx = *Bx;
        }
        else
        {
          *Dx = *Cx;
        }
      }
      else
      {
        *Dx = BDat::Unknown();
      }
      Ax++;
      Bx++;
      Cx++;
      Dx++;
    };
    break;
  default:
    err_invalid_subtype(fName,A); 
    result=-3; }
  if(A__!=&A_) { delete A__; }
  if(B__!=&B_) { delete B__; }
  if(C__!=&C_) { delete C__; }
  return(result);
};
