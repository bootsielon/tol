
#include <stdafx.h>
#include <stdio.h>
#include "testratinterpolation.h"

double ftask(double t, int task);
void buildfloaterhormanntesttable();

bool testri(bool silent)
{
    bool result;
    bool waserrors;
    bool nperrors;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array w;
    ap::real_1d_array w2;
    int n;
    int m;
    int i;
    int j;
    int k;
    int d;
    int pass;
    int passcount;
    double err;
    double maxerr;
    double t;
    double a;
    double b;
    double s;
    int info;

    waserrors = false;
    passcount = 20;
    
    //
    // Testing "No Poles" interpolation
    //
    nperrors = false;
    maxerr = 0;
    for(n = 2; n <= 10; n++)
    {
        x.setbounds(0, n-1);
        y.setbounds(0, n-1);
        w.setbounds(0, n-1);
        w2.setbounds(0, n-1);
        
        //
        // D=1, non-equidistant nodes
        //
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Initialize X, Y, W
            //
            a = -1-1*ap::randomreal();
            b = +1+1*ap::randomreal();
            for(i = 0; i <= n-1; i++)
            {
                x(i) = atan((b-a)*i/(n-1)+a);
            }
            w(0) = -1/(x(1)-x(0));
            s = 1;
            for(i = 1; i <= n-2; i++)
            {
                w(i) = s*(1/(x(i)-x(i-1))+1/(x(i+1)-x(i)));
                s = -s;
            }
            w(n-1) = s/(x(n-1)-x(n-2));
            
            //
            // Mix
            //
            for(i = 0; i <= n-1; i++)
            {
                k = ap::randominteger(n);
                if( k!=i )
                {
                    t = x(i);
                    x(i) = x(k);
                    x(k) = t;
                    t = y(i);
                    y(i) = y(k);
                    y(k) = t;
                    t = w(i);
                    w(i) = w(k);
                    w(k) = t;
                }
            }
            
            //
            // Build and test
            //
            buildfloaterhormannrationalinterpolant(x, n, 1, w2);
            s = w(0)/w2(0);
            ap::vmul(w2.getvector(0, n-1), s);
            for(i = 0; i <= n-1; i++)
            {
                maxerr = ap::maxreal(maxerr, fabs((w(i)-w2(i))/w(i)));
            }
        }
        
        //
        // D = 0, 1, 2. Equidistant nodes.
        //
        for(d = 0; d <= 2; d++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                
                //
                // Skip incorrect (N,D) pairs
                //
                if( n<2*d )
                {
                    continue;
                }
                
                //
                // Initialize X, Y, W
                //
                a = -1-1*ap::randomreal();
                b = +1+1*ap::randomreal();
                for(i = 0; i <= n-1; i++)
                {
                    x(i) = (b-a)*i/(n-1)+a;
                }
                s = 1;
                if( d==0 )
                {
                    for(i = 0; i <= n-1; i++)
                    {
                        w(i) = s;
                        s = -s;
                    }
                }
                if( d==1 )
                {
                    w(0) = -s;
                    for(i = 1; i <= n-2; i++)
                    {
                        w(i) = 2*s;
                        s = -s;
                    }
                    w(n-1) = s;
                }
                if( d==2 )
                {
                    w(0) = s;
                    w(1) = -3*s;
                    for(i = 2; i <= n-3; i++)
                    {
                        w(i) = 4*s;
                        s = -s;
                    }
                    w(n-2) = 3*s;
                    w(n-1) = -s;
                }
                
                //
                // Mix
                //
                for(i = 0; i <= n-1; i++)
                {
                    k = ap::randominteger(n);
                    if( k!=i )
                    {
                        t = x(i);
                        x(i) = x(k);
                        x(k) = t;
                        t = y(i);
                        y(i) = y(k);
                        y(k) = t;
                        t = w(i);
                        w(i) = w(k);
                        w(k) = t;
                    }
                }
                
                //
                // Build and test
                //
                buildfloaterhormannrationalinterpolant(x, n, d, w2);
                s = w(0)/w2(0);
                ap::vmul(w2.getvector(0, n-1), s);
                for(i = 0; i <= n-1; i++)
                {
                    maxerr = ap::maxreal(maxerr, fabs((w(i)-w2(i))/w(i)));
                }
            }
        }
    }
    if( maxerr>100*ap::machineepsilon )
    {
        nperrors = true;
    }
    
    //
    // report
    //
    waserrors = nperrors;
    if( !silent )
    {
        printf("TESTING RATIONAL INTERPOLATION\n");
        printf("FLOATER-HORMANN TEST                     ");
        if( nperrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        if( waserrors )
        {
            printf("TEST FAILED\n");
        }
        else
        {
            printf("TEST PASSED\n");
        }
        printf("\n\n");
    }
    
    //
    // end
    //
    result = !waserrors;
    return result;
}


double ftask(double t, int task)
{
    double result;

    if( task==0||task==1 )
    {
        result = sin(t);
    }
    else
    {
        result = 1/(1+ap::sqr(t));
    }
    return result;
}


void buildfloaterhormanntesttable()
{
    int i;
    int j;
    int n;
    int d;
    ap::real_1d_array ematrix;
    double emin;
    int dmin;
    double err;
    double t;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array w;
    double a;
    double b;
    ap::integer_1d_array ntbl;
    int ncnt;
    int task;

    ncnt = 4;
    ntbl.setbounds(0, ncnt-1);
    ntbl(0) = 11;
    ntbl(1) = 21;
    ntbl(2) = 41;
    ntbl(3) = 81;
    a = -5;
    b = 5;
    
    //
    // Output
    //
    for(task = 0; task <= 3; task++)
    {
        if( task==0||task==1 )
        {
            printf("TESTING f=sin(x)\n");
        }
        else
        {
            printf("\n\nTESTING f=1/(1+x^2)\n");
        }
        if( task==0||task==2 )
        {
            printf("EQUIDISTANT NODES\n");
        }
        else
        {
            printf("CHEBYSHEV NODES\n");
        }
        printf("N     D=3       D=5       D=8       D=N       Optimal D\n");
        for(j = 0; j <= ncnt-1; j++)
        {
            n = ntbl(j);
            x.setbounds(0, n-1);
            y.setbounds(0, n-1);
            w.setbounds(0, n-1);
            ematrix.setbounds(0, n-1);
            
            //
            // Fill X, Y
            //
            for(i = 0; i <= n-1; i++)
            {
                if( task==0||task==2 )
                {
                    x(i) = a+(b-a)*i/(n-1);
                }
                else
                {
                    x(i) = 0.5*(a+b)+0.5*(b-a)*cos(ap::pi()*(2*i+1)/(2*(n-1)+2));
                }
                y(i) = ftask(x(i), task);
            }
            
            //
            // Test
            //
            emin = ap::maxrealnumber;
            dmin = 0;
            for(d = 0; d <= n-1; d++)
            {
                buildfloaterhormannrationalinterpolant(x, n, d, w);
                err = 0;
                t = a;
                while(t<=b*1.000001)
                {
                    err = ap::maxreal(err, fabs(barycentricinterpolation(x, y, w, n, t)-ftask(t, task)));
                    t = t+0.0005*(b-a);
                }
                if( err<emin )
                {
                    emin = err;
                    dmin = d;
                }
                ematrix(d) = err;
            }
            
            //
            // Output
            //
            printf("%3ld   %8.1le  %8.1le  %8.1le  %8.1le  %8.1le (D=%0ld)\n",
                long(n-1),
                double(ematrix(3)),
                double(ematrix(5)),
                double(ematrix(8)),
                double(ematrix(n-1)),
                double(emin),
                long(dmin));
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testratinterpolation_test_silent()
{
    bool result;

    result = testri(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testratinterpolation_test()
{
    bool result;

    result = testri(false);
    return result;
}



