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
DeclareContensClass(BDat, BDatTemporary, BDatKDTreeNew);
DefExtOpr(1, BDatKDTreeNew, "KDTree.New", 1, 1, "Matrix",
          "(Matrix points)",
	  I2("Build a kd-tree and return its identifier. A kd-tree is a "
             "complex data structure useful to perform nearest neighbourhood "
             "searches. When the kd-tree is not needed any more it should be "
             "relased with KDTree.Delete\n"
             "See also KDTree.Delete, KDTree.RSearch, KDTree.KSearch",
             "Construye un kd-tree y retorna su identificador. Un kd-tree es "
             "una estructura de datos compleja util en la busqueda de "
             "vecinos cercanos. Cuando un kd-tree ya no se necesite mas debe "
             "liberarse con KDTree.Delete\n"
             "Vease tambien KDTree.Delete, KDTree.RSearch, KDTree.KSearch"),
	  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BDatKDTreeNew::CalcContens()
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
DeclareContensClass(BDat, BDatTemporary, BDatKDTreeDelete);
DefIntOpr(1, BDatKDTreeDelete, "KDTree.Delete", 1, 1,
	  "(Real kdtree)",
	  I2("Release a kd-tree previously created with KDTree.New. "
             "Returns 1 if success or 0 in case of fail.\n"
             "See also KDTree.New, KDTree.RSearch, KDTree.KSearch",
             "Libera un kd-tree previamente creado con KDTree.New. "
             "Retorna 1 se libero exitosamente o 0 en caso de fallo.\n"
             "Vease tambien KDTree.New, KDTree.RSearch, KDTree.KSearch"),
	  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BDatKDTreeDelete::CalcContens()
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
DeclareContensClass(BSet, BSetTemporary, BSetKDTreeKSearch);
DefExtOpr(1, BSetKDTreeKSearch, "KDTree.KSearch", 3, 3, "Real Matrix Real",
          "(Real kdtree, Matrix qpoints, Real k)",
	  I2("Computes for every point in points the k nearest neighbors. "
             "The result is a Set containing the matrix result for each "
             "point. Each matrix result contain in the column 1 the index "
             "the neighbors and in the column 2 the squared distance to the "
             "query point. The index is referred to the ordering of the "
             "points given in KDTree.New. The argument kdtree must be a "
             "valid kdtree as returned from KDTree.New\n"
             "See also KDTree.New, KDTree.Delete and KDTree.RSearch",
             ""),
	  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------
void BSetKDTreeKSearch::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  tol_kdtree *tree = tol_kdtree::decode_addr( addr );
  DMat &qpoints = dMat( Arg( 2 ) );
  BList* lst_result = NIL;
  BList* lst_aux = NIL;
  
  BDat dK = Dat( Arg( 3 ) );

  if ( dK.IsUnknown( ) || dK.Value() <= 0 ) {
    Error( I2( "Invalid k argument: must be positive",
               "Argumento k es invalido: debe ser positivo" ) );
    return;
  }
  int k = int( round( dK.Value( ) ) );
  ANNpoint pt = annAllocPt( tree->get_dim(), 0.0 );
  ANNidxArray nn_idx = new ANNidx[k];
  ANNdistArray dists = new ANNdist[k];
  double *buffer = ((DMat&)qpoints).GetData().GetBuffer();
  BMat nn_mat; nn_mat.Alloc( k, 2 );
  double *nn_buffer = ((DMat&)nn_mat).GetData().GetBuffer();
  for ( int r = 0; r < qpoints.Rows( ); r++ ) {
    for ( int c = 0; c < qpoints.Columns( ); c++ ) {
      pt[ c ] = *buffer++;
    }
    tree->get_kdtree()->annkSearch( pt,      // query point
                                    k,       // number of near neighbors
                                    nn_idx,  // nearest neighbors (returned)
                                    dists,   // distance (returned)
                                    0.0 );
    // result matrix for pt
    double* ptr = nn_buffer;
    for( int _r = 0; _r < k; ++_r ) {
      *ptr++ = double( nn_idx[ _r ] );
      *ptr++ = dists[ _r ];
    }
    BUserMat* nn_umat = BContensMat::New( "", nn_mat, "" );
    LstFastAppend( lst_result, lst_aux, nn_umat );
  }
  assert( (buffer - ((DMat&)qpoints).GetData().GetBuffer()) ==
          qpoints.Rows()*qpoints.columns() );
  annDeallocPt( pt );
  delete [] nn_idx;
  delete [] dists;
  contens_.RobElement( lst_result );
}
