/* tolTPP.cpp: API for Triangle ++.

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


#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include "del_interface.hpp"


#include <tol/tol_bmatgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bsetgra.h>

#ifndef dMat
#define dMat(arg) ((DMat&)Mat(arg))
#endif

bool MyAssertFunction( bool b, const char* desc, int line, const char* file)
{
#if defined( _DEBUG )
  assert(b);
#endif
  return(b);
}

class tol_tpp {
public:
  tpp::Delaunay* delaunay_;
  tol_tpp( DMat &points )
    : delaunay_( NULL )
  {
    int dim = points.Columns();
    int size = points.Rows();
    int i;
    tpp::Delaunay::Point tempP;
    std::vector< tpp::Delaunay::Point > v;
    for(i=0; i<size; i++)
    {
      tempP[0] = points(i,0);
      tempP[1] = points(i,1);
      v.push_back(tempP);
    }
    delaunay_ = new tpp::Delaunay(v);  
    delaunay_->Triangulate();
  }

  ~tol_tpp( )
  {
    delete delaunay_;
    delaunay_ = NULL;
  }
  
  static double code_addr( tol_tpp* ptr )
  {
    double addr = 0.0;
    *((tol_tpp**)(&addr)) = ptr;
    return addr;
  }
  
  static tol_tpp* decode_addr( double addr )
  {
    return *((tol_tpp**)(&addr));
  }

  void get_vertices(DMat& vertex) const
  {
    vertex.Alloc(delaunay_->nvertices(),2);
    int v=0;
    for(tpp::Delaunay::vIterator vit  = delaunay_->vbegin(); 
                            vit != delaunay_->vend(); 
                        ++vit, v++)
    {
      tpp::Delaunay::Point & p = *vit;
      vertex(v,0)=p[0];
      vertex(v,1)=p[1];
    }
  }

  void get_faces(DMat& face, int first) const
  {
    face.Alloc(delaunay_->ntriangles(),3);
    int f=0;
    for(tpp::Delaunay::fIterator fit  = delaunay_->fbegin(); 
                            fit != delaunay_->fend(); 
                        ++fit, f++)
    {
      face(f,0)=delaunay_->Org(fit)+first;
      face(f,1)=delaunay_->Dest(fit)+first;
      face(f,2)=delaunay_->Apex(fit)+first;
    }
  }

};


//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatTPPDelaunayNew);
DefExtOpr(1, BDatTPPDelaunayNew, "TPP.Delaunay.New", 1, 1, "Matrix",
"(Matrix points)",
I2("Build a Delaunay 2D-triangulation for a list of points defined as rows "
   "of given matrix with 2 columns, and returns an identifier. "
   "When the Delaunay object is not needed any more it should be relased "
   "with TPP.Delaunay.Delete\n\n"
   "See also TPP.Delaunay.Delete, Tpp.Delaunay.GetVertices and "
   "Tpp.Delaunay.GetFaces",
   "Construye una triangulación 2D de Delaunay a aprtir de una lista de "
   "puntos dados como filas de una matriz, y retorna su identificador. "
   "Cuando ya no se necesite mas debe liberarse con TPP.Delaunay.Delete\n\n"
   "Vease tambien TPP.Delaunay.Delete, Tpp.Delaunay.GetVertices y "
   "Tpp.Delaunay.GetFaces"),
BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BDatTPPDelaunayNew::CalcContens()
{
  tol_tpp *aux = NULL;
  DMat &points = dMat( Arg( 1 ) );
  if ( points.Columns() > 0 && points.Rows() > 0 ) {
    aux = new tol_tpp( points );
  } else {
    Error( I2( "Invalid dimension of points matrix",
               "Dimensiones invalida para la matriz points" ) );
  }
  if ( !aux ) {
    Error( I2( "Could not build Delaunay triangulation",
               "no se pudo construir la triangulación de Delaunay" ) );
  }
  contens_ = BDat( tol_tpp::code_addr( aux ) );
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatTPPDelaunayDelete);
DefIntOpr(1, BDatTPPDelaunayDelete, "TPP.Delaunay.Delete", 1, 1,
"(Real delaunay)",
I2("Release a Delaunay triangulation previously created with TPP.Delaunay.New. "
   "Returns 1 if success or 0 in case of fail.\n\n"
   "See also TPP.Delaunay.New, Tpp.Delaunay.GetVertices y "
   "Tpp.Delaunay.GetFaces",
   "Libera una triangulacón de Delaunay previamente creada con TPP.Delaunay.New. "
   "Retorna 1 si se libero exitosamente o 0 en caso de fallo.\n\n"
   "Vease tambien TPP.Delaunay.New, Tpp.Delaunay.GetVertices y "
   "Tpp.Delaunay.GetFaces"),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BDatTPPDelaunayDelete::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  tol_tpp *aux = tol_tpp::decode_addr( addr );
  /* debemos tener un hash de las direccion creadas con New de forma
     tal que podamos verificar si la direccion es valida antes de
     hacer delete */
  delete aux;
  contens_ = BDat( 1.0 );
}

//--------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatTPPDelaunayGetVertices);
DefExtOpr(1, BMatTPPDelaunayGetVertices, "TPP.Delaunay.GetVertices", 1, 1,
"Real",
"(Real delaunay)",
I2("Returns the list of vertices of a Delaunay triangulation previously "
   "created with TPP.Delaunay.New, as a matrix with two columns "
   "storing the coordinates of each triangle.",
   "Devuelve la lista de los vértices de una triangulacón de Delaunay "
   "previamente creada con TPP.Delaunay.New, como una matriz de dos "
   "columnas con las coordenadas de cada vértice."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BMatTPPDelaunayGetVertices::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  tol_tpp *aux = tol_tpp::decode_addr( addr );
  aux->get_vertices((DMat&)contens_);
}


//--------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatTPPDelaunayGetFaces);
DefExtOpr(1, BMatTPPDelaunayGetFaces, "TPP.Delaunay.GetFaces", 1, 1,
"Real",
"(Real delaunay)",
I2("Returns the list of faces of a Delaunay triangulation previously "
   "created with TPP.Delaunay.New, as a matrix with three columns "
   "storing the indexes of vertixes of each triangle.",
   "Devuelve la lista de las caras de una triangulacón de Delaunay "
   "previamente creada con TPP.Delaunay.New, como una matriz de tres "
   "columnas con los índices de los vértices de cada triángulo."),
    BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BMatTPPDelaunayGetFaces::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  tol_tpp *aux = tol_tpp::decode_addr( addr );
  aux->get_faces((DMat&)contens_,1);
}



/* */