/* tol_symmat.hpp: Implementation for BSymMatrix class.
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


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any>::BSymMatrix()

/*! Default constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Alloc(0);
}


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any>::BSymMatrix(const BSymMatrix<Any>& m)
    
/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Copy(&m);
}


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any>::~BSymMatrix()
    
/*! Destructor.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
// Access and manipulation functions for BSymMatrix class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
void 
BSymMatrix<Any>::Alloc(BInt r)

/*! Allocates memory for a r rows symmetric matrix, storing only the
 *  lower triangular half.
 */
//--------------------------------------------------------------------
{
    int i,j;
    this->data_.AllocBuffer((r*(r+1))/2);
    
    this->firstOfRow_.AllocBuffer(r);
    this->buffer_      = this->data_.GetBuffer();
    this->firstBuffer_ = this->firstOfRow_.GetBuffer();
    for(i=j=0; i<r; ) {
	this->firstBuffer_[i] = j; i++; j+=i;
    }
    this->columns_ = this->rows_ = r;
}



//--------------------------------------------------------------------
template <class Any>
BMatA& 
BSyMA::Copy(const BMatrix<Any>* m)

/*! Sets in this diagonal matrix the principal diagonal of a general
 *  matrix m.
 */
//--------------------------------------------------------------------
{
    int r = Minimum(m->Rows(),m->Columns());
    Alloc(r);
    for(BInt i=0; i<r; i++)
    {
	for(BInt j=0; j<=i; j++)
	{
	    (*this)(i,j) = (*m)(i,j);
	}
    }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BSyMA& 
BSyMA::operator=(const BMatA& m) 
{ return (BSyMA&)(Copy(&m)); }

template <class Any>
BSyMA& 
BSyMA::operator=(const BSyMA& m) 
{ return (BSyMA&)(Copy(&m)); }

template <class Any>
BSyMA& 
BSyMA::operator=(const BLTMA& m) 
{ return (BSyMA&)(Copy(&m)); }

template <class Any>
BSyMA& 
BSyMA::operator=(const BUTMA& m) 
{ return (BSyMA&)(Copy(&m)); }

template <class Any>
BSyMA& 
BSyMA::operator=(const BDiMA& m) 
{ return (BSyMA&)(Copy(&m)); }
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class Any>
Any 
BSymMatrix<Any>::Get(BInt i, BInt j) const

/*! Returns the element (i, j) of the matrix.
*/
//--------------------------------------------------------------------
{
    BMAT_ENSURE_DIM(i,j);
    if((i<0) || (j<0) || (i>=this->rows_) || (j>=this->rows_))
    { return(BArray<Any>::Overflow()); }
    else
    {
	return((i>=j)?this->buffer_[this->firstBuffer_[i]+j]:this->buffer_[this->firstBuffer_[j]+i]);
    }
}

//--------------------------------------------------------------------
template <class Any>
Any& 
BSymMatrix<Any>::operator()(BInt i, BInt j) const
    
/*! Returns the element (i, j) of the matrix.
 */
//--------------------------------------------------------------------
{
   BMAT_ENSURE_DIM(i,j);
   return((i>=j)
          ?this->buffer_[this->firstBuffer_[i]+j]
          :this->buffer_[this->firstBuffer_[j]+i]);
}

//--------------------------------------------------------------------
// Operations for BSymMatrix class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any>& 
BSymMatrix<Any>::operator += (const BSymMatrix<Any>& m)

/*! Operator += overloading.
 */
//--------------------------------------------------------------------
{
    if(this->rows_==m.Rows())
    {
	for(BInt i=0; i<this->data_.Size(); i++)
	{
	    this->data_(i) += m.Data()(i);
	}
    }
    else { Alloc(0); }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any> 
BSymMatrix<Any>::operator + (const BSymMatrix<Any>& m) const

/*! Operator overloading to add two matrices.
 */
//--------------------------------------------------------------------
{
    BSymMatrix<Any> s = *this;
    s += m;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any>& 
BSymMatrix<Any>::operator -= (const BSymMatrix<Any>& m)

/*! Operator overloading to difference two matrices.
 */
//--------------------------------------------------------------------
{
    if(this->rows_==m.Rows())
    {
	for(BInt i=0; i<this->data_.Size(); i++)
	{
	    this->data_(i) -= m.Data()(i);
	}
    }
    else { Alloc(0); }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any> 
BSymMatrix<Any>::operator - (const BSymMatrix<Any>& m) const
    
/*! Operator overloading to difference two matrices.
 */
//--------------------------------------------------------------------
{
    BSymMatrix<Any> s = *this;
    s -= m;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any>& 
BSymMatrix<Any>::operator *= (const Any& n)

/*! Multiply member this by n.
 */
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->data_.Size(); i++) { this->data_(i) *= n; }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any> 
BSymMatrix<Any>::operator * (const Any& n) const

/*! Operator overloading to product one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BSymMatrix<Any>  p = *this;
    p *= n;
    return(p);
}


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any>& 
BSymMatrix<Any>::operator /= (const Any& n)

/*! Operator overloading to make the quotient between m and n.
 */
//--------------------------------------------------------------------
{
    (*this)*=(1/n);
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any> 
BSymMatrix<Any>::operator / (const Any& n) const

/*! Operator overloading to make the quotient between "m1" and "n".
 */
//--------------------------------------------------------------------
{
    BSymMatrix<Any> m=*this;
    m/=n;
    return(m);
}



//--------------------------------------------------------------------
template <class Any>
BSymMatrix<Any> 
BSymMatrix<Any>::T() const // Transpose

/*! Operation overloading to traspose one matrix .
 */
//--------------------------------------------------------------------
{
    BSymMatrix<Any> mat=*this;
    return(mat);
}


//--------------------------------------------------------------------
template <class Any>
void 
MMTSquare(const BMatrix<Any>& m,
	        BSymMatrix<Any>& sq)
//--------------------------------------------------------------------
{
    Any aux;
    BInt i,j,k;
    sq.Alloc(m.Rows());
    for(i=0; i<m.Rows(); i++)
    {
	for(j=0; j<=i; j++) { sq(i,j) = 0; }
	for(k=0; k<m.Columns(); k++)
	{
	    aux = m(i,k);
	    for(j=0; j<=i; j++) { sq(i,j) += aux * m(j,k); }
	}
    }
}

//--------------------------------------------------------------------
template <class Any>
void 
MTMSquare(const BMatrix<Any>& m,
	        BSymMatrix<Any>& sq)
//--------------------------------------------------------------------
{
    Any aux;
    BInt i,j,k;
    sq.Alloc(m.Columns());
    for(i=0; i<m.Columns(); i++)
    {
	for(j=0; j<=i; j++) { sq(i,j) = 0;	}
	for(k=0; k<m.Rows(); k++)
	{
	    aux = m.Get(k,i);
	    for(j=0; j<=i; j++) { sq(i,j) += aux * m.Get(k,j);  }
	}
    }
}


/*
//--------------------------------------------------------------------
template <class Any>
BBool Choleski(const BSymMatrix<Any>& s,
BLowTrMatrix<Any>& t)

// PURPOSE: Constructs in t the Choleski's low triangular matrix
//	    corresponding to s.
//
//--------------------------------------------------------------------
{
  BInt r = s.Rows();
  t.Alloc(r);
  Any aux;
  BInt i, j, k;

  for(j=0; j<r; j++)
  {
    aux = s.SafeGet(j, j);
    for(k=0; k<j; k++)
    {
      aux -= t.SafeGet(j, k) * t.SafeGet(j, k);
    }
    if(aux<=0)
    {
      return(BFALSE);
    }
    t.SafePut(j, j, Sqrt(aux));

    for(i=j+1; i<r; i++)
    {
      aux = s.SafeGet(i, j);
      for(k=0; k<j; k++) { aux -= t.SafeGet(i, k) * t.SafeGet(j, k); }
      t.SafePut(i, j, (aux / t.SafeGet(j, j)));
    }
  }
  return(BTRUE);
}
*/

/*
//--------------------------------------------------------------------
   template <class Any>
   BBool Choleski(const BSymMatrix<Any>& s,
		  BLowTrMatrix<Any>& t)

// PURPOSE: Constructs in t the Choleski's low triangular matrix
//	    corresponding to s.
//
//--------------------------------------------------------------------
{
  BInt c = s.Rows();
  t.Alloc(r);
  Any aux;
  BInt i, j, k;

  for(k=0; k<c; k++)
  {
    aux = s.SafeGet(k, k);
    for(r=0; r<k; r++) { aux -= t.SafeGet(k, r) * t.SafeGet(k, r); }
    if(aux<=0)
    {
      return(BFALSE);
    }
    t.SafePut(k, k, Sqrt(aux));

    for(i=k+1; i<c; i++)
    {
      aux = s.SafeGet(i, k);
      for(r=0; r<k; r++) { aux -= t.SafeGet(i, r) * t.SafeGet(k, r); }
      t.SafePut(i, k, (aux / t.SafeGet(k, k)));
    }
  }
  return(BTRUE);
}
*/

//--------------------------------------------------------------------
template <class Any>
BBool 
Choleski(const BSymMatrix<Any>& a,
	       BLowTrMatrix<Any>& l)

/*! Constructs in t the Choleski's low triangular matrix
 *  corresponding to s.
 */
//--------------------------------------------------------------------
{
    BInt n = a.Rows();
    l.Alloc(n);
    BInt i, j, k;
    if(!n || (a(0,0)<=0.0)) 
    { 
      l.Alloc(0);
      return(BFALSE); 
    }
    l(0,0) = Sqrt(a(0,0));
    for(j=1; j<n; j++) { l(j,0) = a(j,0) / l(0,0); }
    for(i=1; i<n; i++)
    {
	l(i,i) = a(i,i);
	for(k=0; k<i; k++) { l(i,i) -= l(i,k)*l(i,k); }
	if(l(i,i)<=0.0) 
  {
    l.Alloc(0);
    return(BFALSE); 
  }
	l(i,i) = Sqrt(l(i,i));
	for(j=i+1; j<n; j++)
	{
	    l(j,i) = a(j,i);
	    for(k=0; k<i; k++) { l(j,i) -= l(j,k)*l(i,k); }
	    l(j,i) /= l(i,i);
	}
    }
    return(BTRUE);
}


//--------------------------------------------------------------------
template <class Any>
BBool 
Inverse(const BSymMatrix<Any>& s, BSymMatrix<Any>& inv)

/*! Constructs in "inv" the inverse matrix corresponding to the
 *  symmetric matrix "s".
 */
//--------------------------------------------------------------------
{
  Any aux;
  BLowTrMatrix<Any> chol, invchol;
  if(Choleski(s, chol))
  {
    Inverse(chol, invchol);
    invchol.MTMSquare(inv);
    return(BTRUE);
  }
  else
  {
    inv.Alloc(0);
    return(BFALSE);
  }
}


//--------------------------------------------------------------------
template <class Any>
void 
CorrSymMatGenerator(const BMatrix<Any>& dat, BSymMatrix<Any>& cor)

/*! Constructs in cor the correlation matrix corresponding to the
 *  data stored in dat. Each variable corresponds with one row.
 */
//--------------------------------------------------------------------
{
    BInt i, j, k;
    BInt r = dat.Rows();
    BInt c = dat.Columns();
    Any aux;
    BArray<Any> mean, dev;
    mean.AllocBuffer(r);
    dev.AllocBuffer(r);
    cor.Alloc(r);
    
    //Means generation:
    for(i=0; i<r; i++)
    {
	aux = 0;
	for(j=0; j<c; j++)
	{
	    aux += dat.Get(i, j);
	}
	mean(i) = aux/c;
	//Std(Out()<<"mean "<<i<<"="<<mean(i)<<"\n");
    }
    //(Typical deviations * sqrt(c)) generation:
    for(i=0; i<r; i++)
    {
	aux = 0;
	for(j=0; j<c; j++)
	{
	    aux += dat.Get(i, j) * dat.Get(i, j);
	}
	aux -= (c * mean(i) * mean(i));
	dev(i) = sqrt(aux);
	//Std(Out()<<"dev "<<i<<"="<<dev(i)<<"\n");
    }
    //Correlations generation:
    for(i=0; i<r; i++)
    {
	for(j=0; j<=i; j++)
	{
	    aux = 0;
	    for(k=0; k<c; k++)
	    {
		aux += dat.Get(i, k) * dat.Get(j, k);
	    }
	    aux -= c * mean(i) * mean(j);
	    aux = (aux/(dev(i) * dev(j))) * (aux/(dev(i) * dev(j)));
	    cor.Put(i, j, aux);
	}
    }
}


//--------------------------------------------------------------------//
template <class Any>
BBool 
SymSystemSolver(      BMatrix<Any>&	 solution,
		      BLowTrMatrix<Any>& chol,
  	        const BSymMatrix<Any>&	 m,
	        const BMatrix<Any>&	 d)
    
/*! Returns the solutions of the systems formed by "m" and the column
 *  vectors of "d", applying previously to m Choleski's factorization.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> solution1, aux1, aux2;
    BBool ok = Choleski(m, chol);
//Std(BText("\nSymSystemSolver; ") + (ok?"OK":"FAIL"));
//Std(Out()<<"chol\n"<<chol.MatToText().String()<<"\n");
    if(ok)
    {
	solution1 = LTrSystemSolver(chol, d);
//Std(Out()<<"partial solution\n"<<solution1.MatToText().String()<<"\n");
	aux1 = chol;
	MatMult(aux1,solution1,aux2);
	//Std(Out()<<"partial verification\n"
	//	     <<aux2.MatToText().String()<<"\n");
	solution = LTrTSystemSolver(chol, solution1);
    }
    return(ok);
}

/*
//--------------------------------------------------------------------
   template <class Any>
   BBool LeastSqrCholeski(BMatrix<Any>& c,  const BMatrix<Any>& D,
			  BMatrix<Any>&	 X, BMatrix<Any>& R,
			  BLowTrMatrix<Any>& chol)

// PURPOSE: Minimizes r*r' by Givens transformations method in the error
//	    equation
//		       C x + d = r
//
//	    Trnsforms C and d and constructs x and r
//	    (Swartz, 7.2.1, 302)
//
//--------------------------------------------------------------------
{
  BInt n = c.Columns();
  BInt N = c.Rows();

  BSymMatrix <Any>	CtC;
  MTMSquare(c,CtC);
  D*=-1;
  BMatrix<Any> B;
  MatMult(c.Traspose(),D,B);
  BMatrix<Any> X;
  BBool ok=SymSystemSolver(X,chol,CtC,B);
  if(ok)
  {
    BMatrix<Any> R=c*X+D;
    x = X.Traspose()(0);
    r = R.Traspose()(0);
  };
  return(ok);
}

*/
