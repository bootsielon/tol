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

DefineMonary(Not, fRR_not,       true,  "Not");
DefineMonary(Yes, fRR_yes,       false, "Yes");
DefineMonary(IsUnknown, fRR_isUnknown, true,  "IsUnknown");
DefineMonary(IsFinite, fRR_isFinite,  true,  "IsFinite");
DefineBinary(And, fR2R_and,      true,  "And");
DefineBinary(Or,  fR2R_or,       true,  "Or");
DefineBinary(EQ,  fR2R_eq,       true,  "EQ");
DefineBinary(NE,  fR2R_ne,       false, "NE");
DefineBinary(LE,  fR2R_le,       true,  "LE");
DefineBinary(LT,  fR2R_lt,       false, "LT");
DefineBinary(GE,  fR2R_ge,       true,  "GE");
DefineBinary(GT,  fR2R_gt,       false, "GT");
DefineBinary(Min, fR2R_min,      true,  "Min");
DefineBinary(Max, fR2R_max,      true,  "Max");


