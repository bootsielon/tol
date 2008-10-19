/* tol_ltrimat.hpp: Implementation for BLowTrMatrix class.
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
BLowTrMatrix<Any>::BLowTrMatrix()

/*! Default constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Alloc(0);
}



//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any>::BLowTrMatrix(const BLowTrMatrix<Any>& m)

/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Copy(&m);
}

//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any>::~BLowTrMatrix()

/*! Destructor.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
template <class Any>
BMatA& 
BLTMA::Copy(const BMatrix<Any>* m)

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
BLTMA& 
BLTMA::operator=(const BMatA& m)  
{ return (BLTMA&)(Copy(&m)); }

template <class Any>
BLTMA& 
BLTMA::operator=(const BSyMA& m)  
{ return (BLTMA&)(Copy(&m)); }

template <class Any>
BLTMA& 
BLTMA::operator=(const BLTMA& m)  
{ return (BLTMA&)(Copy(&m)); }

template <class Any>
BLTMA& 
BLTMA::operator=(const BUTMA& m)  
{ return (BLTMA&)(Copy(&m)); }

template <class Any>
BLTMA& 
BLTMA::operator=(const BDiMA& m)  
{ return (BLTMA&)(Copy(&m)); }
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class Any>
void BLowTrMatrix<Any>::Alloc(BInt r)
    
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
    for(i=j=0; i<r; ) { this->firstBuffer_[i] = j; i++; j+=i; }
    this->columns_ = this->rows_ = r;
}


//--------------------------------------------------------------------
// Access and manipulation functions for BLowTrMatrix class.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class Any>
Any BLowTrMatrix<Any>::Get(BInt i, BInt j) const

/*! Returns the element (i, j) of the matrix.
 */
//--------------------------------------------------------------------
{
    BMAT_ENSURE_DIM(i,j);
    if((i<0) || (j<0) || (i>=this->rows_) || (j>=this->rows_))
    { return(BArray<Any>::Overflow()); }
    else
    { return((i>=j)?this->buffer_[this->firstBuffer_[i]+j]:this->NullValue()); }
}


//--------------------------------------------------------------------
template <class Any>
Any& BLowTrMatrix<Any>::operator()(BInt i, BInt j) const
    
/*! Returns the element (i, j) of the matrix.
 */
//--------------------------------------------------------------------
{
    BMAT_ENSURE_DIM(i,j);
    return((i>=j)?this->buffer_[this->firstBuffer_[i]+j]:this->NullValue());
}

//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any>& BLowTrMatrix<Any>::operator +=
(const BLowTrMatrix<Any>& m)
    
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
BLowTrMatrix<Any> 
BLowTrMatrix<Any>::operator + (const BLowTrMatrix<Any>& m1)

/*! Operator overloading to add two matrices.
 */
//--------------------------------------------------------------------
{
    BLowTrMatrix<Any> s = *this;
    s += m1;
    return(s);
}


//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any>& 
BLowTrMatrix<Any>::operator -= (const BLowTrMatrix<Any>& m)

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
BLowTrMatrix<Any> 
BLowTrMatrix<Any>::operator - (const BLowTrMatrix<Any>& m1)

/*! Operator overloading to difference two matrices.
 */
//--------------------------------------------------------------------
{
    BLowTrMatrix<Any> s = *this;
    s -= m1;
    return(s);
}


//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any>& 
BLowTrMatrix<Any>::operator *= (const Any& n)
    
/*! Operator overloading to product one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->data_.Size(); i++)
    {
	this->data_(i) *= n;
    }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any> 
BLowTrMatrix<Any>::operator * (const Any& n)

/*! Operator overloading to product one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BLowTrMatrix<Any>  p = *this;
    p *= n;
    return(p);
}


//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any>& 
BLowTrMatrix<Any>::operator /= (const Any& n)

/*! Operator overloading to make the quotient between m and n.
 */
//--------------------------------------------------------------------
{
    (*this)*=(1/n);
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any> 
BLowTrMatrix<Any>::operator / (const Any& n)

/*! Operator overloading to make the quotient between "m1" and "n".
 */
//--------------------------------------------------------------------
{
    BLowTrMatrix<Any> m=*this;
    m/=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any>
BLowTrMatrix<Any>::T () const // Transpose

/*! Trasposes the matrix m.
 */
//--------------------------------------------------------------------
{
    BUpTrMatrix<Any> t;
    
    BInt r = this->Rows();
    t.Alloc(r);
    for(BInt i=0; i<r; i++)
    {
	for(BInt j=i; j<r; j++)
	{
	    t(i, j) = (*this)(j, i);
	}
    }
    return(t);
}

//--------------------------------------------------------------------
template <class Any>
  void  BLowTrMatrix<Any>::MTMSquare(BSymMatrix<Any>& sq) const
//--------------------------------------------------------------------
{
  BInt i,j,k;
  sq.Alloc(this->Rows());
  for(i=0; i<this->Rows(); i++)
  {
    for(j=0; j<=i; j++) 
    { 
      Any &aux = sq(i,j);
      aux=0;
      for(k=0; k<=j; k++)
      {
        aux += Get(i,k) * Get(j,k);  
      }
    }
  }
}

//--------------------------------------------------------------------
template <class Any>
void 
PutDiag(BLowTrMatrix<Any>& m, BInt r, const Any& n)

/*! Puts number n in the matrix diagonal and zeros in the rest
 *  of the places.
 */
//--------------------------------------------------------------------
{
    m.Alloc(r);
    for(BInt i=0; i<r; i++)
    {
	m(i, i)=n;
	for(BInt j=0; j<i; j++) { m(i, j)=0; }
    }
}


//--------------------------------------------------------------------
template <class Any>
void 
InverseSchwarz(const BLowTrMatrix<Any>& m,
	             BLowTrMatrix<Any>& inv)

/*! Returns the inverse matrix corresponding to the band low
 *  triangular matrix m.
 */
//--------------------------------------------------------------------
{
    BInt k, i, j, begin, end, zero=0, r = m.Rows();
    Any pivot;
    
    inv = m;
    for(k=0; k<r; k++)
    {
	pivot = inv(k, k);
	// Transformaciron fila pivotal:
	for(j=0; j<k; j++)
	{
	    inv(k, j)=- inv(k, j)/pivot;
	}
	// Transformacion filas anteriores a la pivotal:
	for(i=0; i<k; i++)
	{
	    for(j=0; j<=i; j++)
	    {
		inv(i, j)=inv(i, j)+
		    inv(i, k)*inv(k, j);
	    }
	}
	// Transformacion filas posteriores a la pivotal:
	for(i=k+1; i<r; i++)
	{
	    for(j=0; j<k; j++)
	    {
		inv(i, j)=inv(i, j)+
		    inv(i, k)*inv(k, j);
	    }
	    for(j=k+1; j<=i; j++)
	    {
		inv(i, j)=inv(i, j)+
		    inv(i, k)*inv(k, j);
	    }
	}
	// Transformacion columna pivotal:
	for(i=k; i<r; i++)
	{
	    inv(i, k)=inv(i, k)/pivot;
	}
	// Transformacion pivote:
	inv(k, k)=1/pivot;
    }
}


//--------------------------------------------------------------------
template <class Any>
void Inverse(const BLowTrMatrix<Any>& m,
	           BLowTrMatrix<Any>& inv)

/*! Constructs in "inv" the inverse matrix corresponding to the low
 *  triangular matrix "m".
 */
//--------------------------------------------------------------------
{
    BInt r = m.Rows(), i, j ,k;
    inv.Alloc(r);
    for(i=0; i<r; i++)
    {
	inv(i,i)=1/m(i,i);
	for(j=0; j<i; j++)
	{
	    Any s = 0;
	    for(k=j; k<i; k++) { s-=m(i,k)*inv(k,j); }
	    inv(i, j)=s/m(i,i);
	}
    }
}


//--------------------------------------------------------------------//
template <class Any>
BMatrix<Any> 
LTrSystemSolver(const BLowTrMatrix<Any>& m,
		const BMatrix<Any>& d)

/*! Returns in "solution" the solutions of the systems formed by "m" and
 *  the column vectors of "d".
 */
//--------------------------------------------------------------------//
{
    BInt i, j, k ;
    BMatrix<Any> solution = d;
    BInt r = solution.Rows();
    BInt c = solution.Columns();
    Any det = Det(m);
    Any ithElem;
    if((r == m.Columns())&&(det))
    {
	for(i=0; i<r; i++)
	{
	    for(j=0; j<c; j++)
	    {
		solution(i, j) = solution(i, j)/m(i, i);
		ithElem = solution(i, j);
		for(k=i+1; k<r; k++)
		{
		    solution(k, j) = solution(k, j) - m(k, i)* ithElem;
		}
	    }
	}
    }
    return(solution);
}


//--------------------------------------------------------------------//
template <class Any>
BMatrix<Any> 
LTrTSystemSolver(const BLowTrMatrix<Any>& m,
		 const BMatrix<Any>& d)

/*! Returns in "solution" the solutions of the systems formed by "m"
 *  trasposed and the column vectors of "d".
 */
//--------------------------------------------------------------------//
{
    BInt i, j, k, l;
    BMatrix<Any> solution = d;
    BInt r = solution.Rows();
    BInt c = solution.Columns();
    Any det = Det(m);
    Any ithElem;
    if((r == m.Columns())&&(det))
    {
	for(i=r-1; i>=0; i--)
	{
	    for(j=0; j<c; j++)
	    {
		solution(i, j) = solution(i, j)/m(i, i);
		ithElem = solution.Get(i, j);
		for(k=i-1; k>=0; k--)
		{
		    solution(k, j) = solution(k, j) - m(i, k)* ithElem;
		}
	    }
	}
    }
    return(solution);
}


//--------------------------------------------------------------------
template <class Any>
Any 
Det(const BLowTrMatrix<Any>& m)

/*! Returns the value of the m determinant.
 */
//--------------------------------------------------------------------
{
    Any det=1;
    
    for(BInt i=0; i<m.Rows(); i++) { det *= m(i, i); }
    return(det);
}

//--------------------------------------------------------------------
template <class Any>
void  
MTMSquare(const BLowTrMatrix<Any>& m,
	        BSymMatrix<Any>&   sq)
//--------------------------------------------------------------------
{
    Any aux;
    
    sq.Alloc(m.Rows());
    for(BInt i=0; i<sq.Rows(); i++)
    {
	for(BInt j=i; j<sq.Rows(); j++)
	{
	    aux = 0;
	    for(BInt k=j; k<sq.Rows(); k++)
	    {
		aux += m(k, i) * m(k, j);
	    }
	    sq(j, i)=aux;
	}
    }
}

//--------------------------------------------------------------------
template <class Any>
void  
MMTSquare(const BLowTrMatrix<Any>& m,
	        BSymMatrix<Any>&   sq)
//--------------------------------------------------------------------
{
    Any aux;
    
    sq.Alloc(m.Rows());
    for(BInt i=0; i<sq.Rows(); i++)
    {
	for(BInt j=0; j<=i; j++)
	{
	    aux = 0;
	    for(BInt k=0; k<=j; k++)
	    {
		aux += m(i, k) * m(j, k);
	    }
	    sq(i, j)=aux;
	}
    }
}
