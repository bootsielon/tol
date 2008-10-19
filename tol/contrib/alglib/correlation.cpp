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
#include "correlation.h"

void rankx(ap::real_1d_array& x, int n);

/*************************************************************************
Pearson product-moment correlation coefficient

Input parameters:
    X       -   sample 1 (array indexes: [0..N-1])
    Y       -   sample 2 (array indexes: [0..N-1])
    N       -   sample size.

Result:
    Pearson product-moment correlation coefficient

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
double pearsoncorrelation(const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     int n)
{
    double result;
    int i;
    double xmean;
    double ymean;
    double s;
    double xv;
    double yv;
    double t1;
    double t2;

    xv = 0;
    yv = 0;
    if( n<=1 )
    {
        result = 0;
        return result;
    }
    
    //
    // Mean
    //
    xmean = 0;
    ymean = 0;
    for(i = 0; i <= n-1; i++)
    {
        xmean = xmean+x(i);
        ymean = ymean+y(i);
    }
    xmean = xmean/n;
    ymean = ymean/n;
    
    //
    // numerator and denominator
    //
    s = 0;
    xv = 0;
    yv = 0;
    for(i = 0; i <= n-1; i++)
    {
        t1 = x(i)-xmean;
        t2 = y(i)-ymean;
        xv = xv+ap::sqr(t1);
        yv = yv+ap::sqr(t2);
        s = s+t1*t2;
    }
    if( xv==0||yv==0 )
    {
        result = 0;
    }
    else
    {
        result = s/(sqrt(xv)*sqrt(yv));
    }
    return result;
}


/*************************************************************************
Spearman's rank correlation coefficient

Input parameters:
    X       -   sample 1 (array indexes: [0..N-1])
    Y       -   sample 2 (array indexes: [0..N-1])
    N       -   sample size.

Result:
    Spearman's rank correlation coefficient

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
double spearmanrankcorrelation(ap::real_1d_array x,
     ap::real_1d_array y,
     int n)
{
    double result;

    rankx(x, n);
    rankx(y, n);
    result = pearsoncorrelation(x, y, n);
    return result;
}


/*************************************************************************
Internal ranking subroutine
*************************************************************************/
void rankx(ap::real_1d_array& x, int n)
{
    int i;
    int j;
    int k;
    int t;
    double tmp;
    int tmpi;
    ap::real_1d_array r;
    ap::integer_1d_array c;

    
    //
    // Prepare
    //
    if( n<=1 )
    {
        x(0) = 1;
        return;
    }
    r.setbounds(0, n-1);
    c.setbounds(0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        r(i) = x(i);
        c(i) = i;
    }
    
    //
    // sort {R, C}
    //
    if( n!=1 )
    {
        i = 2;
        do
        {
            t = i;
            while(t!=1)
            {
                k = t/2;
                if( r(k-1)>=r(t-1) )
                {
                    t = 1;
                }
                else
                {
                    tmp = r(k-1);
                    r(k-1) = r(t-1);
                    r(t-1) = tmp;
                    tmpi = c(k-1);
                    c(k-1) = c(t-1);
                    c(t-1) = tmpi;
                    t = k;
                }
            }
            i = i+1;
        }
        while(i<=n);
        i = n-1;
        do
        {
            tmp = r(i);
            r(i) = r(0);
            r(0) = tmp;
            tmpi = c(i);
            c(i) = c(0);
            c(0) = tmpi;
            t = 1;
            while(t!=0)
            {
                k = 2*t;
                if( k>i )
                {
                    t = 0;
                }
                else
                {
                    if( k<i )
                    {
                        if( r(k)>r(k-1) )
                        {
                            k = k+1;
                        }
                    }
                    if( r(t-1)>=r(k-1) )
                    {
                        t = 0;
                    }
                    else
                    {
                        tmp = r(k-1);
                        r(k-1) = r(t-1);
                        r(t-1) = tmp;
                        tmpi = c(k-1);
                        c(k-1) = c(t-1);
                        c(t-1) = tmpi;
                        t = k;
                    }
                }
            }
            i = i-1;
        }
        while(i>=1);
    }
    
    //
    // compute tied ranks
    //
    i = 0;
    while(i<=n-1)
    {
        j = i+1;
        while(j<=n-1)
        {
            if( r(j)!=r(i) )
            {
                break;
            }
            j = j+1;
        }
        for(k = i; k <= j-1; k++)
        {
            r(k) = 1+double(i+j-1)/double(2);
        }
        i = j;
    }
    
    //
    // back to x
    //
    for(i = 0; i <= n-1; i++)
    {
        x(c(i)) = r(i);
    }
}



