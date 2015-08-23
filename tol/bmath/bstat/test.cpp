/* test.cpp: Assesment of the assumtion of normality.
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

#include <tol/tol_nortest.h>
#include <tol/tol_btest.h>

BTraceInit("test.cpp");

//--------------------------------------------------------------------
// American National Standard ANSI N15-1974
// Assesment of the assumtion of normality
//--------------------------------------------------------------------


//--------------------------------------------------------------------
static BDat HannShapiroA(BInt n, BInt j)

/*!  Returns the Hann-Shapiro A series
 */
//--------------------------------------------------------------------
{
    BInt m = n-3;
    BInt i = j-1;
    if((m>=0) && (m<WACOLS)&&
       (i>=0) && (i<WAROWS))
    {
	return(HannShapiroWA_[i][m]);
    }
    else
    {
	return(BDat::Unknown());
    }
}


//--------------------------------------------------------------------
BDat HannShapiroW(const BArray<BDat>& vec, BDat& b2 )

/*! Returns the Hann-Shapiro W statistic of a vector
 */
//--------------------------------------------------------------------
{
    BInt n = vec.Size();
    if(n>WALIMIT) { return(BDat::Unknown()); }
    BDat S2 = Varianze(vec)*n;
    BArray<BDat> x = vec;
    x.Sort(DatCmp);
    BInt k = n/2;
    BDat b = 0;
    for(BInt i=1; i<=k; i++)
    {
	b += HannShapiroA(n,i)*(x[n-i]-x[i-1]);
    }
    b2 = b*b;
    BDat W = b*b/S2;
//Std(BText("\nHannShapiroW b=")+b.Name()+"\tS2="+S2.Name()+"\tW="+W.Name());
    return(W);
}


//--------------------------------------------------------------------
BDat HannShapiroWDist(const BDat& prob, BInt n)

/*! Returns the inverse of distribution function of the Hann-Shapiro
 *  W statistic
 */
//--------------------------------------------------------------------
{
    BDat gamma   = HannShapiroWP_[n][0];
    BDat mu      = HannShapiroWP_[n][1];
    BDat epsilon = HannShapiroWP_[n][2];
    return(gamma + mu * Log((prob-epsilon)/(1-prob)));
}


//--------------------------------------------------------------------
BDat DAgostinoD(const BArray<BDat>& vec, BDat& T)

/*! Returns the DAgostino D statistic of a vector
 */
//--------------------------------------------------------------------
{
    BInt n = vec.Size();
    if(n<=WALIMIT) { return(BDat::Unknown()); }
    BDat S = Sqrt(Varianze(vec)*n);
    BArray<BDat> x = vec;
    x.Sort(DatCmp);
    T = 0;
    for(BInt i=1; i<=n; i++)
    {
	T += (2*i-n-1)*x[i-1]/2;
    }
    BDat D = T/S;
//Std(BText("\nDAgostinoD T=")+T.Name()+"\tS="+S.Name()+"\tD="+D.Name());
    return(D);
}


//--------------------------------------------------------------------
BBool HannShapiroWTest(BDat W, BInt n, BDat alfa)

/*! Returns the Hann-Shapiro W test of normality
 */
//--------------------------------------------------------------------
{
    BInt r = WACOLS;
    BInt c = WALFACOLS;
    BInt i, j;
    for(i=0; (i<r)&&(n>HannShapiroW_[i][0]); i++) { }
    
    if(i==r) { i=r-1; }
    
    for(j=1; (j<=c)&&(alfa>HannShapiroWAlfa_[j-1]); j++)	
    {
//  Std(BText("\n  alfa = ") + alfa);
//  Std(BText("\n  ALFA = ") + HannShapiroWAlfa_[j-1]);
    }

    if(j>c) { j=c; }

    BDat TW = HannShapiroW_[i][j];
//Std(BText("\n	 alfa = ") + alfa);
//Std(BText("\n	 ALFA = ") + HannShapiroWAlfa_[j-1]);
//Std(BText("\n	 TW = ") + TW);
    return(W>=TW);
}


//--------------------------------------------------------------------
BBool DAgostinoDTest(BDat D, BInt n, BDat alfa)

/*! Returns the DAgostino D test of normality
 */
//--------------------------------------------------------------------
{
    BInt r = DALFAROWS;
    BInt c = DALFACOLS;
    BInt i, j1, j2;
    BDat a1 = alfa/2.0;
    // unused
    // BDat a2 = 1.0-a1;
//TRZ(alfa);TRZ(a1);TRZ(a2);
    for(i=0; (i<r)&&(n>DAgostinoD_[i][0]); i++)
    {
//  Std(BText("n(")+i+")="+DAgostinoD_[i][0]);
    }
    if(i==r) { i=r-1; }

    for(j1=1; (j1<c/2)&&(a1>DAgostinoDAlfa_[j1-1]); j1++) { }

    j2 = c-j1+1;
    BDat TD1, TD2;
    
    if(i==r-1) { i = r-2; }

    BDat na   = DAgostinoD_[i][0];
    BDat nb   = DAgostinoD_[i+1][0];
    BDat TD1a = DAgostinoD_[i][j1];
    BDat TD2a = DAgostinoD_[i][j2];
    BDat TD1b = DAgostinoD_[i+1][j1];
    BDat TD2b = DAgostinoD_[i+1][j2];
    TD1 = TD1a + ((TD1b-TD1a)*(n-na))/(nb-na);
    TD2 = TD2a + ((TD2b-TD2a)*(n-na))/(nb-na);
    
//TRZ(na);TRZ(nb);TRZ(TD1a);TRZ(TD1b);TRZ(TD2a);TRZ(TD2b);TRZ(TD1);TRZ(TD2);

    return((D>=TD1)&&(D<=TD2));
}


//--------------------------------------------------------------------
BBool NormalityTest(const BArray<BDat>& vec, BDat alfa)

/*! Returns the Hann-Shapiro test of normality
 */
//--------------------------------------------------------------------
{
    BInt n = vec.Size();
    BDat aux;
    if(n<50) { return(HannShapiroWTest(HannShapiroW(vec,aux), n, alfa)); }
    else     { return(DAgostinoDTest  (DAgostinoD  (vec,aux), n, alfa)); }
}
