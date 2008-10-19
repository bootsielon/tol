/* tol_utrimat.hpp: Implementation for BUpTrMatrix class.
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
BUpTrMatrix<Any>::BUpTrMatrix()

/*! Default constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Alloc(0);
}


//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any>::BUpTrMatrix(const BUpTrMatrix<Any>& m)

/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Copy(&m);
}

//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any>::~BUpTrMatrix()
    
/*! Destructor.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
template <class Any>
BMatA& 
BUTMA::Copy(const BMatrix<Any>* m)

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
BUTMA& 
BUTMA::operator=(const BMatA& m)  
{ return (BUTMA&)(Copy(&m)); }

template <class Any>
BUTMA& 
BUTMA::operator=(const BSyMA& m)  
{ return (BUTMA&)(Copy(&m)); }

template <class Any>
BUTMA& 
BUTMA::operator=(const BLTMA& m)  
{ return (BUTMA&)(Copy(&m)); }

template <class Any>
BUTMA& 
BUTMA::operator=(const BUTMA& m)  
{ return (BUTMA&)(Copy(&m)); }

template <class Any>
BUTMA& 
BUTMA::operator=(const BDiMA& m)  
{ return (BUTMA&)(Copy(&m)); }
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class Any>
void BUpTrMatrix<Any>::Alloc(BInt r)

/*! Allocates memory for a r rows symmetric matrix, storing only the
 *  Uper triangular half.
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
// Access and manipulation functions for BUpTrMatrix class.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class Any>
Any 
BUpTrMatrix<Any>::Get(BInt i, BInt j) const

/*! Returns the element (i, j) of the matrix.
 */
//--------------------------------------------------------------------
{
    BMAT_ENSURE_DIM(i,j);
    if((i<0) || (j<0) || (i>=this->rows_) || (j>=this->rows_))
    { return(BArray<Any>::Overflow()); }
    else
    { return((i<=j)?this->buffer_[this->firstBuffer_[j]+i]:this->NullValue()); }
}


//--------------------------------------------------------------------
template <class Any>
Any& 
BUpTrMatrix<Any>::operator()(BInt i, BInt j) const
    
/*! Returns the element (i, j) of the matrix.
 */
//--------------------------------------------------------------------
{
    BMAT_ENSURE_DIM(i,j);
    return((i<=j)?this->buffer_[this->firstBuffer_[j]+i]:this->NullValue());
}

//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any>& 
BUpTrMatrix<Any>::operator += (const BUpTrMatrix<Any>& m)

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
BUpTrMatrix<Any> 
BUpTrMatrix<Any>::operator + (const BUpTrMatrix<Any>& m1)

/*! Operator overloading to add two matrices.
 */
//--------------------------------------------------------------------
{
    BUpTrMatrix<Any> s = *this;
    s += m1;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any>& 
BUpTrMatrix<Any>::operator -= (const BUpTrMatrix<Any>& m)

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
BUpTrMatrix<Any> 
BUpTrMatrix<Any>::operator - (const BUpTrMatrix<Any>& m)

/*! Operator overloading to difference two matrices.
 */
//--------------------------------------------------------------------
{
    BUpTrMatrix<Any> s = *this;
    s -= m;
    return(s);
}

/*
//--------------------------------------------------------------------
   template <class Any>
   BUpTrMatrix<Any> BUpTrMatrix<Any>::operator *
					      (const BUpTrMatrix<Any>& m1)


// PURPOSE: Operator overloading to product two matrices.
//
//--------------------------------------------------------------------
{
  BUpTrMatrix<Any> p;
  Any aux;

  if(m1.Rows() == Rows())
  {
    BInt r = m1.Rows();
    BInt i;
    p.Alloc(r);
    for(i=0; i<r; i++)
    {
      for(BInt j=i; j<r; j++)
      {
	aux = 0;
	for(BInt k=i; k<=j; k++)
	{
	  aux += (*this)(i, k) * m1(k, j);
	}
	p(i, j) = aux;
      }
    }
  }
  return(p);
}


//--------------------------------------------------------------------
   template <class Any>
   BMatrix<Any> operator * (const  BUpTrMatrix<Any>& m1,
			       const  BLowTrMatrix<Any>& m2)

// PURPOSE: Operator overloading to product one upper triangular matrix and
//	    a lower one.
//
//--------------------------------------------------------------------
{
  BInt max;
  BMatrix<Any> p;
  Any aux;

  if(m1.Rows() == m2.Rows())
  {
    BInt r = m1.Rows();
    p.Alloc(r, r);
    for(BInt i=0; i<r; i++)
    {
      for(BInt j=0; j<r; j++)
      {
	aux = 0;
	max = Maximum(i, j);
	for(BInt k=max; k<r; k++)
	{
	  aux += m1(i, k) * m2(k, j);
	}
	p.Put(i, j, aux);
      }
    }
  }
  return(p);
}
*/

//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any>& 
BUpTrMatrix<Any>::operator*=(const Any& n)

/*! Multiply member this by n.
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
BUpTrMatrix<Any> BUpTrMatrix<Any>::operator * (const Any& n)
    
/*! Operator overloading to product one matrix and an Any element.
*/
//--------------------------------------------------------------------
{
    BUpTrMatrix<Any>  p = *this;
    p *= n;
    return(p);
}


//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any>& 
BUpTrMatrix<Any>::operator /= (const Any& n)

/*! Operator overloading to make the quotient between m and n.
 */
//--------------------------------------------------------------------
{
    (*this)*=(1/n);
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BUpTrMatrix<Any> 
BUpTrMatrix<Any>::operator / (const Any& n)

/*! Operator overloading to make the quotient between "m1" and "n".
 */
//--------------------------------------------------------------------
{
    BUpTrMatrix<Any> m=*this;
    m/=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BLowTrMatrix<Any> BUpTrMatrix<Any>::T () const // Transpose

/*! Trasposes the matrix m.
 */
//--------------------------------------------------------------------
{
    BLowTrMatrix<Any> t;
    BInt r = this->Rows();
    t.Alloc(r);
    for(BInt i=0; i<r; i++)
    {
	for(BInt j=0; j<=i; j++)
	{
	    t(i, j) = (*this)(j, i);
	}
    }
    return(t);
}
