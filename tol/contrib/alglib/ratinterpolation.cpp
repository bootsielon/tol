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

#include <stdafx.h>
#include "ratinterpolation.h"

/*************************************************************************
Rational barycentric interpolation without poles

The subroutine constructs the rational interpolating function without real
poles. It should be noted that the barycentric weights of the  interpolant
constructed are independent of the values of the given function.

Input parameters:
    X   -   interpolation nodes, array[0..N-1].
    N   -   number of nodes, N>0.
    D   -   order of the interpolation scheme, 0 <= D <= N-1.

Output parameters:
    W   -   array of the barycentric weights which  can  be  used  in  the
            BarycentricInterpolate subroutine. Array[0..N-1]

Note:
    this algorithm always succeeds and calculates the weights  with  close
    to machine precision.

  -- ALGLIB PROJECT --
     Copyright 17.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildfloaterhormannrationalinterpolant(ap::real_1d_array x,
     int n,
     int d,
     ap::real_1d_array& w)
{
    double s0;
    double s;
    double v;
    int i;
    int j;
    int k;
    ap::integer_1d_array perm;
    ap::real_1d_array wtemp;

    ap::ap_error::make_assertion(n>0, "BuildRationalInterpolantWithoutPoles: N<=0!");
    ap::ap_error::make_assertion(d>=0&&d<=n, "BuildRationalInterpolantWithoutPoles: incorrect D!");
    
    //
    // Prepare
    //
    w.setbounds(0, n-1);
    s0 = 1;
    for(k = 1; k <= d; k++)
    {
        s0 = -s0;
    }
    perm.setbounds(0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        perm(i) = i;
    }
    for(i = 0; i <= n-2; i++)
    {
        for(j = i+1; j <= n-1; j++)
        {
            if( x(j)<x(i) )
            {
                s = x(i);
                x(i) = x(j);
                x(j) = s;
                k = perm(i);
                perm(i) = perm(j);
                perm(j) = k;
            }
        }
    }
    
    //
    // Calculate Wk
    //
    for(k = 0; k <= n-1; k++)
    {
        
        //
        // Wk
        //
        s = 0;
        for(i = ap::maxint(k-d, 0); i <= ap::minint(k, n-1-d); i++)
        {
            v = 1;
            for(j = i; j <= i+d; j++)
            {
                if( j!=k )
                {
                    v = v/fabs(x(k)-x(j));
                }
            }
            s = s+v;
        }
        w(k) = s0*s;
        
        //
        // Next S0
        //
        s0 = -s0;
    }
    
    //
    // Reorder W
    //
    wtemp.setbounds(0, n-1);
    ap::vmove(&wtemp(0), &w(0), ap::vlen(0,n-1));
    for(i = 0; i <= n-1; i++)
    {
        w(perm(i)) = wtemp(i);
    }
}



