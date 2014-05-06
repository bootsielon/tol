/* polmatgra.cpp: Polinomial Matrix Grammar's functions.
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bpolmatgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bar.h>


//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("polmatgra.cpp");
template<>
BGrammar* BGraContensBase< BPolMat >::ownGrammar_ = NIL;


DefineContensCommonOperators(BPolMat, "PolMatrix");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BPolMat>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}



//--------------------------------------------------------------------
template<>
void BGraContensBase<BPolMat>::InitInstances()

/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
//--------------------------------------------------------------------
{
    BTraceInit("BGraContens<BPolMat>::InitInstances");
    BPolMat mat;
    
    BSystemPolMat* unknown_ = new BSystemPolMat
    (
      "UnknownPolMatrix",
      mat,
      I2("The unknown polynomial matrix.", "La matriz polinomial desconocida.")
    );
    OwnGrammar()->PutDefect(unknown_);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BPolMat>::FindConstant (const BText&)

/*! Returns a valid constant BPolMat for name.
 */
//--------------------------------------------------------------------
{ 
    return(NIL); 
}


//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BPolMat>::Casting(BSyntaxObject* obj)

/*! Returns a valid real for obj.
 */
//--------------------------------------------------------------------
{
    if(!obj) { return(NIL); }
    if(obj->Grammar()==OwnGrammar()) { return(obj); }
    return(NIL);
}


//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
int Degree(const BPolMat& P)
//--------------------------------------------------------------------
{
  int k, s=P.Data().Size();
  const BPol* p = P.Data().Buffer();
  int deg,  maxDeg = 0;
  for(k=0; k<s; k++, p++)
  {
    deg = p->Degree();
    if(deg>maxDeg) { maxDeg = deg; }
  }
  return(maxDeg);
}

//--------------------------------------------------------------------
bool IsStationary(const BPolMat& P)
//--------------------------------------------------------------------
{
  BPol det = P.Determinant();
  return(IsStationary(det));
}

//--------------------------------------------------------------------
bool ApplyMatPol(const BPolMat& P, const BMat& X, BMat& Y)
//--------------------------------------------------------------------
{
  int i,j,t,h,r=P.Rows(), c = P.Columns(), T = X.Rows();
  if(X.Columns()!=c) { return(false); }
  BArray< BArray<BDat> > x(c);
  for(j=0; j<c; j++)
  {
    x(j) = X.SubCol(j).Data();
  }
  int d = Degree(P);
  int Td = T-d;
  Y.Alloc(Td,r);
  Y.SetAllValuesTo(0.0);
  
  for(i=0; i<r; i++)
  {
    for(j=0; j<c; j++)
    {
      BArray<BDat> z;
      const BArray<BDat>& xj = x(j);      
      ApplyPolyn(P(i,j),xj,z);
      for(t=Td-1,h=z.Size()-1; t>=0; t--, h--)
      {
        Y(t,i) += z(h);
      }
    }
  }
  return(true);
}

//--------------------------------------------------------------------
bool BuildCompanionMatrix(const BPolMat& P, BVMat& Y)
//--------------------------------------------------------------------
{
  int i, j, k, d;
  int D = Degree(P);
  int r = P.Rows();
  int c = P.Columns();
  if(r!=c) { return(false); }
  if(D<1) { return(false); }

  BVMat Y1,Y21,Y22;
  Y1.BlasRDense(r,r*D);
  for(k=0,d=1; d<=D; d++)
  {
    for(j=0; j<r; j++, k++)
    {
      for(i=0; i<r; i++)
      {
        Y1.PutCell(i,k,-P(i,j).Coef(d).Value());
      }
    }
  }
  if(D==1) { Y = Y1; }
  else
  {
    Y21.Eye(r*(D-1));
    Y22.Zeros(r*(D-1),r);
    Y = Y1 << (Y21 | Y22);
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatPolMatDegree);
DefExtOpr(1, BDatPolMatDegree, "PolMatDegree", 1, 1, "PolMatrix",
  "(PolMatrix p)",
  I2("Returns the degree of a polynomial matrix",
     "Devuelve el grado de una matriz polinomial."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BDatPolMatDegree::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  contens_ = Degree(P);
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatPolMatRows);
DefExtOpr(1, BDatPolMatRows, "PolMatRows", 1, 1, "PolMatrix",
  "(PolMatrix p)",
  I2("Returns the number of rows of a polynomial matrix",
     "Devuelve el número de filas de una matriz polinomial."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BDatPolMatRows::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  contens_ = P.Rows();
}


//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatPolMatColumns);
DefExtOpr(1, BDatPolMatColumns, "PolMatColumns", 1, 1, "PolMatrix",
  "(PolMatrix p)",
  I2("Returns the number of columns of a polynomial matrix",
     "Devuelve el número de columnas de una matriz polinomial."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BDatPolMatColumns::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  contens_ = P.Columns();
}

//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolDeterminant);
DefExtOpr(1, BPolDeterminant, "Determinant", 1, 1, "PolMatrix",
  "(PolMatrix p)",
  I2("Returns the determinant of a polynomial matrix",
     "Devuelve el determinante de una matriz polinomial."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolDeterminant::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  contens_ = P.Determinant();
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatPolMatIsStationary);
DefExtOpr(1, BDatPolMatIsStationary, "PolMatIsStationary", 1, 1, "PolMatrix",
  "(PolMatrix p)",
  I2("Returns true if the polynomial matrix is stationary",
     "Devuelve cierto si la matriz polinomial es estacionaria."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BDatPolMatIsStationary::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  contens_ = IsStationary(P);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatEvalPolMatrix);
DefExtOpr(1, BMatEvalPolMatrix, "EvalPolMatrix", 2, 2, "PolMatrix Real",
  "(PolMatrix p, Real x)",
  I2("Evaluates a polynomial matrix in a real value.",
     "Evalua una matriz polinomial en un valor real."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BMatEvalPolMatrix::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  BDat &x   = Dat(Arg(2));
  BInt k, r, c, s;
  r = P.Rows();
  c = P.Columns();
  s = P.Data().Size();
  contens_.Alloc(r,c);
  const BPol* p = P.Data().Buffer();
  BDat* y = contens_.GetData().GetBuffer();
  for(k=0; k<s; k++, y++, p++)
  {
    *y = p->Eval(x);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatApplyPolMatrix);
DefExtOpr(1, BMatApplyPolMatrix, ":", 2, 2, "PolMatrix Matrix",
  "(PolMatrix p, Matrix x)",
  I2("Aplies the multivariant backward operator of degree d corresponding to "
     "a polynomial rxc matrix backward operator of degree d "
     "to a Txc real matrix building a (T-d)xr matrix.",
     "Aplica el operador de retardo multivariante de grado d correspondiente a "
     "una matriz polinomial rxc a una matriz real Txc para construir "
     "una matriz real (T-d)xr."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BMatApplyPolMatrix::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  BMat &x   = Mat(Arg(2));
  ApplyMatPol(P,x,contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatCompanionMatrix);
DefExtOpr(1, BVMatCompanionMatrix, "CompanionMatrix", 1, 1, "PolMatrix",
  "(PolMatrix p)",
  I2("Reurns the companion matrix of a VAR process.",
     "Devuelve la matriz de acompañamiento de un proceso VAR."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BVMatCompanionMatrix::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  BuildCompanionMatrix(P,contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatIdentity);
DefIntOpr(1, BPolMatIdentity, " + ", 1, 1,
  "(PolMatrix mat)",
  I2("Returns the same polynomial matrix.",
     "Devuelve la misma matriz polinomial."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatIdentity::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = PolMat(Arg(1));
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatMinus);
DefIntOpr(1, BPolMatMinus, " - ", 1, 1,
  "(PolMatrix mat)",
  I2("Returns a polynomial matrix width the opposite sign.",
     "Devuelve la matriz polinomial con el signo contrario."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatMinus::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = PolMat(Arg(1))*(-1.0);
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatT);
DefIntOpr(1, BPolMatT, "Tra", 1, 1,
  "(PolMatrix mat)",
  I2("Returns the transposed polynomial matrix, that is to say, "
     "the result of changing rows by columns.",
     "Devuelve la matriz polinomial traspuesta, es decir, el resultado de cambiar "
     "filas por columnas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatT::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = PolMat(Arg(1)).T(); 
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatReverse);
DefIntOpr(1, BPolMatReverse, "Reverse", 1, 1,
  "(PolMatrix mat)",
  I2("Returns the polynomial matrix of rows and columns with inverse order.",
     "Devuelve la matriz polinomial con las filas y columnas en orden inverso."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatReverse::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = PolMat(Arg(1));
  contens_.Reverse();
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSum2);
DefExtOpr(1, BPolMatSum2, "+", 2, 2, "PolMatrix {PolMatrix|Real}",
  "a + b {PolMatrix a, {PolMatrix|Real} b}",
  I2("Returns the sum of two polynomial matrices with the same dimensions."
     "Second one may be also a real number",
     "Devuelve la suma de dos matrices polinomiales con las mismas dimensiones."
     "El segundo argumento puede ser tambien un numero Real"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSum2::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraPolMat()) 
  { 
    contens_ = PolMat(Arg(1)) + PolMat(Arg(2)); 
  }
  else if(gra==GraReal()) 
  { 
    contens_ = PolMat(Arg(1)) + Dat(Arg(2)); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for PolMatrix +",
             "no es un tipo valido para PolMatrix +"));
    contens_ = BPolMat::Unknown();
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatDifference);
DefExtOpr(1, BPolMatDifference, "-", 2, 2,"PolMatrix {PolMatrix|Real}",
  "a - b {PolMatrix a, {PolMatrix|Real} b}",
  I2("Returns the subtraction of two polynomial matrices with the same dimensions."
     "Second one may be also a real number",
     "Devuelve la resta de dos matrices polinomiales con las mismas dimensiones."
     "El segundo argumento puede ser tambien un numero Real"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatDifference::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraPolMat()) 
  { 
    contens_ = PolMat(Arg(1)) - PolMat(Arg(2)); 
  }
  else if(gra==GraReal()) 
  { 
    contens_ = PolMat(Arg(1)) - Real(Arg(2)); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for PolMatrix -",
             "no es un tipo valido para PolMatrix -"));
    contens_ = BPolMat::Unknown();
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatProduct2);
DefExtOpr(1, BPolMatProduct2, "*", 2, 2,"{PolMatrix|Matrix} {PolMatrix|Real|Polyn|Matrix}",
  "a * b {{PolMatrix|Matrix} a, {PolMatrix|Real|Polyn|Matrix} b}",
  I2("Returns the product of two polynomial polynomial matrices such that the number of columns "
     "of the first polynomial matrix will be equal to the number of rows of the second."
     "Second one may be also a real number, a polynomial or a real "
     "matrix with same dimensions.",
     "Devuelve el producto de dos matrices polinomiales tales que el numero de "
     "columnas de la primera matriz polinomial sea igual al numero de filas "
     "de la segunda. "
     "El segundo argumento puede ser tambien un numero Real, un polinomio "
     "o una matriz polinomial real con las mismas dimensiones"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatProduct2::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* g1 = Arg(1)->Grammar();
  BGrammar* g2 = Arg(2)->Grammar();
  BPolMat P;
  int r,c,s,k;
  if(g1==GraPolMat())
  {
    P = PolMat(Arg(1));
    r = P.Rows();
    c = P.Columns();
    s = P.Data().Size();
  }
  else if(g1==GraMatrix())
  {
    BMat& a = Mat(Arg(1));
    const BDat* x = a.Data().Buffer();
    r = a.Rows();
    c = a.Columns();
    s = a.Data().Size();
    P.Alloc(r,c); 
    BPol *z = P.GetData().GetBuffer(); 
    for(k=0; k<s; k++, x++, z++)
    {
      *z = BPol(*x);
    }    
  }
  if(g2==GraPolMat()) 
  { 
    MatMult(P,PolMat(Arg(2)),contens_); 
  }
  else if(g2==GraReal()) 
  { 
    contens_ = P * Real(Arg(2)); 
  }
  else if(g2==GraPolyn()) 
  { 
    contens_ = P;
    contens_ *= Pol(Arg(2)); 
  }
  else if(g2==GraMatrix()) 
  { 
    BMat& a = Mat(Arg(2));
    const BDat* x = a.Data().Buffer();
    const BPol* p = P.Data().Buffer(); 
    if((r!=a.Rows())||(c!=a.Columns()))
    {
      Error("[PolMatrix Matrix product] Invalid dimensions of polynomial matrices");
      return;
    }
   
    contens_.Alloc(r,c);
    BPol *z = contens_.GetData().GetBuffer(); 
    for(k=0; k<s; k++, x++, z++, p++)
    {
      *z = *p * *x;
    }
  }
  else
  {
    Error(g2->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for PolMatrix *",
             "no es un tipo valido para PolMatrix *"));
    contens_ = BPolMat::Unknown();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatMtMSqr);
DefIntOpr(1, BPolMatMtMSqr, "MtMSqr", 1, 1,
  "(PolMatrix mat)",
  I2("Returns the product of the transpose of a polynomial matrix by itself.",
     "Devuelve el producto de una matriz polinomial por si misma"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatMtMSqr::CalcContens()
//--------------------------------------------------------------------
{
  const BPolMat& M =PolMat(Arg(1));
  MatMult(M.T(),M,contens_);
}


//--------------------------------------------------------------------
static BText WeightProdDescription()
//--------------------------------------------------------------------
{
  return 
  I2("Returns the cell to cell product of matricess mat1[i,j]*mat2[i,j]."
     "Both m1 and m2 must have same dimensions." 
     "Next both expressions are identical and valids in TOL mat1$*mat2==WeightProd(mat1,mat2)",
     "Devuelve la matriz polinomial de productos celda a celda mat1[i,j]*mat2[i,j]."
     "Ambas matrices polinomiales deben tener las mismas dimensiones."
     "Las siguientes dos expresiones son identicas y validas en TOL mat1$*at2==WeightProd(mat1,mat2)");
};


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatWeightProd);
DefExtOpr(1, BPolMatWeightProd, "WeightProd", 2, 2, "PolMatrix PolMatrix",
  "(PolMatrix mat1, PolMatrix mat2)",
  WeightProdDescription(),
    BOperClassify::MatrixAlgebra_);
DefExtOpr(2, BPolMatWeightProd, "$*", 2, 2, "PolMatrix PolMatrix",
  "mat1 $* mat2 {PolMatrix mat1, PolMatrix mat2}",
  WeightProdDescription(),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatWeightProd::CalcContens()
//--------------------------------------------------------------------
{
  const BPolMat& M =PolMat(Arg(1));
  WeightProd(M.T(),M,contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatPowReal);
DefExtOpr(1, BPolMatPowReal, "RPow", 2, 2, "PolMatrix Real",
  "(PolMatrix mat, Real x)",
  I2("Returns the power of each element of a polynomial matrix to a integer number.",
    "Devuelve la potencia de cada elemnto de una matriz polinomial a un numero entero."),
    BOperClassify::MatrixAlgebra_);
DefExtOpr(2, BPolMatPowReal, "^", 2, 2, "PolMatrix Real",
  "mat ^ x {PolMatrix mat, Real x}",
  I2("Returns the power of each element of a polynomial matrix to a integer number.",
    "Devuelve la potencia de cada elemnto de una matriz polinomial a un numero entero."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatPowReal::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& m = PolMat(Arg(1));
  int  d = (int)Real(Arg(2));
  contens_ = m;
  for(BInt i=0; i<m.Data().Size(); i++)
  {
    BPol& x = contens_.Data()(i);
    x = x^d;
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatProductReal);
DefExtOpr(1, BPolMatProductReal, "RProd", 2, 2, "PolMatrix Real",
  "(PolMatrix mat, Real x)",
  I2("Returns the product of each element of a polynomial matrix by a real number.",
    "Devuelve el producto de cada elemento de una matriz polinomial por un numero real."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatProductReal::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat&   m = PolMat(Arg(1));
  double  d = Real(Arg(2));
  contens_ = m * d;
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSumReal);
DefExtOpr(1, BPolMatSumReal, "RSum", 2, 2, "PolMatrix Real",
  "(PolMatrix mat, Real x)",
  I2("Returns the summ of each element of a polynomial matrix and a real number.",
     "Devuelve la suma de cada elemnto de una matriz polinomial y un numero real."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSumReal::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& m = PolMat(Arg(1));
  double d = Real(Arg(2));
  contens_ = m+d;
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatProductPol);
DefExtOpr(1, BPolMatProductPol, "PolProd", 2, 2, "PolMatrix Polyn",
  "(PolMatrix mat, Polyn p)",
  I2("Returns the product of each element of a polynomial matrix by a polynomial.",
    "Devuelve el producto de cada elemento de una matriz polinomial por un polinomio."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatProductPol::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat&   m = PolMat(Arg(1));
  BPol&  p = Pol(Arg(2));
  contens_ = m * p;
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSumPol);
DefExtOpr(1, BPolMatSumPol, "PolSum", 2, 2, "PolMatrix Polyn",
  "(PolMatrix mat, Polyn p)",
  I2("Returns the summ of each element of a polynomial matrix and a polynomial.",
     "Devuelve la suma de cada elemento de una matriz polinomial y un polinomio."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSumPol::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& m = PolMat(Arg(1));
  BPol&  p = Pol(Arg(2));
  contens_ = m+p;
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSetSum);
DefExtOpr(1, BPolMatSetSum, "SetSum", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the global summatory of all polynomial matrices of a set.",
     "Devuelve el sumatorio de todas las matrices polinomiales de un conjunto."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSetSum::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat & C = contens_;
  BSet& set = Set(Arg(1));
  if(!set.Card()) { return; }
  bool init = false;
  for(BInt n=1; n<=set.Card(); n++)
  {
    if(set[n]->Grammar()==GraPolMat())
    {
      if(!init) { C  = PolMat(set[n]); init = true; }
      else      { C += PolMat(set[n]); }
      if(!contens_.Rows() || !contens_.Columns()) 
      {
        return;
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSetMat2PolMat);
DefExtOpr(1, BPolMatSetMat2PolMat, "SetMat2PolMat", 1, 1, "Set",
  "(Set A)",
  "Given a set of real matrices A, returns the polynomial matrix "
  "A[1]+A[2]*B+...+A[n]*B^(n-1)",
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSetMat2PolMat::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat & C = contens_;
  BSet& set = Set(Arg(1));
  if(!set.Card()) { return; }
  bool init = false; 
  int k, s, r, c;
  const BDat* x;
  BMonome<BDat> m;
  for(BInt n=1; n<=set.Card(); n++)
  {
    if(set[n]->Grammar()!=GraMatrix())
    {
      Error("[SetMat2PolMat] Only real matrices are allowed in argument A");
      return;
    }
    BMat& a = Mat(set[n]);
    x = a.Data().Buffer();
    BPol* z, *y;
    if(n==1)
    { 
      r = a.Rows();
      c = a.Columns();
      s = a.Data().Size();
      C.Alloc(r,c);
      z = y = C.GetData().GetBuffer();
      for(k=0; k<s; k++, x++, z++)
      {
        m.PutCoef(*x);
        m.PutDegree(0);
        *z = m;
      }
    }
    else      
    { 
      if((r != a.Rows())||(c!=a.Columns()))
      {
        Error("[SetMat2PolMat] Invalid dimensions of matrices");
        return;
      }
      z=y;
      for(k=0; k<s; k++, x++, z++)
      {
        m.PutCoef(*x);
        m.PutDegree(n-1);
        *z += m;
      }
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetPolMat2SetMat);
DefExtOpr(1, BSetPolMat2SetMat, "PolMat2SetMat", 1, 1, "PolMatrix",
"(PolMatrix P)",
I2("Returns the set of matricial coefficients of a polynomial matrix",
   "Devuelve el conjunto de matrices coeficiente de una matriz polynomial"),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BSetPolMat2SetMat::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  int r=P.Rows();
  int c=P.Columns();
  int s=P.Data().Size();
  int k,d,deg=0;
  const BPol* p = P.Data().Buffer();
  for(k=0; k<s; k++, p++)
  {
    deg = max(deg,p->Degree());
  }
  contens_.PrepareStore(deg+1);
  for(d=0; d<=deg; d++)
  {
  	BUserMat* uMat = new BContensMat("",BMat(r,c),"");
    uMat->PutName(BText("A")+d);
    contens_.AddElement(uMat);
    BMat& A = uMat->Contens();
    p = P.Data().Buffer();
    BDat* a = A.GetData().GetBuffer();
    for(k=0; k<s; k++, p++, a++)
    {
      *a = p->Coef(d);
    }    
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatTabPol2PolMat);
DefExtOpr(1, BPolMatTabPol2PolMat, "TabPol2PolMat", 1, 1, "Set",
  "(Set A)",
  "Given a table-set of polynomials, returns the polynomial matrix "
  "A[i][j]",
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatTabPol2PolMat::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat & C = contens_;
  BSet& set = Set(Arg(1));
  if(!set.Card()) { return; }
  bool init = false; 
  int i, j, r=set.Card(), c;
  for(i=1; i<=r; i++)
  {
    if(set[i]->Grammar()!=GraSet())
    {
      Error("[TabPol2PolMat] Only set of sets of polynomials are allowed in argument A");
      return;
    }
    BSet& row = Set(set[i]);
    if(i==1) 
    {
      c = row.Card(); 
      contens_.Alloc(r,c);
    }
    for(j=1; j<=c; j++)
    {
      if(row[j]->Grammar()!=GraPolyn())
      {
        Error("[TabPol2PolMat] Only set of sets of polynomials are allowed in argument A");
        return;
      }
      contens_(i-1,j-1) = Pol(row[j]);
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetPolMat2TabPol);
DefExtOpr(1, BSetPolMat2TabPol, "PolMat2TabPol", 1, 1, "PolMatrix",
"(PolMatrix P)",
I2("Returns the table-set of polynomials of a polynomial matrix",
   "Devuelve el conjunto-table de polinomios de una matriz polynomial"),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BSetPolMat2TabPol::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& P = PolMat(Arg(1));
  int r=P.Rows();
  int c=P.Columns();
  int s=P.Data().Size();
  int i,j;
  contens_.PrepareStore(r);
  for(i=0; i<r; i++)
  {
    BSet s;
  	BUserSet* uSet = new BContensSet("",s,"");
    BSet& set = Set(uSet);
    set.PrepareStore(c);
    for(j=0; j<c; j++)
    {
    	BUserPol* uPol = new BContensPol("",P(i,j),"");
      set.AddElement(uPol);
    }
    contens_.AddElement(uSet);
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatDiag);
DefExtOpr(1, BPolMatDiag, "Diag", 2, 3, "Real Polyn Real",
  I2("(Real n, Polyn x, [Real direction=1])",
     "(Real n, Polyn x, [Real direccion=1])"),
  I2("Returns a diagonal polynomial matrix of dimension n and diagonal values all "
     "equal to x.",
     "Devuelve una matriz polinomial diagonal de dimension n y valores diagonales todos "
     "iguales a x."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatDiag::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat & C = contens_;
  BInt i,j;
  BInt n = (BInt)Real(Arg(1));
  BPol val = Pol(Arg(2));
  contens_.Alloc(n,n);
  if(contens_.Rows()!=n) { return; }
  BInt dir = 1;
  if(Arg(3)) { dir = (BInt)Real(Arg(3)); }
  if(dir!=1) { dir = -1; }
  BInt m = (dir==1)?0:n-1;
  for(i=0; i<n; i++)
  {
    for(j=0; j<n; j++)
    {
      if(i==m+dir*j) { C(i,j) = val; }
      else           { C(i,j) = BPol(0.0); }
    }
  }    
}

  
//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatZeros);
DefExtOpr(1, BPolMatZeros, "Zeros",  1, 2, 
  "Real Real",
  "(Real nrow [, Real ncol=nrow])",
  I2("Creates a polynomial matrix with all elements equal to zero.",
     "Devuelve una matriz polinomial cuyas celdas son todas cero."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatZeros::CalcContens()
//--------------------------------------------------------------------
{
  int r = (BInt)Real(Arg(1));
  int c = r;
  if(Arg(2)) { c = (BInt)Real(Arg(2)); }
  contens_.Alloc(r,c);
  int k, s= contens_.Data().Size();
  BPol* x=(BPol*)contens_.GetData().GetBuffer();
  for(k=0; k<s; k++,x++)
  {
    *x = BPol(0.0); 
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatConstant);
DefExtOpr(1, BPolMatConstant, "Constant",  3, 3, 
  "Real Real Polyn",
  "(Real nrow, Real ncol, Polyn value])",
  I2("Creates a polynomial matrix with all elements equal to given value.",
     "Devuelve una matriz polinomial cuyas celdas son todas iguales al valor "
     "especificado."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatConstant::CalcContens()
//--------------------------------------------------------------------
{
  int r = (BInt)Real(Arg(1));
  int c = (BInt)Real(Arg(2));
  BPol& v = Pol(Arg(3));
  contens_.Alloc(r,c);
  int k, s= contens_.Data().Size();
  BPol* x=(BPol*)contens_.GetData().GetBuffer();
  for(k=0; k<s; k++, x++)
  {
    *x = v;
  }
}


//--------------------------------------------------------------------
bool internal_builtin_SetMat(
  const BText& fun_id,
  const BSet &set, 
  BPolMat& mat)
//--------------------------------------------------------------------
{
  BInt i, j;
  BText _mid = I2("Converting set to polynomial matrix with ",
                  "Convirtiendo de conjunto a matriz polinomial con ")+fun_id;
  BText _wrongFormat = 
    _mid+I2("Wrong set format: all elements should "
            "be sets with the same number of reals ",
            "Fallo en formato de conjunto: todos sus elementos "
            "deberían ser conjuntos con el mismo número de reales");
  if(!set.Card() || (set[1]->Grammar()!=GraSet()))
  {
    Error(_wrongFormat);
    mat = BPolMat::Unknown();
    return(false);
  }
  BInt  numRow = set.Card();
  BInt  numCol = Set(set[1]).Card();
  if(!numRow)
  {
    Warning(_mid+I2("Emty set is converted in empty polynomial matrix",
             "El conjunto vacío se convierte en la matriz polinomial vacía"));
    mat = BPolMat::Unknown();
  }
  else if(!numCol)
  {
    Error(_wrongFormat);
  }
  else 
  {
    mat.Alloc(numRow,numCol);
    if(mat.Rows()!=numRow) { return(false); }
    for(i=0; i < numRow; i++)
    {
      if((set[i+1]->Grammar()!=GraSet()) ||
         (Set(set[i+1]).Card()!=numCol))
      {
        Error(_wrongFormat);
        mat = BPolMat::Unknown();
        return(false);  
      }
      BSet& seti = Set(set[i+1]);
      for(j=0; j < numCol; j++)
      {
        if(seti[j+1]->Grammar()==GraPolyn())
        {
          mat(i,j) = Pol(seti[j+1]);
        }
        else if(seti[j+1]->Grammar()==GraReal())
        {
          mat(i,j) = BPol(Dat(seti[j+1]));
        }
        else
        {
          Error(_wrongFormat);
          mat = BPolMat::Unknown();
          return(false);  
        }
      }
    }
  }
  return(true);
};

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSetMat);
DefExtOpr(1, BPolMatSetMat,   "SetMat", 1, 1, "Set",
  "(Set s)",
  I2("Returns a polynomial matrix as of a set of sets of real numbers.",
     "Devuelve una matriz polinomial a partir de un conjunto de conjuntos de numeros "
     "reales."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSetMat::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set  = Set(Arg(1));
  internal_builtin_SetMat("PolMatrix SetMat(Set s)",set,contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatConcatCol);
//DefExtOpr(1, BPolMatConcatCol,  "CatCol", 1, 0, "PolMatrix PolMatrix",
DefExtOpr(1, BPolMatConcatCol,  "ConcatColumns", 1, 0, "PolMatrix PolMatrix",
  "(PolMatrix mat1 [, PolMatrix mat2, ...])",
  I2("Concatenates all the columns of a polynomial matrices list with the same number "
     "of rows.",
     "Concatena todas las columnas de una lista de matrices polinomiales con el mismo "
     "numero de filas."),
  BOperClassify::MatrixAlgebra_);
  DefExtOpr(2, BPolMatConcatCol,  "|", 2, 2, "PolMatrix PolMatrix",
  "mat1 | mat2 {PolMatrix mat1 , PolMatrix mat2}",
  I2("Concatenates all the columns of two polynomial matrices with the same number "
     "of rows.",
     "Concatena todas las columnas de dos matrices polinomiales con el mismo "
     "numero de filas."),
      BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatConcatCol::CalcContens()
//--------------------------------------------------------------------
{
  BInt  i, j, k=0, l;
  BInt  numCol = 0;
  BInt  numRow = 0;
  BInt  numMat = NumArgs();
  for(i=1; i<=numMat; i++)
  {
    BPolMat& Mi = PolMat(Arg(i));
    numCol+=Mi.Columns();
    if(numRow<Mi.Rows()) { numRow = Mi.Rows(); }
  }
  for(i=1; i<=numMat; i++)
  {
    BPolMat& Mi = PolMat(Arg(i));
    if(Mi.Columns() && (Mi.Rows()!=numRow))
    {
      Error(I2("Wrong number of rows for ConcatColumns (or | operator) "
               "in argument number ",
               "Numero de filas erroneo para ConcatColumns (o el "
               "operador | )en la matriz polinomial numero ")+
            i + ".");
      contens_ = BPolMat::Unknown();
      return;
    }
  }
  contens_.Alloc(numRow, numCol);
  if(contens_.Rows()!=numRow) { return; }
  BPol* c = (BPol*)contens_.GetData().GetBuffer();
  for(i=1; i<=numMat; i++)
  {
    BPolMat& Mi = PolMat(Arg(i));
    int aRow = Mi.Rows();
    int aCol = Mi.Columns();
    if(aCol && (aRow==numRow))
    {
      const BPol* a = Mi.Data().Buffer();
      for(j=0; j < aCol; j++, k++)
      {
        BPol* ck = c+k;
        const BPol* aj = a+j;
        for(l=0; l < numRow; l++, ck+=numCol, aj+=aCol)
        {
          *ck = *aj;
        }
      }
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatConcatRow);
//DefExtOpr(1, BPolMatConcatRow,  "CatRow", 1, 0, "PolMatrix PolMatrix",
DefExtOpr(1, BPolMatConcatRow,  "ConcatRows", 1, 0, "PolMatrix PolMatrix",
  "(PolMatrix mat1 [, PolMatrix mat2, ...])",
  I2("Concats all the rows from a polynomial matrices list with the same number of "
     "columns.",
     "Concatena todas las filas de una lista de matrices polinomiales con el mismo "
     "numero de columnas."),
  BOperClassify::MatrixAlgebra_);
  DefExtOpr(2, BPolMatConcatRow,  "<<", 2, 2, "PolMatrix PolMatrix",
  "mat1 << mat2 {PolMatrix mat1, PolMatrix mat2}",
  I2("Concats all the rows from two polynomial matrices with the same number of "
     "columns.",
     "Concatena todas las filas de de dos matrices polinomiales con el mismo "
     "numero de columnas."),
      BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatConcatRow::CalcContens()
//--------------------------------------------------------------------
{
  BInt  i, j, l, k=0;
  BInt  numCol = 0;
  BInt  numRow = 0;
  BInt  numMat = NumArgs();
  for(i=1; i<=numMat; i++)
  {
    BPolMat& Mi = PolMat(Arg(i));
    numRow+=Mi.Rows();
    if(numCol<Mi.Columns()) { numCol = Mi.Columns(); }
  }
  for(i=1; i<=numMat; i++)
  {
    BPolMat& Mi = PolMat(Arg(i));
    int aRow = Mi.Rows();
    int aCol = Mi.Columns();
    if(aRow && (aCol!=numCol))
    {
      Error(I2("Wrong number of columns for ConcatRows ( or << "
               "operator ) in argument number ",
               "Numero de columnas erroneo para ConcatRows ( o "
               "el operador << ) en la matriz polinomial numero ")+
            i + ".");
      contens_ = BPolMat::Unknown();
      return;
    }
  }
  contens_.Alloc(numRow, numCol);
  if(contens_.Rows()!=numRow) { return; }
  BPol* c = (BPol*)contens_.GetData().GetBuffer();
  for(i=1; i <= numMat; i++) 
  {
    BPolMat& Mi = PolMat(Arg(i));
    int aRow = Mi.Rows();
    int aCol = Mi.Columns();
    if(aRow && (aCol==numCol))
    {
      for(j=0; j < Mi.Rows(); j++, k++) 
      {
        for(l=0; l < numCol; l++) 
        {
          contens_(k,l)=Mi(j,l); 
        } 
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatExtractDiag);
DefExtOpr(1, BPolMatExtractDiag,    "SubDiag", 2, 2, "PolMatrix Real",
  I2("(PolMatrix m, Real diagIndex)",
     "(PolMatrix m, Real indiceDeDiagonal)"),
  I2("Extracts from the polynomial matrix m the indicated diagonal.",
     "Extrae de la matriz polinomial la diagonal indicada. La diagonal principal es "
     "la de indice 0, las diagonales inferiores tienen indices negativos "
     "y las superiores los tienen positivos."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatExtractDiag::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat&M = PolMat(Arg(1));
  BInt d = (BInt)Real(Arg(2));
  BInt r = M.Rows();
  BInt c = M.Columns();
  BInt i,j,n;
  if(r*c<=0) { return; }
  if(d<=0) { i = abs(d); j = 0; }
  else     { i = 0;     j = d; }
  n = Minimum(r-i,c-j);
  contens_.Alloc(1,n);
  if(contens_.Rows()!=1) { return; }
  for(BInt k=0; k<n; k++, i++, j++)
  {
    contens_(0,k) = M(i,j);
  }
}



//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSetDiag);
DefExtOpr(1, BPolMatSetDiag,   "SetDiag", 1, 1, "Set",
  I2("(Set diagValues)",
     "(Set diagValues)"),
  I2("Creates a diagonal polynomial matrix with the elements of a set of real numbers.",
     "Crea una matriz polinomial diagonal con los elementos de un conjunto de numeros "
     "reales."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSetDiag::CalcContens()
//--------------------------------------------------------------------
{
  BSet& s = Set(Arg(1));
  BInt  n = s.Card();
  BInt  i;
  contens_.Alloc(n,n);
  if(contens_.Rows()!=n) { return; }
  contens_.SetAllValuesTo(0);
  for(i=0; i<n; i++)
  {
    contens_(i,i) = Pol(s[i+1]);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSubTriang);
DefExtOpr(1, BPolMatSubTriang,    "SubTriang", 2, 2, "PolMatrix Text",
  "(PolMatrix m, Text UPLO)",
  I2("Extracts from the polynomial matrix <m> a row vector containing \n"
     "  the upper triangular part if <UPLO>==\"U\" \n"
     "  the lower triangular part if <UPLO>==\"L\" \n"
     "If polynomial matrix is non square only principal minor submatrix will be used.\n"
     "If the dimension of this principal minor is n then the size of "
     "resulting vector will be k=n*(n+1)/2\n"
     "Use SetTriang to restore the triangular polynomial matrix.",
     "Extrae de la matriz polinomial <m> un vector fila conteniendo:\n"
     "  la parte triangular superior si <UPLO>==\"U\" \n"
     "  la parte triangular inferior si <UPLO>==\"L\" \n"
     "Si la matriz polinomial no es cuadrada silo se tendra en cuenta la submatriz "
     "menor principal.\n"
     "Si el tamaño de <m> es n entonces el tamaño del vector devuelto "
     "sera k=n*(n+1)/2\n" 
     "Para restaurar la matriz polinomial triangular use SetTriang"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSubTriang::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& M    = PolMat(Arg(1));
  BText UPLO = Text(Arg(2));
  int r = M.Rows();
  int c = M.Columns();
  int n = Minimum(r,c);
  int K = n*(n+1)/2;
  contens_.Alloc(1,K);
  if(contens_.Rows()!=1) { return; }
  BInt i,j,k;
  char uplo = toupper(UPLO[0]);
  if(uplo=='U')
  {
    for(i=k=0; i<n; i++)
    {
      for(j=i; j<n; j++, k++)
      {
        contens_(0,k) = M(i,j);
      }
    }
  }
  else if(uplo=='L')
  {
    for(i=k=0; i<n; i++)
    {
      for(j=0; j<=i; j++, k++)
      {
        contens_(0,k) = M(i,j);
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatSetTriang);
DefExtOpr(1, BPolMatSetTriang, "SetTriang", 2, 2, "PolMatrix Text",
  "(PolMatrix v, Text UPLOSI)",
  I2("Converts a row vector in:\n"
     "  a upper triangular polynomial matrix if <UPLOSI>==\"U\" \n"
     "  a lower triangular polynomial matrix if <UPLOSI>==\"L\" \n"
     "  a symmetric polynomial matrix if <UPLOSI>==\"S\" \n"
     "The size of vector <v> must be an integer k matching "
     "k=n*(n+1)/2, being n the size of resulting square polynomial matrix",
     "Convierte un vector fila en:\n"
     "  una matriz polinomial triangular superior si <UPLOSI>==\"U\" \n"
     "  una matriz polinomial triangular inferior si <UPLOSI>==\"L\" \n"
     "  una matriz polinomial simetrica si <UPLOSI>==\"S\" \n"
     "El tamaño del vector <v> debe ser un entero k tal que "
     "k=n*(n+1)/2, siendo n el tamaño de la matriz polinomial cuadrada "
     "resultante"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatSetTriang::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& v    = PolMat(Arg(1));
  BText UPLO = Text(Arg(2));
  int K = v.Rows();
  double n_ = (-1.0+sqrt(1.0+8.0*K))/2.0;
  int n = (int)n_;
  if(n!=n_)
  {
    Error(I2("Invalid dimnesions for SetTriang:",
             "Dimensiones invalidas para SetTriang: ")+
             " "+K+"!=n*(n+1)/2 "+
          I2("for every natural number n",
             "para todo numero natural n"));
    return;
  }
  contens_.Alloc(n,n);
  if(contens_.Rows()!=n) { return; }
  contens_.SetAllValuesTo(0.0);
  BInt i,j,k;
  char uplo = toupper(UPLO[0]);
  if((uplo=='U')||(uplo=='S'))
  {
    for(i=k=0; i<n; i++)
    {
      for(j=i; j<n; j++, k++)
      {
        contens_(i,j) = v.Data()(k);
      }
    }
  }
  else if((uplo=='L')||(uplo=='S'))
  {
    for(i=k=0; i<n; i++)
    {
      for(j=0; j<=i; j++, k++)
      {
        contens_(i,j) = v.Data()(k);
      }
    }
  }
}



//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatExtractRow);
DefExtOpr(1, BPolMatExtractRow,   "SubRow", 2, 2, "PolMatrix Set",
  I2("(PolMatrix m , Set rowsIndex)",
     "(PolMatrix m , Set indiceDeFilas)"),
  I2("Extracts from the polynomial matrix m the indicated rows.",
     "Extrae de la matriz polinomial las filas indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatExtractRow::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& M = PolMat(Arg(1));
  BSet& s = Set(Arg(2));
  register int  r  = s.Card();
  register int  Mr = M.Rows();
  register int  c  = M.Columns();
  register int  i;
  register int  j;
  register double iElement;
  register int iRow;
  register bool ok = true;
  
  contens_.Alloc(r,c);
  if(r*c<=0) { return; }
  if(contens_.Rows()!=r) { return; }
  register const BPol* x0 = M.Data().Buffer();
  register const BPol* x;
  register BPol* y = contens_.GetData().GetBuffer();
  for(i=0; i<r; i++)
  {
    iElement = Real(s[i+1]);
    iRow = (int) (round(iElement)) - 1;
    ok = (iRow>=0) && (iRow<Mr);
    if(!ok) { break; }
    x = x0 + (iRow*c);
    for(j=0; j<c; j++)
    {
      (*y) = (*x);
      y++;
      x++;
    }
  }
  if(!ok)
  {
    BText id = Arg(1)->Identify();
    Warning(I2("Attempt to invalid access of function SubRow(",
               "Intento de acceso no valido de la funcion SubRow(")+
            id+","+Arg(2)->Dump()+")");
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatExtractColumns);
DefExtOpr(1, BPolMatExtractColumns,   "SubCol", 2, 2, "PolMatrix Set",
  I2("(PolMatrix m , Set columnsIndex)",
     "(PolMatrix m , Set indiceDeColumnas)"),
  I2("Extracts from the polynomial matrix m the indicated columns.",
     "Extrae de la matriz polinomial las columnas indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatExtractColumns::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& M = PolMat(Arg(1));
  BSet& s = Set(Arg(2));
  register int  r  = M.Rows();
  register int  Mc = M.Columns();
  register int  c  = s.Card();
  register int  i;
  register int  j;
  register double jElement;
  register bool ok = true;
  
  contens_.Alloc(r,c);
  if(r*c<=0) { return; }
  register const BPol* x0 = M.Data().Buffer();
  register const BPol* x;
  register BPol* y = contens_.GetData().GetBuffer();
  BArray<int> jColumn(c);
  for(j=0; ok && (j<c); j++)
  {
    jElement = Real(s[j+1]);
    jColumn[j] = (int) (round(jElement)) - 1;
    ok = (jColumn[j]>=0) && (jColumn[j]<Mc);
  }
  register const int* jC0 = jColumn.Buffer();
  register const int* jC1 = jC0+c;
  register const int* jC;
  if(ok)
  {
    for(i=0; (i<r); i++)
    {
      x = x0 + (i*Mc);
      for(jC=jC0; jC<jC1; jC++)
      {
        (*y) = x[*jC];
        y++;
      }
    }
  }
  if(!ok)
  {
    BText id = Arg(1)->Identify();
    Warning(I2("Attempt to invalid access of function SubCol(",
               "Intento de acceso no valido de la funcion SubCol(")+
            id+","+Arg(2)->Dump()+")");
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BPolMat, BPolMatTemporary, BPolMatExtractRectangle);
DefExtOpr(1, BPolMatExtractRectangle, "Sub", 5, 5,
  "PolMatrix Real Real Real Real",
  I2("(PolMatrix m, Real row, Real column, Real height, Real width)",
     "(PolMatrix m, Real fila, Real columna, Real alto, Real ancho)"),
  I2("Extracts of the polynomial matrix the submatrix that starts in the given row and "
     "column and that it has the indicated height and width.",
     "Extrae de la matriz polinomial la submatriz que comienza en la fila y la columna "
     "dadas y que tiene el alto y ancho indicados."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BPolMatExtractRectangle::CalcContens()
//--------------------------------------------------------------------
{
  BPolMat& M = PolMat(Arg(1));
  BInt  y = (BInt)Real(Arg(2))-1;
  BInt  x = (BInt)Real(Arg(3))-1;
  BInt  r = (BInt)Real(Arg(4));
  BInt  c = (BInt)Real(Arg(5));
  
  BInt  i,j;
  BBool ok = BTRUE;
  
  contens_.Alloc(r,c);
  if(r*c<=0) { return; }
  if(contens_.Rows()!=r) { return; }
  for(j=0; ok && j<c; j++)
  {
    for(i=0; ok && i<r; i++)
    {
      bool okCell = ((i+y>=0)&&(j+x>=0)&&(i+y<M.Rows())&&(j+x<M.Columns()));
      ok = ok && okCell;
      if(okCell)
      {
        contens_(i,j) = M(i+y,j+x);
      }
    }
  }
  if(!ok)
  {
    BText id = Arg(1)->Identify();
    Warning(I2("Attempt to invalid access of function Sub(",
         "Intento de acceso no valido de la funcion Sub(")+
      id+","+Arg(2)->Dump()+","+Arg(3)->Dump()+","+
      Arg(4)->Dump()+","+Arg(5)->Dump()+")");
  }
}


