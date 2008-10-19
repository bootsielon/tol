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

#ifndef TOL_BAIA_H
#define TOL_BAIA_H 1

#include <tol/tol_bmodel.h>
#include <tol/tol_bratgra.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BAia;


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------


//--------------------------------------------------------------------
class BOutlier : public BObject

/*! An outlier type is a rational function that applied to a Pulse in
 *  a date attempt to explain anormal behaviours of a model in this
 *  date.
 */
//--------------------------------------------------------------------
{
public:
  static BDat	          minOutlierLikelyhood_;
  static BList*	        instances_;
  static BArray<BAtom*> sortedOutliers_;

  BRat		     rat_;
  BRat		     responseFunction_;
  BReal		     student_;
  BAia*		     aia_;
  BBool		     enabled_;
  BInt		     maxRelative_;
  BArray<BDat> expand_;
  BArray<BDat> x_;
  BArray<BDat> xx_;
  BArray<BInt> nearMaxAbs_;
  
public:
  BOutlier() {};
  BOutlier(const BText& name, const BRat& rat);
 ~BOutlier();

  void	CalcExpand(BInt n);
  BDat	Expand	  (BInt n);
  void	PutEnabled(BBool a) { enabled_ = a; }
  void	PutAia	  (BAia* aia);

  BBool		 Enabled	()		   const {return(enabled_);}
  void		 Evaluate	(BInt t, BMatrix<BDat>& x, int row, BDat& sqrSum);
  BReal		 Estimate	(BInt t, BReal& w);
  BText          GetExpression(BInt t, BDat	w) const;
  BSyntaxObject* GetInputDef(BInt t, BDat	w) const;
  BSyntaxObject* RecalcResiduous(BInt t, BReal	w) const;

  static BOutlier* Find(const BText& name);
  static BOutlier* Find(const BRat&  rat);

  friend BDat& MinOutlierLikelyhood();
  DeclareClassNewDelete(BOutlier);
};


//--------------------------------------------------------------------
class BAia

/*! Automatic Intervention Analisys. Searches in a list of BOutlier
 *  objects for the best fit to each outlier found in a serie of
 *  residuous.
 */
//--------------------------------------------------------------------
{
private:
  BInt  tFound_;
  BInt  oFound_;
  BReal wFound_;
 public:
    static BArray<BOutlier*> sysOutliers_;
    static BOutlier* pulseOut_;
    static BOutlier* stepOut_;
    static BOutlier* compensOut_;
    
    BUserTimeSerie*   res_;
    BRat	            rat_;
    BArray<BOutlier*> userOutliers_;
    BList*	          input_;
    BDat              avr_;
    BDat	            sigma_;
    BInt	            N_;
    BInt	            n_;

 public:
    static int StaticInit();

    BAia() {};
    BAia(BModel* model);
    BAia(      BUserTimeSerie*    res,
	       const BRat&              rat);
    BAia(      BUserTimeSerie*    res,
	       const BRat&              rat,
	       const BArray<BOutlier*>& userOutliers);
    virtual ~BAia() {}
    bool SearchNextOutlier();
    virtual void Initialize();
    virtual BList* Input();
};

#endif // TOL_BAIA_H


