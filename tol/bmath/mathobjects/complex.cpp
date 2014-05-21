/* complex.cpp: BComplex: Global class variables, Constructors and Destructors
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

#include <tol/tol_bcomplex.h>
#include <tol/tol_bout.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_poly.h>

//--------------------------------------------------------------------
static BComplex CreateImaginaryUnit()
{
    BComplex i = BComplex::XY(0,1);
    i.ToRA();
    return(i);
}

//--------------------------------------------------------------------
// Static global class variable for BComplex.
//--------------------------------------------------------------------
BTraceInit("complex.cpp");

BComplex  BComplex::i_ = CreateImaginaryUnit();
BComplex  BComplex::unknown_(BDat::Unknown(),BDat::Unknown());

BComplex& BComplex::I()	      { return(i_); }
BComplex& BComplex::Unknown() { return(unknown_); }

#ifndef UNIX
  BArray<BComplex> BArrayBComplexDeclaration_;
#endif


//--------------------------------------------------------------------
void BComplex::PutReal(BDat x)
{
  x_ = x;
  y_ = 0;
  xr_ = 6;
  if(x >=0) { r_ =  x; a_ = 0; }
  else      { r_ = -x; a_ = M_PI; }
}

//--------------------------------------------------------------------
  void BComplex::ToXYNC()
{
    if(!IsXY())
    {
	if(!IsKnown())
	{
	    x_ = y_ = BDat::Unknown();
	}
	else
	{
	    long double c = cosl(a_.Value());
	    long double s = sinl(a_.Value());
	    //if(Abs(c)>0.5) { s = Sign(s)*Sqrt(1.0-c*c); }
	    //if(Abs(s)>0.5) { c = Sign(c)*Sqrt(1.0-s*s); }
	    x_ = r_ * c;
	    y_ = r_ * s;
	    if(xr_%2) { xr_ *= 2; }
	}
    }
}

//--------------------------------------------------------------------
void BComplex::ToRANC()
{
  if(!IsRA())
  {
    if(!IsKnown())
	  {
	    r_ = a_ = BDat::Unknown();
	  }
	  else if((x_==0)&&(y_==0))
	  {
	    r_ = 0;
	    a_ = 0;
	  }
	  else if(y_==0)
	  {
	    if(x_>0)
	    {
		    r_ = x_;
		    a_ = 0;
	    }
	    else
	    {
		    r_ = -x_;
		    a_ = BDat::Pi();
	    }
	  }
	  else if(x_==0)
	  {
	    if(y_>0)
	    {
		    r_ = y_;
		    a_ = BDat::Pi()/2.0;
	    }
	    else
	    {
		    r_ = -y_;
		    a_ = -BDat::Pi()/2.0;
	    }
	  }
	  else
	  {
	    r_ = hypot(x_.Value(),y_.Value()); //Sqrt((x_^2) + (y_^2));
	    a_ = atanl(fabsl((long double)(y_.Value())/(long double)(x_.Value())));
	    if((x_<=0)&&(y_>=0)) { a_ = BDat::Pi()-a_; }
	    else if((x_<=0)&&(y_<=0)) { a_ = BDat::Pi()+a_; }
	    else if((x_>=0)&&(y_<=0)) { a_ =   -a_; }
	  }
	  if(xr_%3) { xr_ *= 3; }
  }
}

//--------------------------------------------------------------------
void BComplex::ToXY() const
{
    BComplex* T = (BComplex*)(this);
    T->ToXYNC();
}

//--------------------------------------------------------------------
void BComplex::ToRA() const
{
    BComplex* T = (BComplex*)(this);
    T->ToRANC();
}

//--------------------------------------------------------------------
BBool BComplex::IsKnown() const
{
    if(IsXY()) { return(x_.IsKnown() && y_.IsKnown()); }
    else       { return(r_.IsKnown() && a_.IsKnown()); }
}

//--------------------------------------------------------------------
/*! Returns the BComplex in text format.
 */
BText BComplex::Name() const
{
    BComplex* th = (BComplex*)(this);
    th->ToXY();
    th->ToRA();
    BText name = BText("(")+x_.Name()+")+i*("+y_.Name()+")";
    return(name);
}

//--------------------------------------------------------------------
// operators and mathematic functions implementation
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BBool operator != (BComplex z1, BComplex z2)
{
    return(!(z1==z2));
}

//--------------------------------------------------------------------
BComplex operator + (BComplex z1, BComplex z2)
{
    return(BComplex::XY(z1.X() + z2.X(), z1.Y() + z2.Y()));
}

//--------------------------------------------------------------------
BComplex operator -(BComplex z1, BComplex z2)
{
    return(BComplex::XY(z1.X() - z2.X(), z1.Y() - z2.Y()));
}

//--------------------------------------------------------------------
BComplex operator *(BComplex z1, BComplex z2)
{
    if(!z1.IsKnown() || !z2.IsKnown())
    {
	return(BComplex::XY(BDat::Unknown(),BDat::Unknown()));
    }
    else if(z1.IsXY() && z2.IsXY())
    {
	long double x1 = z1.X().Value();
	long double y1 = z1.Y().Value();
	long double x2 = z2.X().Value();
	long double y2 = z2.Y().Value();
	BDat x = x1*x2-y1*y2;
	BDat y = y1*x2+x1*y2;
	return(BComplex::XY(x,y));
    }
    else
    {
	return(BComplex::RA(z1.R() * z2.R(), Mod(z1.A() + z2.A(),2*BDat::Pi())));
    }
}

//--------------------------------------------------------------------
BComplex operator /(BComplex z1, BComplex z2)
{
    if(!z1.IsKnown() || !z2.IsKnown())
    {
	return(BComplex::XY(BDat::Unknown(),BDat::Unknown()));
    }
    else if(z1.IsXY() && z2.IsXY())
    {
	long double x1 = z1.X().Value();
	long double y1 = z1.Y().Value();
	long double x2 = z2.X().Value();
	long double y2 = z2.Y().Value();
	long double r2 = x2*x2+y2*y2;
	BDat x = (x1*x2+y1*y2)/r2;
	BDat y = (y1*x2-x1*y2)/r2;
	return(BComplex::XY(x,y));
    }
    else
    {
	return(BComplex::RA(z1.R() / z2.R(), Mod(z1.A() - z2.A(),2*BDat::Pi())));
    }
}


//--------------------------------------------------------------------
BComplex operator ^(BComplex z1, BComplex z2)
{
    if(z2.Y()==0)
    {
	long double r1 = z1.R().Value();
	long double a1 = z1.A().Value();
	long double x2 = z2.X().Value();
	long double r  = powl(r1,x2);
	long double a  = a1*x2;
	return(BComplex::RA(r,a));
    }
    else
    {
	long double r1 = z1.R().Value();
	long double a1 = z1.A().Value();
	long double x2 = z2.X().Value();
	long double y2 = z2.Y().Value();
	long double r  = powl(r1,x2)/expl(a1*y2);
	long double a  = a1*x2+y2*logl(r1);
	return(BComplex::RA(r,a));
    }
}


//--------------------------------------------------------------------
BComplex operator -(BComplex z)
{
    if(z.IsXY()) { return(BComplex::XY(-z.X(),-z.Y())); }
    else         { return(BComplex::RA(z.R(),z.A()+M_PI_2)); }
}

//--------------------------------------------------------------------
  BComplex Conj(const BComplex& z)
{
    if(z.IsXY()) { return(BComplex::XY(z.X(),-z.Y())); }
    else	 { return(BComplex::RA(z.R(),-z.A())); }
}


//--------------------------------------------------------------------
BComplex  Sqrt(BComplex z)
{
    if(!z.IsKnown())
    {
	return(z);
    }
    else
    {
	long double a = z.X().Value();
	long double b = z.Y().Value();
	//Trace(BText("\nSqrt(")+z.Name()+")");
	if(b==0)
	{
	    if(a>=0) { return(BComplex::XY(Sqrt( a), 0)); }
	    else     { return(BComplex::XY(0, Sqrt(-a))); }
	}
	else
	{
	    long double r2,r,x,y;
	    r2 = a*a+b*b;
	    //TraceName(r2);
	    r  = sqrtl(r2);
	    //TraceName(r);
	    x  = sqrtl((a+r)/2.0);
	    //TraceName(x);
	    if(x==0) { y = 0; }
	    else     { y = b / (2*x); }
	    //TraceName(y);
	    return(BComplex::XY(x,y));
	}
    }
}

//--------------------------------------------------------------------
BComplex Log(BComplex z)
{
    if(z.R()==0) { return(BComplex::Unknown()); }
    BDat x = Log(z.R());
    return(BComplex::XY(x,z.A()));
}

//--------------------------------------------------------------------
BComplex Exp(BComplex z)
{
    BDat r = Exp(z.X());
    BDat a = z.Y();
    BComplex z1 = BComplex::RA(r,a);
    z1.ToXY();
    return(z1);
}

//--------------------------------------------------------------------
BComplex Sin(BComplex z)
{
    return(SinH(Ci*z)/Ci);
}

//--------------------------------------------------------------------
BComplex Cos(BComplex z)
{
    return(CosH(Ci*z));
}

//--------------------------------------------------------------------
BComplex Tan(BComplex z)
{
    return(Sin(z)/Cos(z));
}

//--------------------------------------------------------------------
BComplex ASin(BComplex z)
{
    return(ASinH(Ci*z)/Ci);
}

//--------------------------------------------------------------------
BComplex ACos(BComplex z)
{
    return(ACosH(Ci)/Ci);
}

//--------------------------------------------------------------------
BComplex ATan(BComplex z)
{
    return(ATanH(Ci*z));
}

//--------------------------------------------------------------------
BComplex SinH(BComplex z)
{
    return((Exp(z)-Exp(-z))/BComplex::RC(2));
}

//--------------------------------------------------------------------
BComplex CosH(BComplex z)
{
    return((Exp(z)+Exp(-z))/BComplex::RC(2));
}

//--------------------------------------------------------------------
BComplex TanH(BComplex z)
{
    return(SinH(z)/CosH(z));
}

//--------------------------------------------------------------------
BComplex ASinH(BComplex z)
{
    return(Log(z+Sqrt(z*z+BComplex::RC(1))));
}

//--------------------------------------------------------------------
BComplex ACosH(BComplex z)
{
    return(Log(z+Sqrt(z*z-BComplex::RC(1))));
}

//--------------------------------------------------------------------
BComplex ATanH(BComplex z)
{
    return(Log((BComplex::RC(1)+z)/(BComplex::RC(1)-z))/BComplex::RC(2));
}

//--------------------------------------------------------------------
int ComplexRootCmp(const void* v1, const void* v2)

/*! Compairs two BParam receiving the pointers.
 */
//--------------------------------------------------------------------
{
  complex& z1 = *((complex*)v1);
  complex& z2 = *((complex*)v2);
  double z1m = z1.x*z1.x+z1.y*z1.y;
  double z2m = z2.x*z2.x+z2.y*z2.y;
  if(z1m < z2m) { return(-1); }
  if(z1m > z2m) { return( 1); }
  if(z1.x<z2.x) { return(-1); }
  if(z1.x>z2.x) { return( 1); }
  if(z1.y<z2.y) { return(-1); }
  if(z1.y>z2.y) { return( 1); }
  return(0);
}

//--------------------------------------------------------------------
  void tol_gsl_poly_complex_solve(
    const BPolyn<BDat>& pol, 
    BArray<complex>& row)
//--------------------------------------------------------------------
{
  int gcd = pol.Period();
  if(!gcd) 
  { 
    row.AllocBuffer(0);
    return; 
  }
  int n = pol.Degree() / gcd;
  int s = pol.Size();
  BArray<double> a(n+1);
  int k, d;

  for(k=0; k<s; k++)
  {
    d = pol[k].Degree()/gcd;
    a[d] = pol[k].Coef().Value();
  }
  gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc (n+1);
  row.AllocBuffer(n);
  gsl_poly_complex_solve (a.Buffer(), n+1, w, (double*)row.GetBuffer());
  row.Sort(ComplexRootCmp);
  gsl_poly_complex_workspace_free(w);
}

//--------------------------------------------------------------------
  void tol_gsl_poly_complex_solve(
    const BPolyn<BDat>& pol, 
    BArray<BComplex>& row)
//--------------------------------------------------------------------
{
  BArray<complex> row_;
  tol_gsl_poly_complex_solve(pol, row_);
  int n = row_.Size();
  row.AllocBuffer(n);
  int k=0;
  for(k=0; k<n; k++)
  {
    row(k) = BComplex(row_(k));
  }
}

