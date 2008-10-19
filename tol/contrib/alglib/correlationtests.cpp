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
#include "correlationtests.h"

double spearmantail5(double s);
double spearmantail6(double s);
double spearmantail7(double s);
double spearmantail8(double s);
double spearmantail9(double s);
double spearmantail(double t, int n);

/*************************************************************************
Pearson's correlation coefficient significance test

This test checks hypotheses about whether X  and  Y  are  samples  of  two
continuous  distributions  having  zero  correlation  or   whether   their
correlation is non-zero.

The following tests are performed:
    * two-tailed test (null hypothesis - X and Y have zero correlation)
    * left-tailed test (null hypothesis - the correlation  coefficient  is
      greater than or equal to 0)
    * right-tailed test (null hypothesis - the correlation coefficient  is
      less than or equal to 0).

Requirements:
    * the number of elements in each sample is not less than 5
    * normality of distributions of X and Y.

Input parameters:
    R   -   Pearson's correlation coefficient for X and Y
    N   -   number of elements in samples, N>=5.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void pearsoncorrelationsignificance(double r,
     int n,
     double& bothtails,
     double& lefttail,
     double& righttail)
{
    double t;
    double p;

    
    //
    // Some special cases
    //
    if( r>=1 )
    {
        bothtails = 0.0;
        lefttail = 1.0;
        righttail = 0.0;
        return;
    }
    if( r<=-1 )
    {
        bothtails = 0.0;
        lefttail = 0.0;
        righttail = 1.0;
        return;
    }
    if( n<5 )
    {
        bothtails = 1.0;
        lefttail = 1.0;
        righttail = 1.0;
        return;
    }
    
    //
    // General case
    //
    t = r*sqrt((n-2)/(1-ap::sqr(r)));
    p = studenttdistribution(n-2, t);
    bothtails = 2*ap::minreal(p, 1-p);
    lefttail = p;
    righttail = 1-p;
}


/*************************************************************************
Spearman's rank correlation coefficient significance test

This test checks hypotheses about whether X  and  Y  are  samples  of  two
continuous  distributions  having  zero  correlation  or   whether   their
correlation is non-zero.

The following tests are performed:
    * two-tailed test (null hypothesis - X and Y have zero correlation)
    * left-tailed test (null hypothesis - the correlation  coefficient  is
      greater than or equal to 0)
    * right-tailed test (null hypothesis - the correlation coefficient  is
      less than or equal to 0).

Requirements:
    * the number of elements in each sample is not less than 5.

The test is non-parametric and doesn't require distributions X and Y to be
normal.

Input parameters:
    R   -   Spearman's rank correlation coefficient for X and Y
    N   -   number of elements in samples, N>=5.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void spearmanrankcorrelationsignificance(double r,
     int n,
     double& bothtails,
     double& lefttail,
     double& righttail)
{
    double t;
    double p;

    
    //
    // Special case
    //
    if( n<5 )
    {
        bothtails = 1.0;
        lefttail = 1.0;
        righttail = 1.0;
        return;
    }
    
    //
    // General case
    //
    if( r>=1 )
    {
        t = 1.0E10;
    }
    else
    {
        if( r<=-1 )
        {
            t = -1.0E10;
        }
        else
        {
            t = r*sqrt((n-2)/(1-ap::sqr(r)));
        }
    }
    if( t<0 )
    {
        p = spearmantail(t, n);
        bothtails = 2*p;
        lefttail = p;
        righttail = 1-p;
    }
    else
    {
        p = spearmantail(-t, n);
        bothtails = 2*p;
        lefttail = 1-p;
        righttail = p;
    }
}


/*************************************************************************
Tail(S, 5)
*************************************************************************/
double spearmantail5(double s)
{
    double result;

    if( s<0.000e+00 )
    {
        result = studenttdistribution(3, -s);
        return result;
    }
    if( s>=3.580e+00 )
    {
        result = 8.304e-03;
        return result;
    }
    if( s>=2.322e+00 )
    {
        result = 4.163e-02;
        return result;
    }
    if( s>=1.704e+00 )
    {
        result = 6.641e-02;
        return result;
    }
    if( s>=1.303e+00 )
    {
        result = 1.164e-01;
        return result;
    }
    if( s>=1.003e+00 )
    {
        result = 1.748e-01;
        return result;
    }
    if( s>=7.584e-01 )
    {
        result = 2.249e-01;
        return result;
    }
    if( s>=5.468e-01 )
    {
        result = 2.581e-01;
        return result;
    }
    if( s>=3.555e-01 )
    {
        result = 3.413e-01;
        return result;
    }
    if( s>=1.759e-01 )
    {
        result = 3.911e-01;
        return result;
    }
    if( s>=1.741e-03 )
    {
        result = 4.747e-01;
        return result;
    }
    if( s>=0.000e+00 )
    {
        result = 5.248e-01;
        return result;
    }
    result = 0;
    return result;
}


/*************************************************************************
Tail(S, 6)
*************************************************************************/
double spearmantail6(double s)
{
    double result;

    if( s<1.001e+00 )
    {
        result = studenttdistribution(4, -s);
        return result;
    }
    if( s>=5.663e+00 )
    {
        result = 1.366e-03;
        return result;
    }
    if( s>=3.834e+00 )
    {
        result = 8.350e-03;
        return result;
    }
    if( s>=2.968e+00 )
    {
        result = 1.668e-02;
        return result;
    }
    if( s>=2.430e+00 )
    {
        result = 2.921e-02;
        return result;
    }
    if( s>=2.045e+00 )
    {
        result = 5.144e-02;
        return result;
    }
    if( s>=1.747e+00 )
    {
        result = 6.797e-02;
        return result;
    }
    if( s>=1.502e+00 )
    {
        result = 8.752e-02;
        return result;
    }
    if( s>=1.295e+00 )
    {
        result = 1.210e-01;
        return result;
    }
    if( s>=1.113e+00 )
    {
        result = 1.487e-01;
        return result;
    }
    if( s>=1.001e+00 )
    {
        result = 1.780e-01;
        return result;
    }
    result = 0;
    return result;
}


/*************************************************************************
Tail(S, 7)
*************************************************************************/
double spearmantail7(double s)
{
    double result;

    if( s<1.001e+00 )
    {
        result = studenttdistribution(5, -s);
        return result;
    }
    if( s>=8.159e+00 )
    {
        result = 2.081e-04;
        return result;
    }
    if( s>=5.620e+00 )
    {
        result = 1.393e-03;
        return result;
    }
    if( s>=4.445e+00 )
    {
        result = 3.398e-03;
        return result;
    }
    if( s>=3.728e+00 )
    {
        result = 6.187e-03;
        return result;
    }
    if( s>=3.226e+00 )
    {
        result = 1.200e-02;
        return result;
    }
    if( s>=2.844e+00 )
    {
        result = 1.712e-02;
        return result;
    }
    if( s>=2.539e+00 )
    {
        result = 2.408e-02;
        return result;
    }
    if( s>=2.285e+00 )
    {
        result = 3.320e-02;
        return result;
    }
    if( s>=2.068e+00 )
    {
        result = 4.406e-02;
        return result;
    }
    if( s>=1.879e+00 )
    {
        result = 5.478e-02;
        return result;
    }
    if( s>=1.710e+00 )
    {
        result = 6.946e-02;
        return result;
    }
    if( s>=1.559e+00 )
    {
        result = 8.331e-02;
        return result;
    }
    if( s>=1.420e+00 )
    {
        result = 1.001e-01;
        return result;
    }
    if( s>=1.292e+00 )
    {
        result = 1.180e-01;
        return result;
    }
    if( s>=1.173e+00 )
    {
        result = 1.335e-01;
        return result;
    }
    if( s>=1.062e+00 )
    {
        result = 1.513e-01;
        return result;
    }
    if( s>=1.001e+00 )
    {
        result = 1.770e-01;
        return result;
    }
    result = 0;
    return result;
}


/*************************************************************************
Tail(S, 8)
*************************************************************************/
double spearmantail8(double s)
{
    double result;

    if( s<2.001e+00 )
    {
        result = studenttdistribution(6, -s);
        return result;
    }
    if( s>=1.103e+01 )
    {
        result = 2.194e-05;
        return result;
    }
    if( s>=7.685e+00 )
    {
        result = 2.008e-04;
        return result;
    }
    if( s>=6.143e+00 )
    {
        result = 5.686e-04;
        return result;
    }
    if( s>=5.213e+00 )
    {
        result = 1.138e-03;
        return result;
    }
    if( s>=4.567e+00 )
    {
        result = 2.310e-03;
        return result;
    }
    if( s>=4.081e+00 )
    {
        result = 3.634e-03;
        return result;
    }
    if( s>=3.697e+00 )
    {
        result = 5.369e-03;
        return result;
    }
    if( s>=3.381e+00 )
    {
        result = 7.708e-03;
        return result;
    }
    if( s>=3.114e+00 )
    {
        result = 1.087e-02;
        return result;
    }
    if( s>=2.884e+00 )
    {
        result = 1.397e-02;
        return result;
    }
    if( s>=2.682e+00 )
    {
        result = 1.838e-02;
        return result;
    }
    if( s>=2.502e+00 )
    {
        result = 2.288e-02;
        return result;
    }
    if( s>=2.340e+00 )
    {
        result = 2.883e-02;
        return result;
    }
    if( s>=2.192e+00 )
    {
        result = 3.469e-02;
        return result;
    }
    if( s>=2.057e+00 )
    {
        result = 4.144e-02;
        return result;
    }
    if( s>=2.001e+00 )
    {
        result = 4.804e-02;
        return result;
    }
    result = 0;
    return result;
}


/*************************************************************************
Tail(S, 9)
*************************************************************************/
double spearmantail9(double s)
{
    double result;

    if( s<2.001e+00 )
    {
        result = studenttdistribution(7, -s);
        return result;
    }
    if( s>=9.989e+00 )
    {
        result = 2.306e-05;
        return result;
    }
    if( s>=8.069e+00 )
    {
        result = 8.167e-05;
        return result;
    }
    if( s>=6.890e+00 )
    {
        result = 1.744e-04;
        return result;
    }
    if( s>=6.077e+00 )
    {
        result = 3.625e-04;
        return result;
    }
    if( s>=5.469e+00 )
    {
        result = 6.450e-04;
        return result;
    }
    if( s>=4.991e+00 )
    {
        result = 1.001e-03;
        return result;
    }
    if( s>=4.600e+00 )
    {
        result = 1.514e-03;
        return result;
    }
    if( s>=4.272e+00 )
    {
        result = 2.213e-03;
        return result;
    }
    if( s>=3.991e+00 )
    {
        result = 2.990e-03;
        return result;
    }
    if( s>=3.746e+00 )
    {
        result = 4.101e-03;
        return result;
    }
    if( s>=3.530e+00 )
    {
        result = 5.355e-03;
        return result;
    }
    if( s>=3.336e+00 )
    {
        result = 6.887e-03;
        return result;
    }
    if( s>=3.161e+00 )
    {
        result = 8.598e-03;
        return result;
    }
    if( s>=3.002e+00 )
    {
        result = 1.065e-02;
        return result;
    }
    if( s>=2.855e+00 )
    {
        result = 1.268e-02;
        return result;
    }
    if( s>=2.720e+00 )
    {
        result = 1.552e-02;
        return result;
    }
    if( s>=2.595e+00 )
    {
        result = 1.836e-02;
        return result;
    }
    if( s>=2.477e+00 )
    {
        result = 2.158e-02;
        return result;
    }
    if( s>=2.368e+00 )
    {
        result = 2.512e-02;
        return result;
    }
    if( s>=2.264e+00 )
    {
        result = 2.942e-02;
        return result;
    }
    if( s>=2.166e+00 )
    {
        result = 3.325e-02;
        return result;
    }
    if( s>=2.073e+00 )
    {
        result = 3.800e-02;
        return result;
    }
    if( s>=2.001e+00 )
    {
        result = 4.285e-02;
        return result;
    }
    result = 0;
    return result;
}


/*************************************************************************
Tail(T,N), accepts T<0
*************************************************************************/
double spearmantail(double t, int n)
{
    double result;

    if( n==5 )
    {
        result = spearmantail5(-t);
        return result;
    }
    if( n==6 )
    {
        result = spearmantail6(-t);
        return result;
    }
    if( n==7 )
    {
        result = spearmantail7(-t);
        return result;
    }
    if( n==8 )
    {
        result = spearmantail8(-t);
        return result;
    }
    if( n==9 )
    {
        result = spearmantail9(-t);
        return result;
    }
    result = studenttdistribution(n-2, t);
    return result;
}



