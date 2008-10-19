/*************************************************************************
Minpack Copyright Notice (1999) University of Chicago.  All rights reserved

Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the
following conditions are met:

1. Redistributions of source code must retain the above
copyright notice, this list of conditions and the following
disclaimer.

2. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials
provided with the distribution.

3. The end-user documentation included with the
redistribution, if any, must include the following
acknowledgment:

   "This product includes software developed by the
   University of Chicago, as Operator of Argonne National
   Laboratory.

Alternately, this acknowledgment may appear in the software
itself, if and wherever such third-party acknowledgments
normally appear.

4. WARRANTY DISCLAIMER. THE SOFTWARE IS SUPPLIED "AS IS"
WITHOUT WARRANTY OF ANY KIND. THE COPYRIGHT HOLDER, THE
UNITED STATES, THE UNITED STATES DEPARTMENT OF ENERGY, AND
THEIR EMPLOYEES: (1) DISCLAIM ANY WARRANTIES, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE
OR NON-INFRINGEMENT, (2) DO NOT ASSUME ANY LEGAL LIABILITY
OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR
USEFULNESS OF THE SOFTWARE, (3) DO NOT REPRESENT THAT USE OF
THE SOFTWARE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS, (4)
DO NOT WARRANT THAT THE SOFTWARE WILL FUNCTION
UNINTERRUPTED, THAT IT IS ERROR-FREE OR THAT ANY ERRORS WILL
BE CORRECTED.

5. LIMITATION OF LIABILITY. IN NO EVENT WILL THE COPYRIGHT
HOLDER, THE UNITED STATES, THE UNITED STATES DEPARTMENT OF
ENERGY, OR THEIR EMPLOYEES: BE LIABLE FOR ANY INDIRECT,
INCIDENTAL, CONSEQUENTIAL, SPECIAL OR PUNITIVE DAMAGES OF
ANY KIND OR NATURE, INCLUDING BUT NOT LIMITED TO LOSS OF
PROFITS OR LOSS OF DATA, FOR ANY REASON WHATSOEVER, WHETHER
SUCH LIABILITY IS ASSERTED ON THE BASIS OF CONTRACT, TORT
(INCLUDING NEGLIGENCE OR STRICT LIABILITY), OR OTHERWISE,
EVEN IF ANY OF SAID PARTIES HAS BEEN WARNED OF THE
POSSIBILITY OF SUCH LOSS OR DAMAGES.
*************************************************************************/

#include <stdafx.h>
#include "levenbergmarquardt.h"

void funcvecjac(const ap::real_1d_array& x,
     ap::real_1d_array& fvec,
     ap::real_2d_array& fjac,
     int& iflag)
{
}

void levenbergmarquardtqrfac(const int& m,
     const int& n,
     ap::real_2d_array& a,
     const bool& pivot,
     ap::integer_1d_array& ipvt,
     ap::real_1d_array& rdiag,
     ap::real_1d_array& acnorm,
     ap::real_1d_array& wa,
     ap::real_2d_array& w2);
void levenbergmarquardtqrsolv(const int& n,
     ap::real_2d_array& r,
     const ap::integer_1d_array& ipvt,
     const ap::real_1d_array& diag,
     const ap::real_1d_array& qtb,
     ap::real_1d_array& x,
     ap::real_1d_array& sdiag,
     ap::real_1d_array& wa);
void levenbergmarquardtpar(const int& n,
     ap::real_2d_array& r,
     const ap::integer_1d_array& ipvt,
     const ap::real_1d_array& diag,
     const ap::real_1d_array& qtb,
     const double& delta,
     double& par,
     ap::real_1d_array& x,
     ap::real_1d_array& sdiag,
     ap::real_1d_array& wa1,
     ap::real_1d_array& wa2);
void levenbergmarquardtnewiteration(const ap::real_1d_array& x);
bool additionallevenbergmarquardtstoppingcriterion(int iter);

/*************************************************************************
The subroutine minimizes the sum of squares  of  M nonlinear finctions  of
N  arguments  with  Levenberg-Marquardt  algorithm  using  Jacobian    and
information about function values.

Programmer  should  redefine  FuncVecJac  subroutine  which  takes array X
(argument)  whose  index  ranges  from  1 to N as an input and if variable
IFlag is equal to:
    * 1, returns vector of function values in array FVec (in elements from
      1 to M), not changing FJac.
    * 2,  returns  Jacobian  in  array FJac (in elements [1..M,1..N]), not
      changing FVec.
The subroutine can change the IFlag parameter by setting it into a negative
number. It will terminate program.

Programmer  can  also  redefine  LevenbergMarquardtNewIteration subroutine
which is called on each new step.   Current  point  X  is  passed into the
subroutine.   It  is  reasonable  to  redefine  the  subroutine for better
debugging, for example, to visualize the solution process.

The AdditionalLevenbergMarquardtStoppingCriterion could  be  redefined  to
modify stopping conditions.

Input parameters:
    N       –   number of unknowns, N>0.
    M       –   number of summable functions, M>=N.
    X       –   initial solution approximation.
                Array whose index ranges from 1 to N.
    EpsG    –   stopping criterion. Iterations are stopped, if  cosine  of
                the angle between vector of function values  and  each  of
                the  Jacobian  columns  if  less or equal EpsG by absolute
                value. In fact this value defines stopping condition which
                is based on the function gradient smallness.
    EpsF    –   stopping criterion. Iterations are  stopped,  if  relative
                decreasing of sum of function values squares (real and
                predicted on the base of extrapolation)  is  less or equal
                EpsF.
    EpsX    –   stopping criterion. Iterations are  stopped,  if  relative
                change of solution is less or equal EpsX.
    MaxIts  –   stopping  criterion.  Iterations  are  stopped,  if  their
                number exceeds MaxIts.

Output parameters:
    X       –   solution
                Array whose index ranges from 1 to N.
    Info    –   a reason of a program completion:
                    * -1 wrong parameters were specified,
                    * 0 interrupted by user,
                    * 1 relative decrease of sum of function values
                        squares (real and predicted on the base  of
                        extrapolation) is less or equal EpsF.
                    * 2 relative change of solution is less or equal
                        EpsX.
                    * 3 conditions (1) and (2) are fulfilled.
                    * 4 cosine of the angle between vector of function
                        values and each of the Jacobian columns is less
                        or equal EpsG by absolute value.
                    * 5 number of iterations exceeds MaxIts.
                    * 6 EpsF is too small.
                        It is impossible to get a better result.
                    * 7 EpsX is too small.
                        It is impossible to get a better result.
                    * 8 EpsG is too small. Vector of functions is
                        orthogonal to Jacobian columns with near-machine
                        precision.
argonne national laboratory. minpack project. march 1980.
burton s. garbow, kenneth e. hillstrom, jorge j. more

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from FORTRAN to
      pseudocode.
*************************************************************************/
void levenbergmarquardtminimize(const int& n,
     const int& m,
     ap::real_1d_array& x,
     const double& epsg,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     int& info)
{
    ap::real_1d_array fvec;
    ap::real_1d_array qtf;
    ap::integer_1d_array ipvt;
    ap::real_2d_array fjac;
    ap::real_2d_array w2;
    ap::real_1d_array wa1;
    ap::real_1d_array wa2;
    ap::real_1d_array wa3;
    ap::real_1d_array wa4;
    ap::real_1d_array diag;
    int mode;
    int nfev;
    int njev;
    double factor;
    int i;
    int iflag;
    int iter;
    int j;
    int l;
    double actred;
    double delta;
    double dirder;
    double fnorm;
    double fnorm1;
    double gnorm;
    double par;
    double pnorm;
    double prered;
    double ratio;
    double sum;
    double temp;
    double temp1;
    double temp2;
    double xnorm;
    double p1;
    double p5;
    double p25;
    double p75;
    double p0001;

    
    //
    // Factor is a positive input variable used in determining the
    // initial step bound. This bound is set to the product of
    // factor and the euclidean norm of diag*x if nonzero, or else
    // to factor itself. in most cases factor should lie in the
    // interval (.1,100.).
    // 100.0 is a generally recommended value.
    //
    factor = 100.0;
    
    //
    // mode is an integer input variable. if mode = 1, the
    // variables will be scaled internally. if mode = 2,
    // the scaling is specified by the input diag. other
    // values of mode are equivalent to mode = 1.
    //
    mode = 1;
    
    //
    // diag is an array of length n. if mode = 1
    // diag is internally set. if mode = 2, diag
    // must contain positive entries that serve as
    // multiplicative scale factors for the variables.
    //
    diag.setbounds(1, n);
    
    //
    // Initialization
    //
    qtf.setbounds(1, n);
    fvec.setbounds(1, m);
    fjac.setbounds(1, m, 1, n);
    w2.setbounds(1, n, 1, m);
    ipvt.setbounds(1, n);
    wa1.setbounds(1, n);
    wa2.setbounds(1, n);
    wa3.setbounds(1, n);
    wa4.setbounds(1, m);
    p1 = 1.0E-1;
    p5 = 5.0E-1;
    p25 = 2.5E-1;
    p75 = 7.5E-1;
    p0001 = 1.0E-4;
    info = 0;
    iflag = 0;
    nfev = 0;
    njev = 0;
    
    //
    // check the input parameters for errors.
    //
    if( n<=0||m<n )
    {
        info = -1;
        return;
    }
    if( epsf<0||epsx<0||epsg<0 )
    {
        info = -1;
        return;
    }
    if( factor<=0 )
    {
        info = -1;
        return;
    }
    if( mode==2 )
    {
        for(j = 1; j <= n; j++)
        {
            if( diag(j)<=0 )
            {
                info = -1;
                return;
            }
        }
    }
    
    //
    // evaluate the function at the starting point
    // and calculate its norm.
    //
    iflag = 1;
    funcvecjac(x, fvec, fjac, iflag);
    nfev = 1;
    if( iflag<0 )
    {
        info = 0;
        return;
    }
    fnorm = ap::vdotproduct(fvec.getvector(1, m), fvec.getvector(1, m));
    fnorm = sqrt(fnorm);
    
    //
    // initialize levenberg-marquardt parameter and iteration counter.
    //
    par = 0;
    iter = 1;
    
    //
    // beginning of the outer loop.
    //
    while(true)
    {
        
        //
        // New iteration
        //
        levenbergmarquardtnewiteration(x);
        
        //
        // calculate the jacobian matrix.
        //
        iflag = 2;
        funcvecjac(x, fvec, fjac, iflag);
        njev = njev+1;
        if( iflag<0 )
        {
            info = 0;
            return;
        }
        
        //
        // compute the qr factorization of the jacobian.
        //
        levenbergmarquardtqrfac(m, n, fjac, true, ipvt, wa1, wa2, wa3, w2);
        
        //
        // on the first iteration and if mode is 1, scale according
        // to the norms of the columns of the initial jacobian.
        //
        if( iter==1 )
        {
            if( mode!=2 )
            {
                for(j = 1; j <= n; j++)
                {
                    diag(j) = wa2(j);
                    if( wa2(j)==0 )
                    {
                        diag(j) = 1;
                    }
                }
            }
            
            //
            // on the first iteration, calculate the norm of the scaled x
            // and initialize the step bound delta.
            //
            for(j = 1; j <= n; j++)
            {
                wa3(j) = diag(j)*x(j);
            }
            xnorm = ap::vdotproduct(wa3.getvector(1, n), wa3.getvector(1, n));
            xnorm = sqrt(xnorm);
            delta = factor*xnorm;
            if( delta==0 )
            {
                delta = factor;
            }
        }
        
        //
        // form (q transpose)*fvec and store the first n components in
        // qtf.
        //
        for(i = 1; i <= m; i++)
        {
            wa4(i) = fvec(i);
        }
        for(j = 1; j <= n; j++)
        {
            if( fjac(j,j)!=0 )
            {
                sum = 0;
                for(i = j; i <= m; i++)
                {
                    sum = sum+fjac(i,j)*wa4(i);
                }
                temp = -sum/fjac(j,j);
                for(i = j; i <= m; i++)
                {
                    wa4(i) = wa4(i)+fjac(i,j)*temp;
                }
            }
            fjac(j,j) = wa1(j);
            qtf(j) = wa4(j);
        }
        
        //
        // compute the norm of the scaled gradient.
        //
        gnorm = 0;
        if( fnorm!=0 )
        {
            for(j = 1; j <= n; j++)
            {
                l = ipvt(j);
                if( wa2(l)!=0 )
                {
                    sum = 0;
                    for(i = 1; i <= j; i++)
                    {
                        sum = sum+fjac(i,j)*(qtf(i)/fnorm);
                    }
                    gnorm = ap::maxreal(gnorm, fabs(sum/wa2(l)));
                }
            }
        }
        
        //
        // test for convergence of the gradient norm.
        //
        if( gnorm<=epsg )
        {
            info = 4;
        }
        if( info!=0 )
        {
            return;
        }
        
        //
        // rescale if necessary.
        //
        if( mode!=2 )
        {
            for(j = 1; j <= n; j++)
            {
                diag(j) = ap::maxreal(diag(j), wa2(j));
            }
        }
        
        //
        // beginning of the inner loop.
        //
        while(true)
        {
            
            //
            // determine the levenberg-marquardt parameter.
            //
            levenbergmarquardtpar(n, fjac, ipvt, diag, qtf, delta, par, wa1, wa2, wa3, wa4);
            
            //
            // store the direction p and x + p. calculate the norm of p.
            //
            for(j = 1; j <= n; j++)
            {
                wa1(j) = -wa1(j);
                wa2(j) = x(j)+wa1(j);
                wa3(j) = diag(j)*wa1(j);
            }
            pnorm = ap::vdotproduct(wa3.getvector(1, n), wa3.getvector(1, n));
            pnorm = sqrt(pnorm);
            
            //
            // on the first iteration, adjust the initial step bound.
            //
            if( iter==1 )
            {
                delta = ap::minreal(delta, pnorm);
            }
            
            //
            // evaluate the function at x + p and calculate its norm.
            //
            iflag = 1;
            funcvecjac(wa2, wa4, fjac, iflag);
            nfev = nfev+1;
            if( iflag<0 )
            {
                info = 0;
                return;
            }
            fnorm1 = ap::vdotproduct(wa4.getvector(1, m), wa4.getvector(1, m));
            fnorm1 = sqrt(fnorm1);
            
            //
            // compute the scaled actual reduction.
            //
            actred = -1;
            if( p1*fnorm1<fnorm )
            {
                actred = 1-ap::sqr(fnorm1/fnorm);
            }
            
            //
            // compute the scaled predicted reduction and
            // the scaled directional derivative.
            //
            for(j = 1; j <= n; j++)
            {
                wa3(j) = 0;
                l = ipvt(j);
                temp = wa1(l);
                for(i = 1; i <= j; i++)
                {
                    wa3(i) = wa3(i)+fjac(i,j)*temp;
                }
            }
            temp1 = ap::vdotproduct(wa3.getvector(1, n), wa3.getvector(1, n));
            temp1 = sqrt(temp1)/fnorm;
            temp2 = sqrt(par)*pnorm/fnorm;
            prered = ap::sqr(temp1)+ap::sqr(temp2)/p5;
            dirder = -(ap::sqr(temp1)+ap::sqr(temp2));
            
            //
            // compute the ratio of the actual to the predicted
            // reduction.
            //
            ratio = 0;
            if( prered!=0 )
            {
                ratio = actred/prered;
            }
            
            //
            // update the step bound.
            //
            if( ratio>p25 )
            {
                if( par==0||ratio>=p75 )
                {
                    delta = pnorm/p5;
                    par = p5*par;
                }
            }
            else
            {
                if( actred>=0 )
                {
                    temp = p5;
                }
                if( actred<0 )
                {
                    temp = p5*dirder/(dirder+p5*actred);
                }
                if( p1*fnorm1>=fnorm||temp<p1 )
                {
                    temp = p1;
                }
                delta = temp*ap::minreal(delta, pnorm/p1);
                par = par/temp;
            }
            
            //
            // test for successful iteration.
            //
            if( ratio>=p0001 )
            {
                
                //
                // successful iteration. update x, fvec, and their norms.
                //
                for(j = 1; j <= n; j++)
                {
                    x(j) = wa2(j);
                    wa2(j) = diag(j)*x(j);
                }
                for(i = 1; i <= m; i++)
                {
                    fvec(i) = wa4(i);
                }
                xnorm = ap::vdotproduct(wa2.getvector(1, n), wa2.getvector(1, n));
                xnorm = sqrt(xnorm);
                fnorm = fnorm1;
                iter = iter+1;
            }
            
            //
            // tests for convergence.
            //
            if( fabs(actred)<=epsf&&prered<=epsf&&p5*ratio<=1 )
            {
                info = 1;
            }
            if( delta<=epsx*xnorm )
            {
                info = 2;
            }
            if( fabs(actred)<=epsf&&prered<=epsf&&p5*ratio<=1&&info==2 )
            {
                info = 3;
            }
            if( info!=0 )
            {
                return;
            }
            
            //
            // tests for termination and stringent tolerances.
            //
            if( iter>=maxits&&maxits>0 )
            {
                info = 5;
            }
            if( fabs(actred)<=ap::machineepsilon&&prered<=ap::machineepsilon&&p5*ratio<=1 )
            {
                info = 6;
            }
            if( delta<=ap::machineepsilon*xnorm )
            {
                info = 7;
            }
            if( gnorm<=ap::machineepsilon )
            {
                info = 8;
            }
            if( info!=0 )
            {
                return;
            }
            
            //
            // end of the inner loop. repeat if iteration unsuccessful.
            //
            if( ratio<p0001 )
            {
                continue;
            }
            break;
        }
        
        //
        // Termination criterion
        //
        if( additionallevenbergmarquardtstoppingcriterion(iter) )
        {
            info = 0;
            return;
        }
        
        //
        // end of the outer loop.
        //
    }
}


void levenbergmarquardtqrfac(const int& m,
     const int& n,
     ap::real_2d_array& a,
     const bool& pivot,
     ap::integer_1d_array& ipvt,
     ap::real_1d_array& rdiag,
     ap::real_1d_array& acnorm,
     ap::real_1d_array& wa,
     ap::real_2d_array& w2)
{
    int i;
    int j;
    int jp1;
    int k;
    int kmax;
    int minmn;
    double ajnorm;
    double sum;
    double temp;
    double v;

    
    //
    // Copy from a to w2 and transpose
    //
    for(i = 1; i <= m; i++)
    {
        ap::vmove(w2.getcolumn(i, 1, n), a.getrow(i, 1, n));
    }
    
    //
    // compute the initial column norms and initialize several arrays.
    //
    for(j = 1; j <= n; j++)
    {
        v = ap::vdotproduct(w2.getrow(j, 1, m), w2.getrow(j, 1, m));
        acnorm(j) = sqrt(v);
        rdiag(j) = acnorm(j);
        wa(j) = rdiag(j);
        if( pivot )
        {
            ipvt(j) = j;
        }
    }
    
    //
    // reduce a to r with householder transformations.
    //
    minmn = ap::minint(m, n);
    for(j = 1; j <= minmn; j++)
    {
        if( pivot )
        {
            
            //
            // bring the column of largest norm into the pivot position.
            //
            kmax = j;
            for(k = j; k <= n; k++)
            {
                if( rdiag(k)>rdiag(kmax) )
                {
                    kmax = k;
                }
            }
            if( kmax!=j )
            {
                for(i = 1; i <= m; i++)
                {
                    temp = w2(j,i);
                    w2(j,i) = w2(kmax,i);
                    w2(kmax,i) = temp;
                }
                rdiag(kmax) = rdiag(j);
                wa(kmax) = wa(j);
                k = ipvt(j);
                ipvt(j) = ipvt(kmax);
                ipvt(kmax) = k;
            }
        }
        
        //
        // compute the householder transformation to reduce the
        // j-th column of a to a multiple of the j-th unit vector.
        //
        v = ap::vdotproduct(w2.getrow(j, j, m), w2.getrow(j, j, m));
        ajnorm = sqrt(v);
        if( ajnorm!=0 )
        {
            if( w2(j,j)<0 )
            {
                ajnorm = -ajnorm;
            }
            v = 1/ajnorm;
            ap::vmul(w2.getrow(j, j, m), v);
            w2(j,j) = w2(j,j)+1.0;
            
            //
            // apply the transformation to the remaining columns
            // and update the norms.
            //
            jp1 = j+1;
            if( n>=jp1 )
            {
                for(k = jp1; k <= n; k++)
                {
                    sum = ap::vdotproduct(w2.getrow(j, j, m), w2.getrow(k, j, m));
                    temp = sum/w2(j,j);
                    ap::vsub(w2.getrow(k, j, m), w2.getrow(j, j, m), temp);
                    if( pivot&&rdiag(k)!=0 )
                    {
                        temp = w2(k,j)/rdiag(k);
                        rdiag(k) = rdiag(k)*sqrt(ap::maxreal(double(0), 1-ap::sqr(temp)));
                        if( 0.05*ap::sqr(rdiag(k)/wa(k))<=ap::machineepsilon )
                        {
                            v = ap::vdotproduct(w2.getrow(k, jp1, jp1+m-j-1), w2.getrow(k, jp1, jp1+m-j-1));
                            rdiag(k) = sqrt(v);
                            wa(k) = rdiag(k);
                        }
                    }
                }
            }
        }
        rdiag(j) = -ajnorm;
    }
    
    //
    // Copy from w2 to a and transpose
    //
    for(i = 1; i <= m; i++)
    {
        ap::vmove(a.getrow(i, 1, n), w2.getcolumn(i, 1, n));
    }
}


void levenbergmarquardtqrsolv(const int& n,
     ap::real_2d_array& r,
     const ap::integer_1d_array& ipvt,
     const ap::real_1d_array& diag,
     const ap::real_1d_array& qtb,
     ap::real_1d_array& x,
     ap::real_1d_array& sdiag,
     ap::real_1d_array& wa)
{
    int i;
    int j;
    int jp1;
    int k;
    int kp1;
    int l;
    int nsing;
    double cs;
    double ct;
    double qtbpj;
    double sn;
    double sum;
    double t;
    double temp;

    
    //
    // copy r and (q transpose)*b to preserve input and initialize s.
    // in particular, save the diagonal elements of r in x.
    //
    for(j = 1; j <= n; j++)
    {
        for(i = j; i <= n; i++)
        {
            r(i,j) = r(j,i);
        }
        x(j) = r(j,j);
        wa(j) = qtb(j);
    }
    
    //
    // eliminate the diagonal matrix d using a givens rotation.
    //
    for(j = 1; j <= n; j++)
    {
        
        //
        // prepare the row of d to be eliminated, locating the
        // diagonal element using p from the qr factorization.
        //
        l = ipvt(j);
        if( diag(l)!=0 )
        {
            for(k = j; k <= n; k++)
            {
                sdiag(k) = 0;
            }
            sdiag(j) = diag(l);
            
            //
            // the transformations to eliminate the row of d
            // modify only a single element of (q transpose)*b
            // beyond the first n, which is initially zero.
            //
            qtbpj = 0;
            for(k = j; k <= n; k++)
            {
                
                //
                // determine a givens rotation which eliminates the
                // appropriate element in the current row of d.
                //
                if( sdiag(k)!=0 )
                {
                    if( fabs(r(k,k))>=fabs(sdiag(k)) )
                    {
                        t = sdiag(k)/r(k,k);
                        cs = 0.5/sqrt(0.25+0.25*ap::sqr(t));
                        sn = cs*t;
                    }
                    else
                    {
                        ct = r(k,k)/sdiag(k);
                        sn = 0.5/sqrt(0.25+0.25*ap::sqr(ct));
                        cs = sn*ct;
                    }
                    
                    //
                    // compute the modified diagonal element of r and
                    // the modified element of ((q transpose)*b,0).
                    //
                    r(k,k) = cs*r(k,k)+sn*sdiag(k);
                    temp = cs*wa(k)+sn*qtbpj;
                    qtbpj = -sn*wa(k)+cs*qtbpj;
                    wa(k) = temp;
                    
                    //
                    // accumulate the tranformation in the row of s.
                    //
                    kp1 = k+1;
                    if( n>=kp1 )
                    {
                        for(i = kp1; i <= n; i++)
                        {
                            temp = cs*r(i,k)+sn*sdiag(i);
                            sdiag(i) = -sn*r(i,k)+cs*sdiag(i);
                            r(i,k) = temp;
                        }
                    }
                }
            }
        }
        
        //
        // store the diagonal element of s and restore
        // the corresponding diagonal element of r.
        //
        sdiag(j) = r(j,j);
        r(j,j) = x(j);
    }
    
    //
    // solve the triangular system for z. if the system is
    // singular, then obtain a least squares solution.
    //
    nsing = n;
    for(j = 1; j <= n; j++)
    {
        if( sdiag(j)==0&&nsing==n )
        {
            nsing = j-1;
        }
        if( nsing<n )
        {
            wa(j) = 0;
        }
    }
    if( nsing>=1 )
    {
        for(k = 1; k <= nsing; k++)
        {
            j = nsing-k+1;
            sum = 0;
            jp1 = j+1;
            if( nsing>=jp1 )
            {
                for(i = jp1; i <= nsing; i++)
                {
                    sum = sum+r(i,j)*wa(i);
                }
            }
            wa(j) = (wa(j)-sum)/sdiag(j);
        }
    }
    
    //
    // permute the components of z back to components of x.
    //
    for(j = 1; j <= n; j++)
    {
        l = ipvt(j);
        x(l) = wa(j);
    }
}


void levenbergmarquardtpar(const int& n,
     ap::real_2d_array& r,
     const ap::integer_1d_array& ipvt,
     const ap::real_1d_array& diag,
     const ap::real_1d_array& qtb,
     const double& delta,
     double& par,
     ap::real_1d_array& x,
     ap::real_1d_array& sdiag,
     ap::real_1d_array& wa1,
     ap::real_1d_array& wa2)
{
    int i;
    int iter;
    int j;
    int jm1;
    int jp1;
    int k;
    int l;
    int nsing;
    double dxnorm;
    double dwarf;
    double fp;
    double gnorm;
    double parc;
    double parl;
    double paru;
    double sum;
    double temp;
    double v;

    dwarf = ap::minrealnumber;
    
    //
    // compute and store in x the gauss-newton direction. if the
    // jacobian is rank-deficient, obtain a least squares solution.
    //
    nsing = n;
    for(j = 1; j <= n; j++)
    {
        wa1(j) = qtb(j);
        if( r(j,j)==0&&nsing==n )
        {
            nsing = j-1;
        }
        if( nsing<n )
        {
            wa1(j) = 0;
        }
    }
    if( nsing>=1 )
    {
        for(k = 1; k <= nsing; k++)
        {
            j = nsing-k+1;
            wa1(j) = wa1(j)/r(j,j);
            temp = wa1(j);
            jm1 = j-1;
            if( jm1>=1 )
            {
                for(i = 1; i <= jm1; i++)
                {
                    wa1(i) = wa1(i)-r(i,j)*temp;
                }
            }
        }
    }
    for(j = 1; j <= n; j++)
    {
        l = ipvt(j);
        x(l) = wa1(j);
    }
    
    //
    // initialize the iteration counter.
    // evaluate the function at the origin, and test
    // for acceptance of the gauss-newton direction.
    //
    iter = 0;
    for(j = 1; j <= n; j++)
    {
        wa2(j) = diag(j)*x(j);
    }
    v = ap::vdotproduct(wa2.getvector(1, n), wa2.getvector(1, n));
    dxnorm = sqrt(v);
    fp = dxnorm-delta;
    if( fp<=0.1*delta )
    {
        
        //
        // termination.
        //
        if( iter==0 )
        {
            par = 0;
        }
        return;
    }
    
    //
    // if the jacobian is not rank deficient, the newton
    // step provides a lower bound, parl, for the zero of
    // the function. otherwise set this bound to zero.
    //
    parl = 0;
    if( nsing>=n )
    {
        for(j = 1; j <= n; j++)
        {
            l = ipvt(j);
            wa1(j) = diag(l)*(wa2(l)/dxnorm);
        }
        for(j = 1; j <= n; j++)
        {
            sum = 0;
            jm1 = j-1;
            if( jm1>=1 )
            {
                for(i = 1; i <= jm1; i++)
                {
                    sum = sum+r(i,j)*wa1(i);
                }
            }
            wa1(j) = (wa1(j)-sum)/r(j,j);
        }
        v = ap::vdotproduct(wa1.getvector(1, n), wa1.getvector(1, n));
        temp = sqrt(v);
        parl = fp/delta/temp/temp;
    }
    
    //
    // calculate an upper bound, paru, for the zero of the function.
    //
    for(j = 1; j <= n; j++)
    {
        sum = 0;
        for(i = 1; i <= j; i++)
        {
            sum = sum+r(i,j)*qtb(i);
        }
        l = ipvt(j);
        wa1(j) = sum/diag(l);
    }
    v = ap::vdotproduct(wa1.getvector(1, n), wa1.getvector(1, n));
    gnorm = sqrt(v);
    paru = gnorm/delta;
    if( paru==0 )
    {
        paru = dwarf/ap::minreal(delta, 0.1);
    }
    
    //
    // if the input par lies outside of the interval (parl,paru),
    // set par to the closer endpoint.
    //
    par = ap::maxreal(par, parl);
    par = ap::minreal(par, paru);
    if( par==0 )
    {
        par = gnorm/dxnorm;
    }
    
    //
    // beginning of an iteration.
    //
    while(true)
    {
        iter = iter+1;
        
        //
        // evaluate the function at the current value of par.
        //
        if( par==0 )
        {
            par = ap::maxreal(dwarf, 0.001*paru);
        }
        temp = sqrt(par);
        for(j = 1; j <= n; j++)
        {
            wa1(j) = temp*diag(j);
        }
        levenbergmarquardtqrsolv(n, r, ipvt, wa1, qtb, x, sdiag, wa2);
        for(j = 1; j <= n; j++)
        {
            wa2(j) = diag(j)*x(j);
        }
        v = ap::vdotproduct(wa2.getvector(1, n), wa2.getvector(1, n));
        dxnorm = sqrt(v);
        temp = fp;
        fp = dxnorm-delta;
        
        //
        // if the function is small enough, accept the current value
        // of par. also test for the exceptional cases where parl
        // is zero or the number of iterations has reached 10.
        //
        if( fabs(fp)<=0.1*delta||parl==0&&fp<=temp&&temp<0||iter==10 )
        {
            break;
        }
        
        //
        // compute the newton correction.
        //
        for(j = 1; j <= n; j++)
        {
            l = ipvt(j);
            wa1(j) = diag(l)*(wa2(l)/dxnorm);
        }
        for(j = 1; j <= n; j++)
        {
            wa1(j) = wa1(j)/sdiag(j);
            temp = wa1(j);
            jp1 = j+1;
            if( n>=jp1 )
            {
                for(i = jp1; i <= n; i++)
                {
                    wa1(i) = wa1(i)-r(i,j)*temp;
                }
            }
        }
        v = ap::vdotproduct(wa1.getvector(1, n), wa1.getvector(1, n));
        temp = sqrt(v);
        parc = fp/delta/temp/temp;
        
        //
        // depending on the sign of the function, update parl or paru.
        //
        if( fp>0 )
        {
            parl = ap::maxreal(parl, par);
        }
        if( fp<0 )
        {
            paru = ap::minreal(paru, par);
        }
        
        //
        // compute an improved estimate for par.
        //
        par = ap::maxreal(parl, par+parc);
        
        //
        // end of an iteration.
        //
    }
    
    //
    // termination.
    //
    if( iter==0 )
    {
        par = 0;
    }
}


void levenbergmarquardtnewiteration(const ap::real_1d_array& x)
{

}


bool additionallevenbergmarquardtstoppingcriterion(int iter)
{
    bool result;

    result = false;
    return result;
}



