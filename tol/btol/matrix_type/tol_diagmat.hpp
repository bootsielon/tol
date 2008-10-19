/* tol_diagmat.hpp: Implementation for BDiagMatrix class.
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
BDiagMatrix<Any>::BDiagMatrix()
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Alloc(0);
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>::BDiagMatrix(BInt r, const Any& x)
    
/*! Creates a digonal matrix of order r with all elements equal to x.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    BInt n;
    Alloc(r);
    for(n=0; n<this->rows_; n++) { this->data_(n) = x; }
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>::BDiagMatrix(const BDiagMatrix<Any>& m)
    
/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : BMatrix<Any>()
{
    Copy(&m);
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>::~BDiagMatrix()
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
// Access and manipulation functions for BDiagMatrix class.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class Any>
void 
BDiagMatrix<Any>::Alloc(BInt r)
    
/*! Allocates memory for a r rows diagonal matrix, storing only the
 *  diagonal elements.
 */
//--------------------------------------------------------------------
{
    this->data_.AllocBuffer(r);
    this->buffer_= this->data_.GetBuffer();
    this->columns_ = this->rows_ = r;
}


//--------------------------------------------------------------------
template <class Any>
BMatA& 
BDiMA::Copy(const BMatrix<Any>* m)

/*! Sets in this diagonal matrix the principal diagonal of a general
 *  matrix m.
 */
//--------------------------------------------------------------------
{
    int r = Minimum(m->Rows(),m->Columns());
    Alloc(r);
    for(BInt i=0; i<r; i++) { this->data_(i) = (*m)(i,i); }
    return(*this);
}



//--------------------------------------------------------------------
template <class Any>
BDiMA& 
BDiMA::operator=(const BMatA& m)
 { return (BDiMA&)(Copy(&m)); }

template <class Any>
BDiMA& 
BDiMA::operator=(const BSyMA& m)  
{ return (BDiMA&)(Copy(&m)); }

template <class Any>
BDiMA& 
BDiMA::operator=(const BLTMA& m)  
{ return (BDiMA&)(Copy(&m)); }

template <class Any>
BDiMA& 
BDiMA::operator=(const BUTMA& m)  
{ return (BDiMA&)(Copy(&m)); }

template <class Any>
BDiMA& 
BDiMA::operator=(const BDiMA& m)  
{ return (BDiMA&)(Copy(&m)); }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
Any 
BDiagMatrix<Any>::Get(BInt i, BInt j) const

/*! Returns the element (i, j) of the matrix.
 */
//--------------------------------------------------------------------
{
    BMAT_ENSURE_DIM(i,j);
    if((i<0) || (j<0) || (i>=this->rows_) || (j>=this->rows_))
    { return(BArray<Any>::Overflow()); }
    else
    { return((i==j)?this->data_(i):this->NullValue()); }
}


//--------------------------------------------------------------------
template <class Any>
Any& 
BDiagMatrix<Any>::operator() (BInt i, BInt j) const
//--------------------------------------------------------------------
{
    BMAT_ENSURE_DIM(i,j);
    if(i==j)
    { return(this->data_(i));	}
    else
    { return(this->NullValue()); }
}

//--------------------------------------------------------------------
// Operations for BDiagMatrix class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator +=(const BDiagMatrix<Any>& m)
//--------------------------------------------------------------------
{
  if(this->Rows()!=m.Rows()) { this->Alloc(0); }
  else
  {
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) += m.Data()(i); }
  }
  return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator + (const BDiagMatrix<Any>& m1) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> s = *this;
    s += m1;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator -= (const BDiagMatrix<Any>& m)
//--------------------------------------------------------------------
{
  if(this->Rows()!=m.Rows()) { this->Alloc(0); }
  else
  {
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) -= m.Data()(i); }
  }
  return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator - (const BDiagMatrix<Any>& m1) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> s = *this;
    s -= m1;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator *=(const BDiagMatrix<Any>& m)
//--------------------------------------------------------------------
{
  if(this->Rows()!=m.Rows()) { Alloc(0); }
  else
  {
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) *= m.Data()(i); }
  }
  return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>  
BDiagMatrix<Any>::operator * (const BDiagMatrix<Any>& m1) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> s = *this;
    s *= m1;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator /=(const BDiagMatrix<Any>& m)
//--------------------------------------------------------------------
{
  if(this->Rows()!=m.Rows()) { this->Alloc(0); }
  else
  {
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) /= m.Data()(i); }
  }
  return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator / (const BDiagMatrix<Any>& m1) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> s = *this;
    s /= m1;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator+=(const Any& n)
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) += n; }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator + (const Any& n) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any>  p = *this;
    p += n;
    return(p);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator-=(const Any& n)
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) -= n; }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator - (const Any& n) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any>  p = *this;
    p -= n;
    return(p);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator*=(const Any& n)
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) *= n; }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator * (const Any& n) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any>  p = *this;
    p *= n;
    return(p);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator /= (const Any& n)
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->rows_; i++) { this->data_(i) /= n; }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator / (const Any& n) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> m=*this;
    m/=n;
    return(m);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::operator ^= (const Any& n)
//--------------------------------------------------------------------
{
    for(BInt i=0; i<this->rows_; i++) { this->data_(i)=Pow(this->data_(i),n); }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::operator ^ (const Any& n) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> m=*this;
    for(BInt i=0; i<this->rows_; i++) { m.Data()(i)=Pow(this->data_(i),n); }
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> BDiagMatrix<Any>::I() const
//--------------------------------------------------------------------
{
    return((*this)^(Any(-1)));
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::P(const Any chop) const
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> m=*this;
    for(BInt i=0; i<this->rows_; i++)
    {
	if(Abs(this->data_(i))>=chop)
	{
	    m.Data()(i)=1/this->data_(i);
	}
	else
	{
	    m.Data()(i)=0;
	}
    }
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any> 
BDiagMatrix<Any>::T() const	  //Transpose
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> mat=*this;
    return(mat);
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::PutColumnsNorm(const BMatrix<Any>& M)
//--------------------------------------------------------------------
{
    BInt i,j, m=M.Columns(), n=M.Rows();
    Alloc(m);
    for(j=0; j<m; j++)
    {
	this->data_(j)=0;
	for(i=0; i<n; i++)
	{
	    this->data_(j) += M(i,j)*M(i,j);
	}
	this->data_(j) = Sqrt(this->data_(j));
    }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& 
BDiagMatrix<Any>::PutColumnsMaxAbs(const BMatrix<Any>& M)
//--------------------------------------------------------------------
{
    BInt i,j, m=M.Columns(), n=M.Rows();
    Alloc(m);
    Any aux;
    for(j=0; j<m; j++)
    {
	this->data_(j)=0;
	for(i=0; i<n; i++)
	{
	    aux = Abs(M(i,j));
	    if(this->data_(j) < aux)
	    {
		this->data_(j) = aux;
	    }
	}
    }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>  
BDiagMatrix<Any>::GetColumnsNorm(const BMatrix<Any>& M)
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> A;
    A.PutColumnsNorm(M);
    return(A);
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>  
BDiagMatrix<Any>::GetColumnsMaxAbs(const BMatrix<Any>& M)
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> A;
    A.PutColumnsMaxAbs(M);
    return(A);
}


//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>& BDiagMatrix<Any>::PutDiag(const BMatrix<Any>& M, BInt d)
//--------------------------------------------------------------------
{
    BInt D = abs(d);
    BInt j, n=M.Columns()-D, N=M.Rows()-D, k=Minimum(n,N);
    Alloc(k);
    for(j=0; j<k; j++)
    {
	this->buffer_[j] = M(j+(d>=0)?0:D, j+(d<=0)?0:D);
    }
    return(*this);
}

//--------------------------------------------------------------------
template <class Any>
BDiagMatrix<Any>  
BDiagMatrix<Any>::GetDiag(const BMatrix<Any>& M, BInt d)
//--------------------------------------------------------------------
{
    BDiagMatrix<Any> A;
    A.PutDiag(M);
    return(A);
}
