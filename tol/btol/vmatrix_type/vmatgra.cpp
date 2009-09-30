/* vmatgra.cpp: Virtual VMatrix Grammar's built-in variables and functions
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


#include <tol/tol_bvmatgra.h>
#include <tol/tol_bvmat_impl.h>

#include <tol/tol_bmatgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btmsgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bvmat_bsr.h>
#include <tol/tol_bdir.h>



//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("vmatgra.cpp");
template<>
BGrammar* BGraContensBase<BVMat>::ownGrammar_ = NIL;


DefineContensCommonOperators(BVMat, "VMatrix");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BVMat>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}


//--------------------------------------------------------------------
template<>
void BGraContensBase<BVMat>::InitInstances()

/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
//--------------------------------------------------------------------
{
  BTraceInit("BGraContens<BVMat>::InitInstances");
  BVMat vUnk = BVMat::Unknown();
  BSystemVMat* unknown_ = new BSystemVMat
  (
    "UnknownVMatrix",
    vUnk,
    I2("The unknown virtual matrix.", "La matriz virtual desconocida.")
  );
  OwnGrammar()->PutDefect(unknown_);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BVMat>::FindConstant (const BText&)

/*! Returns a valid constant BVMat for name.
 */
//--------------------------------------------------------------------
{ 
  return(NIL); 
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BVMat>::Casting(BSyntaxObject* obj)

/*! Returns a valid real for obj.
 */
//--------------------------------------------------------------------
{
  if(!obj) { return(NIL); }
  if(obj->Grammar()==OwnGrammar()) { return(obj); }
/*
  if(obj->Grammar()==GraMatrix())
  {
	  obj->Do();
    BVMat aux;
    aux.DMat2dense((BMatrix<double>&)Mat(obj));
    BContensVMat* uAux = new BContensVMat("",aux,"");
	  return(reinterpret_cast<BSyntaxObject*>(uAux));
  }
 */
  return(NIL);
}

//--------------------------------------------------------------------
static const BText& warn_auto_dense()
//--------------------------------------------------------------------
{
  static BText aux_=
  I2("Non dense matrices will be converted to dense ones.",          
     "Las matrices no densas seran convertidas a densas.");
  return(aux_);
}  

//--------------------------------------------------------------------
static const BText& warn_readonly_non_stored()
//--------------------------------------------------------------------
{
  static BText aux_=
  I2("For non dense matrices null non stored cells cannot be modified.",          
     "Para matrices no densas las celdas nulas no almacenadas no se pueden "
     "modificar.");
  return(aux_);
}  

/*------------------------------------------------------------------------------
  vmat.cpp: Generic methods  
------------------------------------------------------------------------------*/
//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtSubType);
DefExtOpr(1, BTxtSubType, "SubType", 1, 1,
  "VMatrix",
  "(VMatrix V)",
  I2("Returns the vitual matrix subtype code.",
     "Devuelve el codigo de subtipo de una matriz virtual."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BTxtSubType::CalcContens()
//--------------------------------------------------------------------
{
  contens_=BVMat::CodeName(VMat(Arg(1)).code_);
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatRows);
DefExtOpr(1, BDatRows, "VRows", 1, 1,
  "VMatrix",
  "(VMatrix V)",
  I2("Returns the number of rows of a virtual matrix.",
     "Devuelve el numero de filas de una matriz virtual."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BDatRows::CalcContens()
//--------------------------------------------------------------------
{
  contens_=VMat(Arg(1)).Rows();
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatColumns);
DefExtOpr(1, BDatColumns, "VColumns", 1, 1,
  "VMatrix",
  "(VMatrix V)",
  I2("Returns the number of columns of a virtual matrix.",
     "Devuelve el numero de columnas de una matriz virtual."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BDatColumns::CalcContens()
//--------------------------------------------------------------------
{
  contens_=VMat(Arg(1)).Columns();
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatNonNullCells);
DefExtOpr(1, BDatNonNullCells, "VNonNullCells", 1, 2,
  "VMatrix Real",
  "(VMatrix V [,Real chop])",
  I2("Returns the number of non null cell of a virtual matrix.",
     "Devuelve el numero de celdas no nulas de una matriz virtual."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BDatNonNullCells::CalcContens()
//--------------------------------------------------------------------
{
  double chop = 0.0;
  if(Arg(2)) { chop=Real(Arg(2)); }
  contens_=VMat(Arg(1)).NonNullCells(chop);
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatStoredCells);
DefExtOpr(1, BDatStoredCells, "VStoredCells", 1, 1,
  "VMatrix",
  "(VMatrix V)",
  I2("Returns the number of stored cells of a virtual matrix.",
     "Devuelve el numero de celdas almacenadas de una matriz virtual."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BDatStoredCells::CalcContens()
//--------------------------------------------------------------------
{
  const double *x;
  int nzmax;
  VMat(Arg(1)).StoredData(x,nzmax);
  contens_=nzmax;
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatDat);
  DefExtOpr(1, BDatVMatDat, "VMatDat", 3, 3, "VMatrix Real Real",
  I2("(VMatrix mat, Real row, Real column)",
     "(VMatrix mat, Real fila, Real columna)"),
  I2("Returns an element of a virtual matrix.",
     "Devuelve un elemento de una matriz virtual."),
     BOperClassify::Conversion_);
  void BDatVMatDat::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& v = VMat(Arg(1));
  BInt i = (BInt)Real(Arg(2))-1;
  BInt j = (BInt)Real(Arg(3))-1;
  contens_ = v.GetCell(i,j);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPutVMatDat);
  DefExtOpr(1, BDatPutVMatDat, "PutVMatDat", 4, 4, "VMatrix Real Real Real",
  I2("(VMatrix mat, Real row, Real column, Real newValue)",
     "(VMatrix mat, Real fila, Real columna, Real nuevoValor)"),
  I2("Changes the value of an element of a virtual matrix and returns true "
     "if success.",
     "Cambia el valor de un elemento de una matriz virtual y devuelve cierto "
     "si ha sido posible.")+warn_readonly_non_stored(),
     BOperClassify::Conversion_);
  void BDatPutVMatDat::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& v = VMat(Arg(1));
  int i = (int)Real(Arg(2))-1;
  int j = (int)Real(Arg(3))-1;
  double& d  = (double&)Dat(Arg(4));
  contens_ = v.PutCell(i,j,d);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPutVMatBlock);
  DefExtOpr(1, BDatPutVMatBlock, "PutVMatBlock", 4, 4, 
  "VMatrix Real Real {Matrix|VMatrix}",
  I2("(VMatrix mat, Real row, Real column, {Matrix|VMatrix} block)",
     "(VMatrix mat, Real fila, Real columna, {Matrix|VMatrix} bloque)"),
  I2("Changes the values of a minor block of a virtual matrix and returns true "
     "if success.",
     "Cambia los valores de un bloque menor de una matriz virtual y devuelve "
     "cierto si ha sido posible.")+warn_readonly_non_stored(),
     BOperClassify::Conversion_);
  void BDatPutVMatBlock::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& v = VMat(Arg(1));
  int i = (int)Real(Arg(2))-1;
  int j = (int)Real(Arg(3))-1;
  if(Arg(4)->Grammar()==GraVMatrix())
  {
    BVMat& x  = VMat(Arg(4));
    contens_ = v.PutBlock(i,j,x);
  }
  else if(Arg(4)->Grammar()==GraMatrix())
  {
    const BMatrix<double>& x  = (const BMatrix<double>&)Mat(Arg(4));
    BVMat x_;
    x_.DMat2dense(x);
    contens_ = v.PutBlock(i,j,x_);
  }
}

//--------------------------------------------------------------------
  DeclareContensClass(BVMat, BVMatTemporary, BVMatEnlarge);
  DefExtOpr(1, BVMatEnlarge, "Enlarge", 5, 5, 
  "VMatrix Real Real Real Real",
  "(VMatrix mat, Real nrow, Real ncol, Real row1, Real col1)",
  I2("Enlarge and relocate a Cholmod.R.Triplet matrix.",
     "Agranda y reubica una matriz Cholmod.R.Triplet."),
     BOperClassify::Conversion_);
  void BVMatEnlarge::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& v = VMat(Arg(1));
  int nrow = (int)Real(Arg(2));
  int ncol = (int)Real(Arg(3));
  int row0 = (int)Real(Arg(4))-1;
  int col0 = (int)Real(Arg(5))-1;
  contens_.Convert(v,BVMat::ESC_chlmRtriplet);
  assert(contens_.Check());
  contens_.Enlarge(nrow,ncol,row0,col0);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
  DeclareContensClass(BVMat, BVMatTemporary, BVMatMergeRows);
  DefExtOpr(1, BVMatMergeRows, "MergeRows", 2, 2, 
  "Real Set",
  "(Real nrow, Set items)",
  I2("Merge rows of a set of virtual matrices and row indexes.",
     "Mezcla las filas de un conjunto de matrices virtuales e índices "
     "de filas ."),
     BOperClassify::Conversion_);
  void BVMatMergeRows::CalcContens()
//--------------------------------------------------------------------
{
  int nrow = (int)Real(Arg(1));
  BSet& items = Set(Arg(2));
  contens_.MergeRows(nrow, items);
  assert(contens_.Check());
}


/*------------------------------------------------------------------------------
  vmat_io.cpp: input-output methods
------------------------------------------------------------------------------*/  
//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatPrint);
DefExtOpr(1, BDatPrint, "VMatPrint", 3, 3,
  "VMatrix Text Real",
  "(VMatrix V, Text filePath, Real level)",
  I2("Prints a VMatrix into a file with Matrix Market format.",
     "Escribe una VMatrix dentro de un fichero con formato Matrix Market."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BDatPrint::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& V     =  VMat(Arg(1));
  BText& fName = Text(Arg(2));
  int    level = (int)Real(Arg(3));
  
  FILE* file = (ToLower(fName)=="stdout")?stdout:fopen(fName,"w");
  contens_= file!=NULL;
  if(file)
  {
    V.Print(file,Arg(1)->Identify().String(), level);
  }
  if(file && (file!=stdout)) { fclose(file); }
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BVMatTemporary, BVMatScan);
DefExtOpr(1, BVMatScan, "VMatScan", 1, 1,
  "Text",
  "(Text filePath)",
  I2("Reads a Cholmod.R.Triplet from a file with Matrix Market format.",
     "Lee una Cholmod.R.Triplet de un fichero con formato Matrix Market."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatScan::CalcContens()
//--------------------------------------------------------------------
{
  BText& fName = Text(Arg(1));
  
  FILE* file = fopen(fName,"r");
  if(file)
  {
    contens_.Scan(file);
    assert(contens_.Check());
  }
  if(file) { fclose(file); }
}

  
/*------------------------------------------------------------------------------
  vmat_subtypes.cpp: Identification of subtypes and related methods
------------------------------------------------------------------------------*/
  
/*------------------------------------------------------------------------------
  vmat_convert.cpp: Convering methods 
------------------------------------------------------------------------------*/
//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatMat2VMat);
DefExtOpr(1, BVMatMat2VMat, "Mat2VMat", 1, 4,
  "Matrix Real Real Real",
  "(Matrix data [, Real transpose=FALSE, Real minSparse=0.5, Real drop=0])",
  I2("Converts a Matrix or its transpose in a VMatrix ."
     "If the ratio of null cells is greater or equal than minSparse a "
     "Cholmod.R.Sparse object will be created, and else a Blas.R.Dense. If "
     "drop is great than zero, all cells with absolute value lower than drop "
     "will be assumed as zeroes.",
     "Convuerte una Matrix o su traspuesta en una VMarix. Si el ratio de "
     "celdas no nulas es mayor que minSparse se devolvera una Cholmod.R.Sparse "
     "y en otro caso una Blas.R.Dense. Si drop es mayor o igual que cero todas "
     "las celdas con valot absoluto menor que drop se asumiran como ceros."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatMat2VMat::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& M = Mat(Arg(1));
  bool traspose = false;
  double minSparse = 0.5;
  double drop = 0.0;
  if(Arg(2)) { traspose  = (int)Real(Arg(2))!=0; }
  if(Arg(3)) { minSparse = Real(Arg(3)); }
  if(Arg(4)) { drop      = Real(Arg(4)); }
  contens_.DMat2VMat((const BMatrix<double>&)M, traspose, minSparse, drop);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatVMat2Mat);
DefExtOpr(1, BMatVMat2Mat, "VMat2Mat", 1, 2,
  "VMatrix Real",
  "(VMatrix data [, Real transpose=FALSE])",
  I2("Converts a VMatrix or its transpose in a Matrix.",
     "Convuerte una VMatrix o su traspuesta en una Marix."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatVMat2Mat::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& V = VMat(Arg(1));
  bool traspose = false;
  if(Arg(2)) { traspose  = (int)Real(Arg(2))!=0; }
  if(traspose) { V.GetDMatT((BMatrix<double>&)contens_); }
  else         { V.GetDMat ((BMatrix<double>&)contens_); }
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatConvert);
DefExtOpr(1, BVMatConvert, "Convert", 2, 2, 
  "VMatrix Text",
  "(VMatrix data, Text subType)",
  I2("Convert a VMatrix to another subtype or creates a copy if it is the same "
     "subtype. If the ratio of non-null cells is greater than minSparse return "
     "is a Cholmod.R.Sparse and in another case a Blas.R.Dense. If drop "
     "is greater than or equal to zero all cells with absolute value was "
     "smaller than drop will be assumed as zeros.\n"
     "All allowed pairs of subtypes are:\n",
     "Convierte una Matrix o su traspuesta en una VMarix. Si el ratio de "
     "celdas no nulas es mayor que minSparse se devolvera una Cholmod.R.Sparse "
     "y en otro caso una Blas.R.Dense. Si drop es mayor o igual que cero todas "
     "las celdas con valor absoluto menor que drop se asumiran como ceros.\n"
     "Los pares admitidos de subtipos son:\n")+
  BVMat::AvailConvert(),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatConvert::CalcContens()
//--------------------------------------------------------------------
{
  contens_.Convert(VMat(Arg(1)), Text(Arg(2)));
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatPack);
DefExtOpr(1, BVMatPack, "Pack", 1, 1, 
  "VMatrix",
  "(VMatrix data)",
  I2("Packs an sparse matrix to be stored in the minimum posible size."
     "It has no effect over non sparse matrices.",
     "Empaqueta una matriz sparse para que ocupe el mínimo espacio posible."
     "No tiene ningún efecto sobre matrices no sparse."),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatPack::CalcContens()
//--------------------------------------------------------------------
{
  contens_.Copy(VMat(Arg(1)));
  contens_.Pack();
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatTriplet);
DefExtOpr(1, BVMatTriplet, "Triplet", 3, 5, 
  "{Matrix|Set} Real Real Matrix Matrix",
  "({Matrix|Set} ijx, Real nrow, Real ncol "
  "[, Matrix rowIdx = 1 2 ... nrow, Matrix colIdx = 1 2 ... ncol])",
  I2("Creates a Cholmod.R.Triplet VMatrix T of given dimensions from a list "
     "of triplets ijx that can be expressed as a set of sets with tree "
     "numbers each one or a matrix with three columns. Each triplet (i,j,x) "
     "defines an stored cell T[i,j]=x. Non listed ones are "
     "assumed as zero. If row i or columns j index are not integers will be "
     "truncated.\n"
     "It's posible to remap row and column indexes of cells by specifying it "
     "with rowIdx and colIdx arguments, that must be row or column matrices "
     "of integer numbers.",
     "Construye una VMatrix Cholmod.R.Triplet T con la sdimensiones dadas a "
     "partir de una lista de tripletas ijx que pueden expresarse bien "
     "como un conjunto de conjuntos de tres números cada uno, bien como "
     "una matriz con tres columnas. Cada tripleta (i,j,x) define una celda "
     "almacenada T(i,j)=x. Las celdas no listadas se asumen como cero.\n"
     "Si los índices de fila i ó columna j no son enteros serán truncados.\n"
     "Es posible reubicar los índices de las filas y columnas de las celdas "
     "especificándolo con los argumentos rowIdx y colIdx, que deben ser "
     "matrices fila o columna de números enteros."),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatTriplet::CalcContens()
//--------------------------------------------------------------------
{ 
  BSyntaxObject* ijx_ = Arg(1);
  int nrow = (int)Real(Arg(2));
  int ncol = (int)Real(Arg(3));
  if(!Arg(4))
  {
    if(ijx_->Grammar()==GraMatrix())
    {
      BMatrix<double>& ijx = (BMatrix<double>&)Mat(ijx_);
      contens_.DMat2triplet(ijx,nrow,ncol);
    }
    else //ijx_->Grammar()==GraSet()
    {
      BSet& ijx = Set(ijx_);
      contens_.Set2triplet(ijx,nrow,ncol);
    }
  }
  else
  {
    BMat& rIdx = Mat(Arg(4)); 
    int r = rIdx.Data().Size();
    BArray<int> rowIdx(r);
    BArray<int> colIdx;
    int i, j;
    if(((rIdx.Rows()!=1) && (rIdx.Columns()!=1)) || (r>nrow)) 
    {
      Error(I2(BText("Just row or column matrices of maximum length nrow=")+nrow+
               " are allowed as rowIdx argument of function Triplet",
               BText("Sólo se permiten matrices fila o columna de tamaño máximo ncol=")+nrow+
               " como argumento rowIdx de la función Triplet"));
      return;
    }
    BDat* rIdxData = rIdx.GetData().GetBuffer();
    for(i=0; i<r; i++) 
    { 
      BDat ri = Round(rIdxData[i]);
      if(!rIdxData[i].IsFinite() || (ri<1) || (ri>nrow)) 
      {
        Error(I2(BText("Unknown, infinite or out of range [1,...,nrow=")+nrow+"] values, as "+
                 ri+", are not allowed in rowIdx argument of function Triplet",
                 BText("No se permiten valores desconocidos, infinitos ni fuera del "
                 "rango [1,...,nrow=")+nrow+"], como "+ ri+", en el argumento "
                 "rowIdx de la función Triplet"));
        return;
      }
      rowIdx[i] = (int)rIdxData[i].Value()-1; 
    //Std(BText("\nrowIdx[")+i+"]="+rowIdx[i]);
    }
    if(!Arg(5))
    {
      colIdx.AllocBuffer(ncol);
      for(j=0; j<ncol; j++) { colIdx[j] = j; }
    }
    else
    {
      BMat& cIdx = Mat(Arg(5)); 
      int c = cIdx.Data().Size();
      colIdx.AllocBuffer(c);
      if(((cIdx.Rows()!=1) && (cIdx.Columns()!=1)) || (c>ncol)) 
      {
        Error(I2(BText("Just row or column matrices of maximum length ncol=")+ncol+
                 " are allowed as rowIdx argument of function Triplet",
                 BText("Sólo se permiten matrices fila o columna de tamaño máximo ncol=")+ncol+
                 " como argumento rowIdx de la función Triplet"));
        return;
      }
      BDat* cIdxData = cIdx.GetData().GetBuffer();
      for(j=0; j<c; j++) 
      { 
        BDat cj = Round(cIdxData[j]);
        if(!cIdxData[j].IsFinite() || (cj<1) || (cj>ncol)) 
        {
          Error(I2(BText("Unknown, infinite or out of range [1,...,ncol=")+ncol+"] values, as "+
                   cj+", are not allowed in colIdx argument of function Triplet",
                   BText("No se permiten valores desconocidos, infinitos ni fuera del "
                   "rango [1,...,ncol=")+ncol+"], como "+ cj+", en el argumento "
                   "colIdx de la función Triplet"));
          return;
        }
        colIdx[j] = (int)cj.Value()-1; 
      //Std(BText("\ncolIdx[")+j+"]="+colIdx[j]);
      }
    }
    if(ijx_->Grammar()==GraMatrix())
    {
      BMatrix<double>& ijx = (BMatrix<double>&)Mat(ijx_);
      contens_.DMat2triplet(ijx,nrow,ncol,rowIdx,colIdx);
    }
    else //ijx_->Grammar()==GraSet()
    {
      BSet& ijx = Set(ijx_);
      contens_.Set2triplet(ijx,nrow,ncol,rowIdx,colIdx);
    }
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatVMat2Triplet);
DefExtOpr(1, BMatVMat2Triplet, "VMat2Triplet", 1, 2,
  "VMatrix Real",
  "(VMatrix M [, Real transpose=FALSE])",
  I2("Converts a VMatrix or its transpose in a Matrix with three columns "
     "having the triplets (i,j,M(i,j)) in each row.",
     "Convuerte una VMatrix o su traspuesta en una Marix con tres columnas "
     "que en cada fila contiene una tripleta (i,j,M(i,j))."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatVMat2Triplet::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& V = VMat(Arg(1));
  V.GetTriplet((BMatrix<double>&)contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BPol2VMat);
DefExtOpr(1, BPol2VMat, "Pol2VMat", 3, 3, 
  "Polyn Real Real",
  "(Polyn pol, Real nrow, Real ncol)",
  I2("Returns the matrix representation of a backshift polynomial.",
     "Devuelve la representacion matricial de un polinomio de retardos."
     "Si el polinomio no contiene términos en F, entonces "
     "se trata de una matriz triangular inferior que tiene todos los "
     "valores de la diagonal principal iguales al coeficiente de grado 0 "
     "del polinomio, y los de las diagonales inferiores iguales al "
     "coeficiente del grado correspondiente. "
     "Si tiene térmnos en F, ocurre de forma análoga con las diagonales "
     "superiores"),
    BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BPol2VMat::CalcContens()
//--------------------------------------------------------------------
{
  BPolyn<BDat>& pol = Pol(Arg(1));
  register int r = (BInt)Real(Arg(2));
  register int c = (BInt)Real(Arg(3));
  contens_.BPol2sparse(pol, r, c);
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatZero);
DefExtOpr(1, BVMatZero, "Zeros", 1, 3, 
  "Real Real Text",
  "(Real nrow [, Real ncol=nrow, Text subType=\"Cholmod.R.Sparse\"])",
  I2("Creates a matrix with all elements equal to zero.\n"
     "Allowed subtypes are :\n",
     "Devuelve una matriz cuyas celdas son todas cero.\n"
     "Los subtipos admitidos son:\n")+
  "Cholmod.R.Sparse, Blas.R.Dense",
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatZero::CalcContens()
//--------------------------------------------------------------------
{
  int nrow = (int)Real(Arg(1));
  int ncol = nrow;
  BVMat::ECode code = BVMat::ESC_chlmRsparse; 
  if(Arg(2)) { ncol = (int)Real(Arg(2)); }
  if(Arg(3)) { code = BVMat::FindCodeName(Text(Arg(3))); }
  contens_.Zeros(nrow,ncol,code);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatEye);
DefExtOpr(1, BVMatEye, "Eye", 1, 4, 
  "Real Real Real {VMatrix|Real}",
  "(Real nrow [, Real ncol=nrow, Real diag=0, {VMatrix|Real} x=1)",
  I2("Creates a matrix with all elements equal to zero but in specified "
     "diagonal which values will be given by argument x. If it's a Real then "
     "all them will be equal to x and if it is a row or column VMatrix then "
     "it will be used its sorted values. Main diagonal is referenced as "
     "diag=0, lower sub-diagonals as diag<0 and upper ones as diag>0."
     "elements that are equal to 1.\n",
     "Devuelve una matriz Cholmod.R.Sparse cuyas celdas son todas cero "
     "excepto los elementos de la diagonal especificada cuyos valores "
     "vendrán dados por el argumento x. Si es un Real todos ellos serán igual "
     "a éste y si es una VMatrix fila o columna se utilizarán sus valores "
     "ordenados. La diagonal principal se referencia como diag=0, las "
     "sub-diagonales principales como diag<0 y las superiores como diag>0."),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatEye::CalcContens()
//--------------------------------------------------------------------
{
  int nrow = (int)Real(Arg(1));
  int ncol = nrow;
  if(Arg(2)) { ncol=(int)Real(Arg(2)); }
  if(!Arg(3)) 
  { 
    contens_.Eye(nrow,ncol,BVMat::ESC_chlmRsparse); 
  }
  else
  {
    int diag = (int)Real(Arg(3));
    BSyntaxObject* x_ = Arg(4);
    bool isReal = !x_;
    double x = 1.0;
    if(!isReal)
    {
      if(x_->Grammar()==GraReal()) 
      { 
        x = Real(x_); 
        isReal = true;
      }
      else
      {
        BVMat& v = VMat(x_);
        contens_.Eye(nrow,ncol,diag,v);
      }
    }
    if(isReal)
    {
      contens_.Eye(nrow,ncol,diag,x);
    }
  }
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatDiag);
DefExtOpr(1, BVMatDiag, "Diag", 2, 3, "Real Real Real",
  I2("(Real n, Real x, [Real direction=1])",
     "(Real n, Real x, [Real direccion=1])"),
  I2("Returns a diagonal matrix of dimension n and diagonal values all "
     "equal to x.",
     "Devuelve una matriz diagonal de dimension n y valores diagonales todos "
     "iguales a x."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatDiag::CalcContens()
//--------------------------------------------------------------------
{
  BInt i;
  BInt n = (BInt)Real(Arg(1));
  double val = Real(Arg(2));
  if(!n) { return; }
  BInt dir = 1;
  if(Arg(3)) { dir = (BInt)Real(Arg(3)); }
  if(dir!=1) { dir = -1; }
  BVMat T;
  T.ChlmRTriplet(n,n,n);
  int*    r_ = (int*)   T.s_.chlmRtriplet_->i;
  int*    c_ = (int*)   T.s_.chlmRtriplet_->j;
  double* x_ = (double*)T.s_.chlmRtriplet_->x;
  if(dir==1)
  {
    for(i=0; i<n; i++)
    {
      r_[i] = i;
      c_[i] = i;
      x_[i] = val; 
      if(x_[i]!=0.0) { T.s_.chlmRtriplet_->nnz++; }
    }    
  }
  else
  {
    for(i=0; i<n; i++)
    {
      r_[i] = i;
      c_[i] = n-i-1;
      x_[i] = val; 
      if(x_[i]!=0.0) { T.s_.chlmRtriplet_->nnz++; }
    }    
  }
  contens_.Convert(T,BVMat::ESC_chlmRsparse);
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatSetDiag);
DefExtOpr(1, BVMatSetDiag, "SetDiag", 1, 1, "Set",
  I2("(Set diagValues)",
     "(Set diagValues)"),
  I2("Creates a diagonal matrix with the elements of a set of real numbers.",
     "Crea una matriz diagonal con los elementos de un conjunto de numeros "
     "reales."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatSetDiag::CalcContens()
//--------------------------------------------------------------------
{
  BInt  i;
  BSet& s = Set(Arg(1));
  BInt  n = s.Card();
  if(!n) { return; }
  contens_.Eye(n,n,BVMat::ESC_chlmRsparse); 
  double* x = (double*)contens_.s_.chlmRsparse_->x;
  for(i=0; i<n; i++)
  {
    x[i] = Real(s[i+1]);
  }
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatConcatCol);
DefExtOpr(1, BVMatConcatCol,  "ConcatColumns", 1, 0, "VMatrix VMatrix",
  "(VMatrix vmat1 [, VMatrix vmat2, ...])",
  I2("Concatenates all the columns of a matrixs list with the same number "
     "of rows.",
     "Concatena todas las columnas de una lista de matrices con el mismo "
     "numero de filas."),
  BOperClassify::MatrixAlgebra_);
  DefExtOpr(2, BVMatConcatCol,  "|", 2, 2, "VMatrix VMatrix",
  "vmat1 | vmat2 {VMatrix mat1 , VMatrix vmat2}",
  I2("Concatenates all the columns of two matrixs with the same number "
     "of rows.",
     "Concatena todas las columnas de dos matrices con el mismo "
     "numero de filas."),
      BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatConcatCol::CalcContens()
//--------------------------------------------------------------------
{
  int  i, k=0;
  int  numCol = 0;
  int  numRow = VMat(Arg(1)).Rows();
  int  numMat = NumArgs();
  BArray<BVMat*> arr;
  arr.AllocBuffer(numMat);
  for(i=1; i<=numMat; i++)
  {
    if(!Arg(i) || (Arg(i)->Grammar()!=GraVMatrix()))
    {
      Error(I2("Wrong argument for ConcaRows (or << operator) "
               "in argument number ",
               "Argumento erroneo para ConcatRows (o el "
               "operador << )en el argumento numero ")+
            i + ".");
      contens_ = BVMat::Unknown();
      return;
    }
    else
    {
      BVMat& Mi = VMat(Arg(i));
      numCol+=Mi.Columns();
      if(!numRow) { numRow=Mi.Rows(); }
      else 
      { 
        arr[k++] = &Mi;
      }
      if(Mi.Columns() && (Mi.Rows()!=numRow))
      {
        Error(I2("Wrong number of rows for ConcatColumns (or | operator) "
                 "in argument number ",
                 "Numero de filas erroneo para ConcatColumns (o el "
                 "operador | )en el argumento numero ")+
              i + ".");
        contens_ = BVMat::Unknown();
        return;
      }
    }
  }
  arr.ReallocBuffer(k);
  if(numRow && numCol && arr.Size()) 
  { 
    BVMat::BinGroup(&BVMat::CatCol,arr.GetBuffer(),arr.Size(),contens_); 
  }
  else
  {
    contens_.Zeros(numRow,0);
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatConcatRow);
DefExtOpr(1, BVMatConcatRow,  "ConcatRows", 1, 0, "VMatrix VMatrix",
  "(VMatrix vmat1 [, VMatrix vmat2, ...])",
  I2("Concatenates all the rows of a matrixs list with the same number "
     "of columns.",
     "Concatena todas las filas de una lista de matrices con el mismo "
     "numero de columnas."),
  BOperClassify::MatrixAlgebra_);
  DefExtOpr(2, BVMatConcatRow,  "<<", 2, 2, "VMatrix VMatrix",
  "vmat1 | vmat2 {VMatrix mat1 , VMatrix vmat2}",
  I2("Concatenates all the rows of two matrixs with the same number "
     "of columns.",
     "Concatena todas las filas de dos matrices con el mismo "
     "numero de columnas."),
      BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatConcatRow::CalcContens()
//--------------------------------------------------------------------
{
  int  i, k=0;
  int  numRow = 0;
  int  numCol = VMat(Arg(1)).Columns();
  int  numMat = NumArgs();
  BArray<BVMat*> arr;
  arr.AllocBuffer(numMat);
  for(i=1; i<=numMat; i++)
  {
    if(!Arg(i) || (Arg(i)->Grammar()!=GraVMatrix()))
    {
      Error(I2("Wrong argument for ConcaRows (or << operator) "
               "in argument number ",
               "Argumento erroneo para ConcatRows (o el "
               "operador << )en el argumento numero ")+
            i + ".");
      contens_ = BVMat::Unknown();
      return;
    }
    else
    {
      BVMat& Mi = VMat(Arg(i));
      numRow+=Mi.Rows();
      if(!numCol) { numCol=Mi.Columns(); }
      else 
      { 
        arr[k++] = &Mi;
      }
      if(Mi.Rows() && (Mi.Columns()!=numCol))
      {
        Error(I2("Wrong number of columns for ConcaRows (or << operator) "
                 "in argument number ",
                 "Numero de columnas erroneo para ConcatRows (o el "
                 "operador << )en el argumento numero ")+
              i + ".");
        contens_ = BVMat::Unknown();
        return;
      }
    }
  }
  arr.ReallocBuffer(k);
  if(numRow && numCol && arr.Size()) 
  { 
    BVMat::BinGroup(&BVMat::CatRow,arr.GetBuffer(),arr.Size(),contens_); 
  }
  else
  {
    contens_.Zeros(0,numCol);
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatExtractRow);
DefExtOpr(1, BVMatExtractRow,   "SubRow", 2, 2, "VMatrix Set",
  I2("(VMatrix m , Set rowsIndex)",
     "(VMatrix m , Set indiceDeFilas)"),
  I2("Extracts from the matrix m the indicated rows.",
     "Extrae de la matriz las filas indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatExtractRow::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& M = VMat(Arg(1));
  BSet& s = Set(Arg(2));
  BArray<int> rows(s.Card());
  int i, r = s.Card();
  for(i=0; i<r; i++)
  {
    rows[i] = (int)Real(s[i+1])-1;
  };
  M.SubRows(rows, contens_);
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatExtractColumns);
DefExtOpr(1, BVMatExtractColumns,   "SubCol", 2, 2, "VMatrix Set",
  I2("(VMatrix m , Set columnsIndex)",
     "(VMatrix m , Set indiceDeFilas)"),
  I2("Extracts from the matrix m the indicated columns.",
     "Extrae de la matriz las columnas indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatExtractColumns::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& M = VMat(Arg(1));
  BSet& s = Set(Arg(2));
  BArray<int> cols(s.Card());
  int j, c = s.Card();
  for(j=0; j<c; j++)
  {
    cols[j] = (int)Real(s[j+1])-1;
  };
  M.SubCols(cols, contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatExtractRectangle);
DefExtOpr(1, BVMatExtractRectangle, "Sub", 5, 5,
  "VMatrix Real Real Real Real",
  I2("(VMatrix m, Real row, Real column, Real height, Real width)",
     "(VMatrix m, Real fila, Real columna, Real alto, Real ancho)"),
  I2("Extracts the minor matrix that starts in the given row and "
     "column and that it has the indicated height and width.",
     "Extrae la matriz menor que comienza en la fila y la columna "
     "dadas y que tiene el alto y ancho indicados."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatExtractRectangle::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& M = VMat(Arg(1));
  BInt  y = (BInt)Real(Arg(2))-1;
  BInt  x = (BInt)Real(Arg(3))-1;
  BInt  r = (BInt)Real(Arg(4));
  BInt  c = (BInt)Real(Arg(5));
  M.Sub(y,x,r,c,contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatSubBand);
DefExtOpr(1, BVMatSubBand, "SubBand", 1, 3,
  "VMatrix Real Real",
  "(VMatrix m [, Real fisrtBand=0, Real lastBand=firstBand])",
  I2("Extracts the selected consecutive diagonals. Main diagonal is "
     "referenced as 0, lower ones are negative numbers and upper ones "
     "positive. Returned matrix has same dimensions than incoming one.",
     "Extrae las diagonales consecutivas seleccionadas. La diagonal "
     "principal se referencia como 0, las inferiores como números "
     "negativos y las superiores como positivos. La matriz devuelta "
     "tiene las mismas dimensiones que la entrante."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatSubBand::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& M = VMat(Arg(1));
  int firstBand = 0;
  if(Arg(2)) { firstBand = (int)Real(Arg(2)); }
  int lastBand = firstBand;
  if(Arg(3)) { lastBand = (int)Real(Arg(3)); }
  M.SubBand(firstBand,lastBand,contens_);
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatSubDiag);
DefExtOpr(1, BVMatSubDiag, "SubDiag", 1, 2,
  "VMatrix Real",
  "(VMatrix m [, Real diagNumber=0])",
  I2("Extracts the selected diagonal as a row matrix. Main diagonal is "
     "referenced as 0, lower ones are negative numbers and upper ones "
     "positive.",
     "Extrae la diagonales seleccionada como una matriz fila. La diagonal "
     "principal se referencia como 0, las inferiores como números "
     "negativos y las superiores como positivos."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatSubDiag::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& M = VMat(Arg(1));
  int diagNum = 0;
  if(Arg(2)) { diagNum = (int)Real(Arg(2)); }
  M.SubDiag(diagNum,contens_);
  assert(contens_.Check());
}


/*------------------------------------------------------------------------------
  vmat_arith.cpp: Arithmetic methods 
------------------------------------------------------------------------------*/
//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatIdentityPlus);
DefIntOpr(1, BVMatIdentityPlus, " + ", 1, 1,
  "(VMatrix mat)",
  I2("Returns the same matrix.",
     "Devuelve la misma matriz."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatIdentityPlus::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1));
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatMinus);
DefIntOpr(1, BVMatMinus, " - ", 1, 1,
  "(VMatrix mat)",
  I2("Returns a matrix width the opposite sign.",
     "Devuelve la matriz con el signo contrario."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatMinus::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Prod(VMat(Arg(1)),-1.0,contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatWeightProd);
DefIntOpr(1, BVMatWeightProd, "$*", 2, 2,
  "(VMatrix A, VMatrix B)",
  I2("Returns a VMatrix width the cell to cell products of two VMatrix.",
     "Devuelve la VMatriz con los productos celda a celda de dos VMatrix."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatWeightProd::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::WeightProd(VMat(Arg(1)), VMat(Arg(2)), contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatWeightQuot);
DefIntOpr(1, BVMatWeightQuot, "$/", 2, 2,
  "(VMatrix A, VMatrix B)",
  I2("Returns a VMatrix width the cell to cell quotients of two VMatrix.",
     "Devuelve la VMatriz con los cocientes celda a celda de dos VMatrix."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatWeightQuot::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::WeightQuot(VMat(Arg(1)), VMat(Arg(2)), contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatDrop);
DefExtOpr(1, BVMatDrop, "Drop", 2, 2, "VMatrix Real",
  "(VMatrix mat, Real tolerance)",
  I2("Set to zero all cells wich absolute value less or equal than given "
     "tolerance. Cannot be applied to subtype Cholmod.R.Factor.",
     "Pone a cero las celdas cuyo valor absoluto sea menor o igual que "
     "la tolerancia dada. No puede aplicarse al subtipo Cholmod.R.Factor."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatDrop::CalcContens()
//--------------------------------------------------------------------
{
  contens_.Copy(VMat(Arg(1)));
  double drop = 0;
  if(Arg(2)) { drop = Real(Arg(2)); }
  contens_.Drop(drop);
  assert(contens_.Check());
}

#define DecMonaryF00(FUN,ENGLISH,SPANISH)                                      \
DeclareContensClass(BVMat, BVMatTemporary, BVMat##FUN);                        \
DefIntOpr(1, BVMat##FUN, #FUN, 1, 1,                                           \
"(VMatrix mat)",                                                               \
I2("Returns "ENGLISH" of each element of a virtual matrix.",                   \
   "Devuelve "SPANISH" de cada elemento de una matriz virtual."),              \
BOperClassify::MatrixAlgebra_);                                                \
void BVMat##FUN::CalcContens()                                                 \
{                                                                              \
  BVMat::FUN(VMat(Arg(1)),contens_);                                           \
  assert(contens_.Check());                                                    \
}

#define DecMonary(FUN,ENGLISH,SPANISH)                                         \
DeclareContensClass(BVMat, BVMatTemporary, BVMat##FUN);                        \
DefIntOpr(1, BVMat##FUN, #FUN, 1, 1,                                           \
"(VMatrix mat)",                                                               \
I2("Returns "ENGLISH" of each element of a virtual matrix.",                   \
   "Devuelve "SPANISH" de cada elemento de una matriz virtual.")+              \
warn_auto_dense(),                                                             \
BOperClassify::MatrixAlgebra_);                                                \
void BVMat##FUN::CalcContens()                                                 \
{                                                                              \
  BVMat::FUN(VMat(Arg(1)),contens_);                                           \
  assert(contens_.Check());                                                    \
}

DecMonary   (Log,   "the natural logarithm",
                    "el logaritmo natural");
DecMonary   (Log10, "the decimal logarithm",
                    "el logaritmo decimal");
DecMonary   (Exp,   "the exponential",
                    "la exponencial");
DecMonaryF00(Sqrt,  "the square root",
                    "la raiz cuadrada");
DecMonaryF00(Sign,  "the sign",
                    "el signo");
DecMonaryF00(Abs,   "the absolute value",
                    "el valor absoluto");
DecMonaryF00(Floor, "the truncated value",
                    "el valor truncado");
DecMonaryF00(Round, "the rounded value",
                    "el valor redondeado");
DecMonaryF00(Sin,   "the trygonometric sine",
                    "el seno trigonometrico");
DecMonary   (Cos,   "the trygonometric cosine",
                    "el coseno trigonometrico");
DecMonaryF00(Tan,   "the trygonometric tangent",
                    "la tangente trigonometrica");
DecMonaryF00(ASin,  "the trygonometric arcsine",
                    "el arcoseno trigonometrico");
DecMonary   (ACos,  "the trygonometric arcosine",
                    "el arcocoseno trigonometrico");
DecMonaryF00(ATan,  "the trygonometric arctangent",
                    "la arcotangente trigonometrica");
DecMonaryF00(SinH,  "the hypergeometric sine",
                    "el seno hipergeometrico");
DecMonary   (CosH,  "the hypergeometric cosine",
                    "el coseno hipergeometrico");
DecMonaryF00(TanH,  "the hypergeometric tangent",
                    "la tangente hipergeometrica");
DecMonaryF00(ASinH, "the hypergeometric arcsine",
                    "el arcoseno hipergeometrico");
DecMonary   (ACosH, "the hypergeometric arcosine",
                    "el arcocoseno hipergeometrico");
DecMonaryF00(ATanH, "the hypergeometric arctangent",
                    "la arcotangente hipergeometrica");

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatT);
DefIntOpr(1, BVMatT, "Tra", 1, 1,
  "(VMatrix mat)",
  I2("Returns the transposed matrix, that is to say, "
     "the result of changing rows by columns.",
     "Devuelve la matriz traspuesta, es decir, el resultado de cambiar "
     "filas por columnas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatT::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::T(VMat(Arg(1)),contens_); 
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatSum2);
DefExtOpr(1, BVMatSum2, "+", 2, 2, "VMatrix {VMatrix|Real}",
  "mat1 + mat2 {VMatrix mat1, {VMatrix|Real} mat2}",
  I2("Returns the sum of two matrices with the same dimensions and "
     "subtype Blas.R.Dense or Cholmod.R.Sparse."
     "If second argument is a non zero real number or one of two matrices is "
     "non dense then ",
     "Devuelve la suma de dos matrices con las mismas dimensiones y subtipo "
     "Blas.R.Dense o Cholmod.R.Sparse."
     "Si el segundo argumento es un numero Real no nulo o alguno de los dos es "
     "una matriz no densa entonces ")+
     ToLower(warn_auto_dense()),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatSum2::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraVMatrix()) 
  { 
    BVMat::Sum(VMat(Arg(1)),VMat(Arg(2)),contens_); 
  }
  else if(gra==GraReal()) 
  { 
    BVMat::Sum(VMat(Arg(1)),Real(Arg(2)),contens_); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for VMatrix +",
             "no es un tipo valido para VMatrix +"));
    contens_ = BVMat::Unknown();
  }
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatDifference);
DefExtOpr(1, BVMatDifference, "-", 2, 2,"VMatrix {VMatrix|Real}",
  "mat1 - mat2 {VMatrix mat1, {VMatrix|Real} mat2}",
  I2("Returns the substraction of two matrices with the same dimensions and "
     "subtype Blas.R.Dense or Cholmod.R.Sparse."
     "If second argument is a non zero real number or one of two matrices is "
     "non dense then ",
     "Devuelve la resta de dos matrices con las mismas dimensiones y subtipo "
     "Blas.R.Dense o Cholmod.R.Sparse."
     "Si el segundo argumento es un numero Real no nulo o alguno de los dos es "
     "una matriz no densa entonces ")+
     ToLower(warn_auto_dense()),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatDifference::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraVMatrix()) 
  { 
    BVMat::Rest(VMat(Arg(1)),VMat(Arg(2)),contens_); 
  }
  else if(gra==GraReal()) 
  { 
    BVMat::Rest(VMat(Arg(1)),Real(Arg(2)),contens_); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for VMatrix -",
             "no es un tipo valido para VMatrix -"));
    contens_ = BVMat::Unknown();
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatProduct2);
DefExtOpr(1, BVMatProduct2, "*", 2, 2,"VMatrix {VMatrix|Real}",
  "mat1 * mat2 {VMatrix mat1, {VMatrix|Real} mat2}",
  I2("Returns the product of two matrixs such that the number of columns "
     "of the first matrix will be equal to the number of rows of the second."
     "Second one may be also a real number.\n"
     "All allowed pairs of subtypes are:\n",
     "Devuelve el producto de dos matrices tales que el numero de "
     "columnas de la primera matriz sea igual al numero de filas "
     "de la segunda. "
     "El segundo argumento puede ser tambien un numero Real.\n"
     "Los pares admitidos de subtipos son:\n")+
  "Blas.R.Dense * Real\n"+
  BVMat::AvailProduct(),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatProduct2::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraVMatrix()) 
  { 
    BVMat::Prod(VMat(Arg(1)),VMat(Arg(2)),contens_); 
  }
  else if(gra==GraReal()) 
  { 
    BVMat::Prod(VMat(Arg(1)),Real(Arg(2)),contens_); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for VMatrix *",
             "no es un tipo valido para VMatrix *"));
    contens_ = BVMat::Unknown();
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatQuotient);
DefExtOpr(1, BVMatQuotient, "/", 2, 2,"VMatrix Real",
  "mat1 / a {VMatrix mat1, Real a}",
  I2("Returns the quotient of a matrix and a real number",
     "Devuelve el cociente de una matriz y un numero real"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatQuotient::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Quot(VMat(Arg(1)),Real(Arg(2)),contens_); 
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatPower);
static BText BVMatPower_desc =
I2("Returns each cell of matrix powered to a real number",
   "Devuelve la potencia de una matriz elevada celda a celda a un "
   "numero real");
DefExtOpr(1, BVMatPower, "^", 2, 2,"VMatrix Real",
  "mat1 ^ a {VMatrix mat1, Real a}",
  BVMatPower_desc, BOperClassify::MatrixAlgebra_);
DefExtOpr(2, BVMatPower, "RPow", 2, 2,"VMatrix Real",
  "(VMatrix mat1, Real a)",
  BVMatPower_desc, BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatPower::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Pow(VMat(Arg(1)),Real(Arg(2)),contens_); 
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatMtMSqr);
DefIntOpr(1, BVMatMtMSqr, "MtMSqr", 1, 1,
  "(VMatrix mat)",
  I2("Returns the product of the transpose of a matrix by itself.",
     "Devuelve el producto de una matriz por si misma"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatMtMSqr::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::MtMSqr(VMat(Arg(1)), contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatMMtSqr);
DefIntOpr(1, BVMatMMtSqr, "MMtSqr", 1, 1,
  "(VMatrix mat)",
  I2("Returns the product of a matrix by its transpose.",
     "Devuelve el producto de una matriz por su traspuesta"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatMMtSqr::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::MMtSqr(VMat(Arg(1)), contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatSetSum);
DefExtOpr(1, BVMatSetSum,  "SetSum", 1, 1, "Set",
  "(Set s)",
  I2("Summ all virtual matrices of a given set. All mnatrices must have "
     "the same dimensions and subtype Blas.R.Dense or Cholmod.R.Sparse",
     "Suma las matrices virtuales de un conjunto dado. Todas las matrices"
     "deben tener las mismas dimensiones y subtipo Blas.R.Dense o "
     "Cholmod.R.Sparse"),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatSetSum::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!set.Card()) { return; }
  bool init = false;
  for(int n=1; n<=set.Card(); n++)
  {
    if(set[n]->Grammar()==GraVMatrix())
    {
      if(!init) { contens_ = VMat(set[n]); init = true; }
      else      { contens_ = contens_+VMat(set[n]); }
      if(!contens_.Rows() || !contens_.Columns() || !contens_.s_.undefined_) 
      {
        return;
      }
    }
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatDifEq);
DefExtOpr(1, BVMatDifEq,   "DifEq", 2, 6, 
  "Ratio VMatrix VMatrix VMatrix Real Real",
  "(Ratio R, VMatrix X [, VMatrix X0=0, VMatrix Y0=0, "
  "Real minSparse=0.5, Real chop=0])",
  I2(".",
     "."
     "Resuelve la ecuacion en diferencias Y = R(B)*X con los valores "
     "iniciales X0, Y0."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BVMatDifEq::CalcContens()
//--------------------------------------------------------------------
{
  BRat& R  = Rat(Arg(1));
  BVMat& X  = VMat(Arg(2));
  BVMat X0, Y0;
  double minSparse=0.5;
  double chop=0.0;
  int nd = R.Numerator().Degree();
  int dd = R.Denominator().Degree();
  if(Arg(3)) { X0 = VMat(Arg(3)); } 
  else       { X0.Zeros(nd, X.Columns(),BVMat::ESC_blasRdense); }
  if(Arg(4)) { Y0 = VMat(Arg(4)); } 
  else       { Y0.Zeros(dd, X.Columns(),BVMat::ESC_blasRdense); }
  if(Arg(5)) { minSparse = Real(Arg(5)); }
  if(Arg(6)) { chop      = Real(Arg(6)); }
  BVMat::DifEq(R,X0,X,Y0,contens_,minSparse,chop);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatBackDifEq);
DefExtOpr(1, BVMatBackDifEq, "BackDifEq",2, 6, 
  "Ratio VMatrix VMatrix VMatrix Real Real",
  "(Ratio R, VMatrix X [, VMatrix X0=0, VMatrix Y0=0, "
  "Real minSparse=0.5, Real chop=0])",
  I2(".",
     "."
     "Resuelve la ecuacion en diferencias Y = R(F)*X con los valores "
     "iniciales X0, Y0."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BVMatBackDifEq::CalcContens()
//--------------------------------------------------------------------
{
  BRat& R  = Rat(Arg(1));
  BVMat& X  = VMat(Arg(2));
  BVMat X0, Y0;
  double minSparse=0.5;
  double chop=0.0;
  int nd = R.Numerator().Degree();
  int dd = R.Denominator().Degree();
  if(Arg(3)) { X0 = VMat(Arg(3)); } 
  else       { X0.Zeros(nd, X.Columns(),BVMat::ESC_blasRdense); }
  if(Arg(4)) { Y0 = VMat(Arg(4)); } 
  else       { Y0.Zeros(dd, X.Columns(),BVMat::ESC_blasRdense); }
  if(Arg(5)) { minSparse = Real(Arg(5)); }
  if(Arg(6)) { chop      = Real(Arg(6)); }
  BVMat::BackDifEq(R,X0,X,Y0,contens_,minSparse,chop);
  assert(contens_.Check());
}

/*------------------------------------------------------------------------------
  vmat_logic.cpp: Logic and comparisson methods 
------------------------------------------------------------------------------*/
//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatYes);
DefIntOpr(1, BVMatYes, "Yes", 1, 1,
  "(VMatrix mat)",
  I2("Returns a boolean matrix with cells equal to 1 just for each non zero "
     "cell.",
     "Devuelve una matriz booleana con celdas igual a 1 sólo para las celdas "
     "distintas de cero."),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatYes::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Yes(VMat(Arg(1)),contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatNot);
DefIntOpr(1, BVMatNot, "Not", 1, 1,
  "(VMatrix mat)",
  I2("Returns a boolean matrix with cells equal to 1 just for each zero cell.",
     "Devuelve una matriz booleana con celdas igual a 1 sólo para las celdas "
     "iguales a cero.")+warn_auto_dense(),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatNot::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Not(VMat(Arg(1)),contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatIsUnknown);
DefIntOpr(1, BVMatIsUnknown, "IsUnknown", 1, 1,
  "(VMatrix mat)",
  I2("Returns a boolean matrix with cells equal to 1 just for each unknown cell.",
     "Devuelve una matriz booleana con celdas igual a 1 sólo para las celdas "
     "con valores desconocidos.")+warn_auto_dense(),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatIsUnknown::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::IsUnknown(VMat(Arg(1)),contens_);
  assert(contens_.Check());
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatIsFinite);
DefIntOpr(1, BVMatIsFinite, "IsFinite", 1, 1,
  "(VMatrix mat)",
  I2("Returns a boolean matrix with cells equal to 1 just for each finite known cell.",
     "Devuelve una matriz booleana con celdas igual a 1 sólo para las celdas "
     "con valores finitos conocidos.")+warn_auto_dense(),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatIsFinite::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::IsFinite(VMat(Arg(1)),contens_);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatAnd);
DefIntOpr(1, BVMatAnd, "And", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns the logical AND of arguments.",
     "Devuelve el AND (Y) lógico de los argumentos"),                              
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatAnd::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::And(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux = contens_;
    BVMat::And(aux,VMat(Arg(n)),contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatOr);
DefIntOpr(1, BVMatOr, "Or", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns the logical OR of arguments.",
     "Devuelve el OR (O) lógico de los argumentos"),                              
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatOr::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Or(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux = contens_;
    BVMat::Or(aux,VMat(Arg(n)),contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatEq);
DefIntOpr(1, BVMatEq, "Eq", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns TRUE just in each cell where ",
     "Devuelve CIERTO sólo para las celdas para las que ")+
     "A[i,j]==B[i,j]",   
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatEq::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::EQ(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux1 = contens_;
    BVMat::EQ(VMat(Arg(n-1)),VMat(Arg(n)),contens_);
    BVMat aux2 = contens_;
    BVMat::And(aux1,aux2,contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatNE);
DefIntOpr(1, BVMatNE, "NE", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns TRUE just in each cell where ",
     "Devuelve CIERTO sólo para las celdas para las que ")+
     "A[i,j]!=B[i,j]"+warn_auto_dense(),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatNE::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::NE(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux1 = contens_;
    BVMat::NE(VMat(Arg(n-1)),VMat(Arg(n)),contens_);
    BVMat aux2 = contens_;
    BVMat::And(aux1,aux2,contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatLE);
DefIntOpr(1, BVMatLE, "LE", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns TRUE just in each cell where ",
     "Devuelve CIERTO sólo para las celdas para las que ")+
     "A[i,j]<=B[i,j]",   
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatLE::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::LE(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux1 = contens_;
    BVMat::LE(VMat(Arg(n-1)),VMat(Arg(n)),contens_);
    BVMat aux2 = contens_;
    BVMat::And(aux1,aux2,contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatLT);
DefIntOpr(1, BVMatLT, "LT", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns TRUE just in each cell where ",
     "Devuelve CIERTO sólo para las celdas para las que ")+
     "A[i,j]<B[i,j]"+warn_auto_dense(),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatLT::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::LT(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux1 = contens_;
    BVMat::LT(VMat(Arg(n-1)),VMat(Arg(n)),contens_);
    BVMat aux2 = contens_;
    BVMat::And(aux1,aux2,contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatGE);
DefIntOpr(1, BVMatGE, "GE", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns TRUE just in each cell where ",
     "Devuelve CIERTO sólo para las celdas para las que ")+
     "A[i,j]>=B[i,j]",   
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatGE::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::GE(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux1 = contens_;
    BVMat::GE(VMat(Arg(n-1)),VMat(Arg(n)),contens_);
    BVMat aux2 = contens_;
    BVMat::And(aux1,aux2,contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatGT);
DefIntOpr(1, BVMatGT, "GT", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns TRUE just in each cell where ",
     "Devuelve CIERTO sólo para las celdas para las que ")+
     "A[i,j]>B[i,j]"+warn_auto_dense(),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatGT::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::GT(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux1 = contens_;
    BVMat::GT(VMat(Arg(n-1)),VMat(Arg(n)),contens_);
    BVMat aux2 = contens_;
    BVMat::And(aux1,aux2,contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatMin);
DefIntOpr(1, BVMatMin, "Min", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns in each cell the minimum value of arguments ",
     "Devuelve en cada celda el mínimo de los argumentos ")+
     "Min(A[i,j],B[i,j])",                              
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatMin::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Min(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux = contens_;
    BVMat::Min(aux,VMat(Arg(n)),contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatMax);
DefIntOpr(1, BVMatMax, "Max", 2, 0,
  "(VMatrix A, VMatrix B [, VMatrix C, ...])",
  I2("Returns in each cell the maximum value of arguments ",
     "Devuelve en cada celda el máximo de los argumentos ")+
     "Max(A[i,j],B[i,j])",                              
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatMax::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::Max(VMat(Arg(1)),VMat(Arg(2)),contens_);
  int n = 3;
  while(Arg(n))
  {
    BVMat aux = contens_;
    BVMat::Max(aux,VMat(Arg(n)),contens_);
    n++;
  }
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatIfVMatReal);
DefExtOpr(1, BVMatIfVMatReal, "IfVMat", 3, 3, "VMatrix VMatrix VMatrix",
  "(VMatrix condition, VMatrix A, VMatrix B)",
  I2("For each cell (i,j) if condition(i,j) is true returns the value of A(i,j), else B(i,j).",
     "Para cada celda (i,j) si condition(i,j) es cierto devuelve A(i,j) y si no B(i,j)."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatIfVMatReal::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& C = VMat(Arg(1));
  BVMat& A = VMat(Arg(2));
  BVMat& B = VMat(Arg(3));
  BVMat::If(C,A,B,contens_);
}


/*------------------------------------------------------------------------------
  vmat_chol.cpp: Choleski methods 
------------------------------------------------------------------------------*/
//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatCholeskiFactor);
DefExtOpr(1, BVMatCholeskiFactor, "CholeskiFactor", 1, 4, "VMatrix Text Real Real",
  "(VMatrix X [, Text S=\"X\", Real force_natural_order=false, Real showError=True])",
  I2("Returns the Choleski decomposition of a symmetric positive definite "
     "virtual matrix S, that will be built as: \n"
     "  S = X     if S = \"X\" \n "
     "  S = X'*X  if S = \"XtX\" \n "
     "  S = X *X' if S = \"XXt\" \n "
     "If mat is Blas.R.Dense then returned one is a lower triangular Blas.R.Dense "
     "matrix L matching\n"
     "  S = L*L'\n"
     "If mat is Cholmod.R.Sparse then returns a Cholmod.R.Factor. that internally "
     "contains a lower triangular L and a permutation matrix P matching:"
     "  S = P'*L*L'*P\n"
     "In this case you only can handle with L and P using CholeskiSolve method.\n"
     "Otherwise no operation is performed and unknown virtual matrix is returned."
     "If force_natural_order is true then P is the identity.\n",
     "Devuelve la descomposicion de Choleski de una matriz simetrica S definida "
     "positiva, la cual se construye de esta forma: \n"
     "  S = X     si S = \"X\" \n "
     "  S = X'*X  si S = \"XtX\" \n "
     "  S = X *X' si S = \"XXt\" \n "
     "Si mat es Blas.R.Dense se devuelve una matriz triangular inferior "
     "Blas.R.Dense tal que\n"
     "  S = L*L'\n"
     "Si es Cholmod.R.Sparse una Cholmod.R.Factor que contiene internamente "
     "una matriz triangular inferior L y una permutación P tales que \n"
     "  S = P'*L*L'*P\n"
     "En este caso sólo se puede operar con L y P a través de la función "
     "CholeskiSolve.\n"
     "En otro caso no se realiza ninguna operacion y se devuelve la matriz "
     "virtual desconocida.\n"
     "Si force_natural_order es falso entonces P es la identidad.\n"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatCholeskiFactor::CalcContens()
//--------------------------------------------------------------------
{
  BText S="X";
  bool force = false;
  bool showError = true;
  if(Arg(2)) { S=Text(Arg(2)); }
  if(Arg(3)) { force=Real(Arg(3))!=0.0; }
  if(Arg(4)) { showError=Real(Arg(4))!=0.0; }
  BVMat& X = VMat(Arg(1)); 
  BVMat::CholeskiFactor(X,contens_,S,true,force,showError);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatCholeskiSolve);
DefExtOpr(1, BVMatCholeskiSolve, "CholeskiSolve", 3, 3, 
  "VMatrix VMatrix Text",
  "(VMatrix factor, VMatrix b, Text sys)",
  I2("Solve the linear system F*x=b where F is a matrix obtained from a "
     "Choleski factor in some of these ways: \n"
     "  sys=\"PtLLtP\" => F= P'*L*L'*P*x = b\n"
     "  sys=\"LLt\"    => F=    L*L'  *x = b\n"
     "  sys=\"PtL\"    => F= P'*L     *x = b\n"
     "  sys=\"LtP\"    => F=     *L'*P*x = b\n"
     "  sys=\"Lt\"     => F=     *L'  *x = b\n"
     "  sys=\"L\"      => F=    L     *x = b\n"
     "  sys=\"Pt\"     => F= P'       *x = b\n"
     "  sys=\"P\"      => F=         P*x = b\n"
     "At current version P is allways the identity but next versions could be "
     "able to handle with generic permutation mtrices.\n"
     "The factor can be a lower triangular matrix Blas.R.Dense "
     "and then b must be Blas.R.Dense."
     "If factor is a Cholmod.R.Factor, IE. the result of "
     "CholeskiFactor for a Cholmod.R.Sparse, then, b can be "
     "Blas.R.Dense or Cholmod.R.Sparse interchangeably. In any case "
     "the subtype of returned result will be the same as b, if operation "
     "is performed successfully. Otherwise returned matrix will be "
     "the unknown one.",
     "Resuelve el sistema lineal F*x=b donde F es una matriz qeu se obtiene del "
     "factor de Choleski de alguna de las siguientes formas:\n"
     "  sys=\"PtLLtP\" => F= P'*L*L'*P*x = b\n"
     "  sys=\"LLt\"    => F=    L*L'  *x = b\n"
     "  sys=\"PtL\"    => F= P'*L     *x = b\n"
     "  sys=\"LtP\"    => F=     *L'*P*x = b\n"
     "  sys=\"Lt\"     => F=     *L'  *x = b\n"
     "  sys=\"L\"      => F=    L     *x = b\n"
     "  sys=\"Pt\"     => F= P'       *x = b\n"
     "  sys=\"P\"      => F=         P*x = b\n"
     "La matriz L es triangular"
     "El factor puede ser una matriz Blas.R.Dense triangular inferior, en "
     "cuyo caso b debe ser Blas.R.Dense y P es siempre la identidad; "
     "o bien puede ser Cholmod.R.Factor, es decir, el resultado de "
     "CholeskiFactor para una Cholmod.R.Sparse, y en tal caso b puede ser "
     "Blas.R.Dense o Cholmod.R.Sparse indistintamente. En cualquier caso "
     "el subtipo del resultado devuelto sera el mismo que el de b si se "
     "realiza la operacion con exito. En otro caso se devuelve la matriz "
     "desconocida."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatCholeskiSolve::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::CholeskiSolve(VMat(Arg(1)), VMat(Arg(2)), contens_, Text(Arg(3)));
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatCholeskiInv);
DefIntOpr(1, BVMatCholeskiInv, "CholeskiInv", 1, 1,
  "(VMatrix L)",
  I2("Returns the inverse of symmetric positive definite matrix S = L L'. "
     "The matrix L can be a lower triangular matrix Blas.R.Dense, and then "
     "a symmetric Blas.R.Dens will be returned, "
     "or also can be Cholmod.R.Factor one, IE the result of "
     "CholeskiFactor for Cholmod.R.Sparse, as the returned one.",
     "Devuelve la inversa de la matriz simetrica definida positiva S=L*L'. "
     "La matriz L puede ser una matriz Blas.R.Dense triangular inferior, en "
     "cuyo casose devolvera una Blas.R.Dense simetrica; "
     "o bien puede ser Cholmod.R.Factor, es decir, el resultado de "
     "CholeskiFactor para una Cholmod.R.Sparse como la devuelta."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatCholeskiInv::CalcContens()
//--------------------------------------------------------------------
{
  BVMat::CholeskiInv(VMat(Arg(1)), contens_);
  assert(contens_.Check());
}

/*------------------------------------------------------------------------------
  vmat_iterative.cpp: Iterative methods for solving linear problems
------------------------------------------------------------------------------*/

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatMinimumResidualsSolve);
DefExtOpr(1, BVMatMinimumResidualsSolve, "MinimumResidualsSolve", 
  2, 5, "{VMatrix|Code} VMatrix Real VMatrix Real", 
  BText("({VMatrix|Code} M, VMatrix B "
  "[, Real chop=")+Sqrt(DEpsilon())+", "+
     "VMatrix X0=Tra(M)*B,"+
     "Real maxIter=20*VColumns(M)])",
  I2("Applies the Minimum Residuals method to solve the linear "
     "system M*X=B begining at initial value <X0> until error were "
     "less than <chop> or maximum iteration is reached.",
     "Aplica el metodo de los Minimos Residuos para resolver el "
     "sistema lineal M*X=B comenzando por el valor inicial <X0> y "
     "continuando hasta  que el error sea menor que <chop> o se "
     "supere el número de iteraciones indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatMinimumResidualsSolve::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& B = VMat(Arg(2));
  double chop = -1;
  int maxIter = 20*B.Rows();
  if(Arg(3)) { chop = Real(Arg(3)); }
  if(Arg(5)) { maxIter = (int)Real(Arg(5)); }
  BGrammar* g = Arg(1)->Grammar();
  if(g==GraVMatrix())
  {
    BVMat& M = VMat(Arg(1));
    if(Arg(4)) 
    { 
      BVMat& X0 = VMat(Arg(4));
      contens_ = BVMat::MinimumResidualsSolve(M, B, X0, chop, maxIter);
    }
    else
    { 
      contens_ = BVMat::MinimumResidualsSolve(M, B, chop, maxIter);
    }
  }
  if(g==GraCode())
  {
    BUserCode* uc = UCode(Arg(1));
    BCode& code = uc->Contens();
    BStandardOperator* opr  = (BStandardOperator*)code.Operator();
    if(Arg(4)) 
    { 
      BVMat& X0 = VMat(Arg(4));
      contens_ = BVMat::MinimumResidualsSolve(opr, B, X0, chop, maxIter);
    }
    else
    { 
      contens_ = BVMat::MinimumResidualsSolve(opr, B, chop, maxIter);
    }
  }
}


/*------------------------------------------------------------------------------
  vmat_stats.cpp: Statistics methods
------------------------------------------------------------------------------*/

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatMoment);
  DefExtOpr(1, BDatVMatMoment, "VMatMoment", 2, 2, "VMatrix Real",
  "(VMatrix mat, Real order)",
  I2("Returns the moment of given order of all elements of a matrix.",
     "Devuelve el momento de orden dado de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatMoment::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Moment((int)Real(Arg(2)));
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatCenterMoment);
  DefExtOpr(1, BDatVMatCenterMoment, "VMatCenterMoment", 2, 2, "VMatrix Real",
  "(VMatrix mat, Real order)",
  I2("Returns the moment of given order of all elements of a matrix.",
     "Devuelve el momento centrado de orden dado de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatCenterMoment::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).CenterMoment((int)Real(Arg(2)));
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatSum);
  DefExtOpr(1, BDatVMatSum, "VMatSum", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the sum of all elements of a matrix.",
     "Devuelve la suma de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatSum::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Sum();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatAvr);
  DefExtOpr(1, BDatVMatAvr, "VMatAvr", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the arithmetic average of all elements of a matrix.",
     "Devuelve la media aritmética de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatAvr::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Avr();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatVar);
  DefExtOpr(1, BDatVMatVar, "VMatVar", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the variance of all elements of a matrix.",
     "Devuelve la varianza de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatVar::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Var();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatStDs);
  DefExtOpr(1, BDatVMatStDs, "VMatStDs", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the standard deviation of all elements of a matrix.",
     "Devuelve la desviacion tipica de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatStDs::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).StDs();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatAsymmetry);
  DefExtOpr(1, BDatVMatAsymmetry, "VMatAsymmetry", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the asymmetry coefficient of all elements of a matrix.",
     "Devuelve el coeficiente de asimetria de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatAsymmetry::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Asymmetry();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatKurtosis);
  DefExtOpr(1, BDatVMatKurtosis, "VMatKurtosis", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the kurtosis coefficient of all elements of a matrix.",
     "Devuelve el coeficiente de kurtosis  de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatKurtosis::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Asymmetry();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatMax);
  DefExtOpr(1, BDatVMatMax, "VMatMax", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the maximum of all elements of a matrix.",
     "Devuelve el maximo de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatMax::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Max();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVMatMin);
  DefExtOpr(1, BDatVMatMin, "VMatMin", 1, 1, "VMatrix",
  "(VMatrix mat)",
  I2("Returns the minimum of all elements of a matrix.",
     "Devuelve el minimo de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatVMatMin::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = VMat(Arg(1)).Min();
}


//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatRand);
DefExtOpr(1, BVMatRand, "Rand", 4, 6, 
  "Real Real Real Real Text Real",
  "(Real nrow, Real ncol, Real min, Real max "
  "[, Text SubType=\"Blas.R.Dense\", Real nonZeroCells=0.10*nrow*ncol])",
  I2("Creates a random virtual matrix which cells are distributed as "
     "independent uniform U[min,max]. In sparse case just nonZeroCells "
     "cells will be generated and placed in uniformly random selected cells.\n"
     "Allowed subtypes are :\n",
     "Devuelve una matriz virtual cuyas celdas se distribuiran uniformemente "
     "independientes U[min,max]. En el caso sparse solo se generaran"
     "nonZeroCells y se emplazaran y se ubicaran de forma uniformemnete "
     "aleatoria.\n"
     "Los subtipos admitidos son:\n")+
  "Cholmod.R.Sparse, Cholmod.R.Triplet, Blas.R.Dense",
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatRand::CalcContens()
//--------------------------------------------------------------------
{
  int    nrow = (int)Real(Arg(1));
  int    ncol = (int)Real(Arg(2));
  double min  = Real(Arg(3));
  double max  = Real(Arg(4));
  if(Arg(5)) 
  { 
    BVMat::ECode code = BVMat::FindCodeName(Text(Arg(5))); 
    int nz = (int)round(0.10*double(nrow)*double(ncol));
    if(Arg(6)) { nz = int(Real(Arg(6))); }
    contens_.Rand(nrow,ncol,min,max,code,nz);
  }
  else
  {
    contens_.Rand(nrow,ncol,min,max);
  }      
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatGaussian);
DefExtOpr(1, BVMatGaussian, "Gaussian", 4, 4, 
  "Real Real Real Real",
  "(Real nrow, Real ncol, Real nu, Real sigma)",
  I2("Creates a random virtual matrix which cells are distributed as "
     "independent normal r.v. N(mu,sigma^2).",
     "Devuelve una matriz virtual cuyas celdas se distribuiran como normales "
     "independientes N(mu,sigma^2)."),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatGaussian::CalcContens()
//--------------------------------------------------------------------
{
  int    nrow = (int)Real(Arg(1));
  int    ncol = (int)Real(Arg(2));
  double nu   = Real(Arg(3));
  double sig  = Real(Arg(4));
  contens_.Gaussian(nrow,ncol,nu,sig);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BVMat, BVMatTemporary, BVMatTruncStdGaussian);
DefExtOpr(1, BVMatTruncStdGaussian, "TruncStdGaussian", 3, 5, 
  "VMatrix VMatrix VMatrix Real Real",
  "(VMatrix D, VMatrix d, VMatrix z0 [, Real ncol=1, Real burnin=0])",
  I2("Creates a random virtual matrix X which ncol columns are distributed as "
     "independent standard normal truncated into a polytope",
     "Devuelve una matriz virtual X cuyas columnas se distribuiran como "
     "normales estándar independientes truncadas en un politopo")+
     "\n X ~ TN(0, I, D*X<=d);\n\n"+
  I2("The number of rows of X is obviously the number of columns of D that "
     "must have at least one columns and one row, having also the same number "
     "of rows than d.",
     "El número de filas de X será el de columnas de D, la cual ha de tener "
     "al menos una colmna y una fila, coincidiendo con las filas de d.")+"\n"+
  I2("The internal algorithm is a Gibbs sampler begining at a feasible point "
     "z0 that must match constrain inequations D*z0<=d ",
     "")+"\n"+
     "Efficient Gibbs Sampling of Truncated Multivariate Normal with Application "
     "to Constrained Linear Regression; Gabriel Rodriguez-Yam, Richard A. Davis, "
     "and Louis L. Scharf; March, 2004; "
     "http://www.stat.colostate.edu/~rdavis/papers/CLR.pdf",
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BVMatTruncStdGaussian::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& D    = VMat(Arg(1));
  BVMat& d    = VMat(Arg(2));
  BVMat& z0   = VMat(Arg(3));
  int ncol = 1;
  int burn = 0;
  if(Arg(4)) { ncol = (int)Real(Arg(4)); }
  if(Arg(5)) { burn = (int)Real(Arg(5)); }
  contens_.TruncStdGaussian(D,d,z0,ncol,burn);
  assert(contens_.Check());
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetGetBoundsInPolytope);
DefExtOpr(1, BSetGetBoundsInPolytope, "GetBoundsInPolytope", 4, 4, 
  "VMatrix VMatrix VMatrix Real",
  "(VMatrix D, VMatrix d, VMatrix z, Real j)",
  I2("Calculates minimum and maximum values of j-th component of "
     "vector z inside the polytope D*z <= d",
     "Calcula los valores mínimo y máximo de la j-ésima componente "
     "del vector z dentro del politopo D*z <= d"),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BSetGetBoundsInPolytope::CalcContens()
//--------------------------------------------------------------------
{
  BVMat& D = VMat(Arg(1));
  BVMat& d = VMat(Arg(2));
  BVMat& z = VMat(Arg(3));
  int j =  (int)Real(Arg(4))-1;
  double lower;
  double upper;
  int ok = BVMat::GetBoundsInPolytope(D, d, z, j, lower, upper)==0;
  if(ok)
  {
    contens_.PrepareStore(2);
    BGrammar::IncLevel();
    contens_.AddElement(BContensDat ::New("Lower",lower,""));
    contens_.AddElement(BContensDat ::New("Upper",upper,""));
    contens_.SetIndexByName();
    BGrammar::DecLevel();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetParseResLinReg);
DefExtOpr(1, BSetParseResLinReg, "BSR.Parse", 1, 2, 
  "Text Text",
  "(Text filePath [, Text moduleType=\"\"])",
  I2("Parses an ASCII file written in BSR language (Restricted Linear "
     "Regression) and returns a Set with structure of "
     "BSR.ModelDef containing all needed information to make "
     "bayesian estimation over a linear regression model constrained to "
     "linear inequations using MCMC (MonteCarlo Markov Chain) methods:\n"
     "  Y = X*B + E \n"
     "  A*B <= a\n"
     "Field LinearBlock contains information about variables vector b and "
     "must have structure of BSR.LinearBlock, that includes initial "
     "values B0 matching constrain inequations A*B0<=a.\n"
     "Field NoiseDistrib contains information about noise vector "
     "E and must have structure of BSR.NoiseDistrib.\n"
     "Argument <moduleType> must be one of these:\n"
     ,
     "Analiza un archivo ASCII escrito en lenguaje BSR (Bayesian "
     "Sparse Regression) y devuelve un Set con la estructura de "
     "BSR.ModelDef que contiene toda la información necesaria para "
     "hacer estimación bayesian de un modelo de regresión lineal sujeto "
     "a inecuaciones lineales utilizando métodos MCMC (MonteCarlo Markov "
     "Chain):\n"
     "  Y = X*B + E \n"
     "  A*B <= a\n"
     "El campo LinearBlock contiene información sobre las variables del "
     "vector b y debe tener la estructura de BSR.LinearBlock, que "
     "incluye valores iniciales B0 que cumplen las inecuaciones A * B0 "
     "<= a.\n"
     "El campo NoiseDistrib contiene información sobre el "
     "ruido del vector E y debe tener la estructura de BSR."
     "NoiseDistrib.\n"
     "El argumento <moduleType> debe ser uno de los siguientes:\n")+
     " * \"primary\" \n" 
     " * \"joint\" \n" 
     " * \"master\" \n"+
     I2("If empty string is specified then it will read the file "
     "prior to search ModuleType clause and if not found will take "
     "the default \"joint\" to ensure backwards compatibility.",
     "Si se especifica a cadena vacía entonces se hará una lectura "
     "previa del fichero para buscar la cláusula ModuleType y si no "
     "se encuentra se tomará el valor por defecto \"joint\" para "
     "garantizar la compatibilidad hacia atrás.")+
     BysSparseReg::url_parse_bsr(),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BSetParseResLinReg::CalcContens()
//--------------------------------------------------------------------
{
  BText& filePath = Text(Arg(1));
  BText moduleType = "";
  if(Arg(2)) { moduleType = Text(Arg(2)); }
  BysSparseReg::Parse_Module(filePath, moduleType, contens_);
}


