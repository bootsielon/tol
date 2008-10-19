/*************************************************************************
Copyright (c) 2007, Sergey Bochkanov (ALGLIB project).

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/

#ifndef _polinterpolation_h
#define _polinterpolation_h

#include "ap.h"

/*************************************************************************
Interpolation using barycentric formula

F(t) = SUM(i=0,n-1,w[i]*f[i]/(t-x[i])) / SUM(i=0,n-1,w[i]/(t-x[i]))

Input parameters:
    X   -   interpolation nodes, array[0..N-1]
    F   -   function values, array[0..N-1]
    W   -   barycentric weights, array[0..N-1]
    N   -   nodes count, N>0
    T   -   interpolation point
    
Result:
    barycentric interpolant F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double barycentricinterpolation(const ap::real_1d_array& x,
     const ap::real_1d_array& f,
     const ap::real_1d_array& w,
     int n,
     double t);


/*************************************************************************
Polynomial interpolation on the equidistant nodes using barycentric
formula. O(N) complexity.

Input parameters:
    A,B -   interpolation interval [A,B]
    F   -   function values, array[0..N-1].
            F[i] = F(A+(B-A)*i/(N-1))
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double equidistantpolynomialinterpolation(double a,
     double b,
     const ap::real_1d_array& f,
     int n,
     double t);


/*************************************************************************
Polynomial interpolation on the Chebyshev nodes (first kind) using
barycentric formula. O(N) complexity.

Input parameters:
    A,B -   interpolation interval [A,B]
    F   -   function values, array[0..N-1].
            F[i] = F(0.5*(B+A) + 0.5*(B-A)*Cos(PI*(2*i+1)/(2*n)))
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double chebyshev1interpolation(double a,
     double b,
     const ap::real_1d_array& f,
     int n,
     double t);


/*************************************************************************
Polynomial interpolation on the Chebyshev nodes (second kind) using
barycentric formula. O(N) complexity.

Input parameters:
    A,B -   interpolation interval [A,B]
    F   -   function values, array[0..N-1].
            F[i] = F(0.5*(B+A) + 0.5*(B-A)*Cos(PI*i/(n-1)))
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double chebyshev2interpolation(double a,
     double b,
     const ap::real_1d_array& f,
     int n,
     double t);


/*************************************************************************
Polynomial interpolation on the arbitrary nodes using Neville's algorithm.
O(N^2) complexity.

Input parameters:
    X   -   interpolation nodes, array[0..N-1].
    F   -   function values, array[0..N-1].
    N   -   nodes count
    T   -   interpolation point

Result:
    the value of the interpolation polynomial F(t)

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
double nevilleinterpolation(const ap::real_1d_array& x,
     ap::real_1d_array f,
     int n,
     double t);


/*************************************************************************
Polynomial interpolation on the arbitrary nodes using Neville's algorithm.
O(N^2) complexity. Subroutine  returns  the  value  of  the  interpolation
polynomial, the first and the second derivative.

Input parameters:
    X   -   interpolation nodes, array[0..N-1].
    F   -   function values, array[0..N-1].
    N   -   nodes count
    T   -   interpolation point

Output parameters:
    P   -   the value of the interpolation polynomial F(t)
    DP  -   the first derivative of the interpolation polynomial dF(t)/dt
    D2P -   the second derivative of the interpolation polynomial d2F(t)/dt2

  -- ALGLIB --
     Copyright 28.05.2007 by Bochkanov Sergey
*************************************************************************/
void nevilledifferentiation(const ap::real_1d_array& x,
     ap::real_1d_array f,
     int n,
     double t,
     double& p,
     double& dp,
     double& d2p);


/*************************************************************************
Obsolete algorithm, replaced by NevilleInterpolation.
*************************************************************************/
double lagrangeinterpolate(int n,
     const ap::real_1d_array& x,
     ap::real_1d_array f,
     double t);


/*************************************************************************
Obsolete algorithm, replaced by NevilleInterpolationWithDerivative
*************************************************************************/
void lagrangederivative(int n,
     const ap::real_1d_array& x,
     ap::real_1d_array f,
     double t,
     double& p,
     double& dp);


#endif
