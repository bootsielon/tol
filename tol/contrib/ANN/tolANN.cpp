/* datgra.cpp: Real Grammar's arithmetic functions
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


#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <ANN/ANN.h>

#include <tol/tol_bmatgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bsetgra.h>

#ifndef dMat
#define dMat(arg) ((DMat&)Mat(arg))
#endif

class tol_kdtree {
public:
  tol_kdtree( DMat &points )
    : pa( NULL ), size( 0 )
  {
    int dim = points.Columns();
    size = points.Rows();
    pa = annAllocPts( size, dim );
    /* asume que los puntos estan dispuestos por filas y que BDat es
       un double */
    double *buffer = points.GetData().GetBuffer();
    ANNpoint coords = pa[0];
    for ( int i = 0; i < size * dim; i++ ) {
      coords[ i ] = buffer[ i ];
    }
    /* construyo el kdtree */
    tree = new ANNkd_tree( pa, size, dim, bucket_size );
  }

  ~tol_kdtree( )
  {
    annDeallocPts( pa );
    pa = NULL;
    delete tree;
    tree = NULL;
  }

  ANNkd_tree* get_kdtree( ) { return tree; };
  
  int get_dim( ) { return tree->theDim( ); }
  
  static double code_addr( tol_kdtree* ptr )
  {
    double addr = 0.0;
    *((tol_kdtree**)(&addr)) = ptr;
    return addr;
  }
  
  static tol_kdtree* decode_addr( double addr )
  {
    return *((tol_kdtree**)(&addr));
  }

private:  
  static const int bucket_size;
  ANNpointArray pa;
  int size;
  ANNkd_tree* tree;
};

const int tol_kdtree::bucket_size = 5;

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatANNKDTreeNew);
DefExtOpr(1, BDatANNKDTreeNew, "ANN.KDTree.New", 1, 1, "Matrix",
          "(Matrix points)",
	  I2("Build a kd-tree and return its identifier. A kd-tree is a "
             "complex data structure useful to perform nearest neighbourhood "
             "searches. When the kd-tree is not needed any more it should be "
             "relased with ANN.KDTree.Delete\n\n"
             "See also ANN.KDTree.Delete, ANN.KDTree.RSearch and "
             "ANN.KDTree.KSearch",
             "Construye un kd-tree y retorna su identificador. Un kd-tree es "
             "una estructura de datos compleja util en la busqueda de "
             "vecinos cercanos. Cuando un kd-tree ya no se necesite mas debe "
             "liberarse con ANN.KDTree.Delete\n\n"
             "Vease tambien ANN.KDTree.Delete, ANN.KDTree.RSearch y "
             "ANN.KDTree.KSearch"),
	  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BDatANNKDTreeNew::CalcContens()
{
  tol_kdtree *aux = NULL;
  DMat &points = dMat( Arg( 1 ) );
  if ( points.Columns() > 0 && points.Rows() > 0 ) {
    aux = new tol_kdtree( points );
  } else {
    Error( I2( "Invalid dimension of points matrix",
               "Dimensiones invalida para la matriz points" ) );
  }
  if ( !aux ) {
    Error( I2( "Could not build kd-tree",
               "no se pudo construir el kd-tree" ) );
  }
  contens_ = BDat( tol_kdtree::code_addr( aux ) );
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatANNKDTreeDelete);
DefIntOpr(1, BDatANNKDTreeDelete, "ANN.KDTree.Delete", 1, 1,
	  "(Real kdtree)",
	  I2("Release a kd-tree previously created with ANN.KDTree.New. "
             "Returns 1 if success or 0 in case of fail.\n\n"
             "See also ANN.KDTree.New, ANN.KDTree.RSearch and "
             "ANN.KDTree.KSearch",
             "Libera un kd-tree previamente creado con ANN.KDTree.New. "
             "Retorna 1 si se libero exitosamente o 0 en caso de fallo.\n\n"
             "Vease tambien ANN.KDTree.New, ANN.KDTree.RSearch y "
             "ANN.KDTree.KSearch"),
	  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BDatANNKDTreeDelete::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  tol_kdtree *aux = tol_kdtree::decode_addr( addr );
  /* debemos tener un hash de las direccion creadas con New de forma
     tal que podamos verificar si la direccion es valida antes de
     hacer delete */
  delete aux;
  contens_ = BDat( 1.0 );
}

//--------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetANNKDTreeKSearch);
DefExtOpr(1, BSetANNKDTreeKSearch, "ANN.KDTree.KSearch", 3, 3,
          "Real Matrix Real",
          "(Real kdtree, Matrix qpoints, Real k)",
	  I2("Computes for every point in qpoints the k nearest neighbors. "
             "The result is a Set containing two matrix: indices and dists."
             "The matrix indices contains for each point the set of neighbors "
             "in increasing order of distance. The matrix dists contains the "
             "corresponding squared distances.\n"
             "The indices returned are referred to the ordering of the points "
             "given in ANN.KDTree.New. The argument kdtree must be a valid "
             "kd-tree as returned from ANN.KDTree.New\n\n"
             "See also ANN.KDTree.New, ANN.KDTree.Delete and "
             "ANN.KDTree.RSearch",
             "Calcula para cada punto en qpoints los k vecinos mas cercanos. "
             "El resultado es un conjunto con dos matrices: indices y dists."
             "La matriz indices contiene para cada punto el conjunto de "
             "vecinos en orden creciente de las distancias. La matriz dists "
             "contiene las correspondientes distancias al cuadrado.\n"
             "Los indices que se retornan son referidos al orden de los "
             "puntos dados en la llamada a ANN.KDTree.New. El argumento "
             "kdtree debe ser un kd-tree valido retornado desde "
             "ANN.KDTree.New\n\n"
             "Vease tambien ANN.KDTree.New, ANN.KDTree.Delete y "
             "ANN.KDTree.RSearch"),
	  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BSetANNKDTreeKSearch::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  tol_kdtree *tree = tol_kdtree::decode_addr( addr );
  DMat &qpoints = dMat( Arg( 2 ) );
  
  BDat dK = Dat( Arg( 3 ) );

  if ( dK.IsUnknown( ) || dK.Value() <= 0 ) {
    Error( I2( "Invalid k argument: must be positive",
               "Argumento k es invalido: debe ser positivo" ) );
    return;
  }
  int k = int( round( dK.Value( ) ) );
  ANNpoint pt = annAllocPt( tree->get_dim(), 0.0 );
  ANNidxArray nn_idx = new ANNidx[k];
  ANNdistArray nn_dists = new ANNdist[k];
  double *qp_buffer = ((DMat&)qpoints).GetData().GetBuffer();
  BMat nn_mat; nn_mat.Alloc( qpoints.Rows( ), k );
  double *nn_buffer = ((DMat&)nn_mat).GetData().GetBuffer();
  BMat dists_mat; dists_mat.Alloc( qpoints.Rows( ), k );
  double *dists_buffer = ((DMat&)dists_mat).GetData().GetBuffer();
  for ( int r = 0; r < qpoints.Rows( ); r++ ) {
    for ( int c = 0; c < qpoints.Columns( ); c++ ) {
      pt[ c ] = *qp_buffer++;
    }
    tree->get_kdtree()->annkSearch( pt,      // query point
                                    k,       // number of near neighbors
                                    nn_idx,  // nearest neighbors (returned)
                                    nn_dists,   // distance (returned)
                                    0.0 );
    // results rows for pt
    for( int _k = 0; _k < k; ++_k ) {
      *nn_buffer++ = double( nn_idx[ _k ] +1);
      *dists_buffer++ = nn_dists[ _k ];
    }
  }
  assert( (qp_buffer - ((DMat&)qpoints).GetData().GetBuffer()) ==
          qpoints.Rows()*qpoints.Columns() );
  annDeallocPt( pt );
  delete [] nn_idx;
  delete [] nn_dists;
  BList* lst_result = NIL;
  BUserMat* nn_umat = BContensMat::New( "", nn_mat, "" );
  nn_umat->PutName( "indices" );
  BUserMat* dists_umat = BContensMat::New( "", dists_mat, "" );
  dists_umat->PutName( "dists" );
  lst_result = Cons( nn_umat, Cons( dists_umat, NIL ) );
  contens_.RobElement( lst_result );
}
