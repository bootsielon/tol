/* realfun.cpp: Definition of class BRRFunction.
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_brealfun.h>

BTraceInit("realfun.cpp");

static BDat distance_ = 0.000001;
BDat& Distance() { return(distance_); }


//--------------------------------------------------------------------
// BRRFunction function
//--------------------------------------------------------------------


//--------------------------------------------------------------------
BDat BRRFunction::FirstDerivativeRichardson(BDat point)

/*! Returns the value of the first derivative of the function in
 *  scope for the value given by point. The value of the derivative
 *  is calculated using an aproximation which involves values
 *  separated from point by Distance(). The aproximation
 *  is finally improved by Richardson's extrapolation formulae.
 */
//--------------------------------------------------------------------
{
  BDat h2 = Distance()/2;
  BDat h4 = Distance()/4;
  BDat aux;
  BDat n1h, n1h2, n1h4, n2h, n2h2, n3h;

  n1h = (1/(2*Distance())) * ((*this)[point+Distance()]-(*this)[point-Distance()]);
  //cout<<"(*this)[point+Distance()] ="<<(*this)[point+Distance()]<<"\n";
  //cout<<"(*this)[point-Distance()] ="<<(*this)[point-Distance()]<<"\n";
  //cout<<"(1/2*Distance()) ="<<(1/(2*Distance()))<<"\n";
  //cout<<"n1h ="<<n1h<<"\n";
  n1h2= (1/(2*h2)) * ((*this)[point+h2] - (*this)[point-h2]);
  //cout<<"n1h2 ="<<n1h2<<"\n";
  n1h4= (1/(2*h4)) * ((*this)[point+h4] - (*this)[point-h4]);
  //cout<<"n1h4 ="<<n1h4<<"\n";

  n2h=(4 * n1h2 - n1h)/3;
  //cout<<"n2h ="<<n2h<<"\n";
  n2h2=(4 * n1h4 - n1h2)/3;
  //cout<<"n2h2 ="<<n2h2<<"\n";
  n3h=(16 * n2h2 - n2h)/15;

  return(n3h);
}

//--------------------------------------------------------------------
BDat BRRFunction::FirstDerivative(BDat point, BDat funcValue)

/*! Returns the value of the first derivative of the function in
 *  scope for the value given by point. The value of the derivative
 *  is calculated using an aproximation which involves values
 *  separated from point by Distance(). The aproximation
 *  is finally improved by Richardson's extrapolation formulae.
 */
//--------------------------------------------------------------------
{
  BDat y0 = funcValue;
  BDat y1 = (*this)[point+Distance()];
  return((y1-y0)/Distance());
}


//--------------------------------------------------------------------
BDat BRRFunction::NewtonSolve(BDat x0, BDat y0, BDat tol, BInt maxIter)

/*! Returns the value x that f(x) = y with the Newton method begining
 *  at x0
 */
//--------------------------------------------------------------------
{
  BDat dist;
  BDat yd0;
  BInt n = 0;
  BDat x = x0;
  BDat y = y0;
  Std(BText("\nNewtonSolve [")+n+"] x0="+x0+" y0="+y0+"\n");
  bool ok;
  do
  {
	  n++;
    yd0  = FirstDerivative(x0, y0);
	  x    = x0 - y0/yd0;
	  dist = Abs(x-x0);
	  x0   = x;
    ok = x0.IsKnown() &&  y0.IsKnown() && 
        (Abs(y0)>0.0) &   (dist>tol) && (n<maxIter);
    if(ok)
    {
      y0 = (*this)[x0];
    }
    Std(BText("\nNewtonSolve [")+n+"] x0="+x0+" y0="+y0+"\n");
  }
  while(ok);
  return(x);
}

//--------------------------------------------------------------------
BDat BRRFunction::Integral(BDat a, BDat b, BInt npoints)

/*! Returns the value of the first integral of the function in scope
 *  for the intervale given by a and b. The value of the integral
 *  is calculated using the Newton-Cotes formules. The aproximation
 *  is calculated depending on the exactitude given by the number of
 *  points in npoints.
 *
 * WARNING: This function has to be improved in the future.
 */
//--------------------------------------------------------------------
{
  BDat result;
  BDat sign = 1;
  if(b<a)
  {
    BDat c = b; b=a; a=c;
    sign = -1;
  }
  BDat h = (b-a)/(npoints-1);
  switch(npoints)
  {
	  case(1):
		  result = ((*this)[(a+b)/2]) * (b-a);
		break;
    case(2):
      result = ((*this)[a]) + ((*this)[b]) * (h/2);
		break;
	  case(3):
		  result = (h/3) * ((*this)[a] + (4 * ((*this)[(a+b)/2])) + (*this)[b]);
		  break;
	  case(4):
		  result = ((3*h)/8) * ((*this)[a] + (3* ((*this)[a+h])) +
				  (3* ((*this)[a + (2*h)])) + (*this)[a + (3*h)]);
      break;
    case(5):
    {
      BDat f0 = (*this)[a];
      BDat f1 = (*this)[a+h];
      BDat f2 = (*this)[a+2*h];
      BDat f3 = (*this)[a+3*h];
      BDat f4 = (*this)[a+4*h];
		  result = (2*h/45) * (7*f0 + 32*f1 + 12* f2 + 32*f3 + 7*f4);
		break;
    }
  }
  return(sign*result);
}



#define MaxLevel 10000

//--------------------------------------------------------------------
BDat BRRFunction::AdaptiveSimpson(BDat a0, BDat b, BDat tolerance)

/*! Returns the value of the integral of the function in scope
 *  for the intervale given by a and b with a given tolerance.
 *
 * ALGORYTHM:
 *	      Adaptive integration with Simpson formula.
 *
 *		Title  : Analisis numerico.
 *		Ed.    : Grupo Editorial Iberoamerica.
 *		Author : Burden-Faires (1985) pag.194.
 */
//--------------------------------------------------------------------
{
  BDat sign = 1;
  if(b<a0)
  {
    BDat c = b; b=a0; a0=c;
    sign = -1;
  }
  BDat	 app   = 0;
  BInt	 eval  = 0;
  // Simpson approximation for full interval
  BInt	 i     = 1;
  BDat tol[MaxLevel]; tol[i] = tolerance/2;
  BDat a  [MaxLevel]; a	 [i] = a0;
  BDat h  [MaxLevel]; h	 [i] = (b-a0)/2;
  BDat fa [MaxLevel]; fa [i] = (*this)[a0]; eval++;
  BDat fc [MaxLevel]; fc [i] = (*this)[a0+h[i]]; eval++;
  BDat fb [MaxLevel]; fb [i] = (*this)[b]; eval++;
  BDat s  [MaxLevel]; s	 [i] = h[i]*(fa[i]+4*fc[i]+fb[i])/3;
  BInt l  [MaxLevel]; l	 [i] = 1;
  while((i>0) && app.IsKnown())
  {
    BDat fd = (*this)[a[i]+  h[i]/2]; eval++;
    BDat fe = (*this)[a[i]+3*h[i]/2]; eval++;
    // Simpson approximations for both subintervals
    BDat s1 = h[i]*(fa[i]+4*fd+fc[i])/6;
    BDat s2 = h[i]*(fc[i]+4*fe+fb[i])/6;

    BDat v1 = a	 [i];
    BDat v2 = fa [i];
    BDat v3 = fc [i];
    BDat v4 = fb [i];
    BDat v5 = h	 [i];
    BDat v6 = tol[i];
    BDat v7 = s	 [i];
    BInt v8 = l	 [i];

    i--;
    BDat ss = s1+s2;
//  if((Abs(ss-v7)<v6))
    if((Abs(ss-v7)<2*tolerance/(eval-1)))
    {
      app += ss;
    }
    else
    {
      if(v8>=MaxLevel-1)
      {
	Warning(I2("Adaptive Simpson's method failed",
		   "Fallo en el metodo adaptativo de Simpson"));
	return(0);
      }
      else
      {
	//Left subinterval data
	i++;
	a  [i] = v1;
	fa [i] = v2;
	fc [i] = fd;
	fb [i] = v3;
	h  [i] = v5/2;
	tol[i] = 10*tolerance/v8; //v6/2;
	s  [i] = s1;
	l  [i] = v8+1;

	//Right subinterval data
	i++;
	a  [i] = v1+v5;
	fa [i] = v3;
	fc [i] = fe;
	fb [i] = v4;
	h  [i] = v5/2;
	tol[i] = 10*tolerance/v8; //v6/2;
	s  [i] = s2;
	l  [i] = v8+1;

      }
    }
  }
  i++;
  return(sign*app);
}



//--------------------------------------------------------------------
BDat BRRFunction::Bisection(BDat a, BDat b)

/*! Returns the point p of the intervale given by a and b
 *  whose Distance() to the function's zero is less than accuracy,
 *  or the point p in which f(p)=0. This is calculated dividing
 *  the intervale in halves sucesively.
 */
//--------------------------------------------------------------------
{
    BDat p,fp,fa=(*this)[a],fb=(*this)[a];
    BInt i	= 0;
    BDat accuracy = BDat::Tolerance();
    BInt niter	= (BInt)BDat::MaxIter().Value();
    //BInt(Ceil((Log10(1/accuracy))/Log10(2)).Value());
    
    do
    {
	i++;
	p  = (b+a)/2;
	fp = (*this)[a];
/*
	cout<<"Bisection "<<i<<"\t"<<p.Name()<<"\t"<<fp.Name()<<"\t"
	                           <<a.Name()<<"\t"<<fa.Name()<<"\t"
	                           <<b.Name()<<"\t"<<fb.Name()<<"\n";
*/
	if(fa*fp>0) { a=p; fa=fp; }
	else	    { b=p; fb=fp; }
    }
    while((i<=niter) && (fp!=0) && ((b-a)/2>accuracy));
    return(p);
}


//--------------------------------------------------------------------
BDat BRRFunction::FixedPoint(BDat a, BDat b)

/*! Returns the point p in the function in scope, which is an
 *  aproximation of p0, until the diference between both is less
 *  than accuracy. This is calculated by taking sucesively the
 *  result of the function and introducing this value again in the
 *  function.
 *
 * WARNING: This method depends on what function you take, so that it can
 *	    converg or diverg. This is not a seccure method.
 */
//--------------------------------------------------------------------
{
    BDat p, p0;
    BInt control = 0;
    BInt i = 0;
    BDat accuracy = 0.001;
    BInt niter	= BInt(Ceil((Log10(1/accuracy))/Log10(2)).Value());
    
    p0=(*this).Bisection(a,b);
    do
    {
	p=p0-(*this)[p0];
	i++;
	if(Abs(p-p0)<accuracy)  { control=1; }
	p0=p;
    } while(i<=niter && !control);
    
    return(p);
}


//--------------------------------------------------------------------
BDat BRRFunction::Secant(BDat a, BDat b)

/*! Returns the point p in the function in scope, which is a
 *  maximum aproximation of a zero of the function in scope. This
 *  is calculated calling the Bisection method, and then make the
 *  aproximation more exactly with this method.
 */
//--------------------------------------------------------------------
{
    BDat p,p0,p1,q0,q1;
    BInt control = 0;
    BInt i = 1;
    BDat accuracy = 0.00001;
    BInt niter= 15;
    
    p0=(*this).Bisection(a,b);
    p1=p0+(accuracy*100);
    q0=(*this)[p0];
    cout<<"p1 = "<<p1.Value()<<"\n";
    q1=(*this)[p1];
    do
    {
	p=p1-(q1*(p1-p0)/(q1-q0));
	cout<<(p-p1)<<"\n";
	if(Abs(p-p1)<accuracy)  { control=1; }
	i++;
	cout<<i<<"\t"<<p.Name()<<"\t"<<q1.Name()<<"\n";
	p0=p1;
	q0=q1;
	p1=p;
	q1=(*this)[p];
    } while(i<=niter && !control);
    
    return(p);
}


//--------------------------------------------------------------------
BDat BRRFunction::MullerMethod(BDat a, BDat b)

/*! Returns the point p in the function in scope, which is a
 *  maximum aproximation of a zero of the function in scope. This
 *  is calculated taking three firts aproximations calling the
 *  Bisection method, and then making the aproximation more
 *  exactly with this method.
 *
 * WARNING: This method can cause an error because it can't calculate
 *          complex roots.
 */
//--------------------------------------------------------------------
{
    BDat x0,x1,x2,h,h1,h2,p,d,D,E,b2,q1,q2;
    BInt control = 0;
    BInt i = 1;
    BDat accuracy = 0.00001;
    BInt niter= (BInt)BDat::MaxIter().Value();
    
    x0=(*this).Bisection(a,b);
    x1=x0+(accuracy*100);
    x2=x0-(accuracy*100);
    h1=x1-x0;
    h2=x2-x1;
    BDat y0=(*this)[x0],y1=(*this)[x1],y2=(*this)[x2];
    q1=(y1-y0)/h1;
    q2=(y2-y1)/h2;
    d=(q2-q1)/(h2+h1);
    do
    {
	b2=q2+h2*d;
	D =Sqrt((b*b)-4*d*y2);
	
	if(Abs(b-D)<Abs(b+D))  { E=b+d; }
	else		       { E=b-d; }
	
	h=(-2*y2)/E;
	p=x2+h;
	
	if (Abs(h)<accuracy) { control=1; }
	i++;
	x0=x1; y0=y1;
	x1=x2; y1=y2;
	x2=p;  y2=(*this)[p];
	cout<<"MullerMethod "<<i<<"\t"<<x2.Name()<<"\t"<<y2.Name()<<"\n";
	h1=x1-x0;
	h2=x2-x1;
	q1=(y1-y0)/h1;
	q2=(y2-y1)/h2;
	d=(q2-q1)/(h2+h1);
    } while(i<=niter && !control);
    
    return(p);
}


//--------------------------------------------------------------------
void BRRFunction::MaximumMinimum(BDat a, BDat b)

/*! Consider wether a point of the function in scope that is known
 *  it is f'(p)=0, is a maximum or a minimum, calling the function
 *  SecondDerivative, which will give us his value, so we can know
 *  if is positive, so there will be a minimum, or negative, so
 *  there will be a maximum.
 */
//--------------------------------------------------------------------
{
    BDat p,p2;
    p=(*this).CriticPoints(a,b);
    cout<<"La función tiene un posible 0 en "<<p<<". F(p)= "<<(*this)[p]<<"\n";
    p2=(*this).SecondDerivative(p);
    cout<<"El valor de la segunda derivada en "<<p<<" es "<<p2<<"\n";
    
    if(p2!=0)
    {
	if(p2<0){ cout<<"La funcion tiene un maximo en "<<p<<"\n"; }
	else    { cout<<"La funcion tiene un minimo en "<<p<<"\n"; };
    }
}


//--------------------------------------------------------------------
BDat BRRFunction::SecondDerivative(BDat point)

/*! Returns the value of the second derivative of the function in
 *  scope for the value given by point. The value of the derivative
 *  is calculated using an aproximation which involves values
 *  separated from point by Distance().
 */
//--------------------------------------------------------------------
{
    BDat result;
    result = (1/(Distance()*Distance())) * ((*this)[point-Distance()]-2*(*this)[point]+
					     (*this)[point+Distance()]);
    
    return(result);
}

//--------------------------------------------------------------------
BDat BRRFunction::CriticPoints(BDat a, BDat b)

/*! Returns the point p of the intervale given by a and b whose
 *  Distance() to the first derivative's zero is less than accuracy,
 *  or the point p in which f'(p)=0. This is calculated dividing
 *  the intervale in halves sucesively.
 */
//--------------------------------------------------------------------
{
  BDat p,fp,fa;
  BInt i	= 0;
  BDat accuracy = Sqrt(Distance());
  BInt niter	= BInt(Ceil((Log10(1/accuracy))/Log10(2)).Value());

  do
  {
    p = a + ((b-a)/2);
    fp = (*this)[p];
    i++;
    if((*this).FirstDerivative(a,fa)*(*this).FirstDerivative(p,fp)>0)
    { 
      a=p; 
      fa = (*this)[a];
    }
    else
    { 
      b=p; 
    };
  }
  while(i<=niter && (*this).FirstDerivative(p,fp)!=0 && (b-a)/2>accuracy);

  return(p);
}
