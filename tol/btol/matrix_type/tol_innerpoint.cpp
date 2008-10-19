/* tol_innerpoint.cpp: Finds an inner point of a non empty polytope.

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

#include <tol_binnerpoint.h>
#include <float.h>
#include <gsl/gsl_math.h>

#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdatgra.h>

//#define eps DBL_EPSILON
//#define iszero(x) (x<eps && x>-eps)
//#define ispos(x) (x>=eps)
//#define isneg(x) (x<=-eps)

#define ispos(x) (gsl_fcmp(x+1.0,1.0,DBL_EPSILON)>0)
#define isneg(x) (gsl_fcmp(x+1.0,1.0,DBL_EPSILON)<0)
#define iszero(x) (gsl_fcmp(x+1.0,1.0,DBL_EPSILON)==0)

BTraceInit("tol_innerpoint.cpp");

//-----------------------------------------------------------------------------
static void initgamma(double ** &U, int m, int n, bool * &V, bool ** &A0)
//-----------------------------------------------------------------------------
{
  U = new double*[n];
  for (int c=0; c<n; ++c)
    U[c] = new double[n];
  for (int c=0; c<n; ++c)
    for (int r=0; r<n; ++r)
      U[c][r] = c==r ? 1 : 0;
  V = new bool[n];
  for (int c=0; c<n; ++c)
    V[c] = true;
  A0 = new bool*[n];
  for (int c=0; c<n; ++c) {
    A0[c] = new bool[m];
    for (int r=0; r<m; ++r)
      A0[c][r] = false;
  }
}

//-----------------------------------------------------------------------------
static double * prod_vect_h(int h, const BMat & A, double* U[], int colsU)
//-----------------------------------------------------------------------------
{
  double * t = new double[colsU];

  for (int c=0; c<colsU; ++c) {
    t[c] = 0;
    for (int j=0; j<A.Columns(); ++j)
      t[c] += BReal(A(h,j).Value()) * U[c][j];
  }
  return t;
}

//-----------------------------------------------------------------------------
static int findpivot(double t[], int n, bool V[])
//-----------------------------------------------------------------------------
{
  int pivot = -1;

  for (int c=0; c<n; ++c)
    if (V[c] && !iszero(t[c])) {
      pivot = c;
      break;
    }
  return pivot;
}

//-----------------------------------------------------------------------------
static void topivot(int pivot, double t[], double* U[], int colsU, int n) 
//-----------------------------------------------------------------------------
{
  for (int r=0; r<n; ++r)
    U[pivot][r] /= -t[pivot];
  for (int c=0; c<colsU; ++c)
    if (c != pivot)
      for (int r=0; r<n; ++r)
        U[c][r] += t[c]*U[pivot][r];
}
      
//-----------------------------------------------------------------------------
static void makeI(bool * &Im, bool * &Ip, bool * &Iz, int colsU, int n, bool V[], double t[])
//-----------------------------------------------------------------------------
{
  Im = new bool[colsU];
  Ip = new bool[colsU];
  Iz = new bool[colsU];
  for (int c=0; c<colsU; ++c) {
    Im[c] = Ip[c] = Iz[c] = false;
    if (c>=n || !V[c])
      if (iszero(t[c]))
        Iz[c] = true;
      else
        if (ispos(t[c]))
          Ip[c] = true;
        else
          Im[c] = true;
  }
}

//-----------------------------------------------------------------------------
static bool isemptyI(bool I[], int colsU)
//-----------------------------------------------------------------------------
{
  int c;

  for (c=0; c<colsU; ++c)
    if (I[c])
      break;
  return (c == colsU);
}

//-----------------------------------------------------------------------------
static int card(bool I[], int colsU)
//-----------------------------------------------------------------------------
{
  int n = 0;

  for (int c=0; c<colsU; ++c)
    if (I[c])
      ++n;
  return n;
}

//-----------------------------------------------------------------------------
static void delfromU(int & colsU, bool Ip[], double * U[], bool * A0[], int n, bool * V)
//-----------------------------------------------------------------------------
{
  for (int c=0; c<colsU; ++c)
    if (Ip[c]) {
      delete[] U[c];
      U[c] = NULL;
      delete[] A0[c];
      A0[c] = NULL;
    }
  colsU -= card(Ip, colsU);
  for (int d=0, s=1; d<colsU; ++d, ++s)
    if (!U[d]) {
      while (!U[s])
        ++s;
      U[d] = U[s];
      U[s] = NULL;
      A0[d] = A0[s];
      A0[s] = NULL;
      if (s<n && V[s]) {
        V[d] = true;
        V[s] = false;
      }
    }
}

//-----------------------------------------------------------------------------
static void makeW(double ** &W, bool ** &A0w, int m, int h, bool Im[], bool Ip[],
                  int colsU, int n, double * U[], double t[], bool * A0[])
//-----------------------------------------------------------------------------
{
  int sizeW = card(Im, colsU) * card(Ip, colsU);
  W = new double * [sizeW];
  A0w = new bool * [sizeW];
  int c=0;
  for (int i=0; i<colsU; ++i)
    if (Im[i])
      for (int j=0; j<colsU; ++j)
        if (Ip[j]) {
          W[c] = new double[n];
          A0w[c] = new bool[m];
          for (int r=0; r<n; ++r)
            W[c][r] = t[j]*U[i][r] - t[i]*U[j][r];
          for (int r=0; r<m; ++r)
            A0w[c][r] = A0[i][r] && A0[j][r];
          A0w[c][h] = true;
          c++;
        }
}

//-----------------------------------------------------------------------------
static bool inA0(int idxsrc, int idxdst, bool * A0src[], bool * A0dst[], int m)
//-----------------------------------------------------------------------------
{
  int i;

  for (i=0; i<m; ++i)
    if (A0src[idxsrc][i] && !A0dst[idxdst][i])
      break;
  return (i == m);
}

//-----------------------------------------------------------------------------
static void addtoU(double * W[], int sizeW, double ** &U, int & colsU, int n, bool * A0w[],
				   bool Iz[], bool ** &A0, bool Ip[], int m, bool * V, int & sizeU)
//-----------------------------------------------------------------------------
{
  bool * wtoadd = new bool[sizeW];

  for (int c=0; c<sizeW; ++c) {
    wtoadd[c] = false;
    int r;
    for (r=0; r<n; ++r)
      if (!iszero(W[c][r]))
        break;
    if (r == n)
      continue;
    int i;
    for (i=0; i<sizeW; ++i)
      if (c != i)
        if (inA0(c, i, A0w, A0w, m))
          break;
    if (i != sizeW)
      continue;
    int j;
    for (j=0; j<colsU; ++j)
      if (Iz[j])
        if (inA0(c, j, A0w, A0, m))
          break;
    if (j != colsU)
      continue;
    wtoadd[c] = true;
  }
  delfromU(colsU, Ip, U, A0, n, V);
  if (sizeU < (colsU + card(wtoadd, sizeW))) {
    sizeU = colsU + card(wtoadd, sizeW);
    double ** tU = new double* [sizeU];
    bool ** tA0 = new bool* [sizeU];
    for (int c=0; c<colsU; ++c) {
      tU[c] = U[c];
      tA0[c] = A0[c];
    }
    delete[] U;
    delete[] A0;
    U = tU;
    A0 = tA0;
  }
  for (int c=0; c<sizeW; ++c)
    if (wtoadd[c]) {
      U[colsU] = W[c];
      A0[colsU++] = A0w[c];
      W[c] = NULL;
      A0w[c] = NULL;
    }
  delete[] wtoadd;
}

//-----------------------------------------------------------------------------
static BMat Gamma(const BMat & A)
//-----------------------------------------------------------------------------
{
// Entrada
//  A matriz mxn, A = {a1,...,am}T cjto de generadores que definen un cono en Rn

// Salida
//  Los generadores del cono dual C = {x|x de R^n, Ax<=0} como columnas de una matriz,
//   la fila n+1 (adicional) tendrá los valores 1 o 0 si la columna es parte del
//   espacio vectorial o del cono respectivamente

  int m = A.Rows();
  int n = A.Columns();
  int sizeU = n;
  double ** U = NULL;
  int colsU = n;
  bool * V = NULL;
  bool ** A0 = NULL;
  initgamma(U, m, n, V, A0);

  for (int h=0; h<m; ++h) {

// Etapa 1
    double * t;
    t = prod_vect_h(h, A, U, colsU);

// Etapa 2
    int pivot;
    pivot = findpivot(t, n, V);

// Etapa 3
    if (pivot != -1) {

// Etapa 4
      topivot(pivot, t, U, colsU, n);
      for (int c=0; c<colsU; ++c)
        if (c != pivot)
          A0[c][h] = true;
      V[pivot] = false;
    }
    else {

// Etapa 5
      bool * Im = NULL, * Ip = NULL, * Iz = NULL;
      makeI(Im, Ip, Iz, colsU, n, V, t);
      for (int c=0; c<colsU; ++c)
		 // OJO, en el articulo aparece añadir h para j pert I0
		 // pero esto deja fuera los generadores del espacio
		 // vectorial que son ortogonales a este vector (h)
        if (/*Iz[c]*/iszero(t[c]))
          A0[c][h] = true;
      if (!isemptyI(Ip, colsU)) {
        if (isemptyI(Im, colsU))
          delfromU(colsU, Ip, U, A0, n, V);
        else {
          double ** W = NULL;
          bool ** A0w = NULL;
          makeW(W, A0w, m, h, Im, Ip, colsU, n, U, t, A0);
          int sizeW = card(Im, colsU) * card(Ip, colsU);
          addtoU(W, sizeW, U, colsU, n, A0w, Iz, A0, Ip, m, V, sizeU);
          for (int c=0; c<sizeW; ++c) {
            if (W[c]) delete[] W[c];
            if (A0w[c]) delete[] A0w[c];
          }
          delete[] W;
          delete[] A0w;
        }
      }
      delete[] Ip;
      delete[] Im;
      delete[] Iz;
    }
    delete[] t;

// Etapa 6
  }
  BMat res(n+1, colsU);

  for (int c=0; c<colsU; ++c)
    for (int r=0; r<n; ++r)
      res(r, c) = U[c][r];
  for (int c=0; c<colsU; ++c)
    res(n, c) = (c<n && V[c]);
  for (int c=0; c<colsU; ++c) {
    delete[] U[c];
    delete[] A0[c];
  }
  delete[] U;
  delete[] A0;
  delete[] V;
  return res;
}

//-----------------------------------------------------------------------------
static void Normalize(BMat & X)
//-----------------------------------------------------------------------------
{
  int m = X.Rows() - 1;
  int n = X.Columns();

  for (int j=0; j<n; ++j)
    if (!iszero(X(m-1, j).Value()))
      for (int i=0; i<m; ++i)
        X(i, j) /= X(m-1, j);
  for (int j=0; j<n; ++j)
    if (iszero(X(m, j).Value()))
      X(m, j) = iszero(X(m-1, j).Value()) ? 2.0 : 3.0;
}

//-----------------------------------------------------------------------------
static BMat Solutions(const BMat & B, const BMat & c)
//-----------------------------------------------------------------------------
{
// Entrada
//  B Matriz mxn, c vector mx1

// Salida
//  Los generadores del poliedro S = {x|x de R^n, Bx<=c}, como columnas de una matriz,
//   la fila n+1 (adicional) tendrá los valores 1,2 o 3 si la columna es parte del
//   espacio vectorial, del cono o del politopo respectivamente que forman el poliedro

  int m = B.Rows();
  int n = B.Columns();
  BMat A(m+1, n+1);

  for (int i=0; i<m; ++i)
    for (int j=0; j<n; ++j)
      A(i, j) = B(i, j);
  for (int j=0; j<n; ++j)
    A(m, j) = 0.0;
  for (int i=0; i<m; ++i)
    A(i, n) = -c(i, 0);
  A(m, n) = -1.0;

  BMat G = Gamma(A);
  Normalize(G);
  return G;
}

//-----------------------------------------------------------------------------
static BMat LinearComb(const BMat & AS)
//-----------------------------------------------------------------------------
{
  int m = AS.Rows();
  int n = AS.Columns();
  BMat x(m, 1);

  for (int i=0; i<m; ++i)
    x(i, 0) = 0.0;
  for (int j=0; j<n; ++j)
    for (int i=0; i<m; ++i)
      x(i, 0) += AS(i, j);
  for (int i=0; i<m; ++i)
    x(i, 0) /= n;
  return x;
}

//-----------------------------------------------------------------------------
static BMat AlgebraicS(const BMat & G, int s)
//-----------------------------------------------------------------------------
{
  // s -> estructura algebraica: 1- espacio vectorial, 2- cono, 3- politopo
  int m = G.Rows();
  int n = G.Columns();
  int c = 0;

  for (int j=0; j<n; ++j)
    if (iszero(G(m-1, j).Value()-s))
	  ++c;
  BMat AS;
  if (c) {
    AS.Alloc(m-2, c);
    for (int j=0, c=0; j<n; ++j)
      if (iszero(G(m-1, j).Value()-s)) {
        for (int i=0; i<m-2; ++i)
          AS(i, c) = G(i, j);
	    ++c;
      }
  }
  return AS;
}

//-----------------------------------------------------------------------------
  BMat InnerPoint(const BMat & B, const BMat & c, BMat & V, BMat & W, BMat & Q)
//
//Purpose: It is used to obtain a vector X of R^n, such as B*X < c 
//
//Arguments: 
// B ---> matrix mxn 
// c ---> independent terms vector mx1
//
//Method:
//It finds the generators of the polyhedron S of all the feasible solutions
//of the system B*x <= c, where S comes expressed from the form: S = V + W + Q
//Any solution can be obtained combining linearly the generators
//the coefficients of V can be any real value, them of W any value not negative
//and them of Q any not negative value and that its sum is equal to 1
//
//Return:
// X - InnerPoint, A point especially that is interior to the polyhedron 
//     Is the returned value
//These other matrices are passed by reference in the first interface and
//are hiden in second one:
// V - Vectorial Space. 
// W - Cone. 
// Q - Polytopo. 
//
//-----------------------------------------------------------------------------
{
  BMat G = Solutions(B, c);
  int m = G.Rows();
  BMat X(m-2, 1);

  V = AlgebraicS(G, 1);
  W = AlgebraicS(G, 2);
  Q = AlgebraicS(G, 3);
  for (int i=0; i<m-2; ++i)
    X(i, 0) = 0.0;
  if (V.Columns())
    X += LinearComb(V);
  if (W.Columns())
    X += LinearComb(W);
  if (Q.Columns())
    X += LinearComb(Q);
  return X;
}

//-----------------------------------------------------------------------------
  BMat InnerPoint(const BMat & B, const BMat & c)
//See previous function
//-----------------------------------------------------------------------------
{
  BMat V, W, Q;
  return(InnerPoint(B, c, V, W, Q));
}

//-----------------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetInnerPoint);
DefExtOpr(1, BSetInnerPoint, "InnerPoint", 2, 2, "Matrix Matrix",
  "(Matrix B, Matrix c)",
  "It is used to obtain a vector x of R^n, such as B*x < c \n"
  "Arguments: \n"
	" B ---> matrix mxn \n"
	" c ---> independent terms vector mx1\n\n"
  "It finds the generators of the polyhedron S of all the feasible solutions\n"
  " of the system B*x <= c, where S comes expressed from the form: S = V + W + Q\n"
  "Any solution can be obtained combining linearly the generators,\n"
  " the coefficients of V can be any real value, them of W any value not negative\n"
  " and them of Q any not negative value and that its sum is equal to 1\n\n"
  "Returns a set whose elements are:\n"
  "  Matrix X - InnerPoint, A point especially that is interior to the polyhedron\n"
  "  Matrix V - Vectorial Space\n"
  "  Matrix W - Cone\n"
  "  Matrix Q - Polytopo\n",
  BOperClassify::Statistic_);
//-----------------------------------------------------------------------------
void BSetInnerPoint::CalcContens()
//-----------------------------------------------------------------------------
{
  BMat & B = Mat(Arg(1));
  BMat & c = Mat(Arg(2));
  BMat X, V, W, Q;
  BUserMat * UserX, * UserV, * UserW, * UserQ;

  if (B.Rows()<=0 || B.Columns()<=0) {
    Error("Invalid B matrix: number of rows and columns must be greater than zero");
    return;
  }
  if (c.Columns() != 1) {
    Error("Invalid c vector: number of columns must be one");
    return;
  }
  if (c.Rows() != B.Rows()) {
    Error("Invalid c vector: number of rows must be equal than rows of B matrix");
    return;
  }
  X = InnerPoint(B, c, V, W, Q);
  UserX = new BContensMat("", X, "The found point matching given inequations");
  UserV = new BContensMat("", V, "The base of the Vectorial Space coordinates of the inner point");
  UserW = new BContensMat("", W, "The base of the Cone coordinates of the inner point");
  UserQ = new BContensMat("", Q, "The base of the Polytope coordinates of the inner point");
  UserX->PutName("InnerPoint");
  UserV->PutName("VectorialSpace");
  UserW->PutName("Cone");
  UserQ->PutName("Polytope");
  contens_.RobElement(Cons(UserX,
                        Cons(UserV,
                          Cons(UserW,
                            Cons(UserQ,
                              NIL)))));
}
