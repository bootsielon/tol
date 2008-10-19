/* tol_spamat.hpp: Implementation for BSpaMatrix class.
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
BSpaMatrix<Any>::BSpaMatrix()

/*! Default constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>(), notNullCells_(0), notNullRows_(), vectors_()
{
}


//--------------------------------------------------------------------
template <class Any>
BSpaMatrix<Any>::BSpaMatrix(const BMatrix<Any>& m)

/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>(), notNullCells_(0), notNullRows_(), vectors_()
{
    Copy(&m);
}


//--------------------------------------------------------------------
template <class Any>
BSpaMatrix<Any>::BSpaMatrix(const BSpaMatrix<Any>& m)

/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>(), vectors_(), notNullCells_(0)
{
    
    *this = m;
}


//--------------------------------------------------------------------
template <class Any>
BSpaMatrix<Any>::~BSpaMatrix()
    
/*! Destructor.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
// Access and manipulation functions for BSpaMatrix class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
void 
BSpaMatrix<Any>::Alloc(BInt r, BInt c)

/*! Allocates memory for a r rows symmetric matrix, storing only the
 *  lower triangular half.
 */
//--------------------------------------------------------------------
{
    int i;
    vectors_.AllocBuffer(r);
    for(i=0; i<r; i++)
    {
	vectors_(i).AllocBuffer(c);
    }
    this->columns_ = c;
    this->rows_	   = r;
}


//--------------------------------------------------------------------
template <class Any>
BMatA& 
BSpMA::Copy(const BMatrix<Any>* m)

/*! Sets in this diagonal matrix the principal diagonal of a general
 *  matrix m.
 */
//--------------------------------------------------------------------
{
  int i, j, k, s, r = m->Rows(), c = m->Columns();
  Alloc(r,c);
  BInt r0 = 1+r/16;
  notNullRows_.ReallocBuffer(r0);
  notNullCells_ = 0;
  for(s=i=0; i<r; i++)
  {
    for(k=j=0; j<c; j++)
    {
      if((*m)(i,j)!=0)
      {
        vectors_(i)(k).pos_ = j;
        vectors_(i)(k).dat_ = (*m)(i,j);
        k++;
        notNullCells_++;
      }
    }
    vectors_(i).ReallocBuffer(k);
    if(k)
    {
      if(notNullRows_.Size()<=s)
      {
        notNullRows_.ReallocBuffer(Minimum(r,s*2));
      }
      notNullRows_(s++) = i;
    }
  }
  notNullRows_.ReallocBuffer(s);
  return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BSpMA& 
BSpMA::operator=(const BSpMA& m)
{
    vectors_ = m.Vectors();
    this->rows_    = m.Rows();
    this->columns_ = m.Columns();
    return(*this);
}

template <class Any>
BSpMA& 
BSpMA::operator=(const BMatA& m)  
{ return (BSpMA&)(Copy(&m)); }

template <class Any>
BSpMA& 
BSpMA::operator=(const BSyMA& m)  
{ return (BSpMA&)(Copy(&m)); }

template <class Any>
BSpMA& 
BSpMA::operator=(const BLTMA& m)  
{ return (BSpMA&)(Copy(&m)); }

template <class Any>
BSpMA& 
BSpMA::operator=(const BUTMA& m)  
{ return (BSpMA&)(Copy(&m)); }

template <class Any>
BSpMA& 
BSpMA::operator=(const BDiMA& m)  
{ return (BSpMA&)(Copy(&m)); }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any> 
BSpaMatrix<Any>::operator*(const BMatrix <Any>& B)
//--------------------------------------------------------------------
{
    BMatrix<Any> C;
    BSpaMatrix<Any>& A = *this;
    if(!A.Columns() || !B.Columns() ||  !A.Rows()	   || !B.Rows() ||
       (A.Columns()!=B.Rows()))
    {
	return(C);
    }
    register int M  = A.Rows();
    register int K  = B.Rows();
    register int N  = B.Columns();
    register int m, n, k, j, s;
    register Any aux;
    
    const BArray< BInt >&		     nnv = A.NotNullRows();
    const BArray< BArray< BIndexed<Any> > >& v   = A.Vectors();
    C.Alloc(M,N);
    
    C.SetAllValuesTo(0);
    for(s=0; s<nnv.Size(); s++)
    {
	m = nnv(s);
	for(k=0; k<v(m).Size(); k++)
	{
	    aux = v(m)(k).dat_;
	    j	  = v(m)(k).pos_;
	    for(n=0; n<N; n++)
	    {
		C(m,n) += aux*B(j,n);
	    }
	}
    }
    return(C);
}

//--------------------------------------------------------------------
template<class Any>
BMatrix<Any> 
BSpaMatrix<Any>::MBtProd(const BSpaMatrix <Any>& B)
//--------------------------------------------------------------------
{
    BMatrix<Any> C;
    BSpaMatrix<Any>& A = *this;
    if(!A.Columns() || !B.Columns() || !A.Rows() || !B.Rows() ||
       (A.Columns()!=B.Columns()))
    {
	return(C);
    }
    register int M  = A.Rows();
    register int N  = B.Rows();
    register int m, n, k, i, j, r, s;
    register Any aux;
    
    const BArray< BInt >&			   nnu = A.NotNullRows();
    const BArray< BInt >&			   nnv = B.NotNullRows();
    const BArray< BArray< BIndexed<Any> > >&       u   = A.Vectors();
    const BArray< BArray< BIndexed<Any> > >&       v   = B.Vectors();
    C.Alloc(M,N);
    
    C.SetAllValuesTo(0);
    for(s=0; s<nnu.Size(); s++)
    {
	m = nnu(s);
	for(r=0; r<nnv.Size(); r++)
	{
	    n = nnv(r);
	    for(i=j=0; i<u(m).Size() && j<v(n).Size(); )
	    {
		if(u(m)(i).pos_==v(n)(j).pos_)
		{
		    C(m,n) += u(m)(i).dat_*v(n)(j).dat_;
		    i++;
		    j++;
		}
		else   if(u(m)(i).pos_ > v(n)(j).pos_)	 { j++; }
		else /*if(u(m)(i).pos_ < v(n)(j).pos_)*/ { i++; }
	    }
	}
    }
    return(C);
}
//--------------------------------------------------------------------
template <class Any>
BMatrix<Any> 
BSpaMatrix<Any>::MMtSqr()
//--------------------------------------------------------------------
{
    BMatrix<Any> C;
    BSpaMatrix<Any>& A = *this;
    register int M  = A.Rows();
    register int m, n, k, i, j, r, s;
    register Any aux;
    
    
    const BArray< BInt >&		     nnv = A.NotNullRows();
    const BArray< BArray< BIndexed<Any> > >& v   = A.Vectors();
    C.Alloc(M,M);
    
    C.SetAllValuesTo(0);
    for(s=0; s<nnv.Size(); s++)
    {
	m = nnv(s);
	for(r=0; r<nnv.Size(); r++)
	{
	    n = nnv(r);
	    for(i=j=0; i<v(m).Size() && j<v(n).Size(); )
	    {
		if(v(m)(i).pos_==v(n)(j).pos_)
		{
		    C(m,n) += v(m)(i).dat_*v(n)(j).dat_;
		    i++;
		    j++;
		}
		else   if(v(m)(i).pos_ > v(n)(j).pos_)	 { j++; }
		else /*if(v(m)(i).pos_ < v(n)(j).pos_)*/ { i++; }
	    }
	}
    }
    return(C);
}

//--------------------------------------------------------------------
template<class Any>
BMatrix<Any> 
BSparseMBtProd(const BMatrix <Any>& A, const BMatrix <Any>& B)
//--------------------------------------------------------------------
{
    BSpaMatrix<BDat> A_ = A;
    BSpaMatrix<BDat> B_ = B.T();
    return(A_.MBtProd(B_));
}

//--------------------------------------------------------------------
template<class Any>
BMatrix<Any> 
BSparseMMtSqr(const BMatrix <Any>& A)
//--------------------------------------------------------------------
{
    BSpaMatrix<BDat> A_ = A;
    return(A_.MMtSqr());
}

/*
//--------------------------------------------------------------------
   template <class Any>
   Any BSpaMatrix<Any>::Get(BInt i, BInt j) const

// PURPOSE: Returns the element (i, j) of the matrix.
//
//--------------------------------------------------------------------
{
  BMAT_ENSURE_DIM(i,j);
  if((i<0) || (j<0) || (i>=rows_) || (j>=columns_))
  { return(BArray<Any>::Overflow()); }
  else
  {
    return((i>=j)?buffer_[firstBuffer_[i]+j]:buffer_[firstBuffer_[j]+i]);
  }
}

//--------------------------------------------------------------------
   template <class Any>
   Any& BSpaMatrix<Any>::operator()(BInt i, BInt j) const

// PURPOSE: Returns the element (i, j) of the matrix.
//
//--------------------------------------------------------------------
{
  BMAT_ENSURE_DIM(i,j);
  return((i>=j)?buffer_[firstBuffer_[i]+j]:buffer_[firstBuffer_[j]+i]);
}

//--------------------------------------------------------------------
// Operations for BSpaMatrix class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any>& BSpaMatrix<Any>::operator += (const BSpaMatrix<Any>& m)

// PURPOSE: Operator += overloading.
//
//--------------------------------------------------------------------
{
  if(rows_==m.Rows())
  {
    for(BInt i=0; i<data_.Size(); i++)
    {
      data_(i) += m.Data()(i);
    }
  }
  return(*this);
}


//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any> BSpaMatrix<Any>::operator + (const BSpaMatrix<Any>& m) const

// PURPOSE: Operator overloading to add two matrices.
//
//--------------------------------------------------------------------
{
  BSpaMatrix<Any> s = *this;
  s += m;
  return(s);
}

//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any>& BSpaMatrix<Any>::operator -= (const BSpaMatrix<Any>& m)

// PURPOSE: Operator overloading to difference two matrices.
//
//--------------------------------------------------------------------
{
  if(rows_==m.Rows())
  {
    for(BInt i=0; i<data_.Size(); i++)
    {
      data_(i) -= m.Data()(i);
    }
  }
  return(*this);
}


//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any> BSpaMatrix<Any>::operator - (const BSpaMatrix<Any>& m) const

// PURPOSE: Operator overloading to difference two matrices.
//
//--------------------------------------------------------------------
{
  BSpaMatrix<Any> s = *this;
  s -= m;
  return(s);
}

//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any>& BSpaMatrix<Any>::operator *= (const Any& n)

// PURPOSE: Multiply member this by n.
//
//--------------------------------------------------------------------
{
  for(BInt i=0; i<data_.Size(); i++) { data_(i) *= n; }
  return(*this);
}


//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any> BSpaMatrix<Any>::operator * (const Any& n) const

// PURPOSE: Operator overloading to product one matrix and an Any element.
//
//--------------------------------------------------------------------
{
  BSpaMatrix<Any>  p = *this;
  p *= n;
  return(p);
}


//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any>& BSpaMatrix<Any>::operator /= (const Any& n)

// PURPOSE: Operator overloading to make the quotient between m and n.
//
//--------------------------------------------------------------------
{
  (*this)*=(1/n);
  return(*this);
}


//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any> BSpaMatrix<Any>::operator / (const Any& n) const

// PURPOSE: Operator overloading to make the quotient between "m1" and "n".
//
//--------------------------------------------------------------------
{
  BSpaMatrix<Any> m=*this;
  m/=n;
  return(m);
}



//--------------------------------------------------------------------
   template <class Any>
   BSpaMatrix<Any> BSpaMatrix<Any>::T() const // Transpose

// PURPOSE: Operation overloading to traspose one matrix .
//
//--------------------------------------------------------------------
{
  BSpaMatrix<Any> mat=*this;
  return(mat);
}


*/
