/* tol_matopr.hpp: Implementation for BMatrix class.
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
BMatA
Not (const BMatA& M)
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns(), m=M.Rows();
    BMatA X(m,n);
    for(i=0; i<m; i++)
    {
  for(j=0; j<n; j++)
  {
      X(i,j) = !M(i,j);
  }
    }
    return(X);
}


//--------------------------------------------------------------------
template <class Any>
BMatA 
And (const BMatA& A, const BMatA& B)
//--------------------------------------------------------------------
{
    BInt i,j, n=A.Columns(), m=A.Rows();
    BMatA X;
    if((B.Columns()!=n)||(B.Rows()!=m)) { return(X); }
    X.Alloc(m,n);
    for(i=0; i<m; i++)
    {
  for(j=0; j<n; j++)
  {
      X(i,j) = A(i,j) && B(i,j);
  }
    }
    return(X);
}


//--------------------------------------------------------------------
template <class Any>
BMatA 
Or (const BMatA& A, const BMatA& B)
//--------------------------------------------------------------------
{
    BInt i,j, n=A.Columns(), m=A.Rows();
    BMatA X;
    if((B.Columns()!=n)||(B.Rows()!=m)) { return(X); }
    X.Alloc(m,n);
    for(i=0; i<m; i++)
    {
  for(j=0; j<n; j++)
  {
      X(i,j) = A(i,j) || B(i,j);
  }
    }
    return(X);
}


//--------------------------------------------------------------------
template <class Any>
BMatA 
operator * (const BMatA& M, const BDiMA& D)
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns(), m=M.Rows();
    BMatA X;
    if(n!=D.Rows()) { return(X); }
    X.Alloc(m,n);
    for(i=0; i<m; i++)
    {
  for(j=0; j<n; j++)
  {
      X(i,j) = M(i,j)*D(j,j);
  }
    }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BSyMA 
operator * (const BSyMA& M, const BDiMA& D)
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns();
    BSyMA X;
    if(n!=D.Rows()) { return(X); }
    X.Alloc(n);

    for(i=0; i<n; i++)
    {
  for(j=0; j<i; j++)
  {
      X(i,j) = M(i,j)*D(j,j);
  }
    }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BLTMA 
operator * (const BLTMA& M, const BDiMA& D)
//--------------------------------------------------------------------
    
{
    BInt i,j, n=M.Columns();
    BLTMA X;
    if(n!=D.Rows()) { return(X); }
    X.Alloc(n);
    for(i=0; i<n; i++)
    {
  for(j=0; j<i; j++)
  {
      X(i,j) = M(i,j)*D(j,j);
  }
    }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BUTMA 
operator * (const BUTMA& M, const BDiMA& D)  
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns();
    BUTMA X;
    if(n!=D.Rows()) { return(X); }
    X.Alloc(n);
    for(i=0; i<n; i++)
    {
  for(j=i; j<n; j++)
  {
      X(i,j) = M(i,j)*D(j,j);
  }
    }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BMatA 
operator * (const BDiMA& D, const BMatA& M)
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns(), m=M.Rows();
    BMatA X;
    if(m!=D.Columns()) { return(X); }
    X.Alloc(m,n);
    for(i=0; i<m; i++)
    {
  for(j=0; j<n; j++)
  {
      X(i,j) = D(i,i)*M(i,j);
  }
    }
    return(X);
}


//--------------------------------------------------------------------
template <class Any>
BSyMA 
operator * (const BDiMA& D, const BSyMA& M)
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns(), m=M.Rows();
    BSyMA X;
    if(m!=D.Columns()) { return(X); }
    X.Alloc(n);
    for(i=0; i<n; i++)
    {
  for(j=0; j<i; j++)
  {
      X(i,j) = D(i,i)*M(i,j);
  }
    }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BLTMA 
operator * (const BDiMA& D, const BLTMA& M)
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns(), m=M.Rows();
    BLTMA X;
    if(m!=D.Columns()) { return(X); }
    X.Alloc(n);
    for(i=0; i<n; i++)
    {
  for(j=0; j<i; j++)
  {
      X(i,j) = D(i,i)*M(i,j);
  }
    }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BUTMA 
operator * (const BDiMA& D, const BUTMA& M)
//--------------------------------------------------------------------
{
    BInt i,j, n=M.Columns(), m=M.Rows();
    BUTMA X;
    if(m!=D.Columns()) { return(X); }
    X.Alloc(n);
    for(i=0; i<n; i++)
    {
  for(j=i; j<n; j++)
  {
      X(i,j) = D(i,i)*M(i,j);
  }
    }
    return(X);
}



//--------------------------------------------------------------------
template <class Any>
BSyMA 
operator + (const BSyMA& M, const BDiMA& D)
//--------------------------------------------------------------------
{
    BSyMA X;
    BInt i, n=M.Columns();
    if(n!=D.Columns()) { return(X); }
    X=M;
    for(i=0;i<n;i++) { X(i,i)+=D(i,i); }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BLTMA 
operator + (const BLTMA& M, const BDiMA& D)
//--------------------------------------------------------------------   
{
    BLTMA X;
    BInt i, n=M.Columns();
    if(n!=D.Columns()) { return(X); }
    X=M;
    for(i=0;i<n;i++) { X(i,i)+=D(i,i); }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BUTMA 
operator + (const BUTMA& M, const BDiMA& D)   
//--------------------------------------------------------------------
{
    BUTMA X;
    BInt i, n=M.Columns();
    if(n!=D.Columns()) { return(X); }
    X=M;
    for(i=0;i<n;i++) { X(i,i)+=D(i,i); }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BSyMA 
operator - (const BSyMA& M, const BDiMA& D)
//--------------------------------------------------------------------
{
    BSyMA X;
    BInt i, n=M.Columns();
    if(n!=D.Columns()) { return(X); }
    X=M;
    for(i=0;i<n;i++) { X(i,i)+=D(i,i); }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BLTMA 
operator - (const BLTMA& M, const BDiMA& D)
//--------------------------------------------------------------------
{
    BLTMA X;
    BInt i, n=M.Columns();
    if(n!=D.Columns()) { return(X); }
    X=M;
    for(i=0;i<n;i++) { X(i,i)+=D(i,i); }
    return(X);
}

//--------------------------------------------------------------------
template <class Any>
BUTMA 
operator - (const BUTMA& M, const BDiMA& D)
//--------------------------------------------------------------------
{
  BUTMA X;
  BInt i, n=M.Columns();
  if(n!=D.Columns()) { return(X); }
  X=M;
  for(i=0;i<n;i++) { X(i,i)+=D(i,i); }
  return(X);
}


//--------------------------------------------------------------------
template <class Any>
void 
DiagNormColumnsDecomp (const BMatA& A, BMatA& N, BDiMA& D)

/*! Performs the matrix decomposition
 *
 *      A = N · D
 *
 *       where D is a diagonal matrix and the columns of N are unitary
 *       vectors. The elements of D are the square roots of the sum
 *       of squares of each column of A.
 */
//--------------------------------------------------------------------
{
    D.PutColumnsNorm(A);
    N = A * D.P(DEpsilon());
}

//--------------------------------------------------------------------
template <class Any>
void 
DiagMaxAbsColumnsDecomp (const BMatA& A, BMatA& N, BDiMA& D)

/*! Performs the matrix decomposition
 *
 *      A = N · D
 *
 *       where D is a diagonal matrix and the columns of N are unitary
 *       vectors. The elements of D are the square roots of the sum
 *       of squares of each column of A.
 */
//--------------------------------------------------------------------
{
    BInt i,j, n=A.Columns(), m=A.Rows();
    D.PutColumnsMaxAbs(A);
    N = A * D.P(DEpsilon());
}

//--------------------------------------------------------------------
template <class Any>
void 
SymmDiagNormalDecomp (const BSyMA& A, BSyMA& N, BDiMA& D)
    
/*! Decomposes the symmetric and definite positive matrix A like
 *
 *      A = D · N · D
 *
 *       where D is a diagonal matrix and the diagonal of N are all equal
 *       to one. The elements of D are the square roots of the elements
 *       of the diagonal of A.
 */
//--------------------------------------------------------------------
{
    BInt i,j, n=A.Columns();
    N.Alloc(n);
    D.Alloc(n);
    for(i=0; i<n; i++)
    {
  D(i,i) = Sqrt(A(i,i));
  N(i,i) = 1;
  for(j=0; j<i; j++)
  {
      N(i,j) = A(i,j)/(D(i,i)*D(j,j));
  }
    }
}


//--------------------------------------------------------------------
template <class Any>
void 
ColumnNormPivots(const BMatA& A, BArray<BInt>& pivot)
//--------------------------------------------------------------------
{
    BInt i,j,m = A.Rows(), n = A.Columns();
    BBool ok = BTRUE;
    pivot.ReallocBuffer(n);
    BArray<BBool> used(m);
    BDiagMatrix<Any>D;
    D.PutColumnsNorm(A);
    D*=-1;
    BIndexed<Any>::Order(D.Data(),pivot);
}

//--------------------------------------------------------------------
template <class Any>
void 
ColumnMaxAbsNormPivots(const BMatA& A, BArray<BInt>& pivot)
//--------------------------------------------------------------------
{
    BInt i,j,m = A.Rows(), n = A.Columns();
    BBool ok = BTRUE;
    pivot.ReallocBuffer(n);
    BArray<BBool> used(m);
    BDiagMatrix<Any>D;
    D.PutColumnsMaxAbs(A);
    D*=-1;
    BIndexed<Any>::Order(D.Data(),pivot);
}

//--------------------------------------------------------------------
template <class Any>
void 
ColumnPivots(const BMatA& A, BArray<BInt>& pivot)
//--------------------------------------------------------------------
{
  BInt i,j,m = A.Rows(), n = A.Columns();
  BBool ok = BTRUE;
  pivot.ReallocBuffer(n);
  BArray< BIndexed<Any> >idx(n);
  for(i=0; i<n; i++)
  {
    idx(i).pos_ = i;
    idx(i).dat_ = -1-i;
    for(j=0; j<m; j++)
    {
      Any a = Abs(A(j,i));
      if(a>idx(i).dat_)
      {
        idx(i).dat_ = a;
      }
    }
    idx(i).dat_ *= -1;
  }
  BIndexed<Any>::Sort(idx);
  for(i=0; i<n; i++)
  {
    pivot(i) = idx(i).pos_;
  }
}

//--------------------------------------------------------------------
template <class Any>
void 
RowPivots(const BMatA& A, BArray<BInt>& pivot)
//--------------------------------------------------------------------
{
  BInt i,j,n = A.Rows(), m = A.Columns();
  pivot.ReallocBuffer(n);
  BArray< BIndexed<Any> >idx(n);
  for(i=0; i<n; i++)
  {
    idx(i).pos_ = i;
    idx(i).dat_ = -1-i;
    for(j=0; j<m; j++)
    {
      Any a = Abs(A(i,j));
      if(a>idx(i).dat_)
      {
        idx(i).dat_ = a;
      }
    }
    idx(i).dat_ *= -1;
  }
  BIndexed<Any>::Sort(idx);
  for(i=0; i<n; i++)
  {
    pivot(i) = idx(i).pos_;
  }
}


//--------------------------------------------------------------------
template <class Any>
void 
PivotByColumns(BMatA& A, const BArray<BInt>& pivot)
//--------------------------------------------------------------------
{
  BInt i,j,k,m = A.Rows(), n = A.Columns();
  if(pivot.Size()!=n)
  { 
    Warning(I2("<W>Wrong matrix sizes for ",
           "<W>Tamaño erróneo de matriz para ")+
           "PivotByColumns("+A.Rows() +"x"+ A.Columns()+","+pivot.Size()+")"+
           "</W>\n");
    A.Alloc(0,0);
    return; 
  }
  BMatA B = A;
  for(i=0; i<n; i++)
  {
    j = pivot(i);
    if(i==j)
    if((j<0)||(j>=n)) 
    {
      Warning(I2("<W>Out of range pivot index ",
             "<W>Índice de pivoteo fuera de rango ")+(j+1)+
          I2(" in ", " en ")+
             "PivotByColumns("+A.Rows() +"x"+ A.Columns()+","+pivot.Size()+")"+
             "</W>\n");
      A.Alloc(0,0);
      continue; 
    }
    for(k=0; k<m; k++)
    {
      A(k,i) = B(k,j);
    }
  }
}


//--------------------------------------------------------------------
template <class Any>
void 
PivotByRows(BMatA& A, const BArray<BInt>& pivot)
//--------------------------------------------------------------------
{
  BInt i,j,k,n = A.Rows(), m = A.Columns();
  if(pivot.Size()!=n) 
  { 
    Warning(I2("<W>Wrong matrix sizes for ",
           "<W>Tamaño erróneo de matriz para ")+
           "PivotByRows("+A.Rows() +"x"+ A.Columns()+","+pivot.Size()+")"+
           "</W>\n");
    A.Alloc(0,0);
    return; 
  }
  BMatA B = A;
  for(i=0; i<n; i++)
  {
    j = pivot(i);
    if(i==j) { continue; }
    if((j<0)||(j>=n)) 
    {
      Warning(I2("<W>Out of range pivot index ",
             "<W>Índice de pivoteo fuera de rango ")+(j+1)+
          I2(" in ", " en ")+
             "PivotByRows("+A.Rows() +"x"+ A.Columns()+","+pivot.Size()+")"+
             "</W>\n");
      A.Alloc(0,0);
      continue; 
    }
    for(k=0; k<m; k++)
    {
      A(i,k) = B(j,k);
    }
  }
}

