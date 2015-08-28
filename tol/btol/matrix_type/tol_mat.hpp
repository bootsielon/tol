/* tol_mat.hpp: Implementation for BMatrix class.
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

#include <tol/config.h>

#if HAVE_VALUES_H
# include <values.h>
#endif

#if GCC_VERSION >= 40300
#include <limits>
#endif

//--------------------------------------------------------------------
template <class Any>
Any& BMatrix<Any>::NullValue ()
//--------------------------------------------------------------------
{
    static Any nullValue_ =0;
    nullValue_=0;
    return(nullValue_);
}

//--------------------------------------------------------------------
template <class Any>
void BMatrix<Any>::Initialize()
//--------------------------------------------------------------------
{
}

//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>::BMatrix()
    
/*! Default constructor.
 */
//--------------------------------------------------------------------
    : firstOfRow_(), data_(), rows_(0), columns_(0)
{
    Alloc(0,0);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>::BMatrix(BInt r, BInt c, const BArray<BArray<Any> >& d)
    
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
BMatrix<Any>::BMatrix(BInt r, BInt c, const Any* buffer)
    
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
BMatrix<Any>::BMatrix(const BMatrix<Any>& m)
    
/*! Copy constructor.
 */
//--------------------------------------------------------------------
    : firstOfRow_(), data_(), rows_(0), columns_(0)
{
    operator=(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>::~BMatrix()
    
/*! Destructor.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
template <class Any>
void BMatrix<Any>::Alloc(BInt r, BInt c)

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
  //double rc=(double)r*(double)c;
  //if(rc>=max_size) { rc=0; }
  //data_.ReallocBuffer((int)rc);
  data_.ReallocBuffer( r * c );
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
void BMatrix<Any>::SetAllValuesTo(const Any& x0)
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
BMatrix<Any>& BMatrix<Any>::Copy(const BMatrix<Any>* m)

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

//--------------------------------------------------------------------
template <class Any>
BMatA& 
BMatA::operator=(const BMatA& m)  
{ return(Copy(&m)); }

template <class Any>
BMatA& 
BMatA::operator=(const BSyMA& m)  
{ return(Copy(&m)); }

template <class Any>
BMatA& 
BMatA::operator=(const BLTMA& m)  
{ return(Copy(&m)); }

template <class Any>
BMatA& 
BMatA::operator=(const BUTMA& m)  
{ return(Copy(&m)); }

template <class Any>
BMatA& 
BMatA::operator=(const BDiMA& m)  
{ return(Copy(&m)); }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Access and manipulation functions for BMatrix class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>& 
BMatrix<Any>::Unknown()

/*! Unknown matrix (for grammar).
 */
//--------------------------------------------------------------------
{
    static BMatrix<Any> zero_(0,0,NIL);
    return(zero_);
}



//--------------------------------------------------------------------
template <class Any>
void 
BMatrix<Any>::EnsureDimensions(BInt i, BInt j) const
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
BMatrix<Any> 
BMatrix<Any>::Sub(BInt i0, BInt j0,
		  BInt rows, BInt columns) const
//--------------------------------------------------------------------
{
  BMatrix<Any> S(rows, columns);
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
BMatrix<Any> 
BMatrix<Any>::SafeSub(BInt i0, BInt j0, BInt rows, BInt columns) const
//--------------------------------------------------------------------
{
  BMatrix<Any> S(rows, columns);
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
// Operations for BMatrix class.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>& 
BMatrix<Any>::operator += (const BMatrix<Any>& m)

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
BMatrix<Any> 
BMatrix<Any>::operator + (const BMatrix<Any>& m2) const

/*! Operator overloading to add two matrices.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> s = (*this);
    s += m2;
    return(s);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>& 
BMatrix<Any>::operator -= (const BMatrix<Any>& m)

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
BMatrix<Any> 
BMatrix<Any>::operator - (const BMatrix<Any>& m2) const

/*! Operator overloading to difference two matrices.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> s = (*this);
    s -= m2;
    return(s);
}

//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>& 
BMatrix<Any>::operator+=(const Any& n)

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
BMatrix<Any> 
BMatrix<Any>::operator + (const Any& n) const

/*! Operator overloading to sum one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> m=*this;
    m+=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>& 
BMatrix<Any>::operator-=(const Any& n)

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
BMatrix<Any> 
BMatrix<Any>::operator - (const Any& n) const

/*! Operator overloading to difference one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> m=*this;
    m-=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>&
BMatrix<Any>::operator*=(const Any& n)
    
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
BMatrix<Any> 
BMatrix<Any>::operator - () const

/*! Operator overloading to product two matrices.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> m=*this;
    m*=(-1);
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any> 
BMatrix<Any>::operator * (const Any& n) const
    
/*! Operator overloading to product one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> m=*this;
    m*=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>& 
BMatrix<Any>::operator /= (const Any& n)
    
/*! Operator overloading to make the quotient between m and n.
 */
//--------------------------------------------------------------------
{
    (*this)*=(1/n);
    return(*this);
}


//--------------------------------------------------------------------
template <class Any>
BMatrix<Any> 
BMatrix<Any>::operator / (const Any& n) const

/*! Operator overloading to quotient one matrix and an Any element.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> m=*this;
    m/=n;
    return(m);
}


//--------------------------------------------------------------------
template <class Any>
void	 
MatMult(const BMatrix<Any>& A, const BMatrix<Any>& B, BMatrix<Any>& C)

/*! Multiply the members d1 and d2.
 */
//--------------------------------------------------------------------
{
    register Any aux;
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
	    for(m=0; m<M; m++) { C(m,n) = 0; }
	    for(k=0; k<K; k++)
	    {
		aux = B(k,n);
		if(aux != 0) { for(m=0; m<M; m++) { C(m,n) += A(m,k)*aux; } }
	    }
	}
    }
    else
    {
	for(m=0; m<M; m++)
	{
	    for(n=0; n<N; n++) { C(m,n) = 0; }
	    for(k=0; k<K; k++)
	    {
		aux = A(m,k);
		if(aux != 0) { for(n=0; n<N; n++) { C(m,n) += aux*B(k,n); } }
	    }
	}
    }
}


//--------------------------------------------------------------------
template <class Any>
void 
BMatrix<Any>::Reverse()

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
BMatrix<Any>	 
ConcatColumns(const BMatrix<Any>& m1, const BMatrix<Any>& m2)

/*! Concats two matrices by rows.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> res;
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
BMatrix<Any>	 
ConcatRows(const BMatrix<Any>& m1, const BMatrix<Any>& m2)

/*! Concats two matrices by rows.
 */
//--------------------------------------------------------------------
{
    BMatrix<Any> res;
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
BMatrix<Any>	 
operator <<  (const BMatrix<Any>& m1,
	      const BMatrix<Any>& m2)
//--------------------------------------------------------------------
{
    return(ConcatRows(m1,m2));
}

//--------------------------------------------------------------------
template <class Any>
BMatrix<Any>	 
operator |  (const BMatrix<Any>& m1,
	     const BMatrix<Any>& m2)
//--------------------------------------------------------------------
{
    return(ConcatColumns(m1,m2));
}

//--------------------------------------------------------------------
template <class Any>
BMatrix<Any> 
BMatrix<Any>::T() const // Transpose

/*! Operation overloading to traspose one matrix.
 */
//--------------------------------------------------------------------
{
  BMatrix<Any> mat;
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
BMatrix<Any>::Name() const

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
operator << (ostream& os, const BMatrix<Any>& mat)
    
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
Std(const BMatrix<Any>& mat)
    
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
BMatrix<Any>::Max() const

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
BMatrix<Any>::Min() const
    
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
BMatrix<Any>::FrobeniusNorm() const

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
    BDat ns = BDat(norm)/size;
    return(ns.Sqrt().Value());
}


//--------------------------------------------------------------------
template <class Any>
Any 
BMatrix<Any>::MaxAbsNorm() const
    
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
BMatrix<Any>::NotNull() const
    
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
BBool 
LeastSqrGivens(BMatrix<Any>& c, BArray<Any>& d,
	       BArray<Any>&  x, BArray<Any>& r)
    
/*! Minimizes r*r' by Givens transformations method in the error
 *  equation
 *       C x + d = r
 *
 *    Trnsforms C and d and constructs x and r
 *    (Swartz, 7.2.1, 302)
 */
//--------------------------------------------------------------------
{
    BInt n = c.Columns();
    BInt N = c.Rows();
    r.ReallocBuffer(N);
    x.ReallocBuffer(n);
    
    BInt i, j, k;
    Any w, sinw, cosw, rot, h, s;
    
    for(j=0;j<n;j++)
    {
	for(i=j+1;i<N;i++)
	{
	    if(Abs(c(i,j))>BDat::Zero())
	    {
		if(Abs(c(j,j)) < std::numeric_limits<double>::min())
		{
		    w    = -c(i,j);
		    cosw = 0;
		    sinw = 1;
		    rot  = 1;
		}
		else
		{
		    w    = Sign(c(j,j))*Sqrt(c(j,j)*c(j,j)+c(i,j)*c(i,j));
		    cosw =  c(j,j)/w;
		    sinw = -c(i,j)/w;
		    if(Abs(sinw)<cosw) { rot = sinw;	      }
		    else		     { rot = Sign(sinw)/cosw; }
		}
	    }
	    c(j,j) = w;
	    c(i,j) = rot;
	    for(k=j+1;k<n;k++)
	    {
		h      = cosw*c(j,k)-sinw*c(i,k);
		c(i,k) = sinw*c(j,k)+cosw*c(i,k);
		c(j,k) = h;
	    }
	    h	   = cosw*d(j)-sinw*d(i);
	    d(i) = sinw*d(j)+cosw*d(i);
	    d(j) = h;
	}
    }
    for(i=n-1;i>=0;i--)
    {
	s	    = d(i);
	r(i) = 0;
	for(k=i+1;k<n;k++) { s += c(i,k)*x(k); }
	if(c(i,i)!=0) { x(i) = -s/c(i,i); }
	else
	{
	    return(BFALSE);
	}
    }
    for(i=n;i<N;i++) { r(i)=d(i); }
    for(j=n-1;j>=0;j--)
    {
	for(i=N-1;i>j;i--)
	{
	    rot = c(i,j);
	    Any absRot = Abs(rot);
	    if(rot==1.0)
	    {
		cosw = 0;
		sinw = 1;
	    }
	    else if(absRot<1.0)
	    {
		sinw = rot;
		cosw = Sqrt(1.0-sinw*sinw);
	    }
	    else
	    {
		cosw = 1/absRot;
		sinw = Sign(rot)*Sqrt(1.0-cosw*cosw);
	    }
	    h	   =  cosw*r(j)+sinw*r(i);
	    r(i) = -sinw*r(j)+cosw*r(i);
	    r(j) =  h;
	}
    }
    return(BTRUE);
}


//--------------------------------------------------------------------
template <class Any>
BBool 
HouseholderTransformation(BArray<Any>& t, BMatrix<Any>& c)

/*! Prepares c to use in LeastSqrHouseHoulder transformation method
 *  for different vectors d in error equations
 *       C x + d = r
 *
 *  Transform C and constructs t which contains neccessary information
 *  for posterior systems solving.
 *   (Swartz, 7.2.3, 312)
 */
//--------------------------------------------------------------------
{
    BInt n = c.Columns();
    BInt N = c.Rows();
    
    t.ReallocBuffer(n);
    BInt i, l, k, j;
    Any  w, p, h, s;
    BBool ok;
    for(l=0;l<n;l++)
    {
	s=0;
	for(i=l;i<N;i++) { s += c(i,l)*c(i,l); }
	s = Sqrt(s);
	ok = IsKnown(s) && (s>0);
	if(ok)
	{
	    w = Sqrt((1.0+Abs(c(l,l))/s)/2);
	    if(c(l,l)>=0)  { h =  2*s*w; t(l) = -s; }
	    else	      { h = -2*s*w; t(l) =  s; }
	    c(l,l) = w;
	    for(k=l+1;k<N;k++) { c(k,l)/=h; }
	    for(j=l+1;j<n;j++)
	    {
		p=0;
		for(k=l;k<N;k++) { p+=c(k,l)*c(k,j); }
		p+=p;
		for(i=l;i<N;i++) { c(i,j)-=p*c(i,l); }
	    }
	}
	else
	{
	    return(BFALSE);
	}
    }
    return(BTRUE);
}


//--------------------------------------------------------------------
template <class Any>
void 
LeastSqrHouseholder(const BArray <Any>& t,
		    const BMatrix<Any>& c,
		    const BArray<Any>& d,
		          BArray <Any>& x,
		          BArray<Any>& r)

/*! Minimizes r*r' by Householder transformations method in the error
 *  equation
 *      C x + d = r
 *
 *   Takes C and d and returns x and r
 *   Previously, you must call HouseHolderTransformation function with
 *   t and c parameters.
 *   (Swartz, 7.2.3, 313)
 */
//--------------------------------------------------------------------
{
    BInt n = c.Columns();
    BInt N = c.Rows();
    r.ReallocBuffer(N);
    x.ReallocBuffer(n);
    
    if(d.Size()!=N) { return; }
    BInt i, l, k;
    Any  s;
    
    for(l=0;l<n;l++)
    {
	s=0;
	for(k=l;k<N;k++) { s += c(k,l)*d(k); }
	s+=s;
	for(k=l;k<N;k++) { d(k) -= s*c(k,l); }
    }
    for(i=n-1;i>=0;i--)
    {
	s = d(i);
	r(i) = 0;
	for(k=i+1;k<n;k++)
	{ s += c(i,k)*x(k); }
	x(i) = -s/t(i);
    }
    for(i=n;i<N;i++) { r(i) = d(i); }
    for(l=n-1;l>=0;l--)
    {
	s = 0;
	for(k=l;k<N;k++) { s += c(k,l)*r(k); }
	s+=s;
	for(k=l;k<N;k++) { r(k) -= s*c(k,l); }
    }
}


//--------------------------------------------------------------------
template <class Any>
BInt 
BMatrix<Any>::GaussReduction(BInt   firstColumns,
			     BInt** reducted)
    
/*! Returns the range of first columns of this matrix by gaussian
 *  reduction of rows. In the reducted array appears the column of
 *  pivoting for each row or -1 if this row is not in the base
 *  (when matrix is not regular only).
 */
//--------------------------------------------------------------------
{
    if(Rows()>Columns()) { return(BFALSE); }
    if(!Rows()||!Columns()) { return(BFALSE); }
    if(firstColumns>Columns()) { firstColumns=Columns(); }
    
    BInt i, iter, column;
    
    (*reducted) = new BInt[Rows()];
    
    for(i=0; i<Rows(); i++) { (*reducted)[i] = -1; }
    
    for(iter=column=0;(column<firstColumns) && (iter<Rows());column++)
    {
//  for(i=0; i<Rows(); i++) { Std(BText(" ")+(*reducted)(i)+", "); }
//  Trace(BText("Searching pivot in column ") + (column+1));
	BInt  pivotRow=-1, i, j;
	Any	  pivot, max=0.0;
	for(i=0; i<Rows(); i++)
	{
	    Any piv = Abs((*this)(i, column));
	    if(((*reducted)[i]<0) && (piv>BDat::Zero().Value()))
	    {
//	Std(BText("\nColumn pivot ") + i + " = " + piv.Name() +
//	    ">0 = " + BInt(piv.Value()>Any::Zero()));
		Any sum=0.0;
		for(j=column; j<Columns(); j++) { sum += Abs((*this)(i, j)); }
		Any q = piv/sum;
//	Std(BText("\n  sum  = ") + sum.Name() + "; q = " + q.Name());
		if(q>max)
		{
		    max = q;
		    pivotRow = i;
		}
	    }
//    Std(BText("\nMax ") + i + " = " + max.Name());
	}
	if(pivotRow>=0)
	{
	    iter++;
	    (*reducted)[pivotRow]=column;
	    pivot = (*this)(pivotRow, column);
//    for(i=0; i<Rows(); i++) { Std(BText(" ")+(*reducted)(i)+", "); }
//    Trace(BText("column = ")+(column+1));
//    Trace(BText("iter = ")+iter + ", Pivot" +
//	    "("+(pivotRow+1)+","+(column+1)+") = "+pivot.Name() );
	    (*this)(pivotRow,column) = 1.0;
	    for(j=column+1; j<Columns(); j++)
	    {
		Any x = (*this)(pivotRow,j)/pivot;
		(*this)(pivotRow,j) = x;
	    }
//    Trace(MatToText());
	    for(i=0; i<Rows(); i++)
	    {
		Any aPiv = (*this)(i,column);
		if((i!=pivotRow) && (Abs(aPiv)>BDat::Zero().Value()))
		{
		    (*this)(i,column)=0.0;
		    for(j=column+1; j<Columns(); j++)
		    {
			Any x = (*this)(i,j)-(*this)(pivotRow,j)*aPiv;
			(*this)(i,j)=x;
		    }
//	  Trace(BText("Anti-pivot for ") + (i+1) + "-st row = " +aPiv);
//	  Trace(MatToText());
		}
	    }
	}
//  Trace(MatToText());
    }
    return(iter);
}


//--------------------------------------------------------------------
template <class Any>
BInt 
BMatrix<Any>::GaussReduction(BInt firstColumns)
    
/*! Returns the range of first columns of this matrix by gaussian
 *  reduction of rows. In the reducted array appears the column of
 *  pivoting for each row or -1 if this row is not in the base
 *  (when matrix is not regular only).
 */
//--------------------------------------------------------------------
{
    BInt* reducted = NULL;
    BInt	range = GaussReduction(firstColumns, &reducted);
    if(reducted) { delete [] reducted; }
    return(range);
}


//--------------------------------------------------------------------
template <class Any>
BBool 
BMatrix<Any>::Solve(const BMatrix<Any>& x, BMatrix<Any>& y)

/*! Solves the linnear system
 *
 *	  x = A y
 *
 *    and returns BTRUE if the system is regular
 */
//--------------------------------------------------------------------
{
    if(Rows()!=Columns())
    {
	return(0);
    }
    BInt i, j;
    BInt*	 reducted=NULL;
    BMatrix<Any> A     = ConcatColumns((*this),x);
    BInt	       range = A.GaussReduction(Rows(), &reducted);
    if(range==Rows())
    {
	y.Alloc(x.Rows(),x.Columns());
	for(i=0; i<x.Rows(); i++)
	{
	    for(j=0; j<x.Columns(); j++) { y(reducted[i],j)=A(i,j+Columns()); }
	}
    }
    if(reducted) { delete [] reducted; }
    return(range==Rows());
}


//--------------------------------------------------------------------
template <class Any>
BBool 
BMatrix<Any>::Inverse(BMatrix<Any>& x)

/*! Solves the linnear system
 *
 *		  X * A = Id
 *
 *	    and returns BTRUE if the system is regular
 */
//--------------------------------------------------------------------
{
    x.Alloc(Rows(),Rows());
    if(Rows()!=Columns())
    {
	return(0);
    }
    BInt i, j;
    BInt*	 reducted;
    BMatrix<Any> id(Rows(),Rows());
    for(i=0; i<Rows(); i++)
    { for(j=0; j<Rows(); j++) { id(i,j) = (i==j)?1.0:0.0; } }
    BMatrix<Any> A     = ConcatColumns((*this), id);
    BInt	       range = A.GaussReduction(Rows(), &reducted);
    if(range==Rows())
    {
	for(i=0; i<Rows(); i++)
	{
	    for(j=0; j<Rows(); j++) { x(i,j) = A(i,j+Rows()); }
	}
    }
    delete [] reducted;
    return(range==Rows());
}


//--------------------------------------------------------------------
template <class Any>
BInt 
BMatrix<Any>::LinCombSpace(BMatrix<Any>& equations)

/*! Constructs the reducted equations of vectorial space
 *
 *		   X A = 0
 *
 *	    and returns the dimension of this space.
 */
//--------------------------------------------------------------------
{
    BInt i, j;
    BMatrix<Any> id(Rows(),Rows());
    for(i=0; i<Rows(); i++)
    { for(j=0; j<Rows(); j++) { id(i,j) = (i==j)?1.0:0.0; } }
    BMatrix<Any> extended = ConcatColumns((*this), id);
    BInt*	       reducted;
    BInt	       range	= extended.GaussReduction(Columns(), &reducted);
    BInt	       dim	= Rows() - range;
    
//Trace(extended.Name());
//for(i=0; i<Rows(); i++) { Std(BText(" ")+reducted(i)+", "); }
    
    if(dim)
    {
	equations.Alloc(dim, Rows());
	BInt row=0;
	for(i=0; i<Rows(); i++)
	{
	    if(reducted[i]<0.0)
	    {
		for(j=0; j<Rows(); j++)
		{ equations(row,j) = extended(i,Columns()+j); }
		row++;
	    }
	}
    }
    delete [] reducted;
    return(dim);
    
}

//--------------------------------------------------------------------
template <class Any>
void 
BMatrix<Any>::NormalizeRows()
//--------------------------------------------------------------------
{
    BInt i,j;
    for(i=0;i<Rows();i++)
    {
	Any min = (*this)(i,0);
	Any max = (*this)(i,0);
	for(j=0;j<Rows();j++)
	{
	    Any x = (*this)(i,j);
	    if(x<min) { min = x; }
	    if(x>max) { max = x; }
	}
	Any dif = max-min;
	for(j=0;j<Rows();j++)
	{
	    (*this)(i,j) = ((*this)(i,j)-min)/dif;
	}
    }
}


//--------------------------------------------------------------------
template <class Any>
void 
BMatrix<Any>::NormalizeRows(BMatrix<Any>& m)
//--------------------------------------------------------------------
{
    m = *this;
    m.NormalizeRows();
}



//--------------------------------------------------------------------
template <class Any>
BMatrix<Any> 
BMatrix<Any>::PermutateRows(const BArray<BInt>& perm)
//--------------------------------------------------------------------
{
    BInt i, j;
    BMatrix<Any> P(Rows(),Columns());
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
BMatrix<Any> 
BMatrix<Any>::PermutateColumns(const BArray<BInt>& perm)
//--------------------------------------------------------------------
{
    BInt i, j;
    BMatrix<Any> P(Rows(),Columns());
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
BMatrix<Any>::IsKnown() const
{
  for(int i=0; i<Rows(); i++)
    for(int j=0; j<Columns(); j++)
      if(!::IsKnown((*this)(i,j))) return BFALSE;
  
  return BTRUE;
}

//--------------------------------------------------------------------
template <class Any>
Any BMatrix<Any>::Determinant() const
{
  const BMatrix<Any>& T = *this;
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
    Any c = T(k-1,0)*Any(pow(-1.0,k));
    r = rows_-1;
    BMatrix<Any> A(r,r);
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
