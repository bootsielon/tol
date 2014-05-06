/* tol_matgen.hpp: Implementation for BMatrixGen class.
                GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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


#if HAVE_VALUES_H
# include <values.h>
#endif

#if GCC_VERSION >= 40300
#include <limits>
#endif

//--------------------------------------------------------------------
template <class Any>
Any& BMatrixGen<Any>::NullValue ()
//--------------------------------------------------------------------
{
    static Any nullValue_ =0;
    nullValue_=Any(0.0);
    return(nullValue_);
}

//--------------------------------------------------------------------
template <class Any>
void BMatrixGen<Any>::Initialize()
//--------------------------------------------------------------------
{
}

//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>::BMatrixGen()
    
/*! Default constructor.
 */
//--------------------------------------------------------------------
    : firstOfRow_(), data_(), rows_(0), columns_(0)
{
    Alloc(0,0);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>::BMatrixGen(BInt r, BInt c, const BArray<BArray<Any> >& d)
    
/*! Constructor.
 */
//--------------------------------------------------------------------
    : firstOfRow_(), data_(), rows_(0), columns_(0)
{
    Alloc(r, c);
    for(BInt i=0; i<rows_; i++)
    {
	for(BInt j=0; j<columns_; j++)
	{
	    (*this)(i,j) = d[i][j];
	}
    }
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>::BMatrixGen(BInt r, BInt c, const Any* buffer)
    
/*! Constructor.
 */
//--------------------------------------------------------------------
    : firstOfRow_(), data_(), rows_(0), columns_(0)
{
    Alloc(r, c);
    for(BInt i=0; i<data_.Size(); i++)
    {
	if(buffer) { data_(i) = buffer[i]; }
	else       { data_(i) = Any(0.0); }
    }
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>::BMatrixGen(const BMatrixGen<Any>& m)
    
/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : firstOfRow_(), data_(), rows_(0), columns_(0)
{
  operator=(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>::~BMatrixGen()
    
/*! Destructor.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
template <class Any>
void BMatrixGen<Any>::Alloc(BInt r, BInt c)

/*! Allocates r rows and c columns of memory for the class
 *  member data_.
 */
//--------------------------------------------------------------------
{
  static int max_size = LONG_MAX;
  int i,j;
 
  if (rows_ == r && columns_ == c)
    return;
  
  if(r<0||c<0) 
  { 
    Error(I2("FATAL: Cannot create a matrix with negative dimensions ",
             "FATAL: No se puede crear una matriz de dimensiones negativas ")+
             "("+r+"x"+c+")");
    rows_=columns_=0;
    return;
  }
  double rc=(double)r*(double)c;
  if(rc>=max_size) { rc=0; }
  data_.ReallocBuffer((int)rc);
  buffer_ = data_.GetBuffer();
  if(r && c && !buffer_) 
  { 
    Error(I2("FATAL: Cannot allocate memory for a matrix ",
             "FATAL: No se pudo alojar memoria para una matriz ")+
             "("+r+"x"+c+")");
    rows_=columns_=0;
    firstOfRow_.ReallocBuffer(0);
    return;
  }
  else
  {
    firstOfRow_.ReallocBuffer(r);
    firstBuffer_ = firstOfRow_.GetBuffer();
    for(i=j=0; i<r; i++) { firstBuffer_[i] = j; j+=c; }
    rows_    = r;
    columns_ = c;
  }
}


//--------------------------------------------------------------------
template <class Any>
void BMatrixGen<Any>::SetAllValuesTo(const Any& x0)
//--------------------------------------------------------------------
{
  register int  s = data_.Size();
  register Any* x = data_.GetBuffer();
  register Any* y = x + s;
  for(; x<y; x++)
  {
    (*x) = x0;
  }
}

//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>& BMatrixGen<Any>::Copy(const BMatrixGen<Any>* m)

/*! Sets in this diagonal matrix the principal diagonal of a general
 *  matrix m.
 */
//--------------------------------------------------------------------
{
  register int i;
  register int j;
  register int r = m->Rows();
  register int c = m->Columns();
  Alloc(r, c);
  if((m->StoreType()!=BMST_dense_))
  {
    register Any* y;
    register Any* x = data_.GetBuffer();
    for(i=0; i<r; i++)
    {
	    for(j=0; j<c; j++)
	    {
	      y = &(*m)(i,j);
	      (*x) = (*y);
        x++;
	    }
    }
  }
  else
  {
    int s = m->Data().Size();
    register       Any* x = data_.GetBuffer();
    register const Any* y = m->Data().Buffer();
    memcpy(x,y,s*sizeof(Any));
  }
  return(*this);
}

template <class Any>
BMatrixGen<Any>& BMatrixGen<Any>::operator=(const BMatrixGen<Any>& m)  
{ return(Copy(&m)); }

//--------------------------------------------------------------------
// Access and manipulation functions for BMatrixGen class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>& 
BMatrixGen<Any>::Unknown()

/*! Unknown matrix (for grammar).
 */
//--------------------------------------------------------------------
{
    static BMatrixGen<Any> zero_(0,0,NIL);
    return(zero_);
}



//--------------------------------------------------------------------
template <class Any>
void 
BMatrixGen<Any>::EnsureDimensions(BInt i, BInt j) const
//--------------------------------------------------------------------
{
    if((i<0)||(j<0)||(i>=rows_)||(j>=columns_))
    {
	Error(I2("Forbidden matrix access","Acceso prohibido de matrices")+
	      "(0 < "+i+" < "+rows_+")"+
	      "(0 < "+j+" < "+columns_+")");
    }
}

//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::Sub(BInt i0, BInt j0,
		  BInt rows, BInt columns) const
//--------------------------------------------------------------------
{
  BMatrixGen<Any> S(rows, columns);
  BInt i, j;
  for(i=0; i<S.Rows(); i++)
  {
    for(j=0; j<S.Columns(); j++)
    {
      S(i,j) = Get(i+i0,j+j0);
    }
  }
  return(S);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::SafeSub(BInt i0, BInt j0, BInt rows, BInt columns) const
//--------------------------------------------------------------------
{
  BMatrixGen<Any> S(rows, columns);
  BInt i, j;
  for(i=0; i<S.Rows(); i++)
  {
    for(j=0; j<S.Columns(); j++)
    {
      S(i,j) = (*this)(i+i0,j+j0);
    }
  }
  return(S);
}

//--------------------------------------------------------------------
// Operations for BMatrixGen class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>& 
BMatrixGen<Any>::operator += (const BMatrixGen<Any>& m)

/*! Operator += overloading.
 */
//--------------------------------------------------------------------
{
  register int i;
  register int j;
  register int r = Rows();
  register int c = Columns();
  if((r==m.Rows())&&(c==m.Columns()))
  {
    if((StoreType()!=BMST_dense_)||(m.StoreType()!=BMST_dense_))
    {
      for(i=0; i<r; i++)
      {
        for(j=0; j<c; j++)
        {
          (*this)(i,j) += m(i,j);
        }
      }
    }
    else
    {
      register       int  s = m.Data().Size();
      register       Any* x = data_.GetBuffer();
      register const Any* y = m.Data().Buffer();
      register       Any* z = data_.GetBuffer() + s;
      for(; x<z; x++, y++)
      {
        (*x) += (*y);
      }
    }
  }
  else
  {
    Warning(I2("<W>Wrong sizes when trying to add two matrices ("+
               rows_ +","+ columns_+") and ("+m.Rows()+","+m.Columns()+").",
               "<W>Imposible sumar dos matrices de dimensiones ("+
               rows_ +","+ columns_+") y ("+m.Rows()+","+m.Columns()+").")+
               "</W>\n");
    Alloc(0,0);
  }
  return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::operator + (const BMatrixGen<Any>& m2) const

/*! Operator overloading to add two matrices.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> s = (*this);
    s += m2;
    return(s);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>& 
BMatrixGen<Any>::operator -= (const BMatrixGen<Any>& m)

/*! Operator -= overloading.
 */
//--------------------------------------------------------------------
{
  register int i;
  register int j;
  register int r = Rows();
  register int c = Columns();
  if((r==m.Rows())&&(c==m.Columns()))
  {
    if((StoreType()!=BMST_dense_)||(m.StoreType()!=BMST_dense_))
    {
      for(i=0; i<r; i++)
      {
        for(j=0; j<c; j++)
        {
          (*this)(i,j) -= m(i,j);
        }
      }
    }
    else
    {
      register       int  s = m.Data().Size();
      register       Any* x = data_.GetBuffer();
      register const Any* y = m.Data().Buffer();
      register       Any* z = data_.GetBuffer() + s;
      for(; x<z; x++, y++)
      {
        (*x) -= (*y);
      }
    }
  }
  else
  {
    Warning(I2("<W>Wrong sizes when trying to subtract two matrices ("+
               rows_ +","+ columns_+") and ("+m.Rows()+","+m.Columns()+").",
               "<W>Imposible restar dos matrices de dimensiones ("+
               rows_ +","+ columns_+") y ("+m.Rows()+","+m.Columns()+").")+
               "</W>\n");
    Alloc(0,0);
  }
  return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::operator - (const BMatrixGen<Any>& m2) const

/*! Operator overloading to difference two matrices.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> s = (*this);
    s -= m2;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>& 
BMatrixGen<Any>::operator+=(const Any& n)

/*! Multiply member this by n.
 */
//--------------------------------------------------------------------
{
    for(BInt i=0; i<data_.Size(); i++)
    {
	data_(i) += n;
    }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::operator + (const Any& n) const

/*! Operator overloading to sum one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> m=*this;
    m+=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>& 
BMatrixGen<Any>::operator-=(const Any& n)

/*! Difference member this by n.
 */
//--------------------------------------------------------------------
{
    for(BInt i=0; i<data_.Size(); i++)
    {
	data_(i) -= n;
    }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::operator - (const Any& n) const

/*! Operator overloading to difference one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> m=*this;
    m-=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>&
BMatrixGen<Any>::operator*=(const Any& n)
    
/*! Multiply member this by n.
 */
//--------------------------------------------------------------------
{
    for(BInt i=0; i<data_.Size(); i++)
    {
	data_(i) *= n;
    }
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::operator - () const

/*! Operator overloading to product two matrices.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> m=*this;
    m*=(-1);
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::operator * (const Any& n) const
    
/*! Operator overloading to product one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> m=*this;
    m*=n;
    return(m);
}



//--------------------------------------------------------------------
template <class Any>
void	 
MatMult(const BMatrixGen<Any>& A, const BMatrixGen<Any>& B, BMatrixGen<Any>& C)

/*! Multiply the members d1 and d2.
 */
//--------------------------------------------------------------------
{
    register Any aux;
    register Any zero = Any(0);
    if(!A.Columns() || !B.Columns() || !A.Rows() || !B.Rows())
    {
      C.Alloc(0,0);
	return;
    }
    if(A.Columns()!=B.Rows())
    {
	return;
    }
    register int M  = A.Rows();
    register int K  = B.Rows();
    register int N  = B.Columns();
    C.Alloc(M,N);
    
    register int m, n, k;
    if(M>=N)
    {
	for(n=0; n<N; n++)
	{
	    for(m=0; m<M; m++) { C(m,n) = zero; }
	    for(k=0; k<K; k++)
	    {
		aux = B(k,n);
		if(aux != zero) { for(m=0; m<M; m++) { C(m,n) += A(m,k)*aux; } }
	    }
	}
    }
    else
    {
	for(m=0; m<M; m++)
	{
	    for(n=0; n<N; n++) { C(m,n) = zero; }
	    for(k=0; k<K; k++)
	    {
		aux = A(m,k);
		if(aux != zero) { for(n=0; n<N; n++) { C(m,n) += aux*B(k,n); } }
	    }
	}
    }
}


//--------------------------------------------------------------------
template <class Any>
void	WeightProd(const BMatrixGen<Any>& A, const BMatrixGen<Any>& B, BMatrixGen<Any>& C)

/*! Multiply d1 and d2 cell to cell.
 */
//--------------------------------------------------------------------
{
  register Any aux;
  if(!A.Columns() || !B.Columns() || !A.Rows() || !B.Rows())
  {
    C.Alloc(0,0);
	  return;
  }
  if((A.Columns()!=B.Columns())||(A.Rows()!=B.Rows()))
  {
  	return;
  }
  register int M  = A.Rows();
  register int N  = A.Columns();
  register int S  = A.Data().Size();
  C.Alloc(M,N);
    
  register int k;
  register const Any* a = A.Data().Buffer();
  register const Any* b = B.Data().Buffer();
  register Any* c = C.GetData().GetBuffer();
	for(k=0; k<S; k++, a++, b++, c++)
	{
    *c = *a * *b;
	}
}


//--------------------------------------------------------------------
template <class Any>
void 
BMatrixGen<Any>::Reverse()

/*! Reverse rows and columns.
 */
//--------------------------------------------------------------------
{
    Any x1;
    BInt i=0, j=data_.Size()-1;
    for(; i<j; i++,j--)
    {
	x1 = data_(i);
	data_(i) = data_(j);
	data_(j) = x1;
    }
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>	 
ConcatColumns(const BMatrixGen<Any>& m1, const BMatrixGen<Any>& m2)

/*! Concats two matrices by rows.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> res;
    if(!m1.Columns()||!m1.Rows()) { return(m2); }
    if(!m2.Columns()||!m2.Rows()) { return(m1); }
    if(m1.Rows()==m2.Rows())
    {
	BInt r1 = m1.Rows();
	BInt c1 = m1.Columns();
	BInt c2 = m2.Columns();
	res.Alloc(r1, c1+c2);
	BInt i, j;
	
	for(i=0; i<r1; i++)
	{
	    for(j=0;  j<c1;    j++) { res(i, j) = m1(i, j   ); }
	    for(j=c1; j<c1+c2; j++) { res(i, j) = m2(i, j-c1); }
	}
    }
    return(res);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>	 
ConcatRows(const BMatrixGen<Any>& m1, const BMatrixGen<Any>& m2)

/*! Concats two matrices by rows.
 */
//--------------------------------------------------------------------
{
    BMatrixGen<Any> res;
    if(!m1.Columns()||!m1.Rows()) { return(m2); }
    if(!m2.Columns()||!m2.Rows()) { return(m1); }
    
    if(m1.Columns()==m2.Columns())
    {
	BInt r1 = m1.Rows();
	BInt c1 = m1.Columns();
	BInt r2 = m2.Rows();
	res.Alloc(r1+r2, c1);
	
	BInt i, j;
	for(j=0; j<c1; j++)
	{
	    for(i=0;  i<r1;    i++) { res(i, j) = m1(i,    j); }
	    for(i=r1; i<r1+r2; i++) { res(i, j) = m2(i-r1, j); }
	}
    }
    return(res);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>	 
operator <<  (const BMatrixGen<Any>& m1,
	      const BMatrixGen<Any>& m2)
//--------------------------------------------------------------------
{
    return(ConcatRows(m1,m2));
}

//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any>	 
operator |  (const BMatrixGen<Any>& m1,
	     const BMatrixGen<Any>& m2)
//--------------------------------------------------------------------
{
    return(ConcatColumns(m1,m2));
}

//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::T() const // Transpose

/*! Operation overloading to traspose one matrix.
 */
//--------------------------------------------------------------------
{
  BMatrixGen<Any> mat;
  mat.Alloc(columns_, rows_);
  for(BInt i=0; i<mat.Rows(); i++)
  {
    for(BInt j=0; j<mat.Columns(); j++) 
    { 
      mat(i,j) = (*this)(j,i); 
    }
  }
  return(mat);
}


//--------------------------------------------------------------------
template <class Any>
BText 
BMatrixGen<Any>::Name() const

/*! Introduces the matrix values into a BText object.
 */
//--------------------------------------------------------------------
{
  BText txt;
  for(BInt r=0 ; r<Rows() ; r++)
  {
    for(BInt c=0; c<Columns() ; c++) { txt << Get(r, c)<<"; "; }
    txt<<"\n";
  }
  return(txt);
}


//--------------------------------------------------------------------
template <class Any>
ostream& 
operator << (ostream& os, const BMatrixGen<Any>& mat)
    
/*! Introduces the matrix values into a BText object.
 */
//--------------------------------------------------------------------
{
    for(BInt r=0 ; r<mat.Rows() ; r++)
    {
	for(BInt c=0; c<mat.Columns() ; c++) { os << mat.Get(r, c) << "; "; }
	os<<"\n";
    }
    return(os);
}


//--------------------------------------------------------------------
template <class Any>
void 
Std(const BMatrixGen<Any>& mat)
    
/*! Introduces the matrix values into a BText object.
 */
//--------------------------------------------------------------------
{
    for(BInt r=0 ; r<mat.Rows() ; r++)
    {
	for(BInt c=0; c<mat.Columns() ; c++) {
	    Std(BText("")+mat(r, c) + "; "); 
	}
	Std("\n");
    }
}


//--------------------------------------------------------------------
template <class Any>
Any 
BMatrixGen<Any>::Max() const

/*! Returns the maximum of a matrix, it's to say, the
 *  standard desviation of its elements
 */
//--------------------------------------------------------------------
{
    Any norm=0;
    for(BInt i=0; i<Rows(); i++)
    {
	for(BInt j=0; j<Columns(); j++)
	{
	    if(!(i+j) || (norm < (*this)(i, j))) { norm = (*this)(i, j); }
	}
    }
    return(norm);
}


//--------------------------------------------------------------------
template <class Any>
Any 
BMatrixGen<Any>::Min() const
    
/*! Returns the minimum of a matrix, it's to say, the
 *  standard desviation of its elements
 */
//--------------------------------------------------------------------
{
    Any norm=0;
    for(BInt i=0; i<Rows(); i++)
    {
	for(BInt j=0; j<Columns(); j++)
	{
	    if(!(i+j) || (norm > (*this)(i, j))) { norm = (*this)(i, j); }
	}
    }
    return(norm);
}


//--------------------------------------------------------------------
template <class Any>
Any 
BMatrixGen<Any>::FrobeniusNorm() const

/*! Returns the Frobenius norm of a matrix, it's to say, the
 *  standard desviation of its elements
 */
//--------------------------------------------------------------------
{
    Any norm=0;
    BInt size=Rows()*Columns();
    for(BInt i=0; i<Rows(); i++)
    {
	for(BInt j=0; j<Columns(); j++) {
	    norm += (*this)(i,j) * (*this)(i,j); 
	}
    }
    Any ns = norm/size;
    return(Sqrt(ns));
}


//--------------------------------------------------------------------
template <class Any>
Any 
BMatrixGen<Any>::MaxAbsNorm() const
    
/*! Returns the maximum of absolute values of each element of this
 *  matrix.
 */
//--------------------------------------------------------------------
{
    Any norm=0;
    Any abs;
    BInt maxRow=Rows(), maxCol=Columns();
    
    for(BInt i=0; i<maxRow; i++)
    {
	for(BInt j=0; j<maxCol; j++)
	{
	    abs = Abs((*this)(i, j));
	    if(norm<abs) { norm = abs; }
	}
    }
    return(norm);
}


//--------------------------------------------------------------------
template <class Any>
Any 
BMatrixGen<Any>::NotNull() const
    
/*! Returns 0 if the matrix is null, and the first not null element
 *  found in other case.
 */
//--------------------------------------------------------------------
{
    Any elem=0;
    for(BInt i=0; i<Rows(); i++)
    {
	for(BInt j=0; j<Columns(); j++)
	{
	    if((*this)(i, j)!=Any(0.0)) { elem = (*this)(i, j); }
	}
    }
    return(elem);
}

//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::PermutateRows(const BArray<BInt>& perm)
//--------------------------------------------------------------------
{
    BInt i, j;
    BMatrixGen<Any> P(Rows(),Columns());
    BInt m = Minimum(Rows(),perm.Size());
    for(i=0; i<m; i++)
    {
	for(j=0; j<Columns(); j++)
	{
	    if((perm(i)>=0)&&(perm(i)<Rows()))
	    {
		P(perm(i),j)=(*this)(i,j);
	    }
	}
    }
    for(i=m; i<Rows(); i++)
    {
	for(j=0; j<Columns(); j++)
	{
	    P(i,j)=(*this)(i,j);
	}
    }
    return(P);
}


//--------------------------------------------------------------------
template <class Any>
BMatrixGen<Any> 
BMatrixGen<Any>::PermutateColumns(const BArray<BInt>& perm)
//--------------------------------------------------------------------
{
    BInt i, j;
    BMatrixGen<Any> P(Rows(),Columns());
    BInt m = Minimum(Columns(),perm.Size());
    for(j=0; j<m; j++)
    {
	for(i=0; i<Rows(); i++)
	{
	    if((perm(j)>=0)&&(perm(j)<Columns()))
	    {
		P(i,perm(j))=(*this)(i,j);
	    }
	}
    }
    for(j=m; j<Columns(); j++)
    {
	for(i=0; i<Rows(); i++)
	{
	    P(i,j)=(*this)(i,j);
	}
    }
    return(P);
}

//--------------------------------------------------------------------
template <class Any>
BBool
BMatrixGen<Any>::IsKnown() const
{
  for(int i=0; i<Rows(); i++)
    for(int j=0; j<Columns(); j++)
      if(!::IsKnown((*this)(i,j))) return BFALSE;
  
  return BTRUE;
}


//--------------------------------------------------------------------
template <class Any>
Any BMatrixGen<Any>::Determinant() const
{
  const BMatrixGen<Any>& T = *this;
  if(rows_!=columns_) { return(Any(0.0)); }
  if(rows_==1) { return(T(0,0)); }
  if(rows_==2) { return(T(0,0)*T(1,1)-T(0,1)*T(1,0)); }
  if(rows_==3) { return(
    T(0,0)*T(1,1)*T(2,2)
   +T(0,1)*T(1,2)*T(2,0)
   +T(0,2)*T(1,0)*T(2,1)
   -T(0,0)*T(1,2)*T(2,1)
   -T(0,1)*T(1,1)*T(2,2)
   -T(0,2)*T(1,0)*T(2,0)
  ); }
  Any d = Any(0);
  int i,j,k,r;
  for(k=0; k<rows_; k++)
  {
    Any c = T(k,0)*Any(pow(-1.0,k));
    r = rows_-1;
    BMatrixGen<Any> A(r,r);
    for(i=0; i<r; i++)
    {
      for(j=0; j<r; j++)
      {
        A(i,j) = T(i+(i>=k),j+(j>=k));
      }
    }
    d += A.Determinant()*c;
  }
  return(d);
};


