/* tol_baia.h: Automatic Intervention Analysis
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

#ifndef TOL_BMAIA_H
#define TOL_BMAIA_H 1

#include <tol/tol_baia.h>
#include <tol/tol_bvmat.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BMultAia;


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------



//--------------------------------------------------------------------
class BMultOutlier

/*! A set of outliers that will be estimated in one step.
 */
//--------------------------------------------------------------------
{
public:
  static int optMaxOrder_;
  struct BInput
  {
    int       index_;
    BOutlier* outlier_;
    int       t_;
    BDat      w0_;
    BDat      sqrSum_;
    BDat      tStudent_;
  };
  friend bool operator == (const BInput& a, const BInput& b)
  {
    bool ok = (a.outlier_ == b.outlier_) &&
 (a.t_       == b.t_      );
    return(ok);
  }
  friend bool operator != (const BInput& a, const BInput& b)
  {
    bool ok = (a.outlier_ != b.outlier_) ||
 (a.t_       != b.t_      );
    return(ok);
  }
  static int CompareBicPtr(const void* v1, const void* v2);

  BArray<BInput> input_;
  BVMat x_;
  BVMat w_;
  BVMat e_;
  BVMat LF_;
  BVMat L_;
  BDat sigma_;
  BDat resLogDens_;
  BDat nonZeroParamLogProb_;
  BDat logDens_;
  BDat bic_; 
  BDat minNonZeroParamProb_;
  int  maxIndex_;
public:
  BMultOutlier();
 ~BMultOutlier() {};
  BMultOutlier& operator = (const BMultOutlier& a)
  {
    Copy(a);
    return(*this);
  }
  bool HasInput(BOutlier* outlier, int t) const;
  bool IsDisjoint(const BMultOutlier& a) const;
  BText GetExpression() const;
  void Add(int index, BOutlier* outlier, int t, BDat w);
  void Add(const BMultOutlier& mo);
  void Copy(const BMultOutlier& mo);
  bool Estimate(const BVMat& y, const BVMat& C, int n0,
   BDat optMaxEigenValueRelativeRange_);
  bool Estimate(const BMultAia& aia);
};


//--------------------------------------------------------------------
class BMultAia: public BAia

/*! Automatic Intervention Analisys. Searches in a list of BOutlier
 *  objects for the best fit to each outlier found in a serie of
 *  residuous.
 */
//--------------------------------------------------------------------
{
 public:
  struct BMaxRes
  {
    BDat absRes_;
    int  t_;
  };
  struct BMergingInfo
  {
    int  i_;
    int  j_;
    int  k_;
    bool isDisjoint_;
    BDat bic_;
  };
  struct BComparingInfo
  {
    int  o_;
    int  h_;
    BDat bic_;
  };

  BVMat y_;
  BVMat constant_;
  int optMaxOrder_;
  int optMaxCrossNum_;
  BDat optMinNonZeroParamProb_;
  BDat optMaxEigenValueRelativeRange_;
  BDat resLogDens_;
  BDat nonZeroParamLogProb_;
  BDat minNonZeroParamProb_;
  BDat logDens_;
  BDat bic_;
  BMultOutlier mo_;
 public:
  BMultAia(      BUserTimeSerie*    res,
           const BRat& rat,
           const BArray<BOutlier*>& userOutliers,
    int   optMaxOrder,
    int   optMaxCrossNum,
    BDat  optMinNonZeroParamProb,
    BDat  optMaxEigenValueRelativeRange);
 ~BMultAia() {}
  void SetResiduals         (const BArray<BDat>& y);
  void SetResiduals         (const BVMat& y);
  void Initialize           ();
  bool SearchMaxAbsRes      (BArray<BMaxRes>& mr);
  bool SearchRefObs         (BArray<BMultOutlier>& mo);
  bool SearchNextMultOutlier(BMultOutlier& mo);
  void RecalcResiduous      (const BMultOutlier& mo);
  BList* Input();
};

#endif // TOL_BMAIA_H


