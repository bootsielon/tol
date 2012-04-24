/* unitroot.cpp: Generic statistical functions.
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

#include <tol/tol_bunitroot.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bar.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_matrix.h>

BTraceInit("unitroot.cpp");

BArray< BProbPol > BArrayProbPolDecl__;

#if 0
BPolyn<BDat> BPolyn<BDat>::unknown_;  // it's defined in polgra.cpp
#endif


//--------------------------------------------------------------------
BBool InverseNonStationaryRoots(BPolyn<BDat>& p, 
				BInt periodicity, 
				BText name)
//--------------------------------------------------------------------
{
    BPolyn<BDat> q = p;
    BBool changed = false;
    BBool ok = true;
    BInt deg = p.Degree();
    BInt size = p.Size();
    
    if((size==2)&&(p[0].Degree()==0))
    {
	BDat a = p[1].Coef();
	if(Abs(a)>1)
	{
	    p[1].PutCoef(1/a);
	    changed = true;
	}
    }
    else if((size==3)&&(p[0].Degree()==0)&&(p[2].Degree()==2*p[1].Degree()))
    {
	BInt deg = p[1].Degree();
	BDat b = p[1].Coef();
	BDat a = p[2].Coef();
	BDat d = b*b - a*4.0;
	if(d>=0)
	{
	    BDat rd = Sqrt(d);
	    BDat r1 = (2*a)/(-b+rd);
	    BDat r2 = (2*a)/(-b-rd);
	    if(Abs(r1)>1)
	    {
		r1 = 1/r1;
		changed = true;
	    }
	    if(Abs(r2)>1)
	    {
		r2 = 1/r2;
		changed = true;
	    }
	    if(changed)
	    {
		BPolyn<BDat> p1 = 
		    BPolyn<BDat>::One() - r1 * (BPolyn<BDat>::B()^deg);
		BPolyn<BDat> p2 = 
		    BPolyn<BDat>::One() - r2 * (BPolyn<BDat>::B()^deg);
		p = p1*p2;
	    }
	}
	else
	{
	    if(Abs(a)>1)
	    {
		p[1].PutCoef(b/a);
		p[2].PutCoef(1/a);
		changed = true;
	    }
	}
    }
    else
    {
	ok = IsStationary(p);
    }
    if(changed && name.HasName())
    {
	Warning(BText("\nNon stationary ")+name+
		"("+periodicity+") = "+q.Name()+
		" changed to "+p.Name());
    }
    return(ok);
}


//--------------------------------------------------------------------
static BDat StationarityRegionMeassure(BInt dimension)
//--------------------------------------------------------------------
{
    BDat V=1, M = 2;
    BInt p, n = dimension/2;
    for(p=1; p<dimension; p+=2)
    {
	V *= M;
	M *= BDat(p)/BDat(p+1);
    }
    V*=V;
    if(dimension%2)
    {
	V *= M;
    }
    return(V);
}


//--------------------------------------------------------------------
BDat ProbUnitRoot(const BArray <BDat>& z,
		  const BPolyn <BDat>& ari,
		        BProbPol&      U)
//--------------------------------------------------------------------
{
    BInt q   = ari.Degree();
    BInt d   = U.U_.Degree();
    BInt N   = z.Size()-q;
    BInt Nqd = z.Size()-(q-d);
    
    if((q<d)||(N<=d)) { return(0); } // Falta dar error
    
    BInt i,t,n;
    BDat prob=0, detChoCOV;
    BMatrix<BDat> W(N,q-d), NU(q-d,1), rNU(q-d,1), r(q-d,1);
    BSymMatrix<BDat> invCOV(q-d), COV(q-d);
    BLowTrMatrix<BDat> invChoCOV(q-d), choCOV(q-d);
    BPolyn<BDat> rAr(1+q-d);
    BDat T;
    BTStudentDist tStudent(Nqd);
    
    ApplyPolyn(U.U_,z,U.difZ_);
    U.ar_ = IdentifyAutoReg(U.difZ_,U.a_,U.sigma_,U.swartz_,q-d,q-d);
    T = U.sigma_*U.sigma_*(Nqd);
    for(i=0; i<q-d; i++) {
	NU(i,0)=U.ar_.Coef(i+1); 
    }
    for(t=0; t<N; t++) { for(i=0; i<q-d; i++) {
	W(t,i) = U.difZ_[t+(q-d)-i]; } 
    }
    invCOV = MtMSqr(W);
    Choleski(invCOV, invChoCOV);
    Inverse(invChoCOV, choCOV);
    COV = choCOV.T()=choCOV;
    
    
    for(i=0; i<q-d+1; i++) { rAr[i].PutDegree(i); }
    for(n=0; n<100; n++)
    {
	for(i=0; i<q-d; i++) { r(i,0) = tStudent.Random(); }
	rNU = choCOV*r;
	rAr[0].PutCoef(1);
	for(i=0; i<q-d; i++) { rAr[i+1].PutCoef(-(rNU(i,0)+NU(i,0))); }
	if(IsStationary(rAr)) { prob+=1; }
    }
    detChoCOV = 1;
    for(i=0; i<q-d; i++) { detChoCOV *= choCOV(i,i); }
    
    BDat V = StationarityRegionMeassure(q-d);
    prob /= n;
    prob /= V;
    prob /= Sqrt(T^z.Size());
    prob *= Gamma(BDat(z.Size())/2);
    prob /= Gamma(BDat(q-d)/2);
    prob /= detChoCOV;
    prob /= Sqrt((BDat::Pi()*Nqd)^(q-d));
    
    return(prob);
}




//--------------------------------------------------------------------
BInt BProbPolOrderCriterium(const BAny txt1, const BAny txt2)

/*! Comparision function to sort by name in arrays of BProbPol
 */
//--------------------------------------------------------------------
{
    BProbPol& sym1 = *((BProbPol*)txt1);
    BProbPol& sym2 = *((BProbPol*)txt2);
    BInt cmp = (BInt)Sign(sym2.prob_ - sym1.prob_).Value();
    return(cmp);
}


//--------------------------------------------------------------------
void IdentifyUnitRoot(const BArray <BDat>&	  z,
		      const BPolyn<BDat>&	  ari,
		            BInt		  period,
		            BArray< BProbPol >&   UnitRootTable)
//--------------------------------------------------------------------
{
    BInt i;
    BDat p=0;
    
    UnitRootTable.ReallocBuffer(3);
    UnitRootTable[0].U_ = BPolyn<BDat>::One();
    UnitRootTable[1].U_ = UnitRootTable[0].U_-BPolyn<BDat>::B();
    UnitRootTable[2].U_ = UnitRootTable[1].U_*UnitRootTable[1].U_;
    if(period>1)
    {
	UnitRootTable.ReallocBuffer(9);
	UnitRootTable[3].U_ = UnitRootTable[0].U_-(BPolyn<BDat>::B()^period);
	UnitRootTable[4].U_ = UnitRootTable[3].U_*UnitRootTable[3].U_;
	UnitRootTable[5].U_ = UnitRootTable[1].U_*UnitRootTable[3].U_;
	UnitRootTable[6].U_ = UnitRootTable[2].U_*UnitRootTable[3].U_;
	UnitRootTable[7].U_ = UnitRootTable[1].U_*UnitRootTable[4].U_;
	UnitRootTable[8].U_ = UnitRootTable[2].U_*UnitRootTable[4].U_;
    }
    
    Std(BText("\nCalculating Unit Root Probability"));
    for(i=0; i<UnitRootTable.Size(); i++)
    {
	UnitRootTable[i].prob_ = ProbUnitRoot(z, ari, UnitRootTable[i]);
	Std(BText("\nUnitRoot t")+i+
	    "\tProb= "+UnitRootTable[i].prob_.Name()+
	    "\tDif= "+UnitRootTable[i].U_.Name());
    }
    
    Std(BText("\nSorting Unit Root Polynomials"));
    UnitRootTable.Sort(BProbPolOrderCriterium);
    for(i=0; i<UnitRootTable.Size(); i++)
    {
	Std(BText("\nUnitRoot t")+i+
	    "\tProb= "+UnitRootTable[i].prob_.Name()+
	    "\tDif= "+UnitRootTable[i].U_.Name());
    }   
}
