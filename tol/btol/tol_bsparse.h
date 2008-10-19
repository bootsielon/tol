/* tol_bsparse.h: Selected Linear Algebra Functions for sparse matrix.
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

#ifndef TOL_BSPARSE_H
#define TOL_BSPARSE_H 1

#include <tol/tol_bmfstpro.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
typedef BArray<BIntPair> BMatrixIndex;


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
template<class Any>
void ShermanMorrison(BMatrix<Any>& B,
		     const BMatrix<Any>& u,
		     BInt j);

template<class Any>
void ShermanMorrison(BMatrix<Any>& B,
		     BInt i,
		     const BMatrix<Any>& vt);

template<class Any>
void ShermanMorrison(BMatrix<Any>& B,
		     const BMatrix<Any>& u,
		     const BMatrix<Any>& vt);

template<class Any>
void ShermanMorrison(BMatrix<Any>& B,
		     BInt i,
		     BInt j,
		     const Any& x);

template<class Any>
BMatrix<Any> ShermanMorrisonInverse (const BMatrix<Any>& A);

template<class Any>
BMatrix<Any> ShermanMorrisonInverse (const BMatrix<Any>& A,
				     const BMatrixIndex& pair);

template<class Any>
BMatrix<Any> ShermanMorrisonPseudoInverse (const BMatrix<Any>& A);


//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class Any>
void  ShermanMorrison(BMatrix<Any>& B,
		      const BMatrix<Any>& u,
		      const BMatrix<Any>& v)
//--------------------------------------------------------------------
{
    BInt r,c;
    BMatrix<Any> z = B*u;
    BMatrix<Any> w = v*B;
    Any lambda = (v*z)(0,0);
    BMatrix<Any> C = z*w;
    C /= (1+lambda);
    B -= C;
}


//--------------------------------------------------------------------
template<class Any>
void  ShermanMorrison(BMatrix<Any>& B,
		      const BMatrix<Any>& u,
		      BInt j)
//--------------------------------------------------------------------
{
    BInt n=B.Columns();
//BTimer tm(BText("\nShermanMorrison(")+j+")");
//TRZ(u);
//TRZ(B);
    BMatrix<Any> z = B*u;
//TRZ(z);
    Any lambda = z(j,0);
    BMatrix<Any> C = z*B.Sub(j,0,1,n);
//TRZ(C);
    C /= (1+lambda);
//TRZ(C);
    B -= C;
//TRZ(B);
}

//--------------------------------------------------------------------
template<class Any>
void ShermanMorrison(BMatrix<Any>& B,
		     BInt  i,
		     const BMatrix<Any>& v)
//--------------------------------------------------------------------
{
    BInt r,c,m=B.Rows(),n=B.Columns();
    BMatrix<Any> w = v*B;
    Any lambda = w(0,i);
    BMatrix<Any> C = B.Sub(0,i,m,1)*w;
    C /= (1+lambda);
    B -= C;
}


//--------------------------------------------------------------------
template<class Any>
void ShermanMorrison(BMatrix<Any>& B, 
		     BInt i, 
		     BInt j,
		     const Any& x)
//--------------------------------------------------------------------
{
    BInt k,s,t,r,c,m=B.Rows(),n=B.Columns();
//BTimer tm(BText("\nShermanMorrison(")+i+","+j+","+x+")");
    if((x==0)||(i<0)||(j<0)||(i>=n)||(j>=m)) { return; }
    Any lambda = x*B(j,i);
    Any aux = x/(1+lambda);
    BArray<BInt> I(m), J(n);
//BMatrix<Any> Bi = B.Sub(0,i,m,1);
//BMatrix<Any> Bj = B.Sub(j,0,1,n);
//TRZ(Bi);TRZ(Bj);
    for(s=r=0; r<m; r++) { if(B(r,i)!=0) { I(s++)=r; } }
    I.ReallocBuffer(s);
    for(t=c=0; c<n; c++) { if(B(j,c)!=0) { J(t++)=c; } }
    J.ReallocBuffer(t);
    BMatrix<Any> C(s,t);
    BMatrixIndex pair(I.Size()*J.Size());
    for(k=s=0; s<I.Size(); s++)
    {
	for(t=0; t<J.Size(); t++)
	{
	    C(s,t) = B(I(s),i)*B(j,J(t))*aux;
	    if(C(s,t)!=0)
	    {
		pair(k).r_=s;
		pair(k).c_=t;
		k++;
	    }
	}
    }
//TRZ(C);
    pair.ReallocBuffer(k);
    for(k=0; k<pair.Size(); k++)
    {
	r = pair(k).r_;
	c = pair(k).c_;
	B(I(r),J(c)) -= C(r,c);
    }
//Std(BText("\nsparse(")+(Any(I.Size()*J.Size())/(n*n))+")");
//TRZ(B);
}

//--------------------------------------------------------------------
template<class Any>
BMatrix<Any> ShermanMorrisonInverse (const BMatrix<Any>& A)
//--------------------------------------------------------------------
{
    BInt i,j,m=A.Rows(),n=A.Columns();
    BMatrix<Any> Ai(n,m);
    Ai.SetAllValuesTo(0);
    BInt mn = Minimum(m,n);
    for(i=0; i<mn; i++)
    {
	if(A(i,i)!=0)
	{
	    Ai(i,i) = 1.0/A(i,i);
	}
    }
//TRZ(Ai);
    for(i=0; i<m; i++)
    {
	for(j=0; j<n; j++)
	{
//    Std(BText("\nShermanMorrison(")+i+","+j+","+A(i,j)+")");
	    if((i!=j)&&A(i,j))
	    {
		ShermanMorrison(Ai,i,j,A(i,j));
	    }
	}
    }
//BMatrix<Any> AiA = Ai*A;   TRZ(Ai);  TRZ(AiA);
    return(Ai);
}


//--------------------------------------------------------------------
template<class Any>
BMatrix<Any> ShermanMorrisonInverse (const BMatrix<Any>& A,
				     const BMatrixIndex& pair)
//--------------------------------------------------------------------
{
    BInt i,j,k,m=A.Rows(),n=A.Columns();
    BMatrix<Any> Ai(n,m);
    Ai.SetAllValuesTo(0);
    for(i=0; (i<m)&&(i<n); i++)
    {
	if(A(i,i)!=0)
	{
	    Ai(i,i)=1.0/A(i,i);
	}
    }
    for(k=0; k<pair.Size(); k++)
    {
	i = pair(k).r_;
	j = pair(k).c_;
	if((i!=j)&&A(i,j))
	{
	    ShermanMorrison(Ai,i,j,A(i,j));
	}
    }
//BMatrix<Any> AiA = Ai*A;
//TRZ(Ai);
//TRZ(AiA);
    return(Ai);
}


//--------------------------------------------------------------------
template<class Any>
BMatrix<Any> ShermanMorrisonPseudoInverse (const BMatrix<Any>& A)
//--------------------------------------------------------------------
{
    BInt i,j,m=A.Rows(),n=A.Columns();
    BMatrix<Any> Ai(n,m),u(n,1);
    Ai.SetAllValuesTo(0);
    for(i=0; (i<m)&&(i<n); i++)
    {
	Ai(i,i)=1/A(i,i);
    }
//TRZ(A);
    for(j=0; j<n; j++)
    {
	u=A.Sub(0,j,m,1);
	u(j,j)=0;
	ShermanMorrison(Ai,u,j);
    }
//BMatrix<Any> AiA = Ai*A;
//TRZ(Ai);
//TRZ(AiA);
    return(Ai);
}

#endif	// TOL_BSPARSE_H
