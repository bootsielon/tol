/* tol_brealfun.h: Definition of the class BRnRFunction.
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

#ifndef TOL_BREALFUN_H
#define TOL_BREALFUN_H 1

#include <tol/tol_bfunct.h>
#include <tol/tol_bmatrix.h>

#define HOUSEHOLDER 1
#define GIVENS      2
#define CHOLESKI    3


template class TOL_API BFunction<BInt, BDat>;

//--------------------------------------------------------------------
class TOL_API BRealSuc : public BFunction<BInt, BDat>

/*! Representation of real successions.
 */
//--------------------------------------------------------------------
{
public:
  BRealSuc() : BFunction<BInt, BDat>() {}
 ~BRealSuc() {}

};

template class TOL_API BFunction<BDat, BDat>;

//--------------------------------------------------------------------
class TOL_API BRRFunction : public BFunction<BDat, BDat>

/*! Representation of real to real functions.
 *  ASC: En muchos aspectos no seguia el formato Bayes.
 *  ASC: Es necesario Name()?
 */
//--------------------------------------------------------------------
{
public:
  BRRFunction() : BFunction<BDat, BDat>() {}
 ~BRRFunction() {}

  // Operations:
  BDat  FirstDerivative  (BDat point, BDat funcValue);
  BDat  FirstDerivativeRichardson  (BDat point);
  BDat  NewtonSolve      (BDat x0, BDat y0, BDat tol = BDat::Tolerance(), BInt maxIter=(int)BDat::MaxIter().Value());
  BDat  Integral         (BDat a, BDat b, BInt npoints);
  BDat  AdaptiveSimpson  (BDat a0, BDat b, BDat tol = BDat::Tolerance());
  BDat  Bisection        (BDat a, BDat b);
  BDat  FixedPoint       (BDat a, BDat b);
  BDat  Secant           (BDat a, BDat b);
  BDat  MullerMethod     (BDat a, BDat b);
  void  MaximumMinimum   (BDat a, BDat b);
  BDat  SecondDerivative (BDat point);
  BDat  CriticPoints     (BDat a, BDat b);
};


template class TOL_API BFunction<BArray<BDat>, BDat>;

//--------------------------------------------------------------------
class TOL_API BRnRFunction : public BFunction<BArray<BDat>, BDat>

/*! Abstract representation of n-real to real functions.
 */
//--------------------------------------------------------------------
{
protected:
  BInt n_;

public:
  BRnRFunction(BInt n=2)
  : BFunction<BArray<BDat>, BDat>(), n_(n) {}
 ~BRnRFunction() {}


  // Operations:
  virtual void Gradient (const BArray<BDat>&  x, BArray<BDat>&  G);
  virtual BDat Dimension();
};

//--------------------------------------------------------------------
struct TOL_API BPoint { BDat x_; BDat y_; };
//--------------------------------------------------------------------

template class TOL_API BArray<struct BPoint>;

//--------------------------------------------------------------------
class TOL_API BInitialValueProblem : public BRnRFunction

/*! Abstract representation of initial value problem.
 *
 *                   y' = f(x,y)
 *
 *    You must to inhiterate at least one of the next two functions :
 *    Evaluate or Dy.
 */
//--------------------------------------------------------------------
{
protected:
  BArray<struct BPoint> net_;

public:

  BInitialValueProblem()
  : BRnRFunction(2)
  {
    net_.ReallocBuffer(20);
    net_.ReallocBuffer(0);
  }
 ~BInitialValueProblem() {}

  void RungeKuttaFehlberg(BDat x0, BDat y0, BDat x1,
                          BDat tolerance, BDat hmin, BDat hmax);

  BDat Solve(BDat x, BInt from, BInt size);

  BDat Primitive(BDat x);

  BBool IsUpdated() { return(net_.Size()>2); }

  void  SumConstant(BDat c)
  {
    for(BInt i=0; i<net_.Size(); i++) { net_[i].y_+=c; }
  }
  // Operations:
  void Evaluate(BDat &dy, const BArray<BDat>& xy)
  {
    dy = Dy(xy[0],xy[1]);
  }

  virtual BDat Dy(BDat x, BDat y);
};

template class TOL_API BFunction<BArray<BDat>,BArray<BDat> >;

//--------------------------------------------------------------------
class TOL_API BRnRmFunction : public BFunction<BArray<BDat>,BArray<BDat> >

/*! Representation of n-real to m-real functions.
 */
//--------------------------------------------------------------------
{
protected:
  BInt n_;
  BInt m_;
  BArray<BDat> gsl_X, gsl_Fx, gsl_FStep;

public:
  BRnRmFunction(BInt n=2, BInt m=2)
    : BFunction<BArray<BDat>, BArray<BDat> >(), n_(n), m_(m), gsl_X(n), gsl_Fx(m), gsl_FStep(m) {}
  void SetDimensions(BInt n, BInt m);
 ~BRnRmFunction() {}

 BInt GetDimN() { return n_; }
 BInt GetDimM() { return m_; }
 BArray<BDat> &GetX() { return gsl_X; }
 BArray<BDat> &GetFx() { return gsl_Fx; }
 BArray<BDat> &GetFStep() { return gsl_FStep; }
 
  // Operations:
  virtual void Jacobian (const BArray<BDat>& x, 
			 const BArray<BDat>& fx, 
			       BMatrix<BDat>& G);

  BDat OneWaySearch(      BMatrix <BDat>& x,
                          BMatrix <BDat>& y,
                    const BMatrix <BDat>& x1_,
                    const BMatrix <BDat>& y1_,
                          BInt iter);

  BDat LeastSqrGaussNewton(BArray<BDat>& x,BArray<BDat>& r,BMatrix<BDat>& J);
  BDat LeastSqrMarquardt  (BArray<BDat>& x,BArray<BDat>& r,BMatrix<BDat>& J);
  BDat ConjugateGradient  (BArray<BDat>& x,BArray<BDat>& r,BMatrix<BDat>& J);

  BDat gsl_Marquardt(BArray<BDat>& x,BArray<BDat>& r,BMatrix<BDat>& J);

};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
TOL_API BDat& Distance            ();
TOL_API BDat& MarquardtFactor     ();
TOL_API BDat& MarquardtLinearMethod ();

#endif // TOL_BREALFUN_H
