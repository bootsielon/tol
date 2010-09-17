/* matgra.cpp: Real Matrix Grammar's functions.
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

#include <tol/tol_bdir.h>
#include <tol/tol_gsl.h>
#include <tol/tol_gslmat.h>
#include <tol/tol_matrix.h>
#include <tol/tol_bvmat.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bprdist_internal.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bstat.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_barma.h>
#include <tol/tol_bmultvar.h>
#include <tol/tol_bsparse.h>
#include <tol/tol_ois.h>
#include <tol/tol_bsvdsep.h>
#include <tol/tol_filehandle.h>

#include <tol/tol_blas.h> 
#include <tol/tol_lapack.h> 

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_math.h>

#include "gsl_ext.h"

#define dMat(arg) ((DMat&)Mat(arg))
#define b2dMat(M) ((DMat&)(M))

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("matgra.cpp");
template<>
BGrammar* BGraContensBase<BMat>::ownGrammar_ = NIL;


DefineContensCommonOperators(BMat, "Matrix");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BMat>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}



//--------------------------------------------------------------------
template<>
void BGraContensBase<BMat>::InitInstances()

/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
//--------------------------------------------------------------------
{
    BTraceInit("BGraContens<BMat>::InitInstances");
    BMatrix<BDat> mat;
    
    BSystemMat* unknown_ = new BSystemMat
    (
      "UnknownMatrix",
      mat,
      I2("The unknown matrix.", "La matriz desconocida.")
    );
    OwnGrammar()->PutDefect(unknown_);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BMat>::FindConstant (const BText&)

/*! Returns a valid constant BMat for name.
 */
//--------------------------------------------------------------------
{ 
    return(NIL); 
}


//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BMat>::Casting(BSyntaxObject* obj)

/*! Returns a valid real for obj.
 */
//--------------------------------------------------------------------
{
    if(!obj) { return(NIL); }
    if(obj->Grammar()==OwnGrammar()) { return(obj); }
    return(NIL);
}


//--------------------------------------------------------------------
// Write and Read function
//--------------------------------------------------------------------

#define BBM_CHECK_SIZE

#ifndef BBM_CHECK_SIZE
#  define BBM_CheckSize(function,fileName,rows,columns)
#else
//--------------------------------------------------------------------
  void BBM_CheckSize(const BText& function, const BText& fileName, 
                     int rows, int cols, 
                     char msgType='E', BText comment = "")
//--------------------------------------------------------------------
{
  size_t fs = GetFileSize(fileName);
  if(fs!=rows*cols*sizeof(BDat)+2*sizeof(int))
  {
    BText msg = BText("[")+function+"]"+
          I2("Corrupted BBM file ",
             "Fichero BBM corrupto ")+
          fileName+
          I2("\n Number of rows m= ",
             "\n Numero de filas:")+
          rows+
          I2("\n Number of columns n= ",
             "\n Numero de columnas:")+
          cols+
          I2("\n Bytes of int s_int= ",
             "\n Bytes por int s_int :")+
          (int)sizeof(int)+
          I2("\n Bytes of BDat s_BDat= ",
             "\n Bytes por BDat s_BDat :")+
          (int)sizeof(BDat)+
          I2("\n Expected file size: ",
             "\n Tamaño esperado del fichero:")+
          (int)(rows*cols*sizeof(BDat)+2*sizeof(int))+
          I2("\n Real file size e: ",
             "\n Tamaño real del fichero:")+(int)fs + comment;
         if(msgType=='E') { Error(msg); }
    else if(msgType=='W') { Warning(msg); }
  }
}
#endif

//Time out for open BBM files in milliseconds
#define BBM_TimeOut 1000

//--------------------------------------------------------------------
void BBM_BinWrite(const BText& fileName, const BMat& M)
//--------------------------------------------------------------------
{
  FILE* fil = BSys::FOpenAndLock(fileName.String(),"wb",
   BBM_TimeOut, "Unexpected error writing to BBM file");
  if(!fil) { return; } 
  BInt m = M.Rows();
  BInt n = M.Columns();
  fwrite(&m,sizeof(BInt),1,fil);
  fwrite(&n,sizeof(BInt),1,fil);
  fwrite(M.Data().Buffer(), sizeof(BDat), n*m, fil);
  if(!BSys::FUnlockAndClose(fil,fileName.String())) { return; }
  BBM_CheckSize("BBM_BinWrite",fileName,m,n);
}

//--------------------------------------------------------------------
void BBM_BinAppend(const BText& fileName, const BMat& M)
//--------------------------------------------------------------------
{
  BInt rows, cols, rows2w, cols2w;
  FILE *fil;
  BDir dir = fileName;
  if(!dir.Exist())
  {
    BBM_BinWrite(fileName, M);
    return;
  }
  else if(dir.IsDir())
  {
    Error(I2("Cannot open BBM file ",
       "No se pudo abrir el fichero BBM ") + 
    fileName+I2(" due to it's a directory"," porque es un directorio"));
    return;
  }
  
  /* open for overwrite */
  fil = BSys::FOpenAndLock(fileName.String(),"a+b",
   BBM_TimeOut, "Unexpected error appending BBM file");
  if(!fil) { return; } 
  off_t pos = 0;
  fseeko(fil,0,SEEK_SET);
  
  fread(&rows,sizeof(BInt),1,fil);
  fread(&cols,sizeof(BInt),1,fil);
  rows2w = M.Rows();
  cols2w = M.Columns();
  if(cols!=cols2w) 
  {
    Error("MatAppendFile: Different number of columns");
  }
  else
  {
    rows += rows2w;
    pos = rows*cols*sizeof(BDat)+2*sizeof(int); fseeko(fil,pos,SEEK_SET); 
    fwrite(M.Data().Buffer(), sizeof(BDat), rows2w*cols2w, fil);
    if(!BSys::FUnlockAndClose(fil,fileName.String())) { return; }
    fil = BSys::FOpenAndLock(fileName.String(),"r+b",
     BBM_TimeOut*10, "Unexpected error changing BBM file");
    if(!fil) { return; } 
    fseeko(fil,0,SEEK_SET);
    fwrite(&rows, sizeof(BInt), 1, fil); 
  }
  BSys::FUnlockAndClose(fil,fileName.String());
//BBM_CheckSize("BBM_BinAppend",fileName,rows,cols);
}

//--------------------------------------------------------------------
void BBM_BinRead(const BText& fileName, BMat& M, 
   bool forceReadAvailableRows=false)
//--------------------------------------------------------------------
{
  BInt fs = GetFileSize(fileName);
//Std(BText("\nBBM_BinRead ")+fileName+" bytes "+fs);
  FILE* fil = BSys::FOpenAndLock(fileName.String(),"rb",
   BBM_TimeOut, "Unexpected error reading from BBM file");
  if(!fil) { return; } 

  BInt i, m, n, mn;
  fread(&m,sizeof(BInt),1,fil);
  fread(&n,sizeof(BInt),1,fil);
  if(!m || !n) 
  { 
    BSys::FUnlockAndClose(fil,fileName.String());
    return; 
  }
  int s = fs -2*sizeof(BInt);
  //In TOL versions after 2007-01-15 structure page size = 4 => sizeof(BDat)=12
  mn= m*n;
  if(s==mn*(int)sizeof(BDat)) 
  {
    M.Alloc(m,n);
    if(M.Rows()!=m) { return; }
    fread(M.GetData().GetBuffer(), sizeof(BDat), mn, fil);
    if(!BSys::FUnlockAndClose(fil,fileName.String())) { return; }
  }
  //In TOL versions before 2007-01-15 structure page size = 8 => sizeof(BDat)=16
  else if(s==mn*(int)sizeof(BOis::BDatOld)) 
  {
    M.Alloc(m,n);
    if(M.Rows()!=m) { return; }
    BArray<BOis::BDatOld> M_;
    fread(M_.GetBuffer(), sizeof(BOis::BDatOld), mn, fil);
    if(!BSys::FUnlockAndClose(fil,fileName.String())) { return; }
    for(i=0; i<mn; i++)
    {
      if(M_(i).known_) { M.Data()(i) = M_(i).value_; }
      else             { M.Data()(i) = BDat::Unknown(); }
    }
  //Std("\n");
  }
  else if(s==mn*16) 
  {
    M.Alloc(m,n);
    if(M.Rows()!=m) { return; }
    char* pos;
    BINT64 k;
    double x;
    char* buffer = new char[s];
    BDat* data   = M.GetData().GetBuffer();
    fread(buffer, 1, s, fil);
    if(!BSys::FUnlockAndClose(fil,fileName.String())) { return; }
    for(i=0; i<mn; i++)
    {
    //if(!(i%n)) Std("\n");
      pos = &buffer[(i*16)];
      memcpy(&k,pos,  8 );
      memcpy(&x,pos+8,8);
      if(k) { data[i].PutValue(x); }
      else  { data[i].PutKnown(false); }
    //Std(BText("(")+(int)k+","+x+"); ");
    }
  //Std("\n");
  }
  else if(forceReadAvailableRows)
  {
    int m_ = m;
    m = s/(n*sizeof(BDat));
    mn = m*n;
    BBM_CheckSize("BBM_BinRead",fileName,m_,n,'W',
    BText("\nIt seems that ")+(m_-m)+" rows are lost, but rest of matrix "
    "will be loaded");
    M.Alloc(m,n);
    fread(M.GetData().GetBuffer(), sizeof(BDat), mn, fil);
    if(!BSys::FUnlockAndClose(fil,fileName.String())) { return; }
  }
  else
  {
    BBM_CheckSize("BBM_BinRead",fileName,m,n);
  }
}

//--------------------------------------------------------------------
void BBM_BinReadRows(FILE* fil, 
                     const BText& fileName,
                     BMat& M, 
                     int m, int n, 
                     int i, int r, int t=1)
//--------------------------------------------------------------------
{
  int k, tr = t*r;
//Std(BText("\nBBM_BinRead ")+fileName+" bytes "+fs);
  BInt mn;
  if((m<=0)||(n<=0))
  {
    fseeko(fil,0,SEEK_SET);
    fread(&m,sizeof(BInt),1,fil);
    fread(&n,sizeof(BInt),1,fil);
  }
  mn= m*n;
  if(!m || !n) 
  { 
    return; 
  }
  if(i==-1)
  {
    i = m-tr;
  }
  if((0>  i)||(i     >=m)||
     (0>=tr)||(i+tr-1>=m) ) 
  { 
    return; 
  }
  {
    M.Alloc(r,n);
    if(M.Rows()!=r) { return; }
    off_t pos = 2*sizeof(BInt)+(i*n)*sizeof(BDat);
    fseeko(fil,pos,SEEK_SET);
    BDat* ptr = M.GetData().GetBuffer();
    if(t==1)
    {
      fread(ptr, sizeof(BDat), M.Data().Size(), fil);
    }
    else
    {
      for(k=0; k<r; k++)
      {
        if(k>0)
        {
          pos = 2*sizeof(BInt)+((i+k*t)*n)*sizeof(BDat);
          fseeko(fil,pos,SEEK_SET);
        }
        fread(ptr, sizeof(BDat), n, fil);
        ptr += n;
      }
    }
  }
}

//--------------------------------------------------------------------
void BBM_BinReadCell(FILE* fil, 
                     const BText& fileName,
                     BDat& cell, 
                     int m, int n, 
                     int i, int j)
//--------------------------------------------------------------------
{
  if((m<=0)||(n<=0))
  {
    fseeko(fil,0,SEEK_SET);
    fread(&m,sizeof(BInt),1,fil);
    fread(&n,sizeof(BInt),1,fil);
  }
  if((0>i)||(i>=m)||
     (0>j)||(j>=n) ) 
  { 
    BSys::FUnlockAndClose(fil,fileName.String());
    return; 
  }
  off_t pos = 2*sizeof(BInt)+(i*n+j)*sizeof(BDat);
  fseeko(fil,pos,SEEK_SET);
  fread(&cell, 1, sizeof(BDat), fil);
}

//--------------------------------------------------------------------
void BBM_BinReadDimensions(const BText& fileName, 
                           int& rows, int& columns)
//--------------------------------------------------------------------
{
  BInt fs = GetFileSize(fileName);
//Std(BText("\nBBM_BinRead ")+fileName+" bytes "+fs);
  FILE* fil = BSys::FOpenAndLock(fileName.String(),"rb",
   BBM_TimeOut, "Unexpected error reading from BBM file");
  if(!fil) { return; } 
  fread(&rows,   sizeof(BInt),1,fil);
  fread(&columns,sizeof(BInt),1,fil);
  BSys::FUnlockAndClose(fil,fileName.String());
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatWriteFile);
DefExtOpr(1, BMatWriteFile, "MatWriteFile", 2, 2, "Text Matrix",
  I2("(Text filename, Matrix mat)",
     "(Text nombreFichero, Matrix mat)"),
  I2("Writes the matrix into a file in binary format",
     "Escribe una matriz en un fichero en formato binario"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatWriteFile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("MatWriteFile")) { return; }
  contens_ = Mat(Arg(2));
  BBM_BinWrite(Text(Arg(1)),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatAppendFile);
DefExtOpr(1, BMatAppendFile, "MatAppendFile", 2, 2, "Text Matrix",
    I2("(Text filename, Matrix mat)",
       "(Text nombreFichero, Matrix mat)"),
    I2("Append matrix 'mat' to file 'filename'.",
       "Añade la matrix 'mat' al fichero 'nombreFichero'."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatAppendFile::CalcContens()
{
  if(CheckNonDeclarativeAction("MatAppendFile")) { return; }
  contens_ = Mat(Arg(2));
  BBM_BinAppend(Text(Arg(1)),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatReadFile);
DefExtOpr(1, BMatReadFile, "MatReadFile", 1, 3, "Text Text Real",
  "(Text filename [, Text format=\"BINARY\", "
                     "Real forceReadAvailableRows=True])",
  I2("Reads a matrix from a file in the specified format",
     "Lee una matriz de un fichero en el formato especificado")+":\n"+
     "BINARY: "+
     I2("Standard TOL binary format",
        "Formato estándar binario de TOL")+
     "WGRIB2TXT: "+
     I2("Result of command", "Resultado del comando")+
     " wgrib2 ... -text file\n"+
     " cols rows\n"+
     " datum[1,1]\n" +
     " datum[1,2]\n" +
     " ...\n" +
     " datum[rows,cols]\n"+
     I2("View more on ", "Ver más en ")+
     "http://www.cpc.noaa.gov/products/wesley/wgrib2/",
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatReadFile::CalcContens()
//--------------------------------------------------------------------
{
  BText fileName = Text(Arg(1));
  BText format = "BINARY";
  bool forceReadAvailableRows= true;
  if(Arg(2)) { format = Text(Arg(2)); }
  if(Arg(3)) { forceReadAvailableRows = Real(Arg(3))!=0; }
  if(format=="BINARY")
  {
    BBM_BinRead(fileName,contens_,forceReadAvailableRows);
  }
  else if(format=="WGRIB2TXT")
  {
    FILE* file = fopen(fileName.String(),"r");
    if(!file) 
    {
      Error(BText("[MatReadFile] Cannot open file ")+fileName);  
      return; 
    }
    int i, j, rows, cols;
    fscanf(file, "%ld %ld", &cols, &rows);
    contens_.Alloc(rows, cols);
    double* x = (double*)contens_.GetData().GetBuffer();
    bool errCannotReadCell = false;
    for(i=0; !errCannotReadCell && !feof(file) && (i<rows); i++)
    {
      for(j=0; !errCannotReadCell && (j<cols); j++, x++)
      {
        if(!fscanf(file, "%lf", x))
        {
          Error(BText("[MatReadFile] Cannot read cell")+
            " of file "+fileName);  
          contens_.Alloc(0,0);
          errCannotReadCell = true;
        }
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatReadDimensions);
DefExtOpr(1, BMatReadDimensions, "MatReadDimensions", 1, 1, "Text",
  "(Text filename)",
  I2("Returns rows and columns of the matrix stored in a file with "
     "binary format",
     "Devuelve el número de filas y columnas de la matriz almacenada "
     "en un fichero con formato binario"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatReadDimensions::CalcContens()
//--------------------------------------------------------------------
{
  BText& f = Text(Arg(1));
  int rows, cols;
  BBM_BinReadDimensions(f,rows, cols);
  BUserDat* r_ = BContensDat::New("",rows,"Number of stored rows");
  BUserDat* c_ = BContensDat::New("",cols,"Number of stored columns.");
  r_->PutName("Rows");
  c_->PutName("Columns");
  contens_.PrepareStore(2);
  contens_.AddElement(r_);
  contens_.AddElement(c_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatReadRows);
DefExtOpr(1, BMatReadRows, "MatReadRows", 3, 6, 
  "{Text|Real} Real Real Real Real Real",
  "({Text|Real} file, Real firstRow, Real numRows "
  "[, Real thinning=1, Real numRows=-1, Real numColumns=-1])",
  I2("Reads a range of rows of a matrix from a file in binary format\n"
     "If argument <file> is a Text then the file in these path will be "
     "open before to read and it will be closed after.\n"
     "If argument <file> is Real created with FOpen the file is already "
     "open and the user should close with FClose after. This is a faster "
     "method for multiple reading over the same file.\n"
     "Afterwords, if arguments <numRows> and <numColumns> are greater than "
     "zero, then it will not have to read them from the file, which can "
     "greatly accelerate the process.",
     "Lee un rango de filas de una matriz de un fichero en formato binario\n"
     "Si el argumento <file> es un Text entonces el fichero en ese "
     "camino será abierto antes de leer y cerrado después.\n"
     "Si el argumento <file> es un Real creado con FOpen entonces el "
     "fichero ya está abierto y será el usuario el que lo cierre cuando "
     "se requiera. Este es el método más rápido cuando hay que hacer "
     "múltiples lecturas sobre un mismo fichero.\n"
     "Si además los argumentos <numRows> y <numColumns> son mayores que "
     "cero entonces no será necesario leerlos del propio fichero lo cual "
     "puede acelerarse mucho el proceso."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatReadRows::CalcContens()
//--------------------------------------------------------------------
{
  FILE* file = NULL;
  int i = (int)Real(Arg(2))-1;
  int r = (int)Real(Arg(3));
  int t = 1;
  int m = -1;
  int n = -1;
  if(Arg(4)) { t = (int)Real(Arg(4)); }
  if(Arg(5)) { m = (int)Real(Arg(5)); }
  if(Arg(6)) { n = (int)Real(Arg(6)); }
  if(Arg(1)->Grammar()==GraReal())
  {
    int handle = (int)Real(Arg(1));
    BFileDesc* fd = BFileDesc::FindFileHandle(handle,true,
      I2("Fail in ","Fallo en ")+"BMatReadRows");  
    if(fd)
    {
      BBM_BinReadRows(fd->file_,fd->path_.String(),contens_,m,n,i,r,t);
    }
  }
  else
  {
    BText& path = Text(Arg(1));
    file = BSys::FOpenAndLock(path.String(),"rb",
      BBM_TimeOut, "Unexpected error reading from BBM file");
    if(!file) { return; } 
    BBM_BinReadRows(file,path.String(),contens_,m,n,i,r,t);
    BSys::FUnlockAndClose(file,path.String());
  }
}
//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BMatReadCell);
DefExtOpr(1, BMatReadCell, "MatReadCell", 3, 5, 
  "{Text|Real} Real Real Real Real",
  "({Text|Real} file, Real row, Real column)"
  "[, Real numRows=-1, Real numColumns=-1])",
  I2("Reads a cell matrix from a file in binary format.\n"
     "If argument <file> is a Text then the file in these path will be "
     "open before to read and it will be closed after.\n"
     "If argument <file> is Real created with FOpen the file is already "
     "open and the user should close with FClose after. This is a faster "
     "method for multiple reading over the same file.\n"
     "Afterwords, if arguments <numRows> and <numColumns> are greater than "
     "zero, then it will not have to read them from the file, which can "
     "greatly accelerate the process.",
     "Lee una celda de una matriz de un fichero en formato binario\n"
     "Si el argumento <file> es un Text entonces el fichero en ese "
     "camino será abierto antes de leer y cerrado después.\n"
     "Si el argumento <file> es un Real creado con FOpen entonces el "
     "fichero ya está abierto y será el usuario el que lo cierre cuando "
     "se requiera. Este es el método más rápido cuando hay que hacer "
     "múltiples lecturas sobre un mismo fichero.\n"
     "Si además los argumentos <numRows> y <numColumns> son mayores que "
     "cero entonces no será necesario leerlos del propio fichero lo cual "
     "puede acelerarse mucho el proceso."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatReadCell::CalcContens()
//--------------------------------------------------------------------
{
  FILE* file = NULL;
  int i = (int)Real(Arg(2))-1;
  int j = (int)Real(Arg(3))-1;
  int m = -1;
  int n = -1;
  if(Arg(4)) { m = (int)Real(Arg(4)); }
  if(Arg(5)) { n = (int)Real(Arg(5)); }
  if(Arg(1)->Grammar()==GraReal())
  {
    int handle = (int)Real(Arg(1));
    BFileDesc* fd = BFileDesc::FindFileHandle(handle,true,
      I2("Fail in ","Fallo en ")+"MatReadCell");  
    if(fd)
    {
      BBM_BinReadCell(fd->file_,fd->path_.String(),contens_,m,n,i,j);
    }
  }
  else
  {
    BText& path = Text(Arg(1));
    file = BSys::FOpenAndLock(path.String(),"rb",
     BBM_TimeOut, "Unexpected error reading from BBM file");
    if(!file) { return; } 
    BBM_BinReadCell(file,path.String(),contens_,m,n,i,j);
    BSys::FUnlockAndClose(file,path.String());
  }
}

//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatIdentity);
DefIntOpr(1, BMatIdentity, " + ", 1, 1,
  "(Matrix mat)",
  I2("Returns the same matrix.",
     "Devuelve la misma matriz."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatIdentity::CalcContens()
//--------------------------------------------------------------------
{
  b2dMat(contens_) = dMat(Arg(1));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatMinus);
DefIntOpr(1, BMatMinus, " - ", 1, 1,
  "(Matrix mat)",
  I2("Returns a matrix width the opposite sign.",
     "Devuelve la matriz con el signo contrario."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatMinus::CalcContens()
//--------------------------------------------------------------------
{
  b2dMat(contens_) = dMat(Arg(1))*(-1.0);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatT);
DefIntOpr(1, BMatT, "Tra", 1, 1,
  "(Matrix mat)",
  I2("Returns the transposed matrix, that is to say, "
     "the result of changing rows by columns.",
     "Devuelve la matriz traspuesta, es decir, el resultado de cambiar "
     "filas por columnas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatT::CalcContens()
//--------------------------------------------------------------------
{
  b2dMat(contens_) = dMat(Arg(1)).T(); 
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatReverse);
DefIntOpr(1, BMatReverse, "Reverse", 1, 1,
  "(Matrix mat)",
  I2("Returns the matrix of rows and columns with inverse order.",
     "Devuelve la matriz con las filas y columnas en orden inverso."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatReverse::CalcContens()
//--------------------------------------------------------------------
{
  b2dMat(contens_) = dMat(Arg(1));
  b2dMat(contens_).Reverse();
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatNot);
DefIntOpr(1, BMatNot, "Not", 1, 1,
  "(Matrix mat)",
  I2("Returns the logic Not function of a matrix.",
     "Devuelve el No logico de una matriz."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatNot::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Not(Mat(Arg(1))); 
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatIsFinite);
DefIntOpr(1, BMatIsFinite, "IsFinite", 1, 1,
  "(Matrix mat)",
  I2("Returns true in each cell containing a known finite number.",
     "Devuelve cierto en cada celda que contenga un numero finito "
     "conocido."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatIsFinite::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1)); 
  for(int i=0; i<contens_.Data().Size(); i++)
  {
    contens_.Data()[i] = contens_.Data()[i].IsFinite();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatIsPosInf);
DefIntOpr(1, BMatIsPosInf, "IsPosInf", 1, 1,
  "(Matrix mat)",
  I2("Returns true in each cell containing positive infinity.",
     "Devuelve cierto en cada celda que contenga el infinito "
     "positivo."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatIsPosInf::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1)); 
  for(int i=0; i<contens_.Data().Size(); i++)
  {
    contens_.Data()[i] = contens_.Data()[i].IsPosInf();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatIsNegInf);
DefIntOpr(1, BMatIsNegInf, "IsNegInf", 1, 1,
  "(Matrix mat)",
  I2("Returns true in each cell containing negative infinity.",
     "Devuelve cierto en cada celda que contenga el infinito "
     "negativo."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatIsNegInf::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1)); 
  for(int i=0; i<contens_.Data().Size(); i++)
  {
    contens_.Data()[i] = contens_.Data()[i].IsNegInf();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatIsUnknown);
DefIntOpr(1, BMatIsUnknown, "IsUnknown", 1, 1,
  "(Matrix mat)",
  I2("Returns true in each cell containing an unknown number.",
     "Devuelve cierto en cada celda que contenga un numero "
     "desconocido."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatIsUnknown::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1)); 
  for(int i=0; i<contens_.Data().Size(); i++)
  {
    contens_.Data()[i] = contens_.Data()[i].IsUnknown();
  }
}

typedef  BDat (*BDatBinary)(const BDat&, const BDat&);

//--------------------------------------------------------------------
static void ApplyFunToList(BDatBinary fun,
                           const BList* args,
                           BMat& a)

/*! Applies a two real numbers function to all cells of a list of 
 *! matrices
 */
//--------------------------------------------------------------------
{
  if(!args) { return; }
  BInt i,j;
  BInt  n;
  a = Mat(args->Car());
  if(!a.Rows()||!a.Columns()) { return; }
  BDat dat;
  args=Cdr(args);
  for(n=2; args; n++, args=Cdr(args))
  {
    BMat& b = Mat(args->Car());
    if((a.Rows()!=b.Rows())||(a.Columns()!=b.Columns())) { break; }
    for(i=0; i<a.Rows(); i++)
    {
      for(j=0; j<a.Columns(); j++)
      {
        a(i,j) = (*fun)(a(i,j), b(i,j));
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatMin);
DefIntOpr(1, BMatMin, "Min", 1, 0,
  "(Matrix M1 [, Matrix M2, ...])",
  I2("Returns the minimum of all arguments.",
     "Devuelve el minimo de todos los argumentos."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatMin::CalcContens()
//--------------------------------------------------------------------
{
  ApplyFunToList(Minimum,ArgList(),contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatMax);
DefIntOpr(1, BMatMax, "Max", 1, 0,
  "(Matrix M1 [, Matrix M2, ...])",
  I2("Returns the maximum of all arguments.",
     "Devuelve el maximo de todos los argumentos."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatMax::CalcContens()
//--------------------------------------------------------------------
{
  ApplyFunToList(Maximum,ArgList(),contens_);
}

typedef  BDat& (BDat::*BDatAutoBinary)(const BDat&);
//--------------------------------------------------------------------
static void ApplyAutoFunToList(BDatAutoBinary fun,
                               const BList* args,
                               BMat& a)

/*! Applies a two real numbers function to all cells of a list of 
 *! matrices
 */
//--------------------------------------------------------------------
{
  if(!args) { return; }
  BInt i,j;
  BInt  n;
  a = Mat(args->Car());
  if(!a.Rows()||!a.Columns()) { return; }
  BDat dat;
  args=Cdr(args);
  for(n=2; args; n++, args=Cdr(args))
  {
    BMat& b = Mat(args->Car());
    if((a.Rows()!=b.Rows())||(a.Columns()!=b.Columns())) { break; }
    for(i=0; i<a.Rows(); i++)
    {
      for(j=0; j<a.Columns(); j++)
      {
        (a(i,j).*fun)(b(i,j));
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatAnd);
DefIntOpr(1, BMatAnd, "And", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns the logic AND of all arguments.",
     "Devuelve el AND logico de todos los argumentos."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatAnd::CalcContens()
//--------------------------------------------------------------------
{
  ApplyAutoFunToList(&BDat::And,ArgList(),contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatOr);
DefIntOpr(1, BMatOr, "Or", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns the logic OR of all arguments.",
     "Devuelve el OR logico de todos los argumentos."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatOr::CalcContens()
//--------------------------------------------------------------------
{
  ApplyAutoFunToList(&BDat::Or,ArgList(),contens_);
}


//--------------------------------------------------------------------
void ApplyCmpToList(BDatBinary cmp,
                    const BList* args,
                    BMat& c)

/*! Applies a two real numbers comparisson function to all cells of a 
 *! list of matrices
 */
//--------------------------------------------------------------------
{
  if(!args) { return; }
  BInt i,j;
  BInt  n;
  BMat* a = &Mat(args->Car());
  if(!a->Rows()||!a->Columns()) { return; }
  c.Alloc(a->Rows(),a->Columns());
  if(c.Rows()!=a->Rows()) { return; }
  BDat dat;
  args=Cdr(args);
  for(i=0; i<a->Rows(); i++) for(j=0; j<a->Columns(); j++) c(i,j)=1;
  for(n=2; args; n++, args=Cdr(args))
  {
    BMat* b = &Mat(args->Car());
    if((a->Rows()!=b->Rows())||(a->Columns()!=b->Columns())) { break; }
    for(i=0; i<a->Rows(); i++)
    {
      for(j=0; j<a->Columns(); j++)
      {
        c(i,j).And((*cmp)((*a)(i,j), (*b)(i,j)));
      }
    }
    a = b;
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGt);
DefIntOpr(1, BMatGt, "GT", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns true if each argument is greater than the following.",
     "Devuelve cierto si cada argumento es mayor que el siguiente."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatGt::CalcContens()
//--------------------------------------------------------------------
{
  ApplyCmpToList(Gt,ArgList(),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatLt);
DefIntOpr(1, BMatLt, "LT", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns true if each argument is less than the following.",
     "Devuelve cierto si cada argumento es menor que el siguiente."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatLt::CalcContens()
//--------------------------------------------------------------------
{
  ApplyCmpToList(Lt,ArgList(),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGe);
DefIntOpr(1, BMatGe, "GE", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns true if each argument is greater or equal than the following.",
     "Devuelve cierto si cada argumento es mayor o igual que el siguiente."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatGe::CalcContens()
//--------------------------------------------------------------------
{
  ApplyCmpToList(Ge,ArgList(),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatLe);
DefIntOpr(1, BMatLe, "LE", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns true if each argument is less or equal than the following.",
     "Devuelve cierto si cada argumento es menor o igual que el siguiente."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatLe::CalcContens()
//--------------------------------------------------------------------
{
  ApplyCmpToList(Le,ArgList(),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatEq);
DefIntOpr(1, BMatEq, "Eq", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns true if each argument is equal than the following.\n"
     "Could also be called as EQ",
     "Devuelve cierto si cada argumento es igual que el siguiente.\n"
     "Tambien puede invocarse como EQ"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatEq::CalcContens()
//--------------------------------------------------------------------
{
  ApplyCmpToList(Eq,ArgList(),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatNe);
DefIntOpr(1, BMatNe, "NE", 2, 0,
  "(Matrix M1, Matrix M2 [, Matrix M3, ...])",
  I2("Returns true if each argument is not equal than the following.",
     "Devuelve cierto si cada argumento no es igual que el siguiente."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatNe::CalcContens()
//--------------------------------------------------------------------
{
  ApplyCmpToList(Ne,ArgList(),contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSum2);
DefExtOpr(1, BMatSum2, "+", 2, 2, "Matrix {Matrix|Real}",
  "mat1 + mat2 {Matrix mat1, {Matrix|Real} mat2}",
  I2("Returns the sum of two matrices with the same dimensions."
     "Second one may be also a real number",
     "Devuelve la suma de dos matrices con las mismas dimensiones."
     "El segundo argumento puede ser tambien un numero Real"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSum2::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraMatrix()) 
  { 
    b2dMat(contens_) = dMat(Arg(1)) + dMat(Arg(2)); 
  }
  else if(gra==GraReal()) 
  { 
    b2dMat(contens_) = dMat(Arg(1)) + Real(Arg(2)); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for Matrix +",
             "no es un tipo valido para Matrix +"));
    contens_ = BMat::Unknown();
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatDifference);
DefExtOpr(1, BMatDifference, "-", 2, 2,"Matrix {Matrix|Real}",
  "mat1 - mat2 {Matrix mat1, {Matrix|Real} mat2}",
  I2("Returns the subtraction of two matrixs with the same dimensions."
     "Second one may be also a real number",
     "Devuelve la resta de dos matrices con las mismas dimensiones."
     "El segundo argumento puede ser tambien un numero Real"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatDifference::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraMatrix()) 
  { 
    b2dMat(contens_) = dMat(Arg(1)) - dMat(Arg(2)); 
  }
  else if(gra==GraReal()) 
  { 
    b2dMat(contens_) = dMat(Arg(1)) - Real(Arg(2)); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for Matrix -",
             "no es un tipo valido para Matrix -"));
    contens_ = BMat::Unknown();
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatProduct2);
DefExtOpr(1, BMatProduct2, "*", 2, 2,"Matrix {Matrix|Real}",
  "mat1 * mat2 {Matrix mat1, {Matrix|Real} mat2}",
  I2("Returns the product of two matrixs such that the number of columns "
     "of the first matrix will be equal to the number of rows of the second."
     "Second one may be also a real number",
     "Devuelve el producto de dos matrices tales que el numero de "
     "columnas de la primera matriz sea igual al numero de filas "
     "de la segunda. "
     "El segundo argumento puede ser tambien un numero Real"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatProduct2::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra = Arg(2)->Grammar();
  if(gra==GraMatrix()) 
  { 
    b2dMat(contens_) = dMat(Arg(1)) * dMat(Arg(2)); 
  }
  else if(gra==GraReal()) 
  { 
    b2dMat(contens_) = dMat(Arg(1)) * Real(Arg(2)); 
  }
  else
  {
    Error(gra->Name()+" <"+Arg(2)->Identify()+">"+
          I2("is not a valid type for Matrix *",
             "no es un tipo valido para Matrix *"));
    contens_ = BMat::Unknown();
  }}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatMtMSqr);
DefIntOpr(1, BMatMtMSqr, "MtMSqr", 1, 1,
  "(Matrix mat)",
  I2("Returns the product of the transpose of a matrix by itself.",
     "Devuelve el producto de una matriz por si misma"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatMtMSqr::CalcContens()
//--------------------------------------------------------------------
{
  TolBlas::dsyrk(CblasLower,CblasTrans,1.0,Mat(Arg(1)),0.0,contens_);
  int i,j;
  for(i=0; i<contens_.Rows(); i++)
  {
    for(j=0; j<i; j++)
    {
      contens_(j,i) = contens_(i,j);
    }
  }
}

//--------------------------------------------------------------------
void DoWeightProd(DMat& contens_, const DMat& m1, DMat& m2)
//--------------------------------------------------------------------
{
  if((m1.Rows() == m2.Rows()) && (m1.Columns()==m2.Columns()))
  {
    contens_ = m1;
    for(BInt i=0; i<m1.Rows(); i++)
    {
      for(BInt j=0; j<m1.Columns(); j++)
      {
        contens_(i,j) *= m2(i,j);
      }
    }
  }
  else
  {
    Error(I2("Wrong dimensions in Matrix WeightProd function or $* operator",
             "Dimensiones erroneas para la funcion de matrices WeightProd o el operador $*")+
          ": ("+m1.Rows()+"x"+m1.Columns()+")$*("+m2.Rows()+"x"+m2.Columns()+")");
    contens_ = DMat::Unknown();
  }
}

//--------------------------------------------------------------------
void DoWeightQuotient(DMat& contens_, const DMat& m1, DMat& m2)
//--------------------------------------------------------------------
{
  if((m1.Rows() == m2.Rows()) && (m1.Columns()==m2.Columns()))
  {
    contens_ = m1;
    for(BInt i=0; i<m1.Rows(); i++)
    {
      for(BInt j=0; j<m1.Columns(); j++)
      {
        contens_(i,j) /= m2(i,j);
      }
    }
  }
  else
  {
    Error(I2("Wrong dimensions in Matrix WeightQuotient function or $* operator",
             "Dimensiones erroneas para la funcion de matrices WeightQuotient o el operador $*")+
          ": ("+m1.Rows()+"x"+m1.Columns()+")$/("+m2.Rows()+"x"+m2.Columns()+")");
    contens_ = DMat::Unknown();
  }
}



//--------------------------------------------------------------------
static BText WeightProdDescription()
//--------------------------------------------------------------------
{
  return 
  I2("Returns the cell to cell product of matricess mat1[i,j]*mat2[i,j]."
     "Both m1 and m2 must have same dimensions." 
     "Next both expressions are identical and valids in TOL mat1$*mat2==WeightProd(mat1,mat2)",
     "Devuelve la matriz de productos celda a celda mat1[i,j]*mat2[i,j]."
     "Ambas matrices deben tener las mismas dimensiones."
     "Las siguientes dos expresiones son identicas y validas en TOL mat1$*at2==WeightProd(mat1,mat2)");
};

//--------------------------------------------------------------------
static BText WeightQuotientDescription()
//--------------------------------------------------------------------
{
  return 
  I2("Returns the cell to cell quotient of matricess mat1[i,j]/mat2[i,j]."
     "Both m1 and m2 must have same dimensions." 
     "Next both expressions are identical and valids in TOL mat1$/mat2==WeightQuotient(mat1,mat2)",
     "Devuelve la matriz de cocientes celda a celda mat1[i,j]/mat2[i,j]."
     "Ambas matrices deben tener las mismas dimensiones."
     "Las siguientes dos expresiones son identicas y validas en TOL mat1$/at2==WeightQuotient(mat1,mat2)");
};

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatWeightProd);
DefExtOpr(1, BMatWeightProd, "WeightProd", 2, 2, "Matrix Matrix",
  "(Matrix mat1, Matrix mat2)",
  WeightProdDescription(),
    BOperClassify::MatrixAlgebra_);
DefExtOpr(2, BMatWeightProd, "$*", 2, 2, "Matrix Matrix",
  "mat1 $* mat2 {Matrix mat1, Matrix mat2}",
  WeightProdDescription(),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatWeightProd::CalcContens()
//--------------------------------------------------------------------
{
  DoWeightProd(b2dMat(contens_),dMat(Arg(1)),dMat(Arg(2)));
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatWeightQuotient);
DefExtOpr(1, BMatWeightQuotient, "WeightQuotient", 2, 2, "Matrix Matrix",
  "(Matrix mat1, Matrix mat2)",
  WeightQuotientDescription(),
    BOperClassify::MatrixAlgebra_);
DefExtOpr(2, BMatWeightQuotient, "$/", 2, 2, "Matrix Matrix",
  "mat1 $/ mat2 {Matrix mat1, Matrix mat2}",
  WeightQuotientDescription(),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatWeightQuotient::CalcContens()
//--------------------------------------------------------------------
{
  DoWeightQuotient(b2dMat(contens_),dMat(Arg(1)),dMat(Arg(2)));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSqRt);
DefIntOpr(1, BMatSqRt, "SqRt", 1, 1, "(Matrix mat)",
    I2("Returns the square root of each element of a matrix.",      
       "Devuelve la raiz cuadrada de cada elemento de una matriz."),
    BOperClassify::MatrixAlgebra_);
void BMatSqRt::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Mat(Arg(1));  
    for(BInt i=0; i<contens_.Data().Size(); i++)
    {
  contens_.Data()(i).Sqrt();
    }
}

#define DecMonary(FUN,ENGLISH,SPANISH)               \
                       \
DeclareContensClass(BMat, BMatTemporary, BMat##FUN);           \
DefIntOpr(1, BMat##FUN, #FUN, 1, 1,               \
"(Matrix mat)",                     \
I2("Returns "ENGLISH" of each element of a matrix.",           \
  "Devuelve "SPANISH" de cada elemento de una matriz."),         \
BOperClassify::MatrixAlgebra_);                 \
                       \
void BMat##FUN::CalcContens()                 \
{                       \
  contens_ = Mat(Arg(1));                  \
  for(BInt i=0; i<contens_.Data().Size(); i++)               \
  {                       \
    contens_.Data()(i).FUN();                 \
  }                       \
}

DecMonary(Log,"the natural logarithm","el logaritmo natural");
DecMonary(Log10,"the decimal logarithm","el logaritmo decimal");
DecMonary(Exp,"the exponential","la exponencial");
DecMonary(Sqrt,"the square root","la raiz cuadrada");
DecMonary(Sign,"the sign","el signo");
DecMonary(Abs,"the absolute value","el valor absoluto");
DecMonary(Floor,"the truncated value","el valor truncado");
DecMonary(Round,"the rounded value","el valor redondeado");
DecMonary(Sin,"the trygonometric sine","el seno trigonometrico");
DecMonary(Cos,"the trygonometric cosine","el coseno trigonometrico");
DecMonary(Tan,"the trygonometric tangent","la tangente trigonometrica");
DecMonary(ASin,"the trygonometric arcsine","el arcoseno trigonometrico");
DecMonary(ACos,"the trygonometric arcosine","el arcocoseno trigonometrico");
DecMonary(ATan,"the trygonometric arctangent","la arcotangente trigonometrica");
DecMonary(SinH,"the hypergeometric sine","el seno hipergeometrico");
DecMonary(CosH,"the hypergeometric cosine","el coseno hipergeometrico");
DecMonary(TanH,"the hypergeometric tangent","la tangente hipergeometrica");
DecMonary(ASinH,"the hypergeometric arcsine","el arcoseno hipergeometrico");
DecMonary(ACosH,"the hypergeometric arcosine","el arcocoseno hipergeometrico");
DecMonary(ATanH,"the hypergeometric arctangent","la arcotangente hipergeometrica");

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatf01Real);
DefExtOpr(1, BMatf01Real, "f01", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the standard normal density function of each cell in a matrix.",
    "Devuelve la funcion de densidad normal estandar de cada celda de una matriz."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatf01Real::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1));
  for(BInt i=0; i<contens_.Data().Size(); i++)
  {
    contens_.Data()(i) = BNormalDist::Dens01(contens_.Data()(i));
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatF01Real);
DefExtOpr(1, BMatF01Real, "F01", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the standard normal distribution function of each cell in a matrix.",
    "Devuelve la funcion de distribucion normal estandar de cada celda de una matriz."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatF01Real::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1));
  for(BInt i=0; i<contens_.Data().Size(); i++)
  {
    contens_.Data()(i) = BNormalDist::Dist01(contens_.Data()(i));
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatInvF01Real);
DefExtOpr(1, BMatInvF01Real, "InvF01", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the standard normal inverse distribution function of each cell in a matrix.",
    "Devuelve la funcion inversa de la distribucion normal estandar de cada celda de una matriz."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatInvF01Real::CalcContens()
//--------------------------------------------------------------------
{
  static BNormalDist nor(0, 1);
  contens_ = Mat(Arg(1));
  for(BInt i=0; i<contens_.Data().Size(); i++)
  {
    contens_.Data()(i) = nor.Inverse(contens_.Data()(i));
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatIfMatReal);
DefExtOpr(1, BMatIfMatReal, "IfMat", 3, 3, "Matrix Matrix Matrix",
  "(Matrix condition, Matrix A, Matrix B)",
  I2("For each cell (i,j) if condition(i,j) is true returns the value of A(i,j), else B(i,j).",
     "Para cada celda (i,j) si condition(i,j) es cierto devuelve A(i,j) y si no B(i,j)."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatIfMatReal::CalcContens()
//--------------------------------------------------------------------
{
  BMat& C = Mat(Arg(1));
  BMat& A = Mat(Arg(2));
  BMat& B = Mat(Arg(3));
  if((C.Rows   ()!=A.Rows   ()) || (C.Rows   ()!=B.Rows   ()) ||
     (C.Columns()!=A.Columns()) || (C.Columns()!=B.Columns())    )
  {
    Error(I2("Invalid dimensions in IfMat function.",
             "Dimensiones no validas en la funcion IfMat."));
    contens_ = BMat::Unknown();
    return;
  }
  contens_.Alloc(C.Rows(),C.Columns());
  if(contens_.Rows()!=C.Rows()) { return; }
  for(BInt i=0; i<C.Data().Size(); i++)
  {
    if(C.Data()(i).IsKnown())
    {
      if(C.Data()(i)!=0)
      {
        contens_.Data()(i) = A.Data()(i);
      }
      else
      {
        contens_.Data()(i) = B.Data()(i);
      }
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPowReal);
DefExtOpr(1, BMatPowReal, "RPow", 2, 2, "Matrix Real",
  "(Matrix mat, Real x)",
  I2("Returns the power of each element of a matrix to a real number.",
    "Devuelve la potencia de cada elemnto de una matriz a un numero real."),
    BOperClassify::MatrixAlgebra_);
DefExtOpr(2, BMatPowReal, "^", 2, 2, "Matrix Real",
  "mat ^ x {Matrix mat, Real x}",
  I2("Returns the power of each element of a matrix to a real number.",
    "Devuelve la potencia de cada elemnto de una matriz a un numero real."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatPowReal::CalcContens()
//--------------------------------------------------------------------
{
  DMat&   m = dMat(Arg(1));
  double  d = Real(Arg(2));
  b2dMat(contens_) = m;
  for(BInt i=0; i<m.Data().Size(); i++)
  {
    double& x = b2dMat(contens_).Data()(i);
    x = pow(x, d);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatProductReal);
DefExtOpr(1, BMatProductReal, "RProd", 2, 2, "Matrix Real",
  "(Matrix mat, Real x)",
  I2("Returns the product of each element of a matrix by a real number.",
    "Devuelve el producto de cada elemento de una matriz por un numero real."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatProductReal::CalcContens()
//--------------------------------------------------------------------
{
  DMat&   m = dMat(Arg(1));
  double  d = Real(Arg(2));
  b2dMat(contens_) = m * d;
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSumReal);
DefExtOpr(1, BMatSumReal, "RSum", 2, 2, "Matrix Real",
  "(Matrix mat, Real x)",
  I2("Returns the summ of each element of a matrix and a real number.",
     "Devuelve la suma de cada elemnto de una matriz y un numero real."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSumReal::CalcContens()
//--------------------------------------------------------------------
{
  DMat& m = dMat(Arg(1));
  double d = Real(Arg(2));
  b2dMat(contens_) = m+d;
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSetSum);
DefExtOpr(1, BMatSetSum, "SetSum", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the global summatory of all matrices of a set.",
     "Devuelve el sumatorio de todas las matrices de un conjunto."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSetSum::CalcContens()
//--------------------------------------------------------------------
{
  DMat & C = b2dMat(contens_);
  BSet& set = Set(Arg(1));
  if(!set.Card()) { return; }
  bool init = false;
  for(BInt n=1; n<=set.Card(); n++)
  {
    if(set[n]->Grammar()==GraMatrix())
    {
      if(!init) { C  = dMat(set[n]); init = true; }
      else      { C += dMat(set[n]); }
      if(!contens_.Rows() || !contens_.Columns()) 
      {
        return;
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatDiag);
DefExtOpr(1, BMatDiag, "Diag", 2, 3, "Real Real Real",
  I2("(Real n, Real x, [Real direction=1])",
     "(Real n, Real x, [Real direccion=1])"),
  I2("Returns a diagonal matrix of dimension n and diagonal values all "
     "equal to x.",
     "Devuelve una matriz diagonal de dimension n y valores diagonales todos "
     "iguales a x."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatDiag::CalcContens()
//--------------------------------------------------------------------
{
  DMat & C = b2dMat(contens_);
  BInt i,j;
  BInt n = (BInt)Real(Arg(1));
  double val = Real(Arg(2));
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
      else           { C(i,j) = 0.0; }
    }
  }    
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatColumnPivots);
DefExtOpr(1, BMatColumnPivots, "ColumnPivots", 1, 1, "Matrix",
  "(Matrix x)",
  I2("Returns a matrix with the index of the element of maximum "
     "absolute value for each column.",
     "Devuelve una matriz con el indice del elemento de maximo valor "
     "absoluto para cada columna."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatColumnPivots::CalcContens()
//--------------------------------------------------------------------
{
  DMat & C = b2dMat(contens_);
  DMat& A = dMat(Arg(1));
  BArray<BInt> pivots;
  ColumnPivots(A,pivots);
  contens_.Alloc(1,A.Columns());
  if(contens_.Rows()!=1) { return; }
  for(BInt i=0; i<A.Columns(); i++)
  {
    C(0,i) = pivots(i)+1;
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRowPivots);
DefExtOpr(1, BMatRowPivots, "RowPivots", 1, 1, "Matrix",
  "(Matrix x)",
  I2("Returns a matrix with the index of the element of maximum "
     "absolute value for each column.",
     "Devuelve una matriz con el indice del elemento de maximo valor "
     "absoluto para cada columna."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRowPivots::CalcContens()
//--------------------------------------------------------------------
{
  DMat & C = b2dMat(contens_);
  DMat& A = dMat(Arg(1));
  BArray<BInt> pivots;
  RowPivots(A,pivots);
  contens_.Alloc(A.Rows(),1);
  if(contens_.Rows()!=A.Rows()) { return; }
  for(BInt i=0; i<A.Rows(); i++)
  {
    C(i,0) = pivots(i)+1;
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatTrasposePivots);
DefExtOpr(1, BMatTrasposePivots, "TrasposePivots", 2, 2, "Matrix Real",
  "(Matrix p, Real dim)",
  I2("Returns a matrix with the index of element the element of maximum "
     "absolute value para each column.",
     "Devuelve una matriz con el indice del elemento de maximo valor "
     "absoluto para cada columna."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatTrasposePivots::CalcContens()
//--------------------------------------------------------------------
{
    BMat& P = Mat(Arg(1));
    BInt  d = (BInt)Real(Arg(2));
    BInt  n = P.Rows();
    BInt  m = P.Columns();
    TRZ(P);
    if(n==1)
    {
//Std("\nTRZ 1");
  contens_.Alloc(d,1);
  if(contens_.Rows()!=d) { return; }
  contens_.SetAllValuesTo(0);
//Std("\nTRZ 2");
  for(BInt i=0; i<m; i++)
  {
    //Std(BText("\ni=")+i);
    //Std(BText("\nPi=")+P(0,i));
      BInt j = (BInt)(P(0,i).Value())-1;
    //Std(BText("\nj=")+j);
      contens_(j,0) = i+1;
  }
    }
    if(m==1)
    {
  contens_.Alloc(1,d);
  if(contens_.Rows()!=1) { return; }
  contens_.SetAllValuesTo(0);
  for(BInt i=0; i<n; i++)
  {
      BInt j = (BInt)(P(i,0).Value())-1;
      contens_(0,j) = i+1;
  }
    }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPivotByColumns);
DefExtOpr(1, BMatPivotByColumns, "PivotByColumns", 2, 2, "Matrix Matrix",
  "(Matrix x, Matrix p)",
  I2("Returns the matrix pivoted by columns.",
     "Devuelve la matriz x pivotada por columnas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatPivotByColumns::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Mat(Arg(1));
    BMat& P = Mat(Arg(2));
    BArray<BInt> pivots(P.Columns());
    for(BInt i=0; i<P.Columns(); i++)
    {
  pivots(i) = (BInt)(P(0,i).Value())-1;
    }
    PivotByColumns(contens_,pivots);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPivotByRows);
DefExtOpr(1, BMatPivotByRows, "PivotByRows", 2, 2, "Matrix Matrix",
  "(Matrix x, Matrix p)",
  I2("Returns the matrix pivoted by rows.",
     "Devuelve la matriz x pivotada por filas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatPivotByRows::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1));
  BMat& P  = Mat(Arg(2));
  BArray<BInt> pivots(P.Rows());
  for(BInt i=0; i<P.Rows(); i++)
  {
    pivots(i) = (BInt)(P(i,0).Value())-1;
  }
  PivotByRows(contens_,pivots);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatInversePermutation);
DefExtOpr(1, BMatInversePermutation, "InversePermutation", 1, 1, "Matrix",
  "(Matrix p)",
  I2("Given a column or row of any realignment of integers numbers "
     "1 ... N returns the inverse permutation. ",
     "Dada una fila o columna con una reordenación cualquiera de "
     "los enteros 1 ... N, devuelve la permutación inversa."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatInversePermutation::CalcContens()
//--------------------------------------------------------------------
{
  BMat& P  = Mat(Arg(1));
  int i, k, n = P.Data().Size();
  int pr = P.Rows();
  int pc = P.Columns();
  if((pr!=1) && (pc!=1))
  { 
    Error(I2("Cannot apply InversePermutation to non vectorial matriz ",
               "No se puede aplicar InversePermutation a la matriz no vectorial ")+
            "p("+pr+"x"+pc+") ");
    return; 
  }
//Std(BText("\nTRACE [InversePermutation] 1 n=")+pr);
//Std(BText("\nTRACE [InversePermutation] 2 pr=")+pr);
//Std(BText("\nTRACE [InversePermutation] 3 pc=")+pr);
  contens_.Alloc(pr,pc);
  const double* p = (const double*)P.Data().Buffer();
  double* c = (double*)contens_.GetData().GetBuffer();
  for(i=0; i<n; i++)
  {
    c[k-1] = 0.0;    
  }
  for(i=1; i<=n; i++, p++)
  {
    k = (int)(*p);
  //Std(BText("\nTRACE [InversePermutation] 4.")+i+" k="<<k);
    if(*p!=k)
    {
      Error(I2("Cannot apply InversePermutation to a matrix with non integer cells ",
               "No se puede aplicar InversePermutation a una matriz con celdas o enteras ")+
      (*p)+"!= "+k+"]");
    }
    else if((k<1)||(k>n))
    {
      Error(I2("Cannot apply InversePermutation to a matrix with cells out of range ",
               "No se puede aplicar InversePermutation a una matriz con celdas fuera de rango ")+
      k + "!<: [1, ..., "+n+"]");
      c[k-1] = BDat::Nan();
    }
    else if(c[k-1]!=0.0)
    {
      Error(I2("Cannot apply InversePermutation to a matrix with repeated cells ",
               "No se puede aplicar InversePermutation a una matriz con celdas repetidas ")+
      c[k-1] + " <-> "+i+"]");
    }
    else
    {
      c[k-1] = i;
    }
  //Std(BText("\n[InversePermutation] ")+k+" -> "+i)
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatLinComb);
DefIntOpr(1, BMatLinComb, "LinComb", 1, 1,
  "(Matrix mat)",
  I2("Returns the equations of the linear combinations of the "
     "rows of a matrix that are annulled, if is tried a regular matrix, "
     "or the empty matrix in other case.",
     "Devuelve las ecuaciones de las combinaciones lineales de las "
     "filas de una matriz que se anulan, si se trata una matriz regular, "
     "o la matriz vacia en otro caso."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatLinComb::CalcContens()
//--------------------------------------------------------------------
{
  Mat(Arg(1)).LinCombSpace(contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGaussReduction);
DefExtOpr(1, BMatGaussReduction, "GaussReduction", 2, 2, "Matrix Real",
  "(Matrix mat, Real n)",
  I2("Applies the reduction method of Gauss to the first n columns of "
     "a matrix.",
     "Aplica el metodo de reduccion de Gauss a las n primeras columnas de "
     "una matriz."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatGaussReduction::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1));
  contens_.GaussReduction((BInt)Real(Arg(2)));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSolve);
DefIntOpr(1, BMatSolve, "Solve", 2, 2,
  "(Matrix M, Matrix B)",
  I2("Applies the reduction method of Gauss to solve the linear system M*X=B.",
     "Aplica el metodo de reduccion de Gauss para resolver el sistema "
     "lineal M*X=B."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSolve::CalcContens()
//--------------------------------------------------------------------
{
  Mat(Arg(1)).Solve(Mat(Arg(2)), contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatCholeskiMinimumResiduals);
DefExtOpr(1, BMatCholeskiMinimumResiduals, "CholeskiMinimumResiduals", 
  2, 2, "Matrix Matrix", 
  "(Matrix M, Matrix B)",
  I2("Applies the Cholesky decomposition to solve X in the linear "
     "regresion M*X=B+e ",
     "Aplica la descomposición de Cholesky para resolver X en la "
     "regresión lineal M*X=B+e."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatCholeskiMinimumResiduals::CalcContens()
//--------------------------------------------------------------------
{
  const BMat& M = Mat(Arg(1));
  const BMat& B = Mat(Arg(2));
  contens_=CholeskiMinimumResidualsSolve(M,B);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatMinimumResidualsSolve);
DefExtOpr(1, BMatMinimumResidualsSolve, "MinimumResidualsSolve", 
  2, 5, "Matrix Matrix Real Matrix Real", 
  BText("(Matrix M, Matrix B "
  "[, Real chop=")+Sqrt(DEpsilon())+", "+
     "Matrix X0=Tra(M)*B,"+
     "Real maxIter=20*Columns(M)])",
  I2("Applies the Minimum Residuals method to solve the linear "
     "system M*X=B begining at initial value <X0> until error were "
     "less than <chop> or maximum iteration is reached.",
     "Aplica el metodo de los Minimos Residuos para resolver el "
     "sistema lineal M*X=B comenzando por el valor inicial <X0> y "
     "continuando hasta  que el error sea menor que <chop> o se "
     "supere el número de iteraciones indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatMinimumResidualsSolve::CalcContens()
//--------------------------------------------------------------------
{
  BMat& M = Mat(Arg(1));
  BMat& B = Mat(Arg(2));
  double chop = -1;
  int maxIter = 20*M.Columns();
  if(Arg(3)) { chop = Real(Arg(3)); }
  if(Arg(5)) { maxIter = (int)Real(Arg(5)); }
  if(Arg(4)) 
  { 
    BMat& X0 = Mat(Arg(4));
    contens_ = MinimumResidualsSolve(M, B, X0, chop, maxIter);
  }
  else
  { 
    contens_ = MinimumResidualsSolve(M, B, chop, maxIter);
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatMultiFitLinear);
DefIntOpr(1, BMatMultiFitLinear, "MultiFitLinear", 2, 2,
  "(Matrix A, Matrix b)",
  I2("This function computes the best-fit parameters x of the model b = A x  \n"
     "for the observations b and the matrix of predictor variables A.        \n"
   "The best-fit is found by singular value decomposition of the matrix A. \n"
   "Arguments:                                                             \n"
   "A ---> matrix                                                          \n"
   "b ---> independent terms vector                                        \n",
     "Calcula la solucion de mejor ajuste del sistema b = A x.               \n"
   "El calculo emplea la descomposicion en valor singular de la matriz A.  \n"
   "Argumentos:                                                            \n"
   "A ---> matriz                                                          \n"
   "b ---> vector de terminos independientes                              \n"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatMultiFitLinear::CalcContens()
//--------------------------------------------------------------------
{
  BMat& A  = Mat(Arg(1));
  BMat& b  = Mat(Arg(2));
  contens_ = gsl_MinimumResidualsSolve(A, b);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatInverseDiag);
DefIntOpr(1, BMatInverseDiag, "InverseDiag", 1, 1,
  "(Matrix M)",
  I2("Returns the inverse of a diagonal matrix.",
     "Devuelve la inversa de una matriz diagonal"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatInverseDiag::CalcContens()
//--------------------------------------------------------------------
{
  BDiagMatrix<double> D;
  D = dMat(Arg(1));
  b2dMat(contens_) = D.I();
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPseudoInverseDiag);
DefExtOpr(1, BMatPseudoInverseDiag, "PseudoInverseDiag", 2, 2, "Matrix Real",
  I2("(Matrix M , Real chop )",
     "(Matrix M , Real corte)"),
  I2("Returns the pseudo-inverse of a diagonal matrix by inverting all "
     "elements wich absolute value is greater than or equal to the given "
     "chop value.",
     "Devuelve la pseudo-inversa de una matriz diagonal inviertiendo los "
     "elementos de la diagonal con valor absoluto mayor o igual que el valor "
     "de corte dado."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatPseudoInverseDiag::CalcContens()
//--------------------------------------------------------------------
{
  BDiagMatrix<double> D;
  D = dMat(Arg(1));
  double chop = Real(Arg(2));
  b2dMat(contens_) = D.P(chop);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGaussInverse);
DefIntOpr(1, BMatGaussInverse, "GaussInverse", 1, 1,
  "(Matrix M)",
  I2("Applies the reduction method of Gauss to inverse a square matrix.",
     "Aplica el metodo de reduccion de Gauss para invertir una matriz "
     "cuadrada."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatGaussInverse::CalcContens()
//--------------------------------------------------------------------
{
  DMat&  A = dMat(Arg(1));
  DMat   B = BDiagMatrix<double> (A.Rows(),1.0);
  A.Solve(B, b2dMat(contens_));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatLTInverse);
DefIntOpr(1, BMatLTInverse, "LTInverse", 1, 1,
  "(Matrix L)",
  I2("Applies the reduction method of Gauss to inverse a lower triangular "
     "square matrix.  For more general triangular systems use function TrSolve."
     "passing Diag(n,1) as B argument.",
     "Aplica el metodo de reduccion de Gauss para invertir una matriz "
     "cuadrada triangular inferior. Para sistemas triangulares más generales "
     "use TrSolve pasando Diag(n,1) como argumento B"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatLTInverse::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& L = Mat(Arg(1));
  BMatrix<BDat>  I = BDiagMatrix<BDat>(L.Rows(),1);
  int res = TolBlas::dtrsm(CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, 
                           1.0, L, I, contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatLTSolve);
DefIntOpr(1, BMatLTSolve, "LTSolve", 2, 2,
  "(Matrix L, Matrix B)",
  I2("Applies the reduction method of Gauss to solve the linear system L*X=B"
     "where L is a low triangular matrix. For more general triangular systems "
     "use function TrSolve.",
     "Aplica el metodo de reduccion de Gauss para resolver el sistema "
     "lineal L*X=B donde L es triangular inferior. Para sistemas triangulares "
     "más generales use TrSolve"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatLTSolve::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& L = Mat(Arg(1));
  BMatrix<BDat>& B = Mat(Arg(2));
  BDat alpha = 1.0;
  int res = TolBlas::dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasNonUnit,alpha,
                           L,B,contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatTrSolve);
DefExtOpr(1, BMatTrSolve, "TrSolve", 2, 5, "Matrix Matrix Real Real Real",
"(Matrix T, Matrix B [, Real left=1, Real trans=0, Real lower=1])",
I2("Solve a triangular system of equations."
   "Depending on the values of the arguments left, trans and lower "
   "different type of systems can be solved.\n"
   "If lower is true then a lower triangular system is solved otherwise an "
   "upper systems is solved:\n\n"
   "L*X=B  if (left=1,trans=0,lower=1)\n"
   "L'*X=B if (left=1,trans=1,lower=1)\n"
   "X*L=B  if (left=0,trans=0,lower=1)\n"
   "X*L'=B if (left=0,trans=1,lower=1)\n\n"
   "U*X=B  if (left=1,trans=0,lower=0)\n"
   "U'*X=B if (left=1,trans=1,lower=0)\n"
   "X*U=B  if (left=0,trans=0,lower=0)\n"
   "X*U'=B if (left=0,trans=1,lower=0)",
   "Resuelve un sistema de ecuaciones triangular."
   "Es posible resolver diferentes tipos de sistemas dependiendo de los "
   "valores que tomen los argumentos opciones left, trans y lower.\n"
   "Si lower es verdadero entonces se resuelve un sistema triangular inferior "
   "en otro caso se resuelve un sistema triangular superior:\n\n"
   "L*X=B  si (left=1,trans=0,lower=1)\n"
   "L'*X=B si (left=1,trans=1,lower=1)\n"
   "X*L=B  si (left=0,trans=0,lower=1)\n"
   "X*L'=B si (left=0,trans=1,lower=1)\n\n"
   "U*X=B  si (left=1,trans=0,lower=0)\n"
   "U'*X=B si (left=1,trans=1,lower=0)\n"
   "X*U=B  si (left=0,trans=0,lower=0)\n"
   "X*U'=B si (left=0,trans=1,lower=0)"),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatTrSolve::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& T = Mat(Arg(1));
  BMatrix<BDat>& B = Mat(Arg(2));
  if(!T.Columns()||!T.Rows()||
     !B.Columns()||!B.Rows()||
      T.Columns()!= B.Rows())
  { 
    Warning(I2("Cannot apply TrSolve to matrices ",
               "No se puede aplicar TrSolve a las matrices ")+
            "T("+T.Rows()+"x"+T.Columns()+") "
            "B("+B.Rows()+"x"+B.Columns()+") ");
    return; 
  }
  //BLAS port
  CBLAS_SIDE side = CblasLeft;
  CBLAS_UPLO uplo = CblasLower;
  CBLAS_TRANSPOSE trans = CblasNoTrans;
  CBLAS_DIAG diag = CblasNonUnit;
  BDat alpha = 1.0;
  // left 
  if (Arg(3)) {
    BDat &left = Dat(Arg(3));
    side = left.Value() ? CblasLeft : CblasRight;
    // trans 
    if (Arg(4)) {
      BDat &_trans = Dat(Arg(4));
      trans = _trans.Value() ? CblasTrans : CblasNoTrans ;
      // lower
      if (Arg(5)) {
        BDat &lower = Dat(Arg(5));
        uplo = lower.Value() ? CblasLower : CblasUpper;
      }
    }
  }
  int res = TolBlas::dtrsm(side,uplo,trans,diag,alpha,T,B,contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatShermanMorrisonPseudoInverse);
DefIntOpr(1, BMatShermanMorrisonPseudoInverse,
  "ShermanMorrisonPseudoInverse", 1, 1, "(Matrix M)",
  I2("Apply the method of Sherman-Morrison to inverse a sparse matrix "
     "without zeros on the diagonal.",
     "Aplica el metodo de Sherman-Morrison para invertir una matriz rala "
     "sin ceros en la diagonal."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatShermanMorrisonPseudoInverse::CalcContens()
//--------------------------------------------------------------------
{
  b2dMat(contens_) = ShermanMorrisonPseudoInverse(dMat(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatShermanMorrisonInverse);
DefIntOpr(1, BMatShermanMorrisonInverse, "ShermanMorrisonInverse", 1, 1,
  "(Matrix M)",
  I2("Apply the method of Sherman-Morrison to inverse a sparse matrix "
     "without zeros on the diagonal.",
     "Aplica el metodo de Sherman-Morrison para invertir una matriz rala "
     "sin ceros en la diagonal."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatShermanMorrisonInverse::CalcContens()
//--------------------------------------------------------------------
{
  b2dMat(contens_) = ShermanMorrisonInverse(dMat(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatCholeskiInverse);
DefIntOpr(1, BMatCholeskiInverse, "CholeskiInverse", 1, 1,
  "(Matrix M)",
  I2("Apply the factoring method of Choleski to inverse a positive "
     "defined and symmetrical matrix.",
     "Aplica el metodo de factorizacion de Choleski para invertir una matriz "
     "simetrica y definida positiva."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatCholeskiInverse::CalcContens()
//--------------------------------------------------------------------
{
  BMat& A = Mat(Arg(1));
  BMat  L;
  int res1 = TolLapack::dpotrf(CblasLower,A,L);
  int res2 = TolLapack::dpotri(CblasLower,L,contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatCholeski);
DefIntOpr(1, BMatCholeski, "Choleski", 1, 1,
  "(Matrix M)",
  I2("Returns the inferior triangular matrix of Choleski "
     "for a positive defined and symmetrical matrix.",
     "Devuelve la matriz triangular inferior de Choleski de una matriz "
     "simetrica y definida positiva."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatCholeski::CalcContens()
//--------------------------------------------------------------------
{
  //LAPACK port
  BMat& A = Mat(Arg(1));
  int res = TolLapack::dpotrf(CblasLower,A,contens_);
  bool ok = res==0;
  if(!ok)
  {
    Warning(I2("Numerical fail in Choleski",
               "Error numerico en Choleski "));
    Std(BText("\nCholeski LAPACK port res=")+res);
  //Std(BText("\nA=\n")+A.Name());
  //Std(BText("\nL=\n")+contens_.Name());
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatLBD);
DefExtOpr(1, BMatLBD, "LBD", 1, 2, "Matrix Matrix",
  "(Matrix A [, Matrix b])",
  I2("Returns the Lanczos Bidiagonalization with partial reorthogonalization "
     "of A"
     "\n"
     "        A = U * B * V' \n"
     "\n"
     "where U and V are both orthonormal and B is a bidiagonal matrix "
     "with original point at b. \n"
     "If A is a R^(mxn) matrix with m>=n and b is R^(mx1) column matrix \n"
     "this function founds tree matrix U in R^(mxn+1), B in R^(n+1,n) and "
     "V in R^(n,n) thats \n"
     "\n"
     "        U' * U = I  \n"
     "        V' * V = I  \n"
     ,
     "Devuelve la bidiagonalizacion de Lanczos con reortogonalizacion parcial "
     "de A "
     "\n"
     "        A = U * B * V' \n"
     "\n"
     " donde U y V son ortonormales y B es una matriz bidiagonal inferior\n"
     "\n"
     "        U' * U = I  \n"
     "        V' * V = I  \n"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatLBD::CalcContens()
//--------------------------------------------------------------------
{
  BMat& A = Mat(Arg(1));
  BMat U, B, V;
  BDat anorm;
  if(Arg(2))
  {
    BMat& b = Mat(Arg(2));
    LanczosBidiagonalization(A,b,U,B,V,anorm);
  }
  else
  {
    LanczosBidiagonalization(A,U,B,V,anorm);
  }
  BUserMat* U_ = BContensMat::New("U",U,"Left Krilov vectors");
  BUserMat* B_ = BContensMat::New("B",B,"Bidiagonal matrix");
  BUserMat* V_ = BContensMat::New("V",V,"Left Krilov vectors");
  BList* lst   = NCons(V_);
  contens_.RobElement(Cons(U_,Cons(B_,lst)));
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatSVD);
DefExtOpr(1, BMatSVD, "SVD", 1, 2, 
    "Matrix Text",
    "(Matrix A [, Text method])",
  I2("Computes the Singular Value Decomposition of A                                   \n"
     "        A = U * D * V',                                                         \n"
     "where U and V are both orthonormal matrices and D is a diagonal matrix.        \n\n"
   "Arguments:                                                                       \n"
   "A      ---> matrix to be factorized,"
   "method ---> specifies the algorithm used to obtain the decomposition.            \n"
   "            It can take on of the following values:                              \n"
   "             - Golub_Reinsch, \n"
   "             - Golub_Reinsch_Mod (default. rectangular matrices)\n"
   "             - Jacobi.\n"
   "             - Sparse",
     "Devuelve la descomposicion de valor singular (Singular Value Decomposition) de A \n"
     "        A = U * D * V',                                                        \n"
     "donde U y V son matrices ortonormales y D es una matriz diagonal.                \n"
   "Argumentos:                                                                      \n"
   "A      ---> matriz a factorizar,                                                 \n"
   "method ---> especifica el metodo que se aplica en la descomposicion.             \n"
   "            Este puede ser: "
   "             - Golub_Reinsch, \n"
   "             - Golub_Reinsch_Mod (por defecto. Matrices rectangulares)\n"
   "             - Jacobi.\n"
   "             - Sparse"
     ),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSVD::CalcContens()
//--------------------------------------------------------------------
{
  BMat& A = Mat(Arg(1));
  BText method;
  if (Arg(2))
    method = Text(Arg(2));
  else
    method.Copy("Golub_Reinsch_Mod");
  
  BInt r = A.Rows(), c = A.Columns();
  BMatrix<BDat> U(r, c), V(c, c), D0;
  BDiagMatrix<BDat> D(c, 0);
  
  if(method=="Sparse")
  {
    SingularValueDecomposition(A,U,D,V);
    int d = D.Rows();
    if(d<c)
    {
      BMat D1(d,c-d);
      BMat D2(c-d,c);
      BMat D3(c,c-d);
      BMat D4(r,c-d);
      D1.SetAllValuesTo(0);
      D2.SetAllValuesTo(0);
      D3.SetAllValuesTo(0);
      D4.SetAllValuesTo(0);
      D0 = (D | D1 ) << D2; 
      V  =  V | D3;
      U  =  U | D4;
    }
    else
    {
      D0 = D;
    }
  }
  else
  {
    gsl_SingularValueDecomposition(A,U,D,V, method);
    D0 = D; 
  }
  BUserMat* U_ = BContensMat::New("",U, "Left eigen vector");
  BUserMat* D_ = BContensMat::New("",D0,"Eigen values diagonal matrix");
  BUserMat* V_ = BContensMat::New("",V, "Right eigen vectors");
  BList* lst   = NCons(V_);
  contens_.RobElement(Cons(U_,Cons(D_,lst)));
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatQR);
DefExtOpr(1, BMatQR, "QRDecomposition", 1, 1, "Matrix",
          "(Matrix A)",
          I2("Returns a QR decomposition of a rectangular MxN matrix A.\n"
             "The resulting set if {Q, R}, where Q is an orthogonal MxM square matrix and R is an MxN upper triangular matrix.\n"
             "A = Q*R",
             "Retorna la descomposicion QR de una matriz A MxN.\n"
             "El conjunto resultado es {Q, R}, donde Q es una matriz cuadrada ortogonal MxM y R es una matriz MxN triangular superior.\n"
             "A = Q*R"),
          BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatQR::CalcContens()
//--------------------------------------------------------------------
{
  gsl_matrix * A, * gQ, * gR;
  gsl_vector * tau;
  BMatrix<BDat> * mat = &(((BUserMat*)Arg(1))->Contens());
  BUserMat * Q, * R;
  int r, c, M, N, result;

  M = mat->Rows();
  N = mat->Columns();
  if (!M || !N) {    
    Error(I2("Invalid matrix dimensions in QRDecomposition.",
             "Dimension de matriz invalida en QRDecomposition"));    
      return;
  } else {
    // alloc gsl objects
    A = gsl_matrix_alloc(M, N);
    tau = gsl_vector_alloc(M<N?M:N);
    gQ = gsl_matrix_alloc(M, M);
    gR = gsl_matrix_alloc(M, N);
    // copy from TOL matrix to gsl (this must be avoided in a future
    BDat * datarr = mat->GetData().GetBuffer();
    int iptr = 0;
    for (r=0; r<M; r++)
        for (c=0; c<N; c++,iptr++)
            A->data[r*A->tda + c] = datarr[iptr].Value();
    result = gsl_linalg_QR_decomp(A, tau);
    gsl_linalg_QR_unpack(A, tau, gQ, gR);
    // copy the result to the TOL objects
    BMatrix<BDat> tQ(M,M), tR(M,N);
    // first copy Q matrix
    datarr = tQ.GetData().GetBuffer();
    iptr = 0;
    for (r=0; r<M; r++)
        for (c=0; c<M; c++, iptr++)
            datarr[iptr].PutValue(gQ->data[r*gQ->tda+c]);
    // now copy R matrix
    datarr = tR.GetData().GetBuffer();
    iptr = 0;
    for (r=0; r<M; r++)
        for (c=0; c<N; c++, iptr++)
            if (c>=r)
                datarr[iptr].PutValue(gR->data[r*gR->tda+c]);
    Q = BContensMat::New("Q", tQ, "Orthogonal matrix");
    R = BContensMat::New("R", tR, "Right triangular matrix");        
    // release gsl objects
    gsl_matrix_free(A);
    gsl_matrix_free(gQ);
    gsl_matrix_free(gR);
    gsl_vector_free(tau);
  }
  contens_.RobElement(Cons(Q,Cons(R,NIL)));
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatKernel);
DefExtOpr(1, BMatKernel, "Kernel", 1, 1, "Matrix",
  I2("(Matrix A)",
     "(Matrix A)"),
  I2("Returns an orthonormal base of the kernel of a matrix A, it's to say, "
     "the big matrix K that"
     "\n"
     "        A*K   = 0 \n"
     "        K'*K = I \n"
     "\n"
     ,
     "Devuelve una base ortonormal del nucleo de una matriz A, es decir, la "
     "matriz K mas grande para la que se cumple que "
     "\n"
     "        A*K   = 0 \n"
     "        K'*K = I \n"
     "\n"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatKernel::CalcContens()
//--------------------------------------------------------------------
{
  BMat& A = Mat(Arg(1));
  contens_ = Kernel(A);
}
//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatTridiagSEP);
DefExtOpr(1, BMatTridiagSEP, "TridiagSEPDivideAndConquer", 1, 1, "Matrix",
  "(Matrix M)",
  I2("Solves the symmetric eigenvalue problem for a tridiagonal symmetric "
     "matrix using the Divide and Conquer Method.\n",
     "Resuelve el problema de los autovalores para una matriz tridiagonal "
     "simetrica usando el metodo 'Divide and Conquer'."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatTridiagSEP::CalcContens()
//--------------------------------------------------------------------
{
  BSymMatrix<BDat> M;
  BMatrix   <BDat> U;
  M = Mat(Arg(1));
  BInt  n = M.Rows();
  BDiagMatrix<BDat> D(n,BDat::Unknown());
  if(M.Columns()!=n) { return; }
  TridiagSEPDivideAndConquer(M,U,D);
  BUserMat* U_ = BContensMat::New("U",U,"Eigenvectors");
  BUserMat* D_ = BContensMat::New("D",D,"Eigenvalues");
  contens_.RobElement(Cons(U_,NCons(D_)));
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatHouseholderTransf);
DefExtOpr(1, BMatHouseholderTransf, "HouseholderTransf", 1, 1, "Matrix",
  "(Matrix M)",
  I2("Makes the Householder transformation of a square matrix with range "
     "equal to the number of columns. Else returns the empty set and shows "
     "an error message.",
     "Hace la transformacion de Householder de una matriz con rango igual al "
     "numero de columnas. Si no es asi devuelve el conjunto vacio y da un "
     "mensaje de error."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatHouseholderTransf::CalcContens()
//--------------------------------------------------------------------
{
  BInt i,j;
  BArray<BDat> T;
  BMatrix<BDat> C = Mat(Arg(1));
  BInt n = C.Columns();
  BInt N = C.Rows();
  if(n>N)
  {
    Error(I2("Cannot do Householder transformation of a matrix with more "
             "columns than rows.",
             "No se puede hacer la transformacion de Householder sobre "
             "una matriz con mas columnas que filas. "
             "Probablemente debe trasponer la metriz."));
    return;
  }
  if(HouseholderTransformation(T,C))
  {
    BMatrix<BDat> Q = C;
    BMatrix<BDat> R(n,n);
    for(i=0; i<n; i++)
    {
      for(j=0; j<i;   j++) { R(i,j)=0; }
      R(i,i)=T(i);
      for(j=i+1; j<n; j++) { R(i,j)=C(i,j); Q(i,j)=0; }
    }
    BUserMat* UQ = BContensMat::New("",Q,"Ortonormal vectors.");
    BUserMat* UR = BContensMat::New("",R,"Upper triangular matrix.");
    UQ->PutName("Q");
    UR->PutName("R");
    contens_.PrepareStore(2);
    contens_.AddElement(UQ);
    contens_.AddElement(UR);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatHouseholderLeastSqr);
DefExtOpr(1, BMatHouseholderLeastSqr, "HouseholderLeastSqr", 3, 3,
  "Matrix Matrix Matrix",
  "(Matrix Y, Matrix W, Matrix R)",
  I2(".",
     "Dada la transformacion de Householder {W,R} de la matriz X, resulve el "
     "problema de minimos cuadrados Y = X*B + A. Devuelve un conjunto con "
     "la matriz de coeficientes B y la matriz de residuos A."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatHouseholderLeastSqr::CalcContens()
//--------------------------------------------------------------------
{
  BInt i,j, k;
  BArray<BDat>  T;
  BMatrix<BDat> Y  = Mat(Arg(1));
  BMatrix<BDat> YT = Y.T();
  BMatrix<BDat> W  = Mat(Arg(2));
  BMatrix<BDat> L  = Mat(Arg(3));
  BMatrix<BDat> C  = W;
  BInt n = C.Columns();
  BInt N = C.Rows();
  BInt V = Y.Columns();
  BMatrix<BDat> A(N,V), BT(n,V), B;
  BT.Alloc(V,n);
  if(BT.Rows()!=V) { return; }
  T.ReallocBuffer(n);
  if((n>N) || (Y.Rows()!=N) || (L.Rows()!=n) || (L.Columns()!=n))
  {
    Error(I2("Invalid dimensions in HouseholderLeastSqr function.",
             "Dimensiones no validas en la funcion HouseholderLeastSqr."));
    return;
  }
  for(i=0; i<n; i++)
  {
    for(j=i+1; j<n; j++) { C(i,j)=L(j,i); }
    T(i) = L(i,i);
  }
  for(k=0; k<V; k++)
  {
    BArray<BDat> a(N), b(n);
    LeastSqrHouseholder(T,C,YT.SubCol(k).Data(), b, a);
    for(i=0; i<N; i++) { A(i,k) =  a(i); }
    for(i=0; i<n; i++) { B(i,k) = -b(i); }
  }
  BUserMat* UB = BContensMat::New("B",B,"Coefficient matrix.");
  BUserMat* UA = BContensMat::New("A",A,"Residuals matrix.");
  contens_.RobElement(Cons(UB,NCons(UA)));
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatMarquardt);
DefExtOpr(1, BMatMarquardt, "Marquardt", 4, 5,
  "Real Real Code Matrix Code",
  "(Real n, Real m, Code fun, Matrix x [, Code jacobian])",
  I2(" Applies Marquardt's algorithm to solve the problem                     \n"
   "                   min(f(x)^t*f(x))                                     \n"
   "where f is a vectorial function, f:R^n ---> R^m                       \n\n"
   "Arguments:                                                            \n\n"
   "n        ---> Dimension of the function's domain space                  \n"
   "m        ---> Dimension of the function's image  space                  \n"
   "fun      ---> function to be minimized. It must be defined as           \n"
   "                Matrix (Matrix){...}                                    \n"
   "x        ---> intial parameters vector                                  \n"
   "jacobian ---> (optional) jacobian function of fun                       \n",
     " Aplica el algoritmo de Marquardt para resolver el problema del tipo      \n"
   "                   min(f(x)^t*f(x))                                     \n"
   "siendo f una funcion vectorial, f:R^n ---> R^m                         \n\n"
   "Argumentos:                                                              \n\n"
   "n        ---> Dimension del espacio dominio de la funcion                 \n"
   "m        ---> Dimension del espacio imagen de la funcion                  \n"
   "fun      ---> funcion a minimizar. Debe declararse como                   \n"
   "                Matrix (Matrix){...}                                      \n"
   "x        ---> vector de parametros iniciales                              \n"
   "jacobian ---> funcion (opcional) de calculo del jacobiano de fun          \n"),
    BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BMatMarquardt::CalcContens()
//--------------------------------------------------------------------
{
  BInt    n    = (BInt)Real (Arg(1));
  BInt    m    = (BInt)Real (Arg(2));
  BCode&  code = Code(Arg(3));
  BMat&  X0   = Mat (Arg(4));
  BCode*  jacobian = NIL;
  if(Arg(5)) { jacobian = &Code(Arg(5)); }
  BRnRmCode f(n, m, code, jacobian);
  BMatrix<BDat> J;
  BSymMatrix<BDat> M;
  BArray<BDat> y, x=X0.Data();
  f.LeastSqrMarquardt(x,y,J);
  M = MtMSqr(J)*2;
  BMatrix<BDat> X(1,n,x.Buffer());
  BMatrix<BDat> Y(1,m,y.Buffer());
  BGrammar::IncLevel();
  BUserMat* UX = BContensMat::New("X",X,"Parameters");
  BUserMat* UY = BContensMat::New("Y",Y,"Residuals");
  BUserMat* UM = BContensMat::New("M",M,"Information matrix 2*J'J");
  BUserMat* UJ = BContensMat::New("J",J,"Jacobian matrix J");
  BGrammar::DecLevel();
  contens_.PrepareStore(4);
  contens_.AddElement(UX);
  contens_.AddElement(UY);
  contens_.AddElement(UM);
  contens_.AddElement(UJ);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGradient);
DefExtOpr(1, BMatGradient, "Gradient", 2, 2,
  "Code Matrix",
  "(Code function, Matrix point)",
  I2("Determines the gradient vector of a function evaluated in a given point.  \n"
     "Arguments:                                                                \n" 
     "function ---> function to be analyzed,                                    \n"
     "point    ---> point where the gradient is calculated                      \n",
     "Determina el gradiente de una funcion evaluada en un punto dado.          \n"
     "Argumentos:                                                               \n" 
      "function ---> funcion a analizar,                                         \n"
     "point    ---> punto de calculo del gradiente                              \n"
     ),
    BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BMatGradient::CalcContens()
//--------------------------------------------------------------------
{
  BCode& code = Code(Arg(1));
  BMat&  p_   = Mat (Arg(2));
  BInt   n    = p_.Rows(); 
  BArray<BDat> p(n), G; 
  BDat d;
  BInt i;
  for (i=0; i<n; i++) { p[i] =  p_(i, 0); }
  contens_.Alloc(n,1);
  if(contens_.Rows()!=n) { return; }
  BRnRCode f(n, code);
  /* OPT! : puede pasarse el buffer de contens_? */
  f.Gradient(p, G);
  for (i=0; i<n; i++) contens_(i,0)= G[i];
}

  
//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatZeros);
DefExtOpr(1, BMatZeros, "Zeros",  1, 2, 
  "Real Real",
  "(Real nrow [, Real ncol=nrow])",
  I2("Creates a matrix with all elements equal to zero.",
     "Devuelve una matriz cuyas celdas son todas cero."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatZeros::CalcContens()
//--------------------------------------------------------------------
{
  int r = (BInt)Real(Arg(1));
  int c = r;
  if(Arg(2)) { c = (BInt)Real(Arg(2)); }
  contens_.Alloc(r,c);
  int k, s= contens_.Data().Size();
  double* x=(double*)contens_.GetData().GetBuffer();
  for(k=0; k<s; k++,x++)
  {
    *x = 0; 
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatConstant);
DefExtOpr(1, BMatConstant, "Constant",  3, 3, 
  "Real Real Real",
  "(Real nrow, Real ncol, Real value])",
  I2("Creates a matrix with all elements equal to given value.",
     "Devuelve una matriz cuyas celdas son todas iguales al valor "
     "especificado."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatConstant::CalcContens()
//--------------------------------------------------------------------
{
  int r = (BInt)Real(Arg(1));
  int c = (BInt)Real(Arg(2));
  double v = Real(Arg(3));
  contens_.Alloc(r,c);
  int k, s= contens_.Data().Size();
  double* x=(double*)contens_.GetData().GetBuffer();
  for(k=0; k<s; k++, x++)
  {
    *x = v;
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRandom);
DefExtOpr(1, BMatRandom, "Rand", 4, 4, "Real Real Real Real",
  I2("(Real rows, Real columns, Real min, Real max)",
     "(Real filas, Real columnas, Real min, Real max)"),
  I2("Returns a matrix of independent uniform[min,max] distributed numbers.",
     "Devuelve una matriz de numeros con distribucion uniforme[min, max] "
     "independientes."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRandom::CalcContens()
//--------------------------------------------------------------------
{
  int r = (BInt)Real(Arg(1));
  int c = (BInt)Real(Arg(2));
  contens_.Alloc(r,c);
  if(contens_.Rows()!=r) { return; }
  BDat a = Dat(Arg(3));
  BDat b = Dat(Arg(4));
  BUniformDist u(a,b);
  for(BInt i=0; i<contens_.Data().Size(); i++)
  {
    b2dMat(contens_).Data()(i) = u.Random().Value();
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGaussian);
DefExtOpr(1, BMatGaussian, "Gaussian", 4, 4, "Real Real Real Real",
  I2("(Real rows, Real columns, Real nu, Real sigma)",
     "(Real filas, Real columnas, Real nu, Real sigma)"),
  I2("Returns a matrix of independent normal(mu,sigma^2) distributed numbers.",
     "Devuelve una matriz de numeros con distribucion normal(mu,sigma^2) "
     "independientes."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatGaussian::CalcContens()
//--------------------------------------------------------------------
{
  int r = (BInt)Real(Arg(1));
  int c = (BInt)Real(Arg(2));
  contens_.Alloc(r,c);
  if(contens_.Rows()!=r) { return; }
  BNormalDist nor(Real(Arg(3)), Real(Arg(4)));
  for(BInt i=0; i<contens_.Data().Size(); i++)
  {
    b2dMat(contens_).Data()(i) = nor.Random().Value();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRandMultinomial);
DefExtOpr(1, BMatRandMultinomial, "RandMultinomial", 3, 3, "Real Real Matrix",
  I2("(Real rows, Real N, Matrix p)",
     "(Real filas, Real N, Matrix p)"),
  I2("Returns a matrix of rows distributed as independent multinomial "
     "with population size of <N> and vector of probabilities <p>\n"
     "This function calls to GSL function gsl_ran_multinomial: "
     "http://www.network-theory.co.uk/docs/gslref/TheMultinomialDistribution.html ",
     "Devuelve una matriz de filas distribuidas como multinomiales con "
     "tamaño poblacional <N> y vector de probabilidades <p>\n."
     "Esta funcion llama a la funcion GSL gsl_ran_multinomial: "
     "http://www.network-theory.co.uk/docs/gslref/TheMultinomialDistribution.html "),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRandMultinomial::CalcContens()
//--------------------------------------------------------------------
{
  int i,j,rows = (int)Real(Arg(1));
  int N = (int)Real(Arg(2));
  DMat& p = dMat(Arg(3));
  int K = p.Columns();
  contens_.Alloc(rows, K);
  if(contens_.Rows()!=rows) { return; }
  BArray<unsigned int> n;
  for(i=0; i<rows; i++)
  {
    RandMultinomial(N,p.Data(),n);
    for(j=0; j<K; j++)
    {
      contens_(i,j) = n(j);
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRandPermutation);
DefExtOpr(1, BMatRandPermutation, "RandPermutation", 2, 2, "Real Real",
  "(Real m, Real n)",
  I2("Returns a matrix with m permutations of n numbers.",
     "Devuelve una matriz con m permutaciones de n numeros."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRandPermutation::CalcContens()
//--------------------------------------------------------------------
{
  RandPermutation(contens_,BInt(Real(Arg(1))),BInt(Real(Arg(2))));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRandCombination);
DefExtOpr(1, BMatRandCombination, "RandCombination",3,3,"Real Real Real",
  "(Real numComb, Real m, Real n)",
  I2("Returns a matrix with numComb combinations of m over n.",
     "Devuelve una matriz con m permutaciones de n numeros tomados de n "
     "en n."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRandCombination::CalcContens()
//--------------------------------------------------------------------
{
  RandCombination(contens_,
                  BInt(Real(Arg(1))),
                  BInt(Real(Arg(2))),
                  BInt(Real(Arg(3))));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRow);
DefExtOpr(1, BMatRow,    "Row", 1, 0, "Real Real",
  "(Real x1 [, Real x2, ...])",
  I2("Returns the row matrix [[x1,x2,...,xn]].",
     "Crea una matriz fila [[x1,x2,...,xn]]."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRow::CalcContens()
//--------------------------------------------------------------------
{
  BInt i;
  contens_.Alloc(1,NumArgs());
  if(contens_.Rows()!=1) { return; }
  for(i=0; i < NumArgs(); i++)
  { b2dMat(contens_)(0,i) = Real(Arg(i+1)); }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatCol);
DefExtOpr(1, BMatCol,    "Col", 1, 0, "Real Real",
  "(Real x1 [, Real x2, ...])",
  I2("Returns the column matrix [[x1],[x2],...,[xn]].",
     "Devuelve una matriz columna [[x1],[x2],...,[xn]]."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatCol::CalcContens()
//--------------------------------------------------------------------
{
  BInt i, n = NumArgs();
  contens_.Alloc(n,1);
  if(contens_.Rows()!=n) { return; }
  for(i=0; i < n; i++)
  { b2dMat(contens_)(i,0) = Real(Arg(i+1)); }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSetRow);
DefExtOpr(1, BMatSetRow,   "SetRow", 1, 1, "Set",
  "(Set s)",
  I2("Returns a row matrix from a set of real elements.",
     "Devuelve una matriz fila a partir de un conjunto de reales."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSetRow::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  BInt i, n=set.Card();
  contens_.Alloc(1,n);
  if(contens_.Columns()!=n) { return; }
  double* x = (double*)contens_.GetData().GetBuffer();
  for(i=0; i < n; i++)
  {
    if(set[i+1]->Grammar()->Name()!="Real")
    {
      Error(I2("Wrong set format for SetRow",
               "Fallo en formato de conjunto para SetRow"));
      contens_ = BMat::Unknown();
      return;
    }
    (*x) = Real(set[i+1]);
    x++;
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSetCol);
DefExtOpr(1, BMatSetCol,   "SetCol", 1, 1, "Set",
  "(Set s)",
  I2("Returns a column matrix from a set of real elements.",
     "Devuelve una matriz columna a partir de un conjunto de reales."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSetCol::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  BInt i, n=set.Card();
  contens_.Alloc(n,1);
  if(contens_.Rows()!=n) { return; }
  double* x = (double*)contens_.GetData().GetBuffer();
  for(i=0; i < n; i++)
  {
    if(set[i+1]->Grammar()->Name()!="Real")
    {
      Error(I2("Wrong set format for SetCol",
               "Fallo en formato de conjunto para SetCol"));
      contens_ = BMat::Unknown();
      return;
    }
    (*x) = Real(set[i+1]);
    x++;
  }
}

//--------------------------------------------------------------------
bool internal_builtin_SetMat(
  const BText& fun_id,
  const BSet &set, 
  BMatrix<BDat>& mat)
//--------------------------------------------------------------------
{
  BInt i, j;
  BText _mid = I2("Converting set to matrix with ",
                  "Convirtiendo de conjunto a matriz con ")+fun_id;
  BText _wrongFormat = 
    _mid+I2("Wrong set format: all elements should "
            "be sets with the same number of reals ",
            "Fallo en formato de conjunto: todos sus elementos "
            "deberían ser conjuntos con el mismo número de reales");
  if(!set.Card() || (set[1]->Grammar()->Name()!="Set"))
  {
    Error(_wrongFormat);
    mat = BMat::Unknown();
    return(false);
  }
  BInt  numRow = set.Card();
  BInt  numCol = Set(set[1]).Card();
  if(!numRow)
  {
    Warning(_mid+I2("Emty set is converted in empty matrix",
             "El conjunto vacío se convierte en la matriz vacía"));
    mat = BMat::Unknown();
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
      if((set[i+1]->Grammar()->Name()!="Set") ||
         (Set(set[i+1]).Card()!=numCol))
      {
        Error(_wrongFormat);
        mat = BMat::Unknown();
        return(false);  
      }
      BSet& seti = Set(set[i+1]);
      for(j=0; j < numCol; j++)
      {
        if(seti[j+1]->Grammar()->Name()!="Real")
        {
          Error(_wrongFormat);
          mat = BMat::Unknown();
          return(false);  
        }
        b2dMat(mat)(i,j) = Real(seti[j+1]);
      }
    }
  }
  return(true);
};

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSetMat);
DefExtOpr(1, BMatSetMat,   "SetMat", 1, 1, "Set",
  "(Set s)",
  I2("Returns a matrix as of a set of sets of real numbers.",
     "Devuelve una matriz a partir de un conjunto de conjuntos de numeros "
     "reales."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSetMat::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set  = Set(Arg(1));
  internal_builtin_SetMat("Matrix SetMat(Set s)",set,contens_);
}

/*
//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPolMat);
DefExtOpr(1, BMatPolMat,   "PolMat", 3, 3, "Polyn Real Real",
  I2("(Polyn pol [,Real rows=Degree(pol)+1, Real columns=Degree(pol)+1])",
     "(Polyn pol [,Real filas=Degree(pol)+1, Real columnas=Degree(pol)+1])"),
  I2("Returns the matrix representation of a backshift polynomial.",
     "Devuelve la representacion matricial de un polinomio de retardos."
     "Se trata de una matriz triangular inferior que tiene todos los "
     "valores de la diagonal principal iguales al coeficiente de grado 0 "
     "del polinomio, y los de las diagonales inferiores iguales al "
     "coeficiente del grado correspondiente."),
    BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BMatPolMat::CalcContens()
//--------------------------------------------------------------------
{
  BDat x;
  BInt d, i, j, k;
  BPol& pol = Pol(Arg(1));
  BInt  m   = pol.Size();
  BInt  r   = (BInt)Real(Arg(2));
  BInt  c   = (BInt)Real(Arg(3));
  contens_.Alloc(r,c);
  if(contens_.Rows()!=r) { return; }
  for(j=0; j<c; j++)
  {
    for(i=0; i<r; i++) { contens_(i,j) = 0; }
  }
  for(k=0; k<m; k++)
  {
    x = pol[k].Coef();
    d = pol[k].Degree();
    i = j = 0;
    if(d>=0) { i= d; }
    else     { j=-d; }
    for(;(i<r)&&(j<c); i++,j++)
    {
      contens_(i,j) = x;
    }
  }
}
*/

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPolMat);
DefExtOpr(1, BMatPolMat,   "PolMat", 3, 3, "Polyn Real Real",
  I2("(Polyn pol,Real rows, Real columns)",
     "(Polyn pol,Real filas, Real columnas)"),
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
void BMatPolMat::CalcContens()
//--------------------------------------------------------------------
{
  BPol& pol = Pol(Arg(1));
  register int d;
  register int i;
  register int j;
  register int m = pol.Size();
  register int r = (BInt)Real(Arg(2));
  register int c = (BInt)Real(Arg(3));
  contens_.Alloc(r,c);
  if(contens_.Rows()!=r) { return; }
  if(!r||!c) { return; }
  contens_.SetAllValuesTo(0);
  register double* y0 = (double*)contens_.GetData().GetBuffer();
  register double* y1 = (double*)contens_.GetData().GetBuffer()+(r*c);
  register double* y;
  struct BMon
  { 
    double coef;
    int    degree;
  };
  assert(sizeof(BMon)==sizeof(BMonome<BDat>));
  register BMon* p0 = (BMon*)pol.Buffer();
  register BMon* p1 = (BMon*)pol.Buffer() + m;
  register BMon* pk;
  for(pk=p0; pk<p1; pk++)
  {
    d = pk->degree;
    if(d>=0) { i= d; j=0; }
    else     { j=-d; i=0; }
    y = y0 + (i*c+j);
    for(;(i<r)&&(j<c); i++, j++)
    {
      assert(y<y1);
      if(y>=y1) 
      {
        Error(I2("Invalid access in","Acesso inválido en")+ " PolMat " +
              " r="+r+
              " c="+c+
              " r*c="+(r*c)+
              " m="+m+
              " d="+d+
              " i="+i+
              " j="+d+
              " i*c+j="+(i*c+j));
        return;
      }
      (*y)=pk->coef;
      y += (c+1); 
    }
  }
}
/* */

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatConcatCol);
//DefExtOpr(1, BMatConcatCol,  "CatCol", 1, 0, "Matrix Matrix",
DefExtOpr(1, BMatConcatCol,  "ConcatColumns", 1, 0, "Matrix Matrix",
  "(Matrix mat1 [, Matrix mat2, ...])",
  I2("Concatenates all the columns of a matrixs list with the same number "
     "of rows.",
     "Concatena todas las columnas de una lista de matrices con el mismo "
     "numero de filas."),
  BOperClassify::MatrixAlgebra_);
  DefExtOpr(2, BMatConcatCol,  "|", 2, 2, "Matrix Matrix",
  "mat1 | mat2 {Matrix mat1 , Matrix mat2}",
  I2("Concatenates all the columns of two matrixs with the same number "
     "of rows.",
     "Concatena todas las columnas de dos matrices con el mismo "
     "numero de filas."),
      BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatConcatCol::CalcContens()
//--------------------------------------------------------------------
{
  BInt  i, j, k=0, l;
  BInt  numCol = 0;
  BInt  numRow = Mat(Arg(1)).Rows();
  BInt  numMat = NumArgs();
  for(i=1; i<=numMat; i++)
  {
    BMat& Mi = Mat(Arg(i));
    numCol+=Mi.Columns();
    if(!numRow) { numRow=Mi.Rows(); }
    if(Mi.Columns() && (Mi.Rows()!=numRow))
    {
      Error(I2("Wrong number of rows for ConcatColumns (or | operator) "
               "in argument number ",
               "Numero de filas erroneo para ConcatColumns (o el "
               "operador | )en la matriz numero ")+
            i + ".");
      contens_ = BMat::Unknown();
      return;
    }
  }
  contens_.Alloc(numRow, numCol);
  if(contens_.Rows()!=numRow) { return; }
  for(i=1; i<=numMat; i++)
  {
    DMat& Mi = dMat(Arg(i));
    for(j=0; j < Mi.Columns(); j++, k++)
    {
      for(l=0; l < numRow; l++)
      {
        b2dMat(contens_)(l,k)=Mi(l,j);
      }
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatConcatRow);
//DefExtOpr(1, BMatConcatRow,  "CatRow", 1, 0, "Matrix Matrix",
DefExtOpr(1, BMatConcatRow,  "ConcatRows", 1, 0, "Matrix Matrix",
  "(Matrix mat1 [, Matrix mat2, ...])",
  I2("Concats all the rows from a matrixs list with the same number of "
     "columns.",
     "Concatena todas las filas de una lista de matrices con el mismo "
     "numero de columnas."),
  BOperClassify::MatrixAlgebra_);
  DefExtOpr(2, BMatConcatRow,  "<<", 2, 2, "Matrix Matrix",
  "mat1 << mat2 {Matrix mat1, Matrix mat2}",
  I2("Concats all the rows from two matrixs with the same number of "
     "columns.",
     "Concatena todas las filas de de dos matrices con el mismo "
     "numero de columnas."),
      BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatConcatRow::CalcContens()
//--------------------------------------------------------------------
{
  BInt  i, j, k=0, l;
  BInt  numCol = Mat(Arg(1)).Columns();
  BInt  numRow = 0;
  BInt  numMat = NumArgs();
  for(i=1; i<=numMat; i++)
  {
    BMat& Mi = Mat(Arg(i));
    numRow+=Mi.Rows();
    if(!numCol) { numRow=Mi.Columns(); }
    if(Mi.Rows() && (Mi.Columns()!=numCol))
    {
      Error(I2("Wrong number of columns for ConcatRows ( or << "
               "operator ) in argument number ",
               "Numero de columnas erroneo para ConcatRows ( o "
               "el operador << ) en la matriz numero ")+
            i + ".");
      contens_ = BMat::Unknown();
      return;
    }
  }
  contens_.Alloc(numRow, numCol);
  if(contens_.Rows()!=numRow) { return; }
  for(i=1; i <= numMat; i++) 
  {
    DMat& Mi = dMat(Arg(i));
    for(j=0; j < Mi.Rows(); j++, k++) 
    {
      for(l=0; l < numCol; l++) 
      {
       b2dMat(contens_)(k,l)=Mi(j,l); 
      } 
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatExtractDiag);
DefExtOpr(1, BMatExtractDiag,    "SubDiag", 2, 2, "Matrix Real",
  I2("(Matrix m, Real diagIndex)",
     "(Matrix m, Real indiceDeDiagonal)"),
  I2("Extracts from the matrix m the indicated diagonal.",
     "Extrae de la matriz la diagonal indicada. La diagonal principal es "
     "la de indice 0, las diagonales inferiores tienen indices negativos "
     "y las superiores los tienen positivos."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatExtractDiag::CalcContens()
//--------------------------------------------------------------------
{
  DMat&M = dMat(Arg(1));
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
    b2dMat(contens_)(0,k) = M(i,j);
  }
}



//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSetDiag);
DefExtOpr(1, BMatSetDiag,   "SetDiag", 1, 1, "Set",
  I2("(Set diagValues)",
     "(Set diagValues)"),
  I2("Creates a diagonal matrix with the elements of a set of real numbers.",
     "Crea una matriz diagonal con los elementos de un conjunto de numeros "
     "reales."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSetDiag::CalcContens()
//--------------------------------------------------------------------
{
  BSet& s = Set(Arg(1));
  BInt  n = s.Card();
  BInt  i;
  contens_.Alloc(n,n);
  if(contens_.Rows()!=n) { return; }
  b2dMat(contens_).SetAllValuesTo(0);
  for(i=0; i<n; i++)
  {
    b2dMat(contens_)(i,i) = Real(s[i+1]);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSubTriang);
DefExtOpr(1, BMatSubTriang,    "SubTriang", 2, 2, "Matrix Text",
  "(Matrix m, Text UPLO)",
  I2("Extracts from the matrix <m> a row vector containing \n"
     "  the upper triangular part if <UPLO>==\"U\" \n"
     "  the lower triangular part if <UPLO>==\"L\" \n"
     "If matrix is non square only principal minor submatrix will be used.\n"
     "If the dimension of this principal minor is n then the size of "
     "resulting vector will be k=n*(n+1)/2\n"
     "Use SetTriang to restore the triangular matrix.",
     "Extrae de la matriz <m> un vector fila conteniendo:\n"
     "  la parte triangular superior si <UPLO>==\"U\" \n"
     "  la parte triangular inferior si <UPLO>==\"L\" \n"
     "Si la matriz no es cuadrada silo se tendra en cuenta la submatriz "
     "menor principal.\n"
     "Si el tamaño de <m> es n entonces el tamaño del vector devuelto "
     "sera k=n*(n+1)/2\n" 
     "Para restaurar la matriz triangular use SetTriang"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSubTriang::CalcContens()
//--------------------------------------------------------------------
{
  DMat& M    = dMat(Arg(1));
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
        b2dMat(contens_)(0,k) = M(i,j);
      }
    }
  }
  else if(uplo=='L')
  {
    for(i=k=0; i<n; i++)
    {
      for(j=0; j<=i; j++, k++)
      {
        b2dMat(contens_)(0,k) = M(i,j);
      }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSetTriang);
DefExtOpr(1, BMatSetTriang, "SetTriang", 2, 2, "Matrix Text",
  "(Matrix v, Text UPLOSI)",
  I2("Converts a row vector in:\n"
     "  a upper triangular matrix if <UPLOSI>==\"U\" \n"
     "  a lower triangular matrix if <UPLOSI>==\"L\" \n"
     "  a symmetric matrix if <UPLOSI>==\"S\" \n"
     "The size of vector <v> must be an integer k matching "
     "k=n*(n+1)/2, being n the size of resulting square matrix",
     "Convierte un vector fila en:\n"
     "  una matriz triangular superior si <UPLOSI>==\"U\" \n"
     "  una matriz triangular inferior si <UPLOSI>==\"L\" \n"
     "  una matriz simetrica si <UPLOSI>==\"S\" \n"
     "El tamaño del vector <v> debe ser un entero k tal que "
     "k=n*(n+1)/2, siendo n el tamaño de la matriz cuadrada "
     "resultante"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatSetTriang::CalcContens()
//--------------------------------------------------------------------
{
  DMat& v    = dMat(Arg(1));
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
        b2dMat(contens_)(i,j) = v.Data()(k);
      }
    }
  }
  else if((uplo=='L')||(uplo=='S'))
  {
    for(i=k=0; i<n; i++)
    {
      for(j=0; j<=i; j++, k++)
      {
        b2dMat(contens_)(i,j) = v.Data()(k);
      }
    }
  }
}



//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatOrthonormalCompletion);
DefExtOpr(1, BMatOrthonormalCompletion, "OrthonormalCompletion", 2, 2, "Matrix Real",
  "(Matrix mat, Real m)",
  I2("Adds right columns to a orthonormal matrix in order to obtain a "
     "orthonormal matrix of m columns",
     "Añade columnas a una matriz ortonormal para obtener una matriz "
     "ortonormal de m columnas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatOrthonormalCompletion::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1));
  BInt m = (BInt)Real(Arg(2));
  OrthonormalCompletion(contens_,m);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetNormColumns);
DefExtOpr(1, BSetNormColumns,    "NormColumns", 1, 1, "Matrix",
  I2("(Matrix m)",
     "(Matrix m)"),
  I2("Normalizes the columns of a matrix dividing the result by the "
     "euclidean. ",
     "Normaliza las columnas de una matriz dividiendo el resultado por la "
     "norma euclidea."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BSetNormColumns::CalcContens()
//--------------------------------------------------------------------
{
  BMat& A = Mat(Arg(1));
  BMat  B;
  BDiagMatrix<BDat> D;
  DiagNormColumnsDecomp(A,B,D);
  BUserMat* B_ = BContensMat::New("B", B, "Unary vectors");
  BUserMat* D_ = BContensMat::New("D", D, "Diagonal norms");
  contens_.RobElement(Cons(B_,NCons(D_)));
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatNormDiag);
DefExtOpr(1, BMatNormDiag,   "NormDiag", 1, 1, "Matrix",
  I2("(Matrix m)",
     "(Matrix m)"),
  I2("Returns an equivalent to m matrix with unitary diagonal."
     "For example, if m is a covariance matrix it returns the "
     "correlation matrix. If m is not a square matrix it shows an error.",
     "Devuelve una matriz equivalente a m con diagonal unitaria."
     "Por ejemplo, dada una matriz de covarianzas devuelve la de"
     "correlaciones. Si m no es una matriz cuadrada muestra un error."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatNormDiag::CalcContens()
//--------------------------------------------------------------------
{
  DMat& M = dMat(Arg(1));
  BInt  r = M.Rows();
  BInt  c = M.Columns();
  if(r!=c)
  {
    Error(I2("Cannot apply NormDiag to non square matrix ",
             "No se puede aplicar NormDiag a la matriz no cuadrada ")+
          Arg(1)->Identify());
    contens_ = BMat::Unknown();
    return;
  }
  BInt  i,j;
  contens_.Alloc(r,r);
  if(contens_.Rows()!=r) { return; }
  for(i=0; i<r; i++)
  {
    for(j=0; j<c; j++)
    {
      b2dMat(contens_)(i,j) = M(i,j) / Sqrt(M(i,i)*M(j,j));
    }
    b2dMat(contens_)(i,i) = 1;
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPostProdDiag);
DefExtOpr(1, BMatPostProdDiag,   "PostProdDiag", 2, 2, "Matrix Matrix",
  I2("(Matrix M, Matrix D)",
     "(Matrix M, Matrix D)"),
  I2("It virtually post-multiplies a matrix <M> by the diagonal matrix "
     "whose elements in the main diagonal are those of the row matrix "
     "<D>",
     "Multiplica por la derecha una matriz <M> por la matriz diagonal "
     "cuyos elementos en la diagonal principal son los de la matriz "
     "fila <D>")+
     "\n  PostProdDiag(M,D) == M*SetDiag(MatSet(D)[1])",
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatPostProdDiag::CalcContens()
//--------------------------------------------------------------------
{
  DMat& M = dMat(Arg(1));
  DMat& d = dMat(Arg(2));
  register int s = d.GetData().Size();
  register int m = M.Rows();
  register int n = M.Columns();

  if(s!=n)
  {
    Error(I2("Wrong dimensions for ",
               "Dimensiones erróneas para ")+
            "PostProdDiag({"+m+"x"+n+"},{"+s+"x"+s+"})");
    return;
  }
  DMat& c = M;
  register double* cij = c.GetData().GetBuffer();
  register double* d0  = c.GetData().GetBuffer();
  register double* djj;
  register int i, j;
  for(i=0; i<m; i++)
  {
    djj = d0;
    for(j=0; j<n; j++)
    {
      (*cij)*=(*djj);
      cij++;
      djj++;
    } 
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPreProdDiag);
DefExtOpr(1, BMatPreProdDiag,   "PreProdDiag", 2, 2, "Matrix Matrix",
  I2("(Matrix D, Matrix M)",
     "(Matrix D, Matrix M)"),
  I2("It virtually pre-multiplies a matrix <M> by the diagonal matrix "
     "whose elements in the main diagonal are those of the row matrix "
     "<D> ",
     "Multiplica por la derecha una matriz <M> por la matriz diagonal "
     "cuyos elementos en la diagonal principal son los de la matriz "
     "fila <D> ")+
     "\n  PreProdDiag(D,M) == SetDiag(MatSet(D)[1])*M",
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatPreProdDiag::CalcContens()
//--------------------------------------------------------------------
{
  DMat& d = dMat(Arg(1));
  DMat& M = dMat(Arg(2));
  register int s = d.GetData().Size();
  register int m = M.Rows();
  register int n = M.Columns();
  if(s!=m)
  {
    Error(I2("Wrong dimensions for ",
               "Dimensiones erróneas para ")+
            "PreProdDiag({"+s+"x"+s+"},{"+m+"x"+n+"})");
    return;
  }
  DMat& c = b2dMat(contens_);
  c = M;
  register double* dii = d.GetData().GetBuffer();
  register double* cij = c.GetData().GetBuffer();
  register double dii_;
  register int i, j;
  for(i=0; i<m; i++)
  {
    dii_ = (*dii);
    for(j=0; j<n; j++)
    {
      (*cij) *= dii_;
      cij++;
    } 
    dii++;
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatExtractRow);
DefExtOpr(1, BMatExtractRow,   "SubRow", 2, 2, "Matrix Set",
  I2("(Matrix m , Set rowsIndex)",
     "(Matrix m , Set indiceDeFilas)"),
  I2("Extracts from the matrix m the indicated rows.",
     "Extrae de la matriz las filas indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatExtractRow::CalcContens()
//--------------------------------------------------------------------
{
  DMat& M = dMat(Arg(1));
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
  register const double* x0 = M.Data().Buffer();
  register const double* x;
  register double* y = b2dMat(contens_).GetData().GetBuffer();
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
DeclareContensClass(BMat, BMatTemporary, BMatExtractColumns);
DefExtOpr(1, BMatExtractColumns,   "SubCol", 2, 2, "Matrix Set",
  I2("(Matrix m , Set columnsIndex)",
     "(Matrix m , Set indiceDeColumnas)"),
  I2("Extracts from the matrix m the indicated columns.",
     "Extrae de la matriz las columnas indicadas."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatExtractColumns::CalcContens()
//--------------------------------------------------------------------
{
  DMat& M = dMat(Arg(1));
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
  register const double* x0 = M.Data().Buffer();
  register const double* x;
  register double* y = b2dMat(contens_).GetData().GetBuffer();
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
DeclareContensClass(BMat, BMatTemporary, BMatExtractRectangle);
DefExtOpr(1, BMatExtractRectangle, "Sub", 5, 5,
  "Matrix Real Real Real Real",
  I2("(Matrix m, Real row, Real column, Real height, Real width)",
     "(Matrix m, Real fila, Real columna, Real alto, Real ancho)"),
  I2("Extracts of the matrix the submatrix that starts in the given row and "
     "column and that it has the indicated height and width.",
     "Extrae de la matriz la submatriz que comienza en la fila y la columna "
     "dadas y que tiene el alto y ancho indicados."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatExtractRectangle::CalcContens()
//--------------------------------------------------------------------
{
  DMat& M = dMat(Arg(1));
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
        b2dMat(contens_)(i,j) = M(i+y,j+x);
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

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatFrequency);
DefExtOpr(1, BMatFrequency,  "Frequency", 2, 4, "Matrix Real Real Real",
  "(Matrix M, Real n [, Real min, Real max])",
  I2("Makes a frequency analysis of n intervals for the first column of a "
     "matrix M. Returns a matrix with two columns. "
     "In the first column contains the upper bound  of each interval. "
     "In the second column contains the number of values of M that belong "
     "to each interval.",

     "Hace un analisis de frecuencia de n intervalos para la primera columna "
     "de una matriz M. Devuelve una matriz con dos columnas. "
     "En la primera columna contiene el limite superior de cada intervalo. "
     "En la segunda columna contiene el numero de valores de M que "
     "pertenecen a cada intervalo."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatFrequency::CalcContens()
//--------------------------------------------------------------------
{
  BMat& M     = Mat (Arg(1));
  BInt  parts = (BInt)Real(Arg(2));
  BDat  min,max;
  if(Arg(3)) { min = Dat(Arg(3)); }
  if(Arg(4)) { max = Dat(Arg(4)); }
  Frequency(M.SubCol(0).Data(), contens_, parts, min, max);
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSerFrequency);
DefExtOpr(1, BMatSerFrequency,   "SerFrequency", 2, 4, "Serie Real Real Real",
  I2("(Serie ser, Real parts [, Real min, Real max])",
     "(Serie ser, Real partes [, Real min, Real max])"),
  I2("Makes a frequency analysis of n intervals for serie S. "
     "Returns a matrix with two columns. "
     "In the first column contains the upper bound  of each interval. "
     "In the second column contains the number of values of S that belong "
     "to each interval.",

     "Hace un analisis de frecuencia de n intervalos para una serie S. "
     "Devuelve una matriz con dos columnas. "
     "En la primera columna contiene el limite superior de cada intervalo. "
     "En la segunda columna contiene el numero de valores de S que "
     "pertenecen a cada intervalo."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatSerFrequency::CalcContens()
//--------------------------------------------------------------------
{
  BSerieTable table;
  table.AddSerie(Tsr(Arg(1)));
  table.Fill();
  BInt  parts = (BInt)Real(Arg(2));
  BDat  min,max;
  if(Arg(3)) { min = Dat(Arg(3)); }
  if(Arg(4)) { max = Dat(Arg(4)); }
  Frequency(table.Data().Data(), contens_, parts, min, max);
}


//--------------------------------------------------------------------
void SerieFrequency(BSyntaxObject* ser, int parts, const BText& fName)
//--------------------------------------------------------------------
{
  BList* args = LstStack
  (
    NIL,
    ser,
    new BContensDat(BDat(parts)),
    NIL
  );
  BMatSerFrequency* freq = new BMatSerFrequency(args);
  BList* lst = NCons(freq);
  ApplyMethod("Matrix","Table",lst,fName);
  DESTROY(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSerieMat);
DefExtOpr(1, BMatSerieMat,   "SerMat", 1, 1, "Serie",
  "(Serie ser)",
  I2("Returns the row matrix from data of a serie.",
     "Devuelve una matriz fila a partir de los datos de una serie."),
    BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BMatSerieMat::CalcContens()
//--------------------------------------------------------------------
{
  BSerieTable table;
  table.AddSerie(Tsr(Arg(1)));
  table.Fill();
  b2dMat(contens_) = b2dMat(table.Data());
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSerSetMat);
DefExtOpr(1, BMatSerSetMat,  "SerSetMat", 1, 3, "Set Date Date",
  I2("(Set s [, Date from, Date until] )",
     "(Set s [, Date desde, Date hasta] )"),
  I2("Returns a matrix whose rows are the data of the series of a set.",
     "Devuelve una matriz cuyas filas son los datos de las series de un "
     "conjunto."),
    BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BMatSerSetMat::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  BDate f = BDate::Unknown();
  BDate l = BDate::Unknown();
  if(Arg(2)) { f = Date(Arg(2)); }
  if(Arg(3)) { l = Date(Arg(3)); }
  BSerieTable table;
  table.AddSet(set);
  table.Fill(f,l);
  if(!table.TestDates()) { return; }
  b2dMat(contens_) = b2dMat(table.Data());
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatCov);
DefExtOpr(1, BMatCov,    "Cov", 1, 1, "Matrix",
  "(Matrix s)",
  I2("Returns the covariance matrix between the rows of the given matrix.",
     "Devuelve la matriz de covarianzas entre las filas de una matriz."),
    BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BMatCov::CalcContens()
//--------------------------------------------------------------------
{
  BMat& M = Mat(Arg(1));
  BSymMatrix<BDat> cov;
  Covarianze(M,cov);
  contens_ = cov;
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatCor);
DefExtOpr(1, BMatCor,    "Cor", 1, 1, "Matrix",
  "(Matrix s)",
  I2("Returns the correlation matrix between the rows of the given matrix.",
     "Devuelve la matriz de correlacion entre las filas de una matriz."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatCor::CalcContens()
//--------------------------------------------------------------------
{
  BMat& M = Mat(Arg(1));
  BSymMatrix<BDat> cor;
  Correlation(M,cor);
  contens_ = cor;
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPCor);
DefExtOpr(1, BMatPCor,   "PCor", 1, 1, "Matrix",
  "(Matrix s)",
  I2("Returns the partial correlation matrix between the rows of the given "
     "matrix.",
     "Devuelve la matriz de correlacion parciales entre las filas de una "
     "matriz."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatPCor::CalcContens()
//--------------------------------------------------------------------
{
    BMat& M = Mat(Arg(1));
    BSymMatrix<BDat> pcor;
    PartialCorrelation(M,pcor);
    contens_ = pcor;
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRegr);
DefExtOpr(1, BMatRegr,   "RegressionCoef", 1, 1, "Matrix",
  "(Matrix s)",
  I2("Returns the regression matrix between the rows of the given "
     "matrix.",
     "Devuelve la matriz de regresion entre las filas de una "
     "matriz."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatRegr::CalcContens()
//--------------------------------------------------------------------
{
    BMat& M = Mat(Arg(1));
    BSymMatrix<BDat> cov,cor,pcor;
    BMatrix<BDat> regr;
    Covarianze(M,cov);
    CorrelationF(cov,cor);
    PartialCorrelationF(cor,pcor);
    RegressionCoef(cov,pcor,regr);
    contens_ = regr;
}

//--------------------------------------------------------------------
static BDat GetSymetricACor(const BMat& ACOR, BInt i)
//--------------------------------------------------------------------
{
    BDat a = 1;
    if(i)
    {
  BInt j = abs(i);
  if(j>=ACOR.Rows()) { a = 0; }
  else         { a = ACOR(j,0); }
    }
//Std(BText("\nGetSymetricACor(")+i+") = " + a);
    return(a);
}


//--------------------------------------------------------------------
static void BrockwellVector(const BMat& M, BMat& B, BInt n)
//--------------------------------------------------------------------
{
    BInt N = M.Rows();
    B.Alloc(n,N+1);
    if(B.Rows()!=n) { return; }
    for(BInt i=1; i<=n; i++)
    {
  for(BInt k=0; k<=N; k++)
  {
      
      B(i-1,k) =     GetSymetricACor(M,k+i) + GetSymetricACor(M,k-i) -
           2 * GetSymetricACor(M,i)   * GetSymetricACor(M,k);
  }
    }
}


//--------------------------------------------------------------------
static void BrockwellMatrix(const BMat& M, BMat& C, BInt n)
//--------------------------------------------------------------------
{
    BMat   B;
    BInt   i,j,k;
    BInt   N = M.Rows();
    BrockwellVector(M,B,n);
    
    for(i=0; i<n; i++)
    {
  for(j=0; j<=i; j++)
  {
      BDat s = 0;
      for(k=0; k<=N; k++) { s += B(i,k) * B(j,k); }
      C(i,j)=s;
      C(j,i)=s;
  }
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatBrockwellInvDiag);
DefExtOpr(1, BMatBrockwellInvDiag,   "BrockwellInvDiag", 2, 2, "Matrix Real",
  "(Matrix ACOR, Real n)",
  I2("Returns the inverse standard deviation vector of the "
     "autocorrelation vector.",
     "Devuelve el vector de inversos de desviaciones estandar del vector de "
     "autocorrelaciones."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatBrockwellInvDiag::CalcContens()
//--------------------------------------------------------------------
{
    BMat&   M = Mat(Arg(1));
    BInt   N = M.Rows();
    BInt   n = BInt(Real(Arg(2)));
    BMat   B;
    BInt   i,k;

    contens_.Alloc(n,1);
    if(contens_.Rows()!=n) { return; }
    BrockwellVector(M,B,n);
    for(i=0; i<n; i++)
    {
  contens_(i,0) = 0;
  for(k=0; k<N; k++)
  {
      contens_(i,0) += B(i,k)^2;
  }
  contens_(i,0) = 1.0/Sqrt(contens_(i,0));
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatBrockwellCov);
DefExtOpr(1, BMatBrockwellCov,   "BrockwellCov", 2, 2, "Matrix Real",
  "(Matrix ACOR, Real n)",
  I2("Returns the covariance matrix of the autocorrelation vector.",
     "Devuelve la matriz de covarianzas del vector de autocorrelaciones."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatBrockwellCov::CalcContens()
//--------------------------------------------------------------------
{
    BMat&  M = Mat(Arg(1));
    BInt   n = BInt(Real(Arg(2)));
    contens_.Alloc(n,n);
    if(contens_.Rows()!=n) { return; }
    BrockwellMatrix(M,contens_,n);
}

/*
//--------------------------------------------------------------------
  DeclareContensClass(BMat, BMatTemporary, BMatApplyPolyn);
  DefExtOpr(1, BMatApplyPolyn,   ":", 2, 2, "Polyn Matrix",
  "(Polyn P, Matrix X)",
  I2(".",
     "Aplica un polinomio de retardos a las filas de una matriz."),
  BOperClassify::Statistic_);
  void BMatApplyPolyn::CalcContens()
//--------------------------------------------------------------------
{
  BPol P  = Pol(Arg(1));
  BMat X  = Mat(Arg(2));
}
*/

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatDifEq);
DefExtOpr(1, BMatDifEq,   "DifEq", 2, 4, "Ratio Matrix Matrix Matrix",
  "(Ratio R, Matrix X, Matrix X0=0, Matrix Y0=0)",
  I2(".",
     "."
     "Resuelve la ecuacion en diferencias Y = R(B)*X con los valores "
     "iniciales X0, Y0."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatDifEq::CalcContens()
//--------------------------------------------------------------------
{
    BRat& R  = Rat(Arg(1));
    BMat& X  = Mat(Arg(2));
    BMat X0;
    int nd = R.Numerator().Degree();
    int dd = R.Denominator().Degree();
    if(Arg(3)) {
      X0 = Mat(Arg(3));
    } 
    else 
    {
      X0.Alloc(nd, X.Columns());
      if(X0.Rows()!=nd) { return; }
      X0.SetAllValuesTo(BDat(0));
    }
    BMat Y0;
    if(Arg(4)) 
    {
      Y0 = Mat(Arg(4));
    } 
    else 
    {
      Y0.Alloc(dd, X.Columns());
      if(Y0.Rows()!=dd) { return; }
      Y0.SetAllValuesTo(BDat(0));
    }
    MatBackwardDifEq(R,X0,X,Y0,contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatBackDifEq);
DefExtOpr(1, BMatBackDifEq, "BackDifEq",2,4,"Ratio Matrix Matrix Matrix",
  "(Ratio R, Matrix X, Matrix X0=0, Matrix Y0=0)",
  I2(".",
     "."
     "Resuelve la ecuacion en diferencias Y = R(F)*X con los valores "
     "iniciales X0, Y0."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatBackDifEq::CalcContens()
//--------------------------------------------------------------------
{
    BRat&R  = Rat(Arg(1));
    BMat&X  = Mat(Arg(2));
    BMat X0;
    int nd = R.Numerator().Degree();
    int dd = R.Denominator().Degree();
    if(Arg(3)) {
      X0 = Mat(Arg(3));
    } 
    else 
    {
      X0.Alloc(nd, X.Columns());
      if(X0.Rows()!=nd) { return; }
      X0.SetAllValuesTo(BDat(0));
    }
    BMat Y0;
    if(Arg(4)) 
    {
      Y0 = Mat(Arg(4));
    } 
    else 
    {
      Y0.Alloc(dd, X.Columns());
      if(Y0.Rows()!=dd) { return; }
      Y0.SetAllValuesTo(BDat(0));
    }
    MatForwardDifEq(R,X0,X,Y0,contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatARMAACov);
DefExtOpr(1, BMatARMAACov,   "ARMAACov", 3, 4, "Polyn Polyn Real Real",
  "(Polyn ar, Polyn ma, Real n [, Real sigma=1])",
  I2("Returns the n-order autocovariance matrix of ARMA process.",
     "Devuelve la matriz de autovarianzas de orden n del proceso ARMA"),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatARMAACov::CalcContens()
//--------------------------------------------------------------------
{
    BPol ar = Pol(Arg(1));
    BPol ma = Pol(Arg(2));
    BInt n  = (BInt)Real(Arg(3));
    BDat sigma = 1;
    if(Arg(4)) { sigma = Dat(Arg(4)); }
    contens_ = ARMAAutoCovarianze(ar,ma,n,sigma);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetARMABackasting);
DefExtOpr(1, BSetARMABackasting,   "Backasting", 2, 2, "Ratio Matrix",
  "(Ratio R, Matrix Z)",
  I2(".",
     "."
     "Evalua los residuos A en la ecuacion en diferencias Z = R(B)*A con los "
     "valores iniciales calculados por el metodo del backasting."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BSetARMABackasting::CalcContens()
//--------------------------------------------------------------------
{
    BRat&R  = Rat(Arg(1));
    BMat&Z  = Mat(Arg(2));
    BMat A,A0,Z0;
    ARMABackasting(R,A0,A,Z0,Z);

    BUserMat* uA  = new BContensMat(A);
    BUserMat* uA0 = new BContensMat(A0);
    BUserMat* uZ0 = new BContensMat(Z0);

    BList* lst = Cons(uA,Cons(uA0,NCons(uZ0)));
    contens_.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatDurbinAutoReg);
DefExtOpr(1, BMatDurbinAutoReg,   "DurbinAutoReg", 2, 2, "Matrix Real",
  "(Matrix acor, Real n)",
  I2(".",
     "Devuelve la matriz de polinomios autoregresivos de Durbin-Levinson de "
     "una matriz de autocorrelaciones dada hasta el grado n."
     "En la fila k-esima aparecen los opuestos de los k coeficientes de "
     "grado no nulo pues el de grado 0 siempre es 1."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatDurbinAutoReg::CalcContens()
//--------------------------------------------------------------------
{
    BMat&cor = Mat(Arg(1));
    BInt order = (BInt)Real(Arg(2));
    DurbinAutoReg(cor.SubCol(0).Data(), contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSpectralDensity);
DefExtOpr(1,BMatSpectralDensity,"SpectralDensity",3,3,"Matrix Real Real",
  I2("(Matrix acor, Real periodicity, Real parts)",
     "(Matrix acor, Real periodicidad, Real partes)"),
  I2(".",
     "Devuelve la funcion de densidad espectral de las autocorrelaciones."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatSpectralDensity::CalcContens()
//--------------------------------------------------------------------
{
    BMat&cor = Mat(Arg(1));
    BInt r   = cor.Rows();
    BInt s   = (BInt)Real(Arg(2));
    BInt n   = (BInt)Real(Arg(3));
    BInt i, k;
//Std(BText("\nSpectralDensity ")+r+" "+s+" "+n);
    contens_.Alloc(n+1,2);
    if(contens_.Rows()!=n+1) { return; }
    for(i=0; i<=n; i++)
    {
  BDat u = BDat(i)/BDat(2*n);
  BDat sum = 0;
  for(k=1; k<=r; k++)
  {
      sum += cor(k-1,0)*Cos(2.0*M_PI*u*BDat(s*k));
  }
  sum *= 2;
  sum += 1;
  sum *= 2;
//  Std(BText("\n(")+u+","+sum+")");
  contens_(i,0)=u;
  contens_(i,1)=sum;
    }
}


//--------------------------------------------------------------------
class BMatSeriesStatistic: public BMatTemporary

/*! Base class to generate temporary objects of class BDat.
 */
//--------------------------------------------------------------------
{
protected:
  static BList*     instances_;
   BSyntaxObject*   ser_;
   BDate     ini_;
   BDate     fin_;
public:
  // Constructors and destructors: inline
  BMatSeriesStatistic(BList* arg = NIL);
  void  PutArgs(BSyntaxObject* ser, const BDate& ini, const BDate& fin);
};


#define DeclareMatSeriesStatisticClass(CLASS)         \
class CLASS : public  BMatSeriesStatistic {                  \
  public:                                                    \
    CLASS(BList* arg = NIL) : BMatSeriesStatistic(arg) { }   \
    ~CLASS() { }                                             \
    void CalcContens();                                      \
};                                                           \
DeclareEvaluator(CLASS)


//--------------------------------------------------------------------
BMatSeriesStatistic::BMatSeriesStatistic(BList* arg)
//--------------------------------------------------------------------
    : BMatTemporary(arg), ser_(Arg(1))
{
    if(arg)
    {
  if(Arg(2)) { ini_ = Date(Arg(2)); }
  if(!ini_.HasValue() || (ini_<TsrR(ser_).FirstDate()))
  { ini_=TsrR(ser_).FirstDate(); }
  if(!ini_.HasValue()) { ini_= BDate::DefaultFirst(); }
  if(Arg(3)) { fin_ = Date(Arg(3)); }
  if(!fin_.HasValue() || (fin_>TsrR(ser_).LastDate()))
  { fin_=TsrR(ser_).LastDate(); }
  if(!fin_.HasValue()) { fin_= BDate::DefaultLast(); }
    }
}


//--------------------------------------------------------------------
void BMatSeriesStatistic::PutArgs(BSyntaxObject* ser,
          const BDate& ini,
          const BDate& fin)
//--------------------------------------------------------------------
{
    ser_=ser;
    ini_=ini;
    fin_=fin;
}


//--------------------------------------------------------------------
DeclareMatSeriesStatisticClass(BMatAutoCov);
DefExtOpr(1, BMatAutoCov,   "AutoCov", 2, 2, "Serie Real",
  "(Serie ser, Real n)",
  I2("Returns the sample autocovariances vector of dimension n of a "
     "series between two dates.",
     "Devuelve el vector de autocovarianzas muestrales de dimension n "
     "de una serie entre dos fechas."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatAutoCov::CalcContens()
//--------------------------------------------------------------------
{
    BSerieTable table;
    table.AddSerie(Tsr(Arg(1)));
    table.Fill();
    BInt        order = (BInt)Real(Arg(2));
    contens_.Alloc(2,order);
    if(contens_.Rows()!=2) { return; }
    BArray<BDat> cov;
    AutoCov(table.Data().Data(), cov, order, 1);
    for(BInt i=0; i<order; i++)
    {
  contens_(0,i)=i;
  contens_(1,i)=cov(i);
    }
    contens_ = contens_.T();
}


//--------------------------------------------------------------------
DeclareMatSeriesStatisticClass(BMatAutoCor);
DefExtOpr(1, BMatAutoCor,   "AutoCor", 2, 2, "Serie Real",
  "(Serie ser, Real n)",
  I2("Returns the sample autocorrelations vector of dimension n of a series "
     "between two dates.",
     "Devuelve el vector de autocorrelaciones muestrales de dimension n de "
     "una serie entre dos fechas."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatAutoCor::CalcContens()
//--------------------------------------------------------------------
{
    BSerieTable table;
    table.AddSerie(Tsr(Arg(1)));
    table.Fill();
    BInt        order = (BInt)Real(Arg(2));
    contens_.Alloc(3,order);
    if(contens_.Rows()!=3) { return; }
    BDat sigma = 1/Sqrt(table.NumDates());
    BArray<BDat> c(order);
    AutoCor(table.Data().Data(), c, order, 1);
    for(BInt i=0; i<order; i++)
    {
  contens_(0,i)=i+1;
  contens_(1,i)=c(i);
  contens_(2,i)=sigma;
    }
    contens_ = contens_.T();
}


//--------------------------------------------------------------------
void SerieAutoCor(BSyntaxObject* ser, int lag, const BText& fName)
//--------------------------------------------------------------------
{
    BList* args = LstStack
  (
      NIL,
      ser,
      new BContensDat(BDat(lag)),
      NIL
      );
    BMatAutoCor* freq = new BMatAutoCor(args);
    BList* lst = NCons(freq);
    ApplyMethod("Matrix","Table",lst,fName);
    DESTROY(lst);
}


//--------------------------------------------------------------------
DeclareMatSeriesStatisticClass(BMatPartAutoCor);
DefExtOpr(1, BMatPartAutoCor,    "PartAutoCor", 2, 2, "Serie Real",
  "(Serie ser, Real n)",
  I2("Returns the sample partial autocorrelations vector of dimension n "
     "of a series between two dates.",
     "Devuelve el vector de autocorrelaciones parciales muestrales de "
     "dimension n de una serie entre dos fechas."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatPartAutoCor::CalcContens()
//--------------------------------------------------------------------
{
    BSerieTable table;
    table.AddSerie(Tsr(Arg(1)));
    table.Fill();
    BInt        order = (BInt)Real(Arg(2));
    contens_.Alloc(3,order);
    if(contens_.Rows()!=3) { return; }
    BDat sigma = 1/Sqrt(table.NumDates());
    BArray<BDat> cor, p(order);
    AutoCor(table.Data().Data(), cor, order, 1);
    PartAutoCor(cor, p);
    for(BInt i=0; i<order; i++)
    {
  contens_(0,i)=i+1;
  contens_(1,i)=p(i);
  contens_(2,i)=sigma;
    }
    contens_ = contens_.T();
}


//--------------------------------------------------------------------
void SeriePartAutoCor(BSyntaxObject* ser, int lag, const BText& fName)
//--------------------------------------------------------------------
{
    BList* args = LstStack
  (
      NIL,
      ser,
      new BContensDat(BDat(lag)),
      NIL
        );
    BMatPartAutoCor* freq = new BMatPartAutoCor(args);
    BList* lst = NCons(freq);
    ApplyMethod("Matrix","Table",lst,fName);
    DESTROY(lst);
}


//--------------------------------------------------------------------
DeclareMatSeriesStatisticClass(BMatInverseAutoCor);
DefExtOpr(1, BMatInverseAutoCor,   "InverseAutoCor", 2, 2, "Serie Real",
  "(Serie ser, Real n)",
  I2("Returns the sample inverse autocorrelations vector of dimension n "
     "of a series between two dates.",
     "Devuelve el vector de autocorrelaciones inversas muestrales de "
     "dimension n de una serie entre dos fechas."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatInverseAutoCor::CalcContens()
//--------------------------------------------------------------------
{
    BSerieTable table;
    table.AddSerie(Tsr(Arg(1)));
    table.Fill();
    BInt        order = (BInt)Real(Arg(2));
    contens_.Alloc(3,order);
    if(contens_.Rows()!=3) { return; }
    BDat sigma = 1/Sqrt(table.NumDates());
    BArray<BDat> cor, ia(order);
    AutoCor(table.Data().Data(), cor, order, 1);
    InverseAutoCor(cor, ia);
    for(BInt i=0; i<order; i++)
    {
  contens_(0,i)=i+1;
  contens_(1,i)=ia(i);
  contens_(2,i)=sigma;
    }
    contens_ = contens_.T();
}


//--------------------------------------------------------------------
void SerieInverseAutoCor(BSyntaxObject* ser, int lag, const BText& fName)
//--------------------------------------------------------------------
{
    BList* args = LstStack
  (
      NIL,
      ser,
      new BContensDat(BDat(lag)),
      NIL
        );
    BMatInverseAutoCor* freq = new BMatInverseAutoCor(args);
    BList* lst = NCons(freq);
    ApplyMethod("Matrix","Table",lst,fName);
    DESTROY(lst);
}




//--------------------------------------------------------------------
DeclareMatSeriesStatisticClass(BMatInvPartAutoCor);
DefExtOpr(1, BMatInvPartAutoCor,   "InvPartAutoCor", 2, 2, "Serie Real",
  "(Serie ser, Real n)",
  I2("Returns the sample inverse partial autocorrelations vector of "
     "dimension n of a series between two dates.",
     "Devuelve el vector de autocorrelaciones parciales inversas muestrales "
     "de dimension n de una serie entre dos fechas."),
    BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatInvPartAutoCor::CalcContens()
//--------------------------------------------------------------------
{
    BSerieTable table;
    table.AddSerie(Tsr(Arg(1)));
    table.Fill();
    BInt        order = (BInt)Real(Arg(2));
    contens_.Alloc(3,order);
    if(contens_.Rows()!=3) { return; }
    BDat sigma = 1/Sqrt(table.NumDates());
    BArray<BDat> cor,ipa(order);
    AutoCor(table.Data().Data(), cor, order, 1);
    InvPartAutoCor(cor, ipa);
    for(BInt i=0; i<order; i++)
    {
  contens_(0,i)=i+1;
  contens_(1,i)=ipa(i);
  contens_(2,i)=sigma;
    }
    contens_ = contens_.T();
}


//--------------------------------------------------------------------
void SerieInvPartAutoCor(BSyntaxObject* ser, int lag, const BText& fName)
//--------------------------------------------------------------------
{
    BList* args = LstStack
  (
      NIL,
      ser,
      new BContensDat(BDat(lag)),
      NIL
      );
    BMatInvPartAutoCor* freq = new BMatInvPartAutoCor(args);
    BList* lst = NCons(freq);
    ApplyMethod("Matrix","Table",lst,fName);
    DESTROY(lst);
}


void GetReal(BSyntaxObject* obj, BArray<BDat>& p)
{
    BDat dat = Dat(obj);
    p.Add(dat);
};

void GetPolyn(BSyntaxObject* obj, BArray<BDat>& p)
{
    BPol pol = Pol(obj);
    BInt j;
    if((pol.Size()!=1)||(pol[0].Coef()!=0))
    {
  for(j=0; j<pol.Size(); j++) { p.Add(pol[j].Coef()); }
    }
};

void GetRatio(BSyntaxObject* obj, BArray<BDat>& p)
{
    BRat rat = Rat(obj);
    BPol num = rat.Numerator  ();
    BPol den = rat.Denominator();
    BInt j;
    for(j=1; j<num.Size(); j++) { p.Add(num[j].Coef()); }
    for(j=1; j<den.Size(); j++) { p.Add(den[j].Coef()); }
};

void GetMatrix(BSyntaxObject* obj, BArray<BDat>& p)
{
    BMat&mat = Mat(obj);
    for(BInt i=0; i<mat.Rows(); i++)
    {
  for(BInt j=0; j<mat.Columns(); j++)
  {
      p.Add(mat(i,j));
  }
    }
};


void GetSet(BSyntaxObject* obj, BArray<BDat>& p)
{
    BSet& sample = Set(obj);
    BInt i;
    for(i=1; i<=sample.Card(); i++)
    {
  if(sample[i])
  {
      BGrammar* gra = sample[i]->Grammar();
           if(gra==GraReal   ()) { GetReal   (sample[i],p); }
      else if(gra==GraPolyn  ()) { GetPolyn  (sample[i],p); }
      else if(gra==GraRatio  ()) { GetRatio  (sample[i],p); }
      else if(gra==GraMatrix ()) { GetMatrix (sample[i],p); }
      else if(gra==GraSet    ()) { GetSet    (sample[i],p); }
  }
    }
};

void GetDeepSet(BSyntaxObject* obj, BArray<BDat>& p)
{
    p.ReallocBuffer(100);
    p.ReallocBuffer(0);
    GetSet(obj,p);
};



//--------------------------------------------------------------------
DeclareContensClass(BSet, BMatTemporary, BMatGetNumeric);
DefExtOpr(1, BMatGetNumeric, "GetNumeric", 1, 1, "Set",
  I2("(Set sample)",
     "(Set ejemplo)"),
  I2(".",
     "Construye un conjunto semejante al ejemplo dado en el que "
     ", opcionalmente, se sustituyen los datos numericos por los datos de "
     "la matriz dada."),
    BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BMatGetNumeric::CalcContens()
//--------------------------------------------------------------------
{
    BArray<BDat> p;
    GetDeepSet(Arg(1),p);
    contens_ = BMat(1,p.Size(),p.Buffer());
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatEvalPol);
DefExtOpr(1, BMatEvalPol, "EvalPol", 2, 2, "Polyn Matrix",
  "(Polyn p, Matrix x)",
  I2(".",
     "Evalua la funcion polinomial correspondiente en cada celda de la "
     "matriz dada."),
    BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BMatEvalPol::CalcContens()
//--------------------------------------------------------------------
{
    BPol pol = Pol (Arg(1));
    BMat&x   = Mat(Arg(2));
    BInt i, j;
    contens_ = x;
    
    for(i=0; i<x.Rows(); i++)
    {
  for(j=0; j<x.Columns(); j++)
  {
      contens_(i,j) = pol.Eval(x(i,j));
  }
    }
}

//--------------------------------------------------------------------------
DeclareContensClass(BSet, BMatTemporary, BMatCokeBoots);
DefExtOpr(1, BMatCokeBoots, "CokeBoots", 8, 8, "Set Code Code "
                                                 "Code Code Code Code Real",
          I2("(Set model, Code Estim, Code GetEffect, "
             "Code GetNoiseStruct, Code GetParam, Code GetResiduals, "
             "Code NewModel, Real B)",
             "(Set model, Code Estim, Code GetEffect, "
             "Code GetNoiseStruct, Code GetParam, Code GetResiduals, "
             "Code NewModel, Real B)"),
          I2("Perform a bootstrap", "Ejecuta un bootstrap"),
          BOperClassify::Sthocastic_);
//--------------------------------------------------------------------------

#define TOL_DEBUG_BOOTS
#ifdef TOL_DEBUG_BOOTS
#define BOOTS_STD(T) Std((T)+"\n");
#else
#define BOOTS_STD(T)
#endif

inline
void add_tmp_result(BSyntaxObject * so,
                    BSyntaxObject * container[], int & last)
{
  container[++last] = so;
  so->IncNRefs();
}

inline
void kill_tmp_results(BSyntaxObject * container[], int last)
{  
  while(last>=0) {
    container[last]->DecNRefs();
    DESTROY(container[last]);
    --last;
  }
}

inline
void boots_kill(BSyntaxObject * so_estim0,
                BSyntaxObject * so_newoutput,
                BSyntaxObject * so_model_boots,
                double * sample_param,
                double * buffer_tsr_effect0,
                gsl_matrix * gsl_matrix_param,
                BSyntaxObject * container_so[],
                int last_so)
{
  /*
  if (so_estim0)
    so_estim0->DecNRefs();
  if (so_newoutput)
    so_newoutput->DecNRefs();
  if (so_model_boots)
    so_model_boots->DecNRefs();
  */
  kill_tmp_results(container_so, last_so);
  if (gsl_matrix_param)
    gsl_matrix_free(gsl_matrix_param);
  if (sample_param)
    delete []sample_param;
  if (buffer_tsr_effect0)
    delete []buffer_tsr_effect0;
}

#define BOOTS_KILL_RESULTS \
boots_kill(so_estim0,so_newoutput,so_model_boots,\
           sample_param,buffer_tsr_effect0, gsl_matrix_param,\
           container_so,last_so);

inline
int boots_check(BSyntaxObject * so, const BText & msg,
                BSyntaxObject * so_estim0,
                BSyntaxObject * so_newoutput,
                BSyntaxObject * so_model_boots,
                double * sample_param,
                double * buffer_tsr_effect0,
                gsl_matrix * gsl_matrix_param,
                BSyntaxObject * container_so[],
                 int last_so)
{
  if(!so) {
    Error(msg);
    BOOTS_KILL_RESULTS;
    return 0;
  }
  return 1;
}
#define BOOTS_CHECK_SO(so, msg) \
if (!boots_check(so, msg, \
            so_estim0,so_newoutput,so_model_boots,\
            sample_param,buffer_tsr_effect0, gsl_matrix_param,\
            container_so,last_so)) return;
#define BOOTS_CHECK_SO_(so, msg) \
if (!boots_check(so, msg, \
            so_estim0,so_newoutput,so_model_boots,\
            sample_param,buffer_tsr_effect0, gsl_matrix_param,\
            container_so,last_so)) return 0;
/*
#define BOOTS_CHECK_SO(so, msg) \
if(!so) { \
  Error(msg); \
  BOOTS_KILL_RESULTS; \
  return; \
}
*/

/* Check the result from each Evaluator to detect failure !!!!*/
//--------------------------------------------------------------------------
void BMatCokeBoots::CalcContens()
//--------------------------------------------------------------------------
{
  BCode * cd_estim =     &Code(Arg(2));
  BCode * cd_effect =    &Code(Arg(3));
  BCode * cd_noise_str = &Code(Arg(4));
  BCode * cd_param =     &Code(Arg(5));
  BCode * cd_getres =    &Code(Arg(6));
  BCode * cd_newmodel =  &Code(Arg(7));
  BDat  & dat_B =        Dat(Arg(8));
  
  if (cd_estim->Grammar() != GraSet()) {
    Error(I2("Estim argument should return a Set",
             "El argumento Estim debe retornar un Set")+
          ":"+cd_estim->Name());
    return;
  }
  if (cd_effect->Grammar() != GraSerie()) {
    Error(I2("GetEffect argument should return a Serie",
             "El argumento GetEffect debe retornar una Serie")+
          ":"+cd_effect->Name());
    return;
  }
  if (cd_noise_str->Grammar() != GraRatio()) {
    Error(I2("GetNoiseStruct argument should return a Ration",
             "El argumento NoiseStruct debe retornar un Ration")+
          ":"+cd_noise_str->Name());
    return;
  }
  if (cd_param->Grammar() != GraSet()) {
    Error(I2("GetParam argument should return a Set",
             "El argumento GetParam debe retornar un Set")+
      ":"+cd_param->Name());
    return;
  }
  
  if (cd_getres->Grammar() != GraSerie()) {
    Error(I2("GetResiduals argument should return a Serie",
             "El argumento GetResiduals debe retornar un Serie")+
      ":"+cd_getres->Name());
    return;
  }
  
  if (cd_newmodel->Grammar() != GraSet()) {
    Error(I2("NewModel argument should return a Set",
             "El argumento NewModel debe retornar un Set")+
      ":"+cd_newmodel->Name());
    return;
  }
  
  int B;
  if (!dat_B.IsKnown() || dat_B.Value() <= 0) {
    Warning(I2("Boostrap size should be a positive value, "
               "assuming BSize = 1000",
               "El tamaño del boostrap debe ser positivo, "
               "asumiremos BSize = 1000"));
    B = 1000;
  } else {
    B = int(dat_B.Value());
  }  

  BSyntaxObject * container_so[16];
  int last_so = -1;
  
  BSet * ptr_set_tmp;
  BSyntaxObject * so_newoutput = NULL;
  BSyntaxObject * so_model_boots = NULL;
  double * buffer_tsr_effect0 = NULL;
  double * sample_param = NULL;
  gsl_matrix * gsl_matrix_param = NULL;
  size_t i;
  
  /* initial estimation */

  BOOTS_STD(BText("Doing initial estimation"));
  
  BSyntaxObject * so_estim0 = cd_estim->Evaluator(NCons(Arg(1)));
  
  BOOTS_CHECK_SO(so_estim0, BText("failed initial estimation\n"));
  
  // should I have to make Do()?
 
  //so_estim0->IncNRefs();  // keep it save
  add_tmp_result(so_estim0, container_so, last_so);
  
  /* get parameters estimated in initial estimation */
  BSyntaxObject * so_param0 = cd_param->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO(so_param0, "failed evaluation of GetParam for "
                 "initial estimation");
  add_tmp_result(so_param0, container_so, last_so);
  // sould I have to make Do()?
  ptr_set_tmp = &Set(so_param0);
  gsl_matrix_param = gsl_matrix_alloc(B, ptr_set_tmp->Card());
  
  /* evaluate Effect for initial estimation */
  BSyntaxObject * so_effect0 = cd_effect->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO(so_effect0, "failed evaluation of GetEffect");
  add_tmp_result(so_effect0, container_so, last_so);
  // sould I have to make Do()?
  
  BUserTimeSerie * tsr_effect0 = Tsr(so_effect0);
  
  BOOTS_STD(BText("\nNRef for effect0 = ") + so_effect0->NRefs());

  BSerieTable table_effect0;
  table_effect0.AddSerie(tsr_effect0);  
  table_effect0.Fill();

  BOOTS_STD(BText("NRef for effect0 after table = ") + so_effect0->NRefs());
  
  buffer_tsr_effect0 = new double[table_effect0.NumDates()];
  for (i = 0; i < (size_t)table_effect0.NumDates(); i++) {
    buffer_tsr_effect0[i] = table_effect0.Data()(0,i).Value();
  }
  
  /* get the residuals */
  BSyntaxObject * so_residuals = cd_getres->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO(so_residuals, "failed evaluation of GetResiduals");
  add_tmp_result(so_residuals, container_so, last_so);

  BUserTimeSerie * tsr_residuals0 = Tsr(so_residuals);
  // residuals length
  int len_residuals = tsr_residuals0->GetLength();
  // array of BDat with the residuals.
  // this can be done because resdiduals is a buffered Tsr.
  BDat* buffer_tsr_residuals0 = (BDat*)tsr_residuals0->Data().Buffer();

  /* get the ARIMA structure of the noise N_t */
  BSyntaxObject * so_rat_ARIMA = cd_noise_str->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO(so_rat_ARIMA, "failed evaluation of GetNoiseStruct");
  add_tmp_result(so_rat_ARIMA, container_so, last_so);
  so_rat_ARIMA->Do();
  BRat * ptr_rat_ARIMA = &Rat(so_rat_ARIMA);
  
  BOOTS_STD(BText("ARIMA struct = ") + ptr_rat_ARIMA->Name());

  int has_ARIMA = (ptr_rat_ARIMA->Numerator()!=BDat(1.0) ||
                   ptr_rat_ARIMA->Denominator()!=BDat(1.0));
  
  BOOTS_STD(BText("Noise ")+(has_ARIMA?"has":"hasn't")+" structure");
  
  /* prepare the model definition to be used inside the bootstrap,
   * will reuse the item definition. Instead of using ["Definition"]
   * I think is more general to provide a DefineModel Code argument. */

  so_newoutput = tsr_residuals0->CopyContens();
  BOOTS_CHECK_SO(so_newoutput,
                 "failed creating temporal boostrap output serie");
  add_tmp_result(so_newoutput, container_so, last_so);
  //so_newoutput->IncNRefs();
  //so_newoutput->IncNRefs();
  // now create the boostrap model.
  BOOTS_STD(BText("newoutput->NRefs = ")+so_newoutput->NRefs());
  so_model_boots = cd_newmodel->Evaluator(Cons(so_newoutput,
                                                Cons(Arg(1),NIL)));
  BOOTS_STD(BText("after buildinf new model, newoutput->NRefs = ")
            +so_newoutput->NRefs());
  BOOTS_CHECK_SO(so_model_boots, "failed creation of temporal boostrap model");
  add_tmp_result(so_model_boots, container_so, last_so);
  //so_model_boots->IncNRefs();
  
  // get a reference to the output serie
  BUserTimeSerie * tsr_newoutput = Tsr(so_newoutput);
  BDat * buffer_tsr_output = (BDat*)tsr_newoutput->Data().Buffer();

  /*
  BSerieTable table_output;
  table_output.Fill(NCons(tsr_output_original), first, last);
  BData buffer_tsr(table_output.NumDates());
  for (size_t i = 0; i < buffer_tsr.Size(); i++) {
    buffer_tsr[i] = table_output.Data()(0,i);
  }
  BTsrPrimary * tsr_output = new BTsrPrimary("","",
                                             tsr_output_original->Dating(),
                                             first, last, buffer_tsr);
  BDat * buffer_tsr_output = tsr_output->GetDataBuffer().GetBuffer();
  // now replace the ouput
  set_model_boots.ChangeElement(1, tsr_output);
  */
  
  /* main bootstrap loop --
   *
   * - resample residuals
   * - rebuild output: rebuild noise from residuals then
   *     output = noise + effect.
   * - estimate
   * - collect parameters
   */

  // matrix to solve de noise.
  BMat mat_noise(len_residuals,1); 
  BDat * buffer_mat_noise = mat_noise.GetData().GetBuffer();
  // matrix to put the resampled residuals.
  BMat mat_residuals(len_residuals,1);
  BDat * buffer_mat_residuals = mat_residuals.GetData().GetBuffer();
  size_t j, i_rand;
  
  double y;
  BSyntaxObject * so_estim;  // estimation result.
  BSyntaxObject * so_param;  // parameters estimated.
  
#define LO_INDEX       0
#define HI_INDEX       1
#define PARAM_INDEX    2
#define PARAM_M_INDEX  3
#define PARAM_SD_INDEX 4
#define CONTENTS_COLUMNS 5

  for (int iB = 0; iB < B; iB++) {
    
    // resample residuals.
    if (has_ARIMA) {
      for (i = 0; i < (int)len_residuals; i++) {
        i_rand = gsl_rng_uniform_int(getGslRng(), len_residuals);
        buffer_mat_residuals[i] = buffer_tsr_residuals0[i_rand];
      }
      // rebuild noise.
      MatBackwardDifEq(*ptr_rat_ARIMA, mat_residuals, mat_noise);
    } else {
      // noise is equal to residuals
      for (i = 0; i < (int)len_residuals; i++) {
        i_rand = gsl_rng_uniform_int(getGslRng(), len_residuals);
        buffer_mat_noise[i] = buffer_tsr_residuals0[i_rand];
      }
    }
    // rebuild output.
    for (i = 0; i < (int)len_residuals; i++) {
      if (buffer_tsr_output[i].IsKnown()) {
        y = buffer_tsr_effect0[i] + buffer_mat_noise[i].Value();
        buffer_tsr_output[i].PutValue(y);
      }
    }
    
    // estimate.
    so_estim = cd_estim->Evaluator(NCons(so_model_boots));
    // sould I have to make Do()?
    so_estim->IncNRefs();
    // collect parameters.
    so_param = cd_param->Evaluator(NCons(so_estim));
    // sould I have to make Do()?
    ptr_set_tmp = &Set(so_param);
    int row_offset = iB*B;
    for (j = 1; j <= ptr_set_tmp->Card(); j++) {
      gsl_matrix_set(gsl_matrix_param, iB, j-1, Dat((*ptr_set_tmp)[j]).Value());
    }    
    DESTROY(so_param);
    assert(so_estim->NRefs()==1);
    so_estim->DecNRefs();
    DESTROY(so_estim);
  }
  /*
  // restore the output serie
  set_model_boots.ChangeElement(1, tsr_output_original);
  DESTROY(tsr_output);
  */
  
  /* computing intervals */
  double p;
  sample_param = new double[B];
  double av, sdev;
  
  ptr_set_tmp = &Set(so_param0);
  for (j = 0; j < ptr_set_tmp->Card(); j++) {
    for (i = 0; i < B; i++) {
      sample_param[i] = gsl_matrix_get(gsl_matrix_param,i,j);
    }
    av=gsl_stats_mean(sample_param, 1, B);
    sdev=gsl_stats_sd_m(sample_param, 1, B, av);
    
    p = Dat((*ptr_set_tmp)[j+1]).Value(); // Set are indexed starting from 1
    contens_(j,LO_INDEX) = 2*p - av - 1.96*sdev;
    contens_(j,HI_INDEX) = 2*p - av + 1.96*sdev;
    contens_(j,PARAM_INDEX) = p;
    contens_(j,PARAM_M_INDEX) = av;
    contens_(j,PARAM_SD_INDEX) = sdev;
  }

  BOOTS_KILL_RESULTS;
}

int TOL_BoostrapSample(BSyntaxObject * model,
                       BCode * cd_estim, BCode * cd_effect,
                       BCode * cd_noise_str, BCode * cd_param,
                       BCode * cd_getres, BCode * cd_newmodel,
                       size_t B, BMat & boots);

//--------------------------------------------------------------------------
DeclareContensClass(BSet, BMatTemporary, BMatBootsSample);
DefExtOpr(1, BMatBootsSample, "BootsSample", 8, 8, "Set Code Code "
                                                   "Code Code Code Code Real",
          I2("(Set model, Code Estim, Code GetEffect, "
             "Code GetNoiseStruct, Code GetParam, Code GetResiduals, "
             "Code NewModel, Real B)",
             "(Set model, Code Estim, Code GetEffect, "
             "Code GetNoiseStruct, Code GetParam, Code GetResiduals, "
             "Code NewModel, Real B)"),
          I2("Perform a bootstrap", "Ejecuta un bootstrap"),
          BOperClassify::Sthocastic_);
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void BMatBootsSample::CalcContens()
//--------------------------------------------------------------------------
{
  BCode * cd_estim =     &Code(Arg(2));
  BCode * cd_effect =    &Code(Arg(3));
  BCode * cd_noise_str = &Code(Arg(4));
  BCode * cd_param =     &Code(Arg(5));
  BCode * cd_getres =    &Code(Arg(6));
  BCode * cd_newmodel =  &Code(Arg(7));
  BDat  & dat_B =        Dat(Arg(8));
  
  if (cd_estim->Grammar() != GraSet()) {
    Error(I2("Estim argument should return a Set",
             "El argumento Estim debe retornar un Set")+
          ":"+cd_estim->Name());
    return;
  }
  if (cd_effect->Grammar() != GraSerie()) {
    Error(I2("GetEffect argument should return a Serie",
             "El argumento GetEffect debe retornar una Serie")+
          ":"+cd_effect->Name());
    return;
  }
  if (cd_noise_str->Grammar() != GraRatio()) {
    Error(I2("GetNoiseStruct argument should return a Ration",
             "El argumento NoiseStruct debe retornar un Ration")+
          ":"+cd_noise_str->Name());
    return;
  }
  if (cd_param->Grammar() != GraSet()) {
    Error(I2("GetParam argument should return a Set",
             "El argumento GetParam debe retornar un Set")+
      ":"+cd_param->Name());
    return;
  }
  
  if (cd_getres->Grammar() != GraSerie()) {
    Error(I2("GetResiduals argument should return a Serie",
             "El argumento GetResiduals debe retornar un Serie")+
      ":"+cd_getres->Name());
    return;
  }
  
  if (cd_newmodel->Grammar() != GraSet()) {
    Error(I2("NewModel argument should return a Set",
             "El argumento NewModel debe retornar un Set")+
      ":"+cd_newmodel->Name());
    return;
  }

  int B;
  if (!dat_B.IsKnown() || dat_B.Value() <= 0) {
    Warning(I2("Boostrap size should be a positive value, "
               "assuming BSize = 1000",
               "El tamaño del boostrap debe ser positivo, "
               "asumiremos BSize = 1000"));
    B = 1000;
  } else {
    B = int(dat_B.Value());
  }  
  TOL_BoostrapSample(Arg(1), cd_estim, cd_effect, cd_noise_str, cd_param,
                     cd_getres, cd_newmodel, B, contens_);
}

inline
void TOL_GetData(BUserTimeSerie * uts, const BDate & f, const BDate & l,
                 BData & buffer)
{
  uts->IncNRefs();
  {
    BSerieTable table;
    table.AddSerie(uts);
    table.Fill(f, l);
    
    for (int i = 0; i < table.NumDates(); i++) {
      buffer[i] = table.Data()(0,i).Value();
    }
  }
  uts->DecNRefs();
}

inline
void TOL_GetData(BUserTimeSerie * uts, const BDate & f, const BDate & l,
                 double * buffer)
{
  uts->IncNRefs();
  {
    BSerieTable table;
    table.AddSerie(uts);
    table.Fill(f, l);
    
    for (int i = 0; i < table.NumDates(); i++) {
      buffer[i] = table.Data()(0,i).Value();
    }
  }
  uts->DecNRefs();
}
                 
int TOL_BoostrapSample(BSyntaxObject * model,
                       BCode * cd_estim, BCode * cd_effect,
                       BCode * cd_noise_str, BCode * cd_param,
                       BCode * cd_getres, BCode * cd_newmodel,
                       size_t B, BMat & boots)
{
  BSyntaxObject * container_so[16];
  int last_so = -1;
  
  BSyntaxObject * so_tmp;
  BSet * ptr_set_tmp;
  BSyntaxObject * so_newoutput = NULL;
  BSyntaxObject * so_model_boots = NULL;
  double * buffer_tsr_effect0 = NULL;
  double * sample_param = NULL;
  gsl_matrix * gsl_matrix_param = NULL;
  size_t i;
  
  /* initial estimation */

  BOOTS_STD(BText("Doing initial estimation"));
  
  BSyntaxObject * so_estim0 = cd_estim->Evaluator(NCons(model));
  
  BOOTS_CHECK_SO_(so_estim0, BText("failed initial estimation\n"));
  
  // should I have to make Do()?
 
  //so_estim0->IncNRefs();  // keep it save
  add_tmp_result(so_estim0, container_so, last_so);
  
  /* get parameters estimated in initial estimation */
  so_tmp = cd_param->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO_(so_tmp, "failed evaluation of GetParam for "
                 "initial estimation");
  // sould I have to make Do()?
  ptr_set_tmp = &Set(so_tmp);
  boots.Alloc(B+1, ptr_set_tmp->Card());
  for (i=0; i < boots.Columns(); i++)
    boots(0,i) = Dat((*ptr_set_tmp)[i+1]);

  /* get the ARIMA structure of the noise N_t */
  BSyntaxObject * so_rat_ARIMA = cd_noise_str->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO_(so_rat_ARIMA, "failed evaluation of GetNoiseStruct");
  add_tmp_result(so_rat_ARIMA, container_so, last_so);
  so_rat_ARIMA->Do();
  BRat * ptr_rat_ARIMA = &Rat(so_rat_ARIMA);
  BInt P, Q;

  Q = ptr_rat_ARIMA->Numerator().Degree();
  P = ptr_rat_ARIMA->Denominator().Degree();
  
  BOOTS_STD(BText("ARIMA struct = ") + ptr_rat_ARIMA->Name());

  int has_ARIMA = (P > 0 || Q > 0);
  
  BOOTS_STD(BText("Noise ")+(has_ARIMA?"has":"hasn't")+" structure");
  
  /* get the residuals */
  BSyntaxObject * so_residuals = cd_getres->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO_(so_residuals, "failed evaluation of GetResiduals");
  add_tmp_result(so_residuals, container_so, last_so);
 
  BUserTimeSerie * tsr_residuals0 = Tsr(so_residuals);
  // residuals length
  int len_residuals = tsr_residuals0->GetLength();
  // array of BDat with the residuals.
  // this can be done because residuals is a buffered Tsr.
  BDat* buffer_tsr_residuals0 = (BDat*)tsr_residuals0->Data().Buffer();
  BUserTimeSet * tms = tsr_residuals0->Dating();
  BDate first_out = tms->Prev(tsr_residuals0->FirstDate(), P);
  BDate last_out = tsr_residuals0->LastDate();

  /* evaluate Effect for initial estimation */
  BSyntaxObject * so_effect0 = cd_effect->Evaluator(NCons(so_estim0));
  BOOTS_CHECK_SO_(so_effect0, "failed evaluation of GetEffect");
  //add_tmp_result(so_effect0, container_so, last_so);
  // sould I have to make Do()?
  
  BUserTimeSerie * tsr_effect0 = Tsr(so_effect0);
  
  BOOTS_STD(BText("\nNRef for effect0 = ") + so_effect0->NRefs());

  buffer_tsr_effect0 = new double[len_residuals+P];
  TOL_GetData(tsr_effect0, first_out, last_out, buffer_tsr_effect0);
  BOOTS_STD(BText("NRef for effect0 after table = ") + so_effect0->NRefs());
  
  DESTROY(so_effect0);  
  
  /* prepare the model definition to be used inside the bootstrap,
   * will reuse the item definition. Instead of using ["Definition"]
   * I think is more general to provide a DefineModel Code argument. */

  ptr_set_tmp = &Set(model);
  BUserTimeSerie * output = Tsr((*ptr_set_tmp)[1]);
  BOOTS_CHECK_SO_(output, "First element of model definition "
                  "should be a Serie ");
  
  // fill the first P value of the output.
  BData buffer_Y(len_residuals+P);
  BOOTS_STD(BText("NRef for original out before TOL_GetData = ") + output->NRefs());
  TOL_GetData(output, first_out, last_out, buffer_Y);
  BOOTS_STD(BText("NRef for original out after TOL_GetData = ") + output->NRefs());
  /*
  BTsrPrimary * tsr_output = new BTsrPrimary("","",
                                             tms,
                                             first_out, last_out, buffer_Y);
  BDat * buffer_tsr_output = tsr_output->GetDataBuffer().GetBuffer();
  add_tmp_result(tsr_output, container_so, last_so);
  */
  /*
  so_newoutput = tsr_residuals0->CopyContens();
  BOOTS_CHECK_SO_(so_newoutput,
                "failed creating temporal boostrap output serie");
  add_tmp_result(so_newoutput, container_so, last_so);
  */
  //so_newoutput->IncNRefs();
  //so_newoutput->IncNRefs();
  //
  // now create the boostrap model.
  /*
  BOOTS_STD(BText("tsr_output->NRefs = ")+tsr_output->NRefs());
  so_model_boots = cd_newmodel->Evaluator(Cons(tsr_output,
                                               Cons(model,NIL)));
  BOOTS_STD(BText("after buildinf new model, tsr_output->NRefs = ")
            +tsr_output->NRefs());
  BOOTS_CHECK_SO_(so_model_boots, "failed creation of temporal boostrap model");
  add_tmp_result(so_model_boots, container_so, last_so);
  */
  //so_model_boots->IncNRefs();
  
  // get a reference to the output serie
  //BUserTimeSerie * tsr_newoutput = Tsr(so_newoutput);
  //BDat * buffer_tsr_output = tsr_newoutput->GetDataBuffer().GetBuffer();

  /*
  BSerieTable table_output;
  table_output.Fill(NCons(tsr_output_original), first, last);
  BData buffer_tsr(table_output.NumDates());
  for (size_t i = 0; i < buffer_tsr.Size(); i++) {
    buffer_tsr[i] = table_output.Data()(0,i);
  }
  BTsrPrimary * tsr_output = new BTsrPrimary("","",
                                             tsr_output_original->Dating(),
                                             first, last, buffer_tsr);
  BDat * buffer_tsr_output = tsr_output->GetDataBuffer().GetBuffer();
  // now replace the ouput
  set_model_boots.ChangeElement(1, tsr_output);
  */
  
  /* main bootstrap loop --
   *
   * - resample residuals
   * - rebuild output: rebuild noise from residuals then
   *     output = noise + effect.
   * - estimate
   * - collect parameters
   */
  
  // initial noise
  BMat mat_ini_noise(P,1);
  for (i = 0; i < P; i++) {
    mat_ini_noise(i,0) = buffer_Y[i]-buffer_tsr_effect0[i];
  }
  // matrix to solve de noise.
  BMat mat_noise(len_residuals,1); 
  BDat * buffer_mat_noise = mat_noise.GetData().GetBuffer();
  // initial residuals
  BMat mat_ini_res(Q,1);
  mat_ini_res.SetAllValuesTo(0);
  
  // matrix to put the resampled residuals.
  BMat mat_residuals(len_residuals,1);
  BDat * buffer_mat_residuals = mat_residuals.GetData().GetBuffer();
  size_t j, i_rand;
  
  double y;
  BSyntaxObject * so_estim;  // estimation result.
  BSyntaxObject * so_param;  // parameters estimated.
  
  for (int iB = 1; iB < B+1; iB++) {
    
    // resample residuals.
    if (has_ARIMA) {
      for (i = 0; i < len_residuals; i++) {
        i_rand = gsl_rng_uniform_int(getGslRng(), len_residuals);
        buffer_mat_residuals[i] = buffer_tsr_residuals0[i_rand];
      }
      // rebuild noise.
      MatBackwardDifEq(*ptr_rat_ARIMA, mat_ini_res, mat_residuals,
                       mat_ini_noise, mat_noise);
    } else {
      // noise is equal to residuals
      for (i = 0; i < len_residuals; i++) {
        i_rand = gsl_rng_uniform_int(getGslRng(), len_residuals);
        buffer_mat_noise[i] = buffer_tsr_residuals0[i_rand];
      }
    }
    // rebuild output.
    for (i = 0; i < len_residuals; i++) {
      if (buffer_Y[i+P].IsKnown()) {
        y = buffer_tsr_effect0[i+P] + buffer_mat_noise[i].Value();
        //buffer_tsr_output[i+P] = BDat(y);
        buffer_Y[i+P] = BDat(y);
      }
    }

    // build the model.

    BTsrPrimary * tsr_output = new BTsrPrimary("","",
                                               tms,
                                               first_out, last_out, buffer_Y);
    tsr_output->IncNRefs();
    BSyntaxObject * so_model = cd_newmodel->Evaluator(Cons(tsr_output,
                                                           Cons(model,NIL)));
    BOOTS_CHECK_SO_(so_model, "failed evaluation of NewModel");
    so_model->IncNRefs();
    // estimate.
    so_estim = cd_estim->Evaluator(NCons(so_model));
    // sould I have to make Do()?
    so_estim->IncNRefs();
    // collect parameters.
    so_param = cd_param->Evaluator(NCons(so_estim));
    // sould I have to make Do()?
    ptr_set_tmp = &Set(so_param);
    for (j = 0; j < ptr_set_tmp->Card(); j++)
      boots(iB,j) = Dat((*ptr_set_tmp)[j+1]);
    assert(so_param->NRefs()==0);
    DESTROY(so_param);
    //assert(so_estim->NRefs()==1);
    so_estim->DecNRefs();
    DESTROY(so_estim);
    //assert(so_model->NRefs()==1);
    so_model->DecNRefs();
    DESTROY(so_model);
    //assert(tsr_output->NRefs()==1);
    tsr_output->DecNRefs();
    DESTROY(tsr_output);
  }
  BOOTS_KILL_RESULTS;
}

static
int GibbsCMNCheckArgs(const char * name,
                      const BSet & Args,
                      size_t & size, size_t & burning,
                      gsl_rcmnorm_workspace_t * work,
                      int has_B)
{
  BMat * matMean = &Mat(Args[1]);
  BSyntaxObject *soCOV = Args[2];
  BMat *matCOV = NULL;
  BMat *matB = &Mat(Args[3]);
  BMat *matb = &Mat(Args[4]);
  BMat *matX0 = NULL;

  gsl_vector *gsl_mean = NULL;
  cov_t cov_type;
  double s2=1.0;
  gsl_matrix *gsl_COV = NULL;
  gsl_vector *gsl_W = NULL;
  gsl_matrix *gsl_B = NULL;
  gsl_vector *gsl_b = NULL;
  gsl_vector *gsl_x0 = NULL;

#define _CMNFREE \
  if (gsl_COV)  gsl_matrix_free(gsl_COV);                       \
  if (gsl_W)    gsl_vector_free(gsl_W);                         \
  if (gsl_B)    gsl_matrix_free(gsl_B);                         \
  if (gsl_b)    gsl_vector_free(gsl_b);                         \
  if (gsl_x0)   gsl_vector_free(gsl_x0);
  
#define _CMNFAIL \
  do {                                                          \
    _CMNFREE;                                                   \
    return 0;                                                   \
  } while (0);
  
#define _CMNOK \
  do {                                                          \
    _CMNFREE;                                                   \
    return 1;                                                   \
  } while (0);
  
  if (matMean->Columns() != 1 || matMean->Rows() == 0) {
    Error(I2(BText("Invalid dimension for mu argument in ")+name+
             ", should be a column vector",
             BText("Dimension invalida del argumento mu en ")+name+
             ", debe ser un vector columna"));
    _CMNFAIL;
  }
  bmat_to_gsl(*matMean, gsl_mean);
  if (soCOV->Grammar()==GraText()) {
    BText *txtArg = &Text(Args[2]);
    if (!strcasecmp(txtArg->String(),"IDENT")) {
      cov_type = cov_scalar;
    } else {
      Error(I2(BText("In ")+name+
               ", expected \"IDENT\" in covariance argument given as Text",
               BText("En ")+name+
               ", se esperaba la cadena \"IDENT\" cuando se da la covarianza como tipo Text"));
      _CMNFAIL;
    }
  } else if (soCOV->Grammar()==GraSet()) {
    BSet *setCOV = &Set(Args[2]);
    if (setCOV->Card()<2) {
        Error(I2(BText("In ")+name+
                 ", covariance matrix defined as a Set should have at leat 2 elements",
                 BText("En ")+name+
                 ", la matrix de covarianza definida como un Set debe tener al menos 2 elementos"));
        _CMNFAIL;
    }
    if ((*setCOV)[1]->Grammar()!=GraText()) {
      Error(I2(BText("In ")+name+
               ", first element of covariance matrix defined as a Set must be a Text",
               BText("En ")+name+
               ", el primer elemento de una matrix de covarianza definida como un Set debe ser  de tipo Text"));
      _CMNFAIL;
    }
      
    BText *txtCOVType = &Text((*setCOV)[1]);
    if ((*setCOV)[2]->Grammar()!=GraMatrix()) {
      Error(I2(BText("In ")+name+
               ", second element in covariance defined as a Set must be a Matrix",
               BText("En ")+name+
               ", el segundo elemento de una covarianza definida como un Set debe ser de tipo Matrix"));
      _CMNFAIL;
    }
    matCOV = &Mat((*setCOV)[2]);
    assert(matCOV);
    
    if (setCOV->Card()==2) {
      /* {"DIAG" diag_vector}
         {"FULL_CHOL" S}
         {"FACT_CHOL" L}
         {"FACT_ICHOL" L}
         {"FULL_SVD" S}
       */
      if (!strcasecmp(txtCOVType->String(),"DIAG")) {
        /* COV given as a diagonal vector, should match mean vector */
        cov_type = cov_diag;
        if (matCOV->Rows()==matMean->Rows() && matCOV->Columns()==1) {
          bmat_to_gsl(*matCOV, gsl_W, bycol);
        } else if (matCOV->Rows()==1 && matCOV->Columns()==matMean->Rows()) {
          bmat_to_gsl(*matCOV, gsl_W, byrow);
        } else if (matCOV->Rows()==matMean->Rows() &&
                   matCOV->Columns()==matMean->Rows()) {
          bmat_to_gsl(*matCOV, gsl_W, bydiag);
        } else {
          Error(I2(BText("In ")+name+
                   "missmatch matrix dimensions between diagonal covariance and mean vector",
                   BText("En ")+name+
                   "dimensiones discordantes entre la matriz de covarianza diagonal y el vector media"));
          _CMNFAIL;
        }
      } else {
        if (matCOV->Rows()!=matMean->Rows() ||
            matCOV->Columns()!=matMean->Rows()) {
          Error(I2(BText("In ")+name+
                   "missmatch matrix dimensions between covariance matrix and mean vector",
                   BText("En ")+name+
                   "dimensiones discordantes entre la matriz de covarianza y el vector media"));
          _CMNFAIL;
        }
        bmat_to_gsl(*matCOV, gsl_COV);
        if (!strcasecmp(txtCOVType->String(),"FULL_CHOL")) {
          cov_type = full_chol;
        } else if (!strcasecmp(txtCOVType->String(),"FACT_CHOL")) {
          cov_type = fact_chol;
        }  else if (!strcasecmp(txtCOVType->String(),"FACT_ICHOL")) {
          cov_type = fact_ichol;
        } else if (!strcasecmp(txtCOVType->String(),"FULL_SVD")) {
          cov_type = full_svd;
        } else {
          Error(I2(BText("In ")+name+", invalid covariance description "+
                   *txtCOVType+" must be \"DIAG\", \"FULL_CHOL\", \"FACT_CHOL\" or \"FULL_SVD\"",
                   BText("En ")+name+", descripcion de covarianza invalida "+
                   *txtCOVType+" debe ser \"DIAG\", \"FULL_CHOL\", \"FACT_CHOL\" o \"FULL_SVD\""
                   ));
          _CMNFAIL;
        }
      }
    } else if (setCOV->Card()==3) {
      /* {"FACT_SVD" V W} {"FACT_ISVD" V W} */
      if ((*setCOV)[3]->Grammar()!=GraMatrix()) {
        Error(I2(BText("In ")+name+
                 ", third element in covariance defined as a Set must be a Matrix",
               BText("En ")+name+
               ", el tercer elemento de una covarianza definida como un Set debe ser de tipo Matrix"));
        _CMNFAIL;
      }
      bmat_to_gsl(*matCOV, gsl_COV);
      matCOV = &Mat((*setCOV)[3]);
      assert(matCOV);
      if (matCOV->Rows()==matMean->Rows() && matCOV->Columns()==1) {
        bmat_to_gsl(*matCOV, gsl_W, bycol);
      } else if (matCOV->Rows()==1 && matCOV->Columns()==matMean->Rows()) {
        bmat_to_gsl(*matCOV, gsl_W, byrow);
      } else if (matCOV->Rows()==matMean->Rows() &&
                 matCOV->Columns()==matMean->Rows()) {
        bmat_to_gsl(*matCOV, gsl_W, bydiag);
      } else {
        Error(I2(BText("In ")+name+
                 "missmatch matrix dimensions between diagonal matrix W and mean vector",
                 BText("En ")+name+
                 "dimensiones discordantes entre la matriz diagonal W y el vector media"));
        _CMNFAIL;
      }
      if (!strcasecmp(txtCOVType->String(),"FACT_SVD")) {
        cov_type = fact_svd;
      } else if (!strcasecmp(txtCOVType->String(),"FACT_ISVD")) {
        cov_type = fact_isvd;
      } else {
        Error(I2(BText("In ")+name+
                 ", expected \"FACT_SVD\" or \"FACT_ISVD\" covariance type",
                 BText("En ")+name+
                 ", se esperaba \"FACT_SVD\" o \"FACT_ISVD\" en especificacion de tipo de covarianza"));
        _CMNFAIL;
      }
    } else {
      Error(I2(BText("In ")+name+
               ", the covariance matrix must be defined by a Set of 2 o 3 elements",
               BText("En ")+name+
               ", la matrix de covarianzas debe definirse con un Set de 2 o 3 elementos"));
      _CMNFAIL;
    }
  } else if (soCOV->Grammar()==GraMatrix()) {
    cov_type = full_chol;
    matCOV = &Mat(Args[2]);
    if (matCOV->Rows()!=matMean->Rows() ||
        matCOV->Columns()!=matMean->Rows()) {
      Error(I2(BText("In ")+name+
               "missmatch matrix dimensions between covariance matrix and mean vector",
               BText("En ")+name+
               "dimensiones discordantes entre la matriz de covarianza y el vector media"));
      _CMNFAIL;
    }
    bmat_to_gsl(*matCOV, gsl_COV);
  } else {
      Error(I2(BText("In ")+name+
               ", covariance argument must be a Set or a Matrix",
               BText("En ")+name+
               ", el argumento covarianza debe ser de tipo Set o Matrix"));
    _CMNFAIL;
  }
  
  if (Args[5]) {
    BDat *d = &Dat(Args[5]);
    if (!d->IsKnown() || d->Value() <= 0) {
      Error(I2(BText("s2 argument in ")+name+
               " should positive",
               BText("El argumento s2 en ")+name+
               " debe ser positivo"));
      _CMNFAIL;
    }
    s2 = d->Value();
    if (Args[6]) {
      d = &Dat(Args[6]);
      if (!d->IsKnown() || d->Value() <= 0) {
        Error(I2(BText("size argument in ")+name+
                 " should positive",
                 BText("El argumento size en ")+name+
                 " debe ser positivo"));
        _CMNFAIL;
      }
      size = size_t(d->Value());
      if (Args[7]) {
        d = &Dat(Args[7]);
        if (!d->IsKnown() || d->Value() <= 0) {
          Error(I2(BText("burning argument in ")+name+
                   " should positive",
                   BText("El argumento burning en ")+name+
                   " debe ser positivo"));
          _CMNFAIL;
        }
        burning = size_t(d->Value());
        if (Args[8]) {
          matX0 = &Mat(Args[8]);
          if (matX0->Columns() != 1 || matX0->Rows() != matB->Columns()) {
            Error(I2(BText("Invalid initial state X0 in ")+name+
                     ", should be a column vector compatible with B",
                     BText("Estado inicial X0 en ")+name+
                     " invalido, debe ser un vector columna compatible con B"));
            _CMNFAIL;
          }
        }
      }
    }
  }
  if (!has_B) {
    /* (B,b) define a "rectangular" domain
     */
    if (matB->Columns() != 1 ||
        matB->Rows() != matMean->Rows()) {
      Error(I2(BText("Invalid 'low' corner dimension in ")+name+
               " should be a colum vector compatible with mu",
               BText("Dimension de esquina 'inferior' invalida en ")+name+
               " debe ser un vector columna compatible con mu"));
      _CMNFAIL;
    }
    if (matb->Columns() != 1 || matb->Rows() != matMean->Rows()) {
      Error(I2(BText("Invalid 'high' corner dimension in")+name+
               " should be a colum vector compatible with mu",
               BText("Dimension de esquina 'superior' invalida")+name+
               " debe ser un vector columna compatible con nu"));
      _CMNFAIL;
    }
    /* build constraints for "rectangle" */
    gsl_vector *gsl_low=NULL, *gsl_high=NULL;

    bmat_to_gsl(*matB, gsl_low);
    bmat_to_gsl(*matb, gsl_high);
    assert(gsl_B==NULL);
    assert(gsl_b==NULL);
    gsl_rectangle_alloc(gsl_low, gsl_high, &gsl_B, &gsl_b, &gsl_x0);
    gsl_vector_free(gsl_low);
    gsl_vector_free(gsl_high);
  } else {
    /* system Bx <= b is defined */
    if (matB->Rows()==0 || matB->Columns()==0) {
      Error(I2(BText("Invalid constraint matrix B in ")+name,
               BText("Matriz de restricciones B invalida en ")+name));
      _CMNFAIL;
    }
    if (matB->Columns() != matMean->Rows()) {
      Error(I2(BText("Invalid number of columns for matrix B in ")+name,
               BText("Numero invalido de columnas de la matrix B en ")+name));
      _CMNFAIL;
    }
    if (matb->Columns() != 1 || matb->Rows() != matB->Rows()) {
      Error(I2(BText("Invalid vector b in ")+name+
               ", should be a column vector compatible with B",
               BText("Vector b en ")+name+
               " invalido, debe ser un vector columna compatible con B"));
      _CMNFAIL;
    }
    assert(gsl_B==NULL);
    bmat_to_gsl(*matB, gsl_B);
    assert(gsl_b==NULL);
    bmat_to_gsl(*matb, gsl_b);
    if (matX0) {
      assert(gsl_x0==NULL);
      bmat_to_gsl(*matX0, gsl_x0);
    } else {
      /* here we should find a feasible solution to B*x<=b */
      
    }
  }
  int gsl_ret = gsl_rcmnorm_init(gsl_mean, cov_type, s2, gsl_COV, gsl_W,
                                 gsl_B, gsl_b, gsl_x0, work);
  if (gsl_ret != GSL_SUCCESS) {
    Error(I2(BText("Initializing ")+name+
             ": "+gsl_strerror(gsl_ret),
             BText("Inicializando ")+name+
             ": "+gsl_strerror(gsl_ret)));
    _CMNFAIL;
  }
  _CMNOK;
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGibbsCMN);
DefExtOpr(1, BMatGibbsCMN, "GibbsConstrainedMNormal", 4, 8,
          "Matrix Anything Matrix Matrix Real Real Real Matrix",
          "(Matrix mu, Anything A, Matrix B, Matrix b [, Real s2=1.0, Real size=100, Real burning=3, Matrix X0=0])",
    I2("Run a markov chain for a multivariate normal X ~ N_R[mu, s2*A] constrained to the region R, "
       "where R is defined by a system of linear inequalities Bx <= b.\n\n"
       "Parameters:\n\n"
       "     mu: parameter mean of the distibution\n"
       "   s2,A: s2*A define the parameter covariance of the distribution. See below description of parameter A.\n"
       "    B,b: B*X<=b define the region R of truncation.\n"
       "   size: the size of markov chain generated.\n"
       "burning: number of samples discarded before taking the samples.\n"
       "     X0: initial point of the chain. Must belong to R, ie. B*X0<=b.\n\n"
       "Covariance matrix description:\n\n"
       "The covariance matrix SIGMA=s2*A can be given in multiple way as a Matrix or a Set\n\n"
       "            Matrix A: cholesky is used internally to factorize the matrix\n"
       "  [[\"FULL_CHOL\", A]]: cholesky is used internally to factorize A, it is similar to only provide Matrix A\n"
       "  [[\"FACT_CHOL\", L]]: L is the cholesky factor of A, it is assumed that A = L*L'\n"
       " [[\"FACT_ICHOL\", L]]: L is the cholesky factor of A, it is assumed that A^-1 = L*L'\n"
       "   [[\"FULL_SVD\", A]]: SVD is used internally to factorize A\n"
       "[[\"FACT_SVD\", V, W]]: V,W are the SVD factors of A, it is assumed that A = V*W^2*V'\n"
       "[[\"FACT_ISVD\", V W]]: V,W are the SVD factors of A, it is assumed that A^-1 = V*W^2*V\n"
       "  [[Text \"DIAG\", D]]: it is assumed that A=D is a diagonal matrix.\n\n"
       "Diagonal matrices such as D or W can be given as a column vector, row vector or square matrix. If given as a square matrix only the diagonal elements are accessed.\n\n"
       "If dimension of mu is 1 then argument A is ignored and a truncated unidimensional normal with parameters mean mu(1,1) and variance s2 is considered.\n\n"
       "Example:\n\n"
       "    Matrix mu = Col(2.0, 2.0);\n"
       "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
       "    Matrix B = ((-1.0, 1.0), (0.0, -1.0));\n"
       "    Matrix b = Col(0.0, 0.0);\n"
       "    Matrix MCsample = GibbsConstrainedMNormal(mu, COV, B, b, 1, 100);\n\n"
       "References:\n\n"
       "The implementation is based on \"Eficient Gibbs Sampling of Truncated Multivariate Normal with Application to Constrained Linear Regression\" : http://www.stat.colostate.edu/~rdavis/papers/CLR.pdf",
       
       "Genera una cadena de markov a partir de una multinormal X ~ N_R[mu, s2*A] restringida a la region R, donde R viene dada por un sistema de desigualdades lineales Bx <= b.\n\n"
       "Parametros:\n\n"
       "     mu: parametro media de la distribucion N_R.\n"
       "   s2,A: s2*A es parametro covarianza de la distribucion N_R. Ver debajo la descripcion del parametro A.\n"
       "    B,b: B*X<=b define la region R asociada a las restricciones lineales.\n"
       "   size: tamaño de la cadena de markov solicitado.\n"
       "burning: numero de muestras descartadas.\n"
       "     X0: punto inicial de la cadena. Debe pertencer a R, es decir B*X0<=b.\n\n"
       "Descripcion de la matriz de covarianza:\n\n"
       "La matriz de covarianza SIGMA=s2*A puede darse en forma de Matriz (Matrix) o conjunto (Set)\n\n"
       "            Matrix A: se utiliza cholesky como metodo de factorizacion de A\n"
       "  [[\"FULL_CHOL\", A]]: se utiliza cholesky como metodo de factorizacion de A, es equivalente a pasar la matriz A como argumento.\n"
       "  [[\"FACT_CHOL\", L]]: L es el factor cholesky de A, se asume que A = L*L'\n"
       " [[\"FACT_ICHOL\", L]]: L es el factor cholesky de A^-1, se asume que  A^-1 = L*L'\n"
       "   [[\"FULL_SVD\", A]]: se utiliza SVD como metodo de factorizacion de A A\n"
       "[[\"FACT_SVD\", V, W]]: factorizacion SVD de A, se asume que A = V*W^2*V'\n"
       "[[\"FACT_ISVD\", V W]]: factorizacion SVD de A^-1, se asume que A^-1 = V*W^2*V\n"
       "  [[Text \"DIAG\", D]]: A=D es una matriz diagonal.\n\n"
       "Las matrices diagonales D o W se pueden especificar como vectores filas o columnas o como matriz cuadrada de la cual solo la diagonal es considerada.\n\n"
       "Si la dimension del vector mu es 1, entonces el argumento A no tiene significado y se simula una normal unvariante truncada con parametro media mu(1,1) y varianza s2.\n\n"       
       "Ejemplo:\n\n"
       "    Matrix mu = Col(2.0, 2.0);\n"
       "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
       "    Matrix B = ((-1.0, 1.0), (0.0, -1.0));\n"
       "    Matrix b = Col(0.0, 0.0);\n"
       "    Matrix MCsample = GibbsConstrainedMNormal(mu, COV, B, b, 1, 100);\n\n"
       "Referencias:\n\n"
       "La implementacion esta basada en \"Eficient Gibbs Sampling of Truncated Multivariate Normal with Application to Constrained Linear Regression\" : http://www.stat.colostate.edu/~rdavis/papers/CLR.pdf"
),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatGibbsCMN::CalcContens()
//--------------------------------------------------------------------
{
  size_t size = 100;
  size_t burning = 3;
  gsl_rcmnorm_workspace_t work;

  /* check first if we are dealing with an unidimensional distribution */
  BMat &muMat = Mat(Arg(1));
  if (muMat.Rows()==1) {
    if (!muMat(0,0).IsFinite()) {
      Error(I2("In GibbsConstrainedMNormal: mean parameter mu should be finite",
               "En GibbsConstrainedMNormal: el parametro media mu debe ser finito"));
      contens_ = BMat::Unknown();
      return;      
    }
    double mu = muMat(0,0).Value();
    
    /* unidimensional case */
    int low_bounded = 0;
    int high_bounded = 0;
    double low = -1.E+6, high = +1.E+6;
    double s2, s;
    int i, size = 100;
    
    /* look for the interval */
    BMat &B = Mat(Arg(3));
    BMat &b = Mat(Arg(4));
    if (B.Columns()!=1) {
      Error(I2("In GibbsConstrainedMNormal: invalid dimensions for argument B, should be a one column matrix.",
               "En GibbsConstrainedMNormal: dimensiones invalidas en argumento B, debe ser una matriz de una columna."));
      contens_ = BMat::Unknown();
      return;
    }
    if (b.Columns()!=1 || b.Rows()!=B.Rows()) {
      Error(I2("In GibbsConstrainedMNormal: invalid dimensions for argument b, should have the dimensions as B,",
               "En GibbsConstrainedMNormal: dimensiones invalidas en argumento b, debe tener las mismas dimensiones que B."));
      contens_ = BMat::Unknown();
      return;
    }
    if (Arg(5)) {
      BDat &dat_s2 = Dat(Arg(5));
      if (!dat_s2.IsFinite() || ((s2=dat_s2.Value())<=0)) {
        Error(I2("In GibbsConstrainedMNormal: argument s2 should be positive.",
                 "En GibbsConstrainedMNormal: el argumento s2 debe ser positivo."));
        contens_ = BMat::Unknown();
        return;
      }
      s = sqrt(s2);
      if (Arg(6)) {
        BDat &dat_size = Dat(Arg(6));
        if (!dat_size.IsFinite() || (size=size_t(floor(dat_size.Value())))<=0) {
          Error(I2("In GibbsConstrainedMNormal: argument size should be positive.",
                   "En GibbsConstrainedMNormal: el argumento size debe ser positivo."));
          contens_ = BMat::Unknown();
          return;
        }
      } else {
        size = 100;
      }
    } else {
      s2 = 1.0;
      s = 1;
    }
    for (i = 0; i < B.Rows(); i++) {
      BDat &vDat = B.Data()[i];
      if (vDat.IsFinite()) {
        double v = vDat.Value();
        int v_sgn = gsl_fcmp(v+1.0,1.0,DBL_EPSILON);
        if (v_sgn < 0) {
          double d = b.Data()[i].Value()/v;
          if (low_bounded) {
            if (d>low) {
              low = d;
            }
          } else {
            low_bounded = 1;
            low = d;
          }
        } else if (v_sgn > 0) {
          double d = b.Data()[i].Value()/v;
          if (high_bounded) {
            if (d<high) {
              low = d;
            }
          } else {
            high_bounded = 1;
            high = d;
          }
        }
      }
    }
    gsl_rng *r = getGslRng();
    const int iter = 10;
    if (low_bounded) {
      if (high_bounded) {
        if (low>=high) {
          Error(I2("In GibbsConstrainedMNormal: B*x<=b is empty",
                   "En GibbsConstrainedMNormal: B*x<=b es vacio"));
          contens_ = BMat::Unknown();
          return;
        } else {
          contens_.Alloc(size, 1);
          for (i=0; i<size; i++) {
            contens_(i,0) = gsl_rtnorm_combo(r, mu, s, low, high);
          }
        }
      } else {
        contens_.Alloc(size, 1);
        for (i=0; i<size; i++) {
          contens_(i,0) = gsl_rtbnorm_combo(r, mu, s, low, iter);
        }
      }
    } else {
      if (high_bounded) {
        contens_.Alloc(size, 1);
        for (i=0; i<size; i++) {
          contens_(i,0) = gsl_rtanorm_combo(r, mu, s, high, iter);
        }
      } else {
        contens_.Alloc(size, 1);
        for (i=0; i<size; i++) {
          contens_(i,0) = mu + gsl_ran_gaussian(r, s);
        }
      }
    }
  } else {
    /* multidimensional case */
    BSet args; args.PutElement(ArgList());
    if (GibbsCMNCheckArgs("GibbsConstrainedMNormal", args,
                          size, burning, &work, 1)) {
      size_t i, j;
      size_t dim = work.alpha->size;
      gsl_vector *sample = gsl_vector_alloc(dim);
      contens_.Alloc(size, dim);
      for (i = 0; i < burning; i++) {
        gsl_rcmnorm_draw(getGslRng(), &work, NULL);
      }
      for (i = 0; i < size; i++) {
        gsl_rcmnorm_draw(getGslRng(), &work, sample);
        for (j = 0; j < dim; j++)
          contens_(i,j) = gsl_vector_get(sample, j);
      }
      gsl_vector_free(sample);
      gsl_rcmnorm_free(&work);
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRandCMN);
DefExtOpr(1, BMatRandCMN, "RandConstrainedMNormal", 4, 8,
          "Matrix Anything Matrix Matrix Real Real Real Matrix",
          "(Matrix mu, Anything COV, Matrix B, Matrix b [, Real s2=1.0, Real burning, Real size=3, Matrix X0=0])",
    I2("Draw a sample from a truncated multivariate normal. Truncation "
             "is done as linear system of inequalities Bx <= b. Parameter 'size' indicate the size of markov chain to be run in order to obtain the sample.\n"
       "Covarianze matrix COV can be given in multiple way as a Matrix or a Set:\n"
       "  Matrix COV : cholesky is used internally to factorize the matrix\n"
       "  [[Text \"FULL_CHOL\" Matrix COV]] : cholesky is used internally to factorize the matrix, it is similar to only provide Matrix COV\n"
       "  [[Text \"FACT_CHOL\" Matrix L]] : it is assumed that COV = L*L'\n"
       "  [[Text \"FACT_ICHOL\" Matrix L]] : it is assumed that COV^-1 = L*L'\n"
       "  [[Text \"FULL_SVD\" Matrix COV]] : SVD is used internally to factorize the matrix\n"
       "  [[Text \"FACT_SVD\" Matrix L]] : it is assumed that COV = V*W^2*V'\n"
       "  [[Text \"FACT_ISVD\" Matrix L]] : it is assumed that COV^-1 = V*W^2*V\n"
       "  [[Text \"DIAG\", D]] : COV is assumed a diagonal matrix being D a column vector with the diagonal elements of COV.\n\n"
       "Diagonal matrices such as D or W can be given as a column vector, row vector or square matrix. If given as a square matrix only the diagonal elements are accessed.\n\n"
             "Example:\n\n"
             "    Matrix mu = Col(2.0, 2.0);\n"
             "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
             "    Matrix B = ((-1.0, 1.0), (0.0, -1.0));\n"
             "    Matrix b = Col(0.0, 0.0);\n"
             "    Matrix sample = RandConstrainedMNormal(mu, COV, B, b, 1, 100);",
       "Realiza una muestra a partir de una multinormal truncada donde la zona de muestreo viene dada por un sistema de desigualdades lineales Bx <= b. El parametro 'size' indica el tamaño de la cadena de markov a generar para obtener la muestra.\n"
       "La matriz de covarianza COV puede especificarse en diversas formas:\n"
       "  Matrix COV : iternamente se usa cholesky para descomponer la matrix\n"
       "  [[Text \"FULL_CHOL\" Matrix COV]] : iternamente se usa cholesky para descomponer la matrix, es similar a dar solo Matrix COV\n"
       "  [[Text \"FACT_CHOL\" Matrix L]] : se asume que COV = L*L'\n"
       "  [[Text \"FACT_ICHOL\" Matrix L]] :  se asume que COV^-1 = L*L'\n"
       "  [[Text \"FULL_SVD\" Matrix COV]] : se usa SVD internamente para descomponer COV\n"
       "  [[Text \"FACT_SVD\" Matrix L]] :  se asume que COV = V*W^2*V'\n"
       "  [[Text \"FACT_ISVD\" Matrix L]] :  se asume que COV^-1 = V*W^2*V\n"
       "  [[Text \"DIAG\", D]] : se asume que COV es una matriz diagonal siendo D un vector columna con los elementos de la diagonal de COV.\n\n"
       "Las matrices diagonales D o W se pueden especificar como vectores filas o columnas o como matriz cuadrada de la cual solo la diagonal es considerada.\n\n"
            "Ejemplo:\n\n"
              "    Matrix mu = Col(2.0, 2.0);\n"
             "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
             "    Matrix B = ((-1.0, 1.0), (0.0, -1.0));\n"
             "    Matrix b = Col(0.0, 0.0);\n"
             "    Matrix sample = RandConstrainedMNormal(mu, COV, B, b, 1, 100);"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRandCMN::CalcContens()
//--------------------------------------------------------------------
{
  size_t size = 100;
  size_t burning = 3;
  gsl_rcmnorm_workspace_t work;
  
  BSet args; args.PutElement(ArgList());
  if (GibbsCMNCheckArgs("RandConstrainedMNormal", args,
                        size, burning, &work, 1)) {
    size_t i, j;
    
    for (i = 0; i < burning+size; i++) {
      gsl_rcmnorm_draw(getGslRng(), &work, NULL);
    }
    /* use the last sample */
    size_t dim = work.alpha->size;
    gsl_vector * sample = gsl_vector_alloc(dim);
    gsl_rcmnorm_last(&work, sample);
    contens_.Alloc(1, dim);
    for (j = 0; j < dim; j++)
      contens_(0,j) = gsl_vector_get(sample, j);
    gsl_vector_free(sample);
    gsl_rcmnorm_free(&work);
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatGibbsRMN);
DefExtOpr(1, BMatGibbsRMN, "GibbsRectangleMNormal", 4, 6,
          "Matrix Anything Matrix Matrix Real Real",
          "(Matrix mu, Anything COV, Matrix corner1, Matrix corner2 [, Real s2=1, Real size=1])",
    I2("Run a markov chain for a truncated multivariate normal. Truncation "
             "is done in the multidimensional rectangle defined by the corners (corner1,corner2). Parameter 'size' indicate the size of chain.\n"
       "Covarianze matrix COV can be given in multiple way as a Matrix or a Set:\n"
       "  Matrix COV : cholesky is used internally to factorize the matrix\n"
       "  [[Text \"FULL_CHOL\" Matrix COV]] : cholesky is used internally to factorize the matrix, it is similar to only provide Matrix COV\n"
       "  [[Text \"FACT_CHOL\" Matrix L]] : it is assumed that COV = L*L'\n"
       "  [[Text \"FACT_ICHOL\" Matrix L]] : it is assumed that COV^-1 = L*L'\n"
       "  [[Text \"FULL_SVD\" Matrix COV]] : SVD is used internally to factorize the matrix\n"
       "  [[Text \"FACT_SVD\" Matrix L]] : it is assumed that COV = V*W^2*V'\n"
       "  [[Text \"FACT_ISVD\" Matrix L]] : it is assumed that COV^-1 = V*W^2*V\n"
       "  [[Text \"DIAG\", D]] : COV is assumed a diagonal matrix being D a column vector with the diagonal elements of COV.\n\n"
       "Diagonal matrices such as D or W can be given as a column vector, row vector or square matrix. If given as a square matrix only the diagonal elements are accessed.\n\n"
             "Example:\n\n"
             "    Matrix mu = Col(0.0, 2.0);\n"
             "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
             "    Matrix c1 = Col(-1.0, 0.5);\n"
             "    Matrix c2 = Col(1.0, 1.5);\n"
             "    Matrix MCsample = GibbsRectangleMNormal(mu, COV, c1, c2, 1, 100);",
       "Genera una cadena de markov para una multinormal truncada donde la zona de muestreo es el rectangulo multidimensional definido por las esquinas c1,c2. El parametro 'size' indica el tamaño de la cadena.\n"
       "La matriz de covarianza COV puede especificarse en diversas formas:\n"
       "  Matrix COV : iternamente se usa cholesky para descomponer la matrix\n"
       "  [[Text \"FULL_CHOL\" Matrix COV]] : iternamente se usa cholesky para descomponer la matrix, es similar a dar solo Matrix COV\n"
       "  [[Text \"FACT_CHOL\" Matrix L]] : se asume que COV = L*L'"
       "  [[Text \"FACT_ICHOL\" Matrix L]] :  se asume que COV^-1 = L*L'"
       "  [[Text \"FULL_SVD\" Matrix COV]] : se usa SVD internamente para descomponer COV"
       "  [[Text \"FACT_SVD\" Matrix L]] :  se asume que COV = V*W^2*V'"
       "  [[Text \"FACT_ISVD\" Matrix L]] :  se asume que COV^-1 = V*W^2*V"
       "  [[Text \"DIAG\", D]] : se asume que COV es una matriz diagonal siendo D un vector columna con los elementos de la diagonal de COV.\n\n"
       "Las matrices diagonales D o W se pueden especificar como vectores filas o columnas o como matriz cuadrada de la cual solo la diagonal es considerada.\n\n"
            "Ejemplo:\n\n"
             "    Matrix mu = Col(0.0, 2.0);\n"
             "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
             "    Matrix c1 = Col(-1.0, 0.5);\n"
             "    Matrix c2 = Col(1.0, 1.5);\n"
             "    Matrix MCsample = GibbsRectangleMNormal(mu, COV, c1, c2, 1, 100);"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatGibbsRMN::CalcContens()
//--------------------------------------------------------------------
{
  size_t size = 1;
  size_t burning = 3;
  gsl_rcmnorm_workspace_t work;
  
  BSet args; args.PutElement(ArgList());
  if (GibbsCMNCheckArgs("GibbsRectangleMNormal", args,
                        size, burning, &work, 0)) {
    size_t i, j;
   
    size_t dim = work.alpha->size;
    gsl_vector * sample = gsl_vector_alloc(dim);
    contens_.Alloc(size, dim);
    for (i = 0; i < size; i++) {
      gsl_rcmnorm_draw(getGslRng(), &work, sample);
      for (j = 0; j < dim; j++)
        contens_(i,j) = gsl_vector_get(sample, j);
    }
    gsl_vector_free(sample);
    gsl_rcmnorm_free(&work);
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRandRMN);
DefExtOpr(1, BMatRandRMN, "RandRectangleMNormal", 4, 6,
          "Matrix Anything Matrix Matrix Real Real",
          "(Matrix mu, Anything COV, Matrix corner1, Matrix corner2 [, Real s2=1, Real size=1])",
          I2("Draw a sample from a truncated multivariate normal. Truncation "
             "is done in the multidimensional rectangle defined by the corners (corner1,corner2). Parameter 'size' indicate the size of chain.\n"
       "Covarianze matrix COV can be given in multiple way as a Matrix or a Set:\n"
       "  Matrix COV : cholesky is used internally to factorize the matrix\n"
       "  [[Text \"FULL_CHOL\" Matrix COV]] : cholesky is used internally to factorize the matrix, it is similar to only provide Matrix COV\n"
       "  [[Text \"FACT_CHOL\" Matrix L]] : it is assumed that COV = L*L'\n"
       "  [[Text \"FACT_ICHOL\" Matrix L]] : it is assumed that COV^-1 = L*L'\n"
       "  [[Text \"FULL_SVD\" Matrix COV]] : SVD is used internally to factorize the matrix\n"
       "  [[Text \"FACT_SVD\" Matrix L]] : it is assumed that COV = V*W^2*V'\n"
       "  [[Text \"FACT_ISVD\" Matrix L]] : it is assumed that COV^-1 = V*W^2*V\n"
       "  [[Text \"DIAG\", D]] : COV is assumed a diagonal matrix being D a column vector with the diagonal elements of COV.\n\n"
       "Diagonal matrices such as D or W can be given as a column vector, row vector or square matrix. If given as a square matrix only the diagonal elements are accessed.\n\n"
             "Example:\n\n"
             "    Matrix mu = Col(0.0, 2.0);\n"
             "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
             "    Matrix c1 = Col(-1.0, 0.5);\n"
             "    Matrix c2 = Col(1.0, 1.5);\n"
             "    Matrix MCsample = RandRectangleMNormal(mu, COV, c1, c2, 1, 100);",
       "Realiza una muestra a partir de una multinormal truncada donde la zona de muestreo es el rectangulo multidimensional definido por las esquinas c1,c2. El parametro 'size' indica el tamaño de la cadena.\n"
       "La matriz de covarianza COV puede especificarse en diversas formas:\n"
       "  Matrix COV : iternamente se usa cholesky para descomponer la matrix\n"
       "  [[Text \"FULL_CHOL\" Matrix COV]] : iternamente se usa cholesky para descomponer la matrix, es similar a dar solo Matrix COV\n"
       "  [[Text \"FACT_CHOL\" Matrix L]] : se asume que COV = L*L'"
       "  [[Text \"FACT_ICHOL\" Matrix L]] :  se asume que COV^-1 = L*L'"
       "  [[Text \"FULL_SVD\" Matrix COV]] : se usa SVD internamente para descomponer COV"
       "  [[Text \"FACT_SVD\" Matrix L]] :  se asume que COV = V*W^2*V'"
       "  [[Text \"FACT_ISVD\" Matrix L]] :  se asume que COV^-1 = V*W^2*V"
       "  [[Text \"DIAG\", D]] : se asume que COV es una matriz diagonal siendo D un vector columna con los elementos de la diagonal de COV.\n\n"
       "Las matrices diagonales D o W se pueden especificar como vectores filas o columnas o como matriz cuadrada de la cual solo la diagonal es considerada.\n\n"
             "Ejemplo:\n\n"
             "    Matrix mu = Col(0.0, 2.0);\n"
             "    Matrix COV = ((1.0, 4/5.0),(4/5.0, 1.0));\n"
             "    Matrix c1 = Col(-1.0, 0.5);\n"
             "    Matrix c2 = Col(1.0, 1.5);\n"
             "    Matrix MCsample = RandRectangleMNormal(mu, COV, c1, c2, 1, 100);"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRandRMN::CalcContens()
//--------------------------------------------------------------------
{
  size_t size = 1;
  size_t burning = 3;
  gsl_rcmnorm_workspace_t work;
  
  BSet args; args.PutElement(ArgList());
  if (GibbsCMNCheckArgs("GibbsRectangleMNormal", args,
                        size, burning, &work, 0)) {
    size_t i, j;
   
    for (i = 0; i < size; i++) {
      gsl_rcmnorm_draw(getGslRng(), &work, NULL);
    }
    size_t dim = work.alpha->size;
    gsl_vector * sample = gsl_vector_alloc(dim);
    gsl_rcmnorm_last(&work, sample);
    contens_.Alloc(1, dim);
    for (j = 0; j < dim; j++)
      contens_(0,j) = gsl_vector_get(sample, j);

    gsl_vector_free(sample);
    gsl_rcmnorm_free(&work);
  }

}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatRandTruncatedMultNormal);
DefExtOpr(1, BMatRandTruncatedMultNormal, "RandTruncatedMultNormal", 4, 7,
          "Matrix Matrix Matrix Matrix Real Real Real",
          "(Matrix mu, Matrix L, Matrix corner1, Matrix corner2, [Real s2=1, Real size=1, Real test=true])",
          I2("Draw a sample from a truncated multivariate normal. Truncation "
             "is done in the multidimensional rectangle defined by the corners (corner1,corner2).\n"
             "Due to efficience reasons covariance matrix is given by triangular decomposition cov=L*Tra(L)\n"
             "If L is the 1x1 matrix with element 1 then covariance is asumed as identity matrix.\n"
             "Parameter 'size' indicate the number of random instances generated, that is the returned columns.\n"
             "If test==true function checks that all return values match bounds.\n"
             "Example:\n\n"
             "    Matrix mu  = Col(0, 2);\n"
             "    Matrix Cov = ((1, 4/5),(4/5, 1));\n"
             "    Matrix L   = Choleski(Cov);\n"
             "    Matrix c1  = Col(-1, 0.5);\n"
             "    Matrix c2  = Col( 1, 1.5);\n"
             "    Matrix MCsample = RandTruncatedMultNormal(mu, L, c1, c2, 1, 100);\n\n"
             ,
             "Realiza una muestra a partir de una multinormal truncada donde la zona de muestreo "
             "es el rectangulo multidimensional definido por las esquinas c1,c2.\n"
             "Por razones de eficiencia la matriz de covarianzas viene dada por la descomposicion triangular cov=L*Tra(L)\n"
             "El parametro 'size' indica el tamaño de la cadena.\n"
             "Si L es la matriz 1x1 con el elemento 1 entonces la matriz de covarianzas se asumira unitaria.\n"
             "Si test==Cierto la funcion chequeara que todos los valores devueltos cumplen las restricciones\n."
             "Ejemplo:\n\n"
             "    Matrix mu  = Col(0, 2);\n"
             "    Matrix Cov = ((1, 4/5),(4/5, 1));\n"
             "    Matrix L   = Choleski(Cov);\n"
             "    Matrix c1  = Col(-1, 0.5);\n"
             "    Matrix c2  = Col( 1, 1.5);\n"
             "    Matrix MCsample = RandTruncatedMultNormal(mu, L, c1, c2, 1, 100);\n\n"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatRandTruncatedMultNormal::CalcContens()
//--------------------------------------------------------------------
{
  BMat tn;
  BMat& nu   = Mat(Arg(1)); 
  BMat& S    = Mat(Arg(2)); 
  BMat& A    = Mat(Arg(3)); 
  BMat& B    = Mat(Arg(4)); 
  BDat  s2   = (Arg(5))?      Dat (Arg(5)):1.0;
  int   size = (Arg(6))?(int )Real(Arg(6)):1;
  bool  test = (Arg(7))?Real(Arg(7))!=0.0:true; 
  bool  ok;
  if((S.Rows()==1)&&(S.Columns()==1)&&(S(0,0)==1))
  {
    ok = RandTruncatedMultNormal(tn,A,B,nu,s2,size);
  }
  else
  {
  //ok = RandTruncatedMultNormal(contens_, A,B,nu,S,s2,size);
    BLowTrMatrix<BDat> L; L= S;
    ok = RandTruncatedMultNormal(tn, A,B,nu,L,s2,size);
  }
  if(ok)
  {
    contens_ = tn;
  }
//Std(BText("\nTRACE BMatRandTruncatedMultNormal LCov=\n")+LCov->Name());
  if(ok && test)
  {
    int i,k;
    BText msg; 
    for(k=0; k<size; k++)
    {
      for(i=0; i<nu.Rows(); i++)
      {
        if((A(i,0)>contens_(i,k))||(B(i,0)<contens_(i,k)))
        {
          msg += BText("\n (")+(i+1)+","+k+") : "+contens_(i,k)+" !<: ["+A.Get(i,0)+","+B.Get(i,0)+"]";
          ok=false;
        }
      } 
    }
    if(!ok)
    {
       Error(I2("TruncatedMultiNormal: Bounds were unreachable at restrictions",
                "TruncatedMultiNormal: No se pudieron cumplir las restriciones")+
             msg);
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatLogDensTruncatedMultNormal);
DefExtOpr(1, BDatLogDensTruncatedMultNormal, "LogDensTruncatedMultNormal", 6, 7,
          "Matrix Matrix Matrix Matrix Matrix Matrix Real",
          "(Matrix x, Matrix mu, Matrix L, Matrix Li, Matrix corner1, Matrix corner2, [Real s2=1])",
          I2("Returns log of density of a vector with a truncated multivariate normal. Truncation "
             "is done in the multidimensional rectangle defined by the corners (corner1,corner2).\n"
             "Due to efficience reasons covariance matrix is given by triangular decomposition cov=L*Tra(L)\n"
             "If L is the 1x1 matrix with element 1 then covariance is asumed as identity matrix.\n"
             "Also calculates logarithm of density of each generated draw and put them in second returned element"
             "Example:\n\n"
             "    Matrix nu  = Col(0, 2);\n"
             "    Matrix Cov = ((1, 4/5),(4/5, 1));\n"
             "    Matrix L   = Choleski(Cov);\n"
             "    Matrix Li  = CholeskiInverse(L);\n"
             "    Matrix c1  = Col(-1, 0.5);\n"
             "    Matrix c2  = Col( 1, 1.5);\n"
             "    Matrix x   = RandTruncatedMultNormal(mu, L, c1, c2, 1, 1);\n\n"
             "    Real   ld  = LogDensTruncatedMultNormal(x, mu, L, Li, c1, c2, 1);\n\n"
             ,
             "Realiza una muestra a partir de una multinormal truncada donde la zona de muestreo "
             "es el rectangulo multidimensional definido por las esquinas c1,c2.\n"
             "Por razones de eficiencia la matriz de covarianzas viene dada por la descomposicion triangular cov=L*Tra(L)\n"
             "Si L es la matriz 1x1 con el elemento 1 entonces la matriz de covarianzas se asumira unitaria.\n"
             "Tambien calcula el logaritmode la densidad de cada vector generado poniendo el resultado en el segundo elemento devuelto.\n"
             "Ejemplo:\n\n"
             "    Matrix nu  = Col(0, 2);\n"
             "    Matrix Cov = ((1, 4/5),(4/5, 1));\n"
             "    Matrix L   = Choleski(Cov);\n"
             "    Matrix Li  = CholeskiInverse(L);\n"
             "    Matrix c1  = Col(-1, 0.5);\n"
             "    Matrix c2  = Col( 1, 1.5);\n"
             "    Matrix x   = RandTruncatedMultNormal(mu, L, c1, c2, 1, 1);\n\n"
             "    Real   ld  = LogDensTruncatedMultNormal(x, mu, L, Li, c1, c2, 1);\n\n"
             ),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BDatLogDensTruncatedMultNormal::CalcContens()
//--------------------------------------------------------------------
{
  BMat& x    = Mat(Arg(1)); 
  BMat& nu   = Mat(Arg(2)); 
  BMat& S    = Mat(Arg(3)); 
  BMat& Si   = Mat(Arg(4)); 
  BMat& A    = Mat(Arg(5)); 
  BMat& B    = Mat(Arg(6)); 
  BDat  s2   = (Arg(7))?Dat (Arg(7)):1.0;
  if((S.Rows()==1)&&(S.Columns()==1)&&(S(0,0)==1))
  {
    contens_ = LogDensTruncatedMultNormal(x,A,B,nu,s2);
  }
  else
  {
  //ok = RandTruncatedMultNormal(contens_, A,B,nu,S,s2,size);
    BLowTrMatrix<BDat> L;  L = S;
    BUpTrMatrix <BDat> Li; Li= Si;
    contens_ = LogDensTruncatedMultNormal(x,A,B,nu,L,Li,s2);
  }
//Std(BText("\nTRACE BMatRandTruncatedMultNormal LCov=\n")+LCov->Name());

}


///////////////////////////////////////////////////////////////

  DeclareContensClass(BMat, BMatTemporary, BMatKroneckerProduct);
  DefExtOpr(1, BMatKroneckerProduct, "KroneckerProduct", 2, 2, 
  "Matrix Matrix",
  "(Matrix A, Matrix B)",
  I2("In mathematics, the Kronecker product, denoted by x inside a circle, "
     "is an operation on two matrices of arbitrary size resulting in the "
     "block matrix (aij*B)"
     "It is a special case of a tensor product. The Kronecker "
     "product should not be confused with the usual matrix multiplication, "
     "which is an entirely different operation."
     ,
     "En matematicas, el producto de Kronecker, denotado por una x dentro "
     "de un circulo, es una operacion de dos matrices de tamaño arbitrario "
     "dando por resultado la matriz del bloque (aij*B).  "
     "Es un caso especial de un producto del tensor. El producto de "
     "Kronecker no se debe confundir con la multiplicacion usual de matrices, "
     "que es una operacion enteramente distinta."),
     BOperClassify::MatrixAlgebra_);
   void BMatKroneckerProduct::CalcContens()
///////////////////////////////////////////////////////////////
{
  BMat *A_ = &Mat(Arg(1));
  BMat *B_ = &Mat(Arg(2));

  gsl_matrix  * gsl_A = NULL;
  gsl_matrix  * gsl_B = NULL;
  gsl_matrix  * gsl_K = NULL;
  
  bmat_to_gsl(*A_, gsl_A);
  bmat_to_gsl(*B_, gsl_B);

  gsl_kronecker_prod(gsl_A, gsl_B, &gsl_K);
  gsl_to_bmat(gsl_K, contens_);

  gsl_matrix_free(gsl_A);
  gsl_matrix_free(gsl_B);
  gsl_matrix_free(gsl_K);
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
  DeclareContensClass(BMat, BMatTemporary, BMatRwish);
  DefExtOpr(1, BMatRwish, "RandWishart", 2, 2, 
  "Real Matrix",
  "(Real v, Matrix S)",
  I2("Random Wishart (mu, Scale matrix)\n" 
     "The interface is the same as explained in Gelman et al., page 574",
     "Random Wishart (mu, S).\n"
     "Es el mismo ointerfaz que en Gelman et al., p574"),
     BOperClassify::MatrixAlgebra_);
   void BMatRwish::CalcContens()
///////////////////////////////////////////////////////////////
{
  BDat *v_ = &Dat(Arg(1));
  BMat *S_ = &Mat(Arg(2));

  int  v  = (int)v_->Value();
  gsl_matrix  * gsl_S = NULL;
  gsl_matrix  * gsl_X = NULL;
  
  bmat_to_gsl(*S_, gsl_S);
  bmat_to_gsl(*S_, gsl_X);

  gsl_rwish(getGslRng(), v,gsl_S,gsl_X);

  gsl_to_bmat(gsl_X,contens_);

  gsl_matrix_free(gsl_S);
  gsl_matrix_free(gsl_X);

}
///////////////////////////////////////


///////////////////////////////////////////////////////////////
  DeclareContensClass(BMat, BMatTemporary, BMatRiwish);
  DefExtOpr(1, BMatRiwish, "RandIWishart", 2, 2, 
  "Real Matrix",
  "(Real v, Matrix S)",
  I2("Random Inverse Wishart (dof, Inverse Scale matrix)\n" 
     "If W is a InvWishart(mu,S) then W^{-1} is Wishart(mu, S^{-1}) ",
     "Random Wishart (mu, S). \n"
     "Si W es InvWishart(mu,S) entonces W^{-1} es Wishart(mu, S^{-1})"),
     BOperClassify::MatrixAlgebra_);
   void BMatRiwish::CalcContens()
///////////////////////////////////////////////////////////////
{
  BDat *v_ = &Dat(Arg(1));
  BMat *S_ = &Mat(Arg(2));

  int  v  = (int)v_->Value();
  gsl_matrix  * gsl_S = NULL;
  gsl_matrix  * gsl_X = NULL;
  
  bmat_to_gsl(*S_, gsl_S);
  bmat_to_gsl(*S_, gsl_X);

  gsl_riwish(getGslRng(), v,gsl_S,gsl_X);

  gsl_to_bmat(gsl_X,contens_);

  gsl_matrix_free(gsl_S);
  gsl_matrix_free(gsl_X);

}

///////////////////////////////////////////////////////////////
  DeclareContensClass(BMat, BMatTemporary, BMatVech);
  DefExtOpr(1, BMatVech, "Vech", 1, 1,
  "Matrix",
  "(Matrix mat)",
  I2("Vec Operator..","Vec Operator."),
     BOperClassify::MatrixAlgebra_);
   void BMatVech::CalcContens()
///////////////////////////////////////////////////////////////
{
  BMat *A = &Mat(Arg(1));
  size_t  r = A->Rows();
  size_t  c = A->Columns();

  if (r > 0 && c > 0) {
    contens_.Alloc(r*c,1);
    BDat * src, * dest;
    size_t i, j, d_ptr, s_ptr;
    
    src = A->GetData().GetBuffer();
    dest = contens_.GetData().GetBuffer();
    for (d_ptr = j = 0; j < c; j++)
      for (s_ptr = j, i = 0; i < r; i++, d_ptr++, s_ptr+=c)
        dest[d_ptr] = src[s_ptr];
  } else {
    Error(I2("Could no apply Vech to and invalid matrix, rows and columns must be positive",
             "Imposible aplicar operador Vech a una matriz con numero de filas o columnas no positivas"));
    contens_ = BMat::Unknown();
  }
}

DeclareContensClass(BMat, BMatTemporary, BMatSliceSampler1D);
DefExtOpr(1, BMatSliceSampler1D, "SliceSampler1D", 3, 9,
          "Code Real Real Real Real Real Set Real Real",
	  "(Code logf, Real L, Real U [, Real x0=(L+U)/2, Real size=1, Real burn=0, Set data=Empty, Real w=1, Real m=+Inf])",
	  I2("Slice Sampler: an adaptive MCMC sampler for univariate density functions\n\n"
	     "Arguments:\n\n"
	     "    logf         --> A function to evaluate the log-density. "
	     "This function must fulfill the API Real logf(Real x [, Set data]) "
	     "where x is a value from the domain of logf and data is a Set "
             "normally used to pass parameters to logf.\n"
	     "    L, U         --> Left and right bounds. Bounds must be supplied even "
	     "if the support is truly unbounded. We can use -Inf and +Inf for "
             "unbounded case.\n"
	     "    x0           --> Previous value from a Markov chain, for "
	     "instance when using SliceSampler withing a n-dimensional Gibbs "
             "sampler.\n"
             "    size         --> size of the sample returned.\n"
             "    burn         --> samples discarded before starting to collect the sample.\n"
	     "    data         --> parameters for the function logf"
	     "Example:\n\n"
	     "\tReal LogNorm( Real x )\n"
	     "\t{\n"
	     "\t    Real -0.5 * Pow((x-10.0)/5.0,2.0)\n"
	     "\t};\n\n"
	     "\tReal xsamp = SliceSampler1D(LogNorm, -100, 100, 1000, 10);\n",
             ""),
	  BOperClassify::Statistic_);

void BMatSliceSampler1D::CalcContens()
{
  BDat *dat_L, *dat_U, *dat_x0 = NULL;
  BDat *dat_size = NULL, *dat_burn = NULL;
  BDat *dat_w = NULL, *dat_m = NULL; 

  double L, U, x0,  w = 1.0;
  int size = 1, burn = 0, m = -1;
  int nArgs = NumArgs();
  
  BText english("In SliceSampler1D, ");
  BText spanish("En SliceSampler1D, ");

  BUserSet * uset = NULL;

  BCode * cod = &Code(Arg(1));
  dat_L = &Dat(Arg(2));
  dat_U = &Dat(Arg(3));

  /* check inf and sup */

  if (!dat_L->IsKnown() || !dat_U->IsKnown()) {
    english += "L and U values must be known";
    spanish += "los valores L y U deben ser conocidos";
    Error(I2(english,spanish));
    return;
  } else {
    L = dat_L->Value();
    U = dat_U->Value();
  }
  
  /* check valid interval */

  if (L >= U) {
    english += "invalid interval: upper must be greater than lower";
    spanish += "intervalo inválido: sup debe ser mayor que inf";
    Error(I2(english,spanish));
    return;
  }

  /* check for initial value */
  if (nArgs > 3) {
    dat_x0 = &Dat(Arg(4));
    x0 =dat_x0->Value();
    if (!dat_x0->IsFinite() || x0<L || x0>U) {
      english += "invalid x0 argument value: must be finite and included in [L,U]";
      spanish += "valor de argumento x0 inválido: debe ser finito e incluido en [L,U]";
      Error(I2(english,spanish));
      return;
    }
    
    /* check for sample size */
    if (nArgs > 4) {
      dat_size = &Dat(Arg(5));
      size = dat_size->Value();
      if (!dat_size->IsFinite()) {
        english += "invalid size argument value: must be finite";
        spanish += "valor de argumento size inválido";
        Error(I2(english,spanish));
        return;
      }

      /* check for burn size */
      if (nArgs > 5) {
        dat_burn = &Dat(Arg(6));
        if (!dat_burn->IsFinite()) {
          english += "invalid size argument value: must be finite";
          spanish += "valor de argumento size inválido";
          Error(I2(english,spanish));
          return;
        }

        if (nArgs > 6) {
          /* function data provided */
          uset = USet(Arg(7));
          if (uset->Contens().Card()) {
            /* check user function's number of arguments */
            if (cod->Operator()->MinArg()!=2 && 
                cod->Operator()->MaxArg()!=2) {
              english += "invalid argument number for logf argument: should be 2";
              spanish += "número de argumentos inválidos para argumento logf: debe ser 2";
              Error(I2(english,spanish));  
              return;
            }
            /* check grammar of user function's second argument */
            if (cod->Operator()->GrammarForArg(2) != GraSet()) {
              english += "invalid type ";
              english += cod->Operator()->GrammarForArg(2)->Name();
              english += " for second argument of logf argument: should be Set";
              spanish += "tipo inválido ";
              spanish += cod->Operator()->GrammarForArg(2)->Name();
              spanish += " en el segundo argumento del argumento logf: debe ser Set";
              Error(I2(english,spanish));
              return;
            }
          } else {
            /* Empty Set is the same as no providing argument data */
            uset = NULL;
          }
          if (nArgs > 7) {
            dat_w = &Dat(Arg(8));
            w = dat_w->Value();
            if (!dat_w->IsFinite()) {
              english += "invalid w argument value: must be finite";
              spanish += "valor de argumento w inválido";
              Error(I2(english,spanish));
              return;
            }
            if (nArgs > 8) {
              dat_m = &Dat(Arg(9));
              if (!dat_m->IsKnown()) {
                english += "invalid m argument value: must be known";
                spanish += "valor de argumento m inválido: debe ser conocido";
                Error(I2(english,spanish));
                return;
              }
              m = dat_m->IsFinite()?int(dat_m->Value()):-1;
            } else {
              /* m defaults to -1 meaning +Inf */
            }
          } else {
            /* w defaults to 1.0 */
            w = 1.0;
          }
        }
            
        if (uset == NULL) {
          /* no function data or Empty provided */
          /* check user function's number of arguments */
          if (cod->Operator()->MinArg()!=1 && 
              cod->Operator()->MaxArg()!=1) {
            english += "invalid argument number for logf argument: should be 1";
            spanish += "número de argumentos inválidos para argumento logf: debe ser 1";
            Error(I2(english,spanish));  
            return;
          }  
        }
      }  else {
          /* burns defaults to 10 */
          burn = 0;
      }
    } else {
      /* size defaults to 1 */
      size = 1;
    }
  } else {
    /* x0 not given, choose it as 0.5*(L+U) */
    x0 = 0.5*(L+U);
  }
  
  /* check user function */
  if (!cod->Operator()) {
    english += "invalid code argument";
    spanish += "argumento código inválido";
    Error(I2(english,spanish));
    return;
  }

  /* check user function's grammar */
  if (cod->Grammar() != GraReal()) {
    english += "invalid type ";
    english += cod->Grammar()->Name();
    english += " for logf argument: should be Real";
    spanish += " tipo inválido ";
    spanish += cod->Grammar()->Name();
    spanish += " para argumento logf: debe ser Real";
    Error(I2(english,spanish));
    return;
  } 

  /* check grammar of user function's first argument */
  if (cod->Operator()->GrammarForArg(1) != GraReal()) {
    english += "invalid type ";
    english += cod->Operator()->GrammarForArg(1)->Name();
    english += " for first argument of logf argument: should be Real";
    spanish += "tipo inválido ";
    spanish += cod->Operator()->GrammarForArg(1)->Name();
    spanish += " en el primer argumento del argumento logf: debe ser Real";
    Error(I2(english,spanish));
    return;
  }

  /* create the helper class to invoke a TOL user code from C */
  RealGSLFunctionR1 ucode_eval(cod, uset);

  contens_.Alloc(size,1);
  double sample = x0;
  
  /* do burnin */
  for (int bi=0; bi<burn; bi++) {
    sample = slice_sampler_1D(getGslRng(), sample,
                              &RealGSLFunction::StaticEvalReal,
                              &ucode_eval, NULL, w, m, L, U);
  }
  /* collect the samples */
  BDat *cells = contens_.GetData().GetBuffer();
  for (int si=0; si<size; si++) {
    sample = slice_sampler_1D(getGslRng(), sample,
                              &RealGSLFunction::StaticEvalReal,
                              &ucode_eval, NULL, w, m, L, U);
    cells[si].PutValue(sample);
  }
}

//--------------------------------------------------------------------
  DeclareContensClass(BMat, BMatTemporary, BMatQuantile);
  DefExtOpr(1, BMatQuantile, "Quantile", 2, 2, "Matrix Matrix",
  "(Matrix mat, Matrix p)",
  I2("Returns the quantile of probabilty p of all elements of a matrix.",
     "Devuelve el cuantil de probabilidad p de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BMatQuantile::CalcContens()
//--------------------------------------------------------------------
{
  BMat& m = Mat(Arg(1));
  BMat& p = Mat(Arg(2));
  const BArray<BDat>& m_ = m.Data();
  const BArray<BDat>& p_ = p.Data();
  contens_.Alloc(p.Rows(),p.Columns());
  Quantile(m_, p_, contens_.GetData());
}

//--------------------------------------------------------------------
class BMatOrder
//--------------------------------------------------------------------
{
 private:
  static const BMatrix<BDat>* mat_;
  static const BArray<int>*   criterium_;
 public:
  static int CompareValue(int i1, int i2)
  {
    int k,j,h;
    for(k=0; k<criterium_->Size(); k++)
    {
      h = (*criterium_)[k];
      if(h>0)
      {
        j=h-1;
        const BDat& m1 = (*mat_)(i1,j);
        const BDat& m2 = (*mat_)(i2,j);
        if(m1<m2) { return(-1); }
        if(m1>m2) { return(+1); }
      }
      else 
      {
        j=-h-1;
        const BDat& m1 = (*mat_)(i1,j);
        const BDat& m2 = (*mat_)(i2,j);
        if(m1<m2) { return(+1); }
        if(m1>m2) { return(-1); }
      }
    }
    return(0);
  }
  static int Compare(int i1, int i2)
  {
    int cmp = CompareValue(i1,i2);
    if(!cmp) { cmp = i1-i2;}
    return(cmp);
  }

  static int Compare(const void* v1, const void* v2)
  {
    return(Compare(*(int*)v1,*(int*)v2));
  }
  static void Sort(const BMatrix<BDat>& mat, 
            const BArray<int>& criterium,
            BArray<int>& perm) 
  {
    mat_ = &mat;
    criterium_ = &criterium;
    int i;
    perm.AllocBuffer(mat.Rows());
    for(i=0;i<perm.Size();i++)
    {
      perm[i]=i;
    }
    perm.Sort(Compare);
  }
};

const BMatrix<BDat>* BMatOrder::mat_ = NULL;
const BArray<int>*   BMatOrder::criterium_ = NULL;
  
  //--------------------------------------------------------------------
  DeclareContensClass(BMat, BMatTemporary, BMatSort);
  DefExtOpr(1, BMatSort, "Sort", 2, 3, "Matrix Set Real",
  "(Matrix mat, Set criterium [, Real unique])",
  I2("Sorts rows of a matrix given a mixed criterium of ascent or "
     "descent columns. For example, criterium=[[-2,3]] sorts by 2-nd "
     "column descent and 3-th one ascent in case of tie."
     "Returns the permutation matrix p such that PivotByRows(mat,p) "
     "is the sorted matrix.",
     "Ordena las filas de una matriz dado un criterio mezclado de "
     "columnas ascendentes y descendentes. Por ejemplo, criterium="
     "[[-2,3]] ordena por la segunda columna descendente y por la "
     "tercera ascendente en caso de empate."
     "Devuelve la matriz de permutación tal que PivotByRows(mat,p) "
     "es la matriz ordenada."),
     BOperClassify::Statistic_);
  void BMatSort::CalcContens()
//--------------------------------------------------------------------
{
  BMat& m = Mat(Arg(1));
  BSet& c = Set(Arg(2));
  bool unique = false;
  if(Arg(3))
  {
    unique = Real(Arg(3))!=0.0;
  }
  BArray<int> c_(c.Card());
  BArray<int> p(m.Rows());
  int j,k;
  bool ok=true;
  double x;
  for(k=0; k<c.Card(); k++)
  {
    ok=false;
    if(c[k+1]->Grammar()==GraReal())
    {
      x = Real(c[k+1]);
      c_[k]=(int)x;
      j=abs(c_[k]);
      if((x==c_[k])&&(1<=j)&&(j<=m.Columns()))
      {
        ok=true;
      }
      if(!ok)
      {
        Error(BText("[Matrix Sort] Wrong criterium order field number ")+
          (k+1)+": "+c[k+1]->Identify());
      }
    }
  }
  BMatOrder::Sort(m,c_,p);
  if(!unique)
  {
    contens_.Alloc(m.Rows(),1);
    for(k=0;k<m.Rows();k++)
    {
      contens_(k,0)=p[k]+1;
    }
  }
  else
  {
    int d=1;
    BArray<int> cmp(m.Rows());
    cmp[0]=1;
    for(k=1;k<m.Rows();k++)
    {
      if(cmp[k]=BMatOrder::CompareValue(p[k-1],p[k])) { d++; }
    }
    contens_.Alloc(d,1);
    for(k=d=0;k<m.Rows();k++)
    {
      if(cmp[k]) 
      { 
        contens_(d,0)=p[k]+1;
        d++; 
      }
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatForMat);
DefExtOpr(1, BMatForMat, "ForMat", 3, 3, "Real Real Code",
  "(Real rows, Real columns, Code fun)",
  I2("Returns a matrix with the given dimensions and the values generated "
  "by the indicator function, which takes two real arguments corresponding "
  "to the coordinates (i, j) and returns the element (i, j) of the matrix.\n"
  "Example : \n"
  ,
  "Devuelve una matriz con las dimensiones dadas y los valores generados "
  "por la funcion indicada, la cual toma dos argumentos reales "
  "correspondientes a las coordenadas (i,j) y devuelve el elemento (i,j) "
  "de la matriz.\n"
  "Ejemplo : \n")+
  "Matrix ForMat(2, 3, Real (Real n, Real m){n*m});",
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatForMat::CalcContens()
//--------------------------------------------------------------------
{
  int i,j;
  int rows = (int)Real(Arg(1));
  int cols = (int)Real(Arg(2));
  BCode&  code = Code(Arg(3));
  BStandardOperator* opr = (BStandardOperator*)code.Operator();
  if(!opr) 
  {
    Error(I2("<fun> argument of function ForMat has no operator",
             "El argumento <fun> de la función ForMat no tiene operador"));
    return;
  }
  if((opr->Grammar()!= GraReal()) ||
     (opr->NumArg()!=2)||
     (opr->GrammarForArg(1)!=GraReal())||
     (opr->GrammarForArg(2)!=GraReal())) 
  {
    Error(I2("<fun> argument of function ForMat ",
             "El argumento <fun> de la función ForMat ")+"\n"+
          opr->Grammar()->Name()+" "+code.Name()+opr->Arguments()+"\n"+
          I2("should be declared as ",
             "debería estar declarada como ")+"\n"+
          "Real "+code.Name()+"(Real i, Real j) \n");
    return;
  }

  contens_.Alloc(rows, cols);
  BSyntaxObject* ev;
  for(i=0; i<rows; i++)
  {
    for(j=0; j<cols; j++)
    {
      BContensDat* r = new BContensDat("",i+1);
      BContensDat* c = new BContensDat("",j+1);
      BList* args = Cons(r, NCons(c));
      ev = opr->Evaluator(args);
      if(ev)
      {
        contens_(i,j) = Dat(ev);
        DESTROY(ev);
      }
    }
  }  
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatReplace);
DefExtOpr(1, BMatReplace, "MatReplace", 3, 3, "Matrix Matrix Matrix",
  "(Matrix data, Matrix old, Matrix new)",
  I2("Replaces in arbitrary matrix 'data' all occurrences of cell "
  "given in column matrix 'old' by corresponding cell in column "
  "matrix 'new', having obviously the same dimensions that 'old'.\n"
  "CAUTION!: 'old' cannot have repeated cells\n",
  "Reemplaza en la matriz arbitraria 'data' todas las ocurrencias "
  "de las celdas de la matriz columna 'old' por la celda "
  "correspondiente de la matriz columna 'new', que lógicamente "
  "debe tener las mismas dimensiones que 'old'.\n"
  "¡CUIDADO!: 'old' no debe tener celdas repetidas\n"),
  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatReplace::CalcContens()
//--------------------------------------------------------------------
{
  int k;
  DMat& A    = dMat(Arg(1));
  DMat& old_ = dMat(Arg(2));
  DMat& new_ = dMat(Arg(3));
  int m = old_.Rows(); 
  if((new_.Rows()!=m)|(old_.Columns()!=1)|(new_.Columns()!=1))
  {
    Error(I2("Arguments 'old' and 'new' must be column matrices with "
             "the same number of rows.",
             "Los argumentos 'old' y 'mew' deben ser matrices columna "
             "con el mismo numero de filas.")+
             "\n  old:("+old_.Rows()+"x"+old_.Columns()+"); "+
             "\n  new:("+new_.Rows()+"x"+new_.Columns()+"); ");
    return;
  }
  hash_map_by_double<double>::sparse_ hashMap;
  hash_map_by_double<double>::sparse_::const_iterator fc;
  const double* x = old_.Data().Buffer();
  const double* y = new_.Data().Buffer();
  for(k=0; k<m; k++, x++, y++)
  {
    const double& xk = *x;
    fc = hashMap.find(xk);
    if(fc!=hashMap.end()) 
    { 
      Error(I2("Argument 'old' cannot have repeated cells.",
               "El argumento 'old' no puede tener celdas repetidas.")+
               "\n  old["+(k+1)+"] = "+xk+"; ");
      return;
    }
    hashMap[xk] = *y;
  }
  int rows = A.Rows();
  int cols = A.Columns();
  contens_.Alloc(rows, cols);
  const BArray<double>& data = A.Data();
  int s = data.Size();
  const double* a = data.Buffer();
  double* c = (double*)(contens_.GetData().GetBuffer());
  for(k=0; k<s; k++, a++, c++)
  {
    fc = hashMap.find(*a);
    if(fc!=hashMap.end()) 
    { 
      *c = fc->second;
    }
    else
    {
      *c = *a;
    }
  }
   
}


