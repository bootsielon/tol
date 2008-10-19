/* kmeans_tolapi.cpp: API to kmeans methods.
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

#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdatgra.h>
#include "KMlocal.h"			// k-means algorithms

void get_KM_output(BSet& result, KMfilterCenters& ctrs)
{
  int i, j;

  BUserMat* umat_centers = new BContensMat("Centers",BMat(),
					"Cluster centers");
  BMat* centers = &(umat_centers->Contens());
  centers->Alloc(ctrs.getK(), ctrs.getDim());
  
  for (i = 0; i < ctrs.getK(); i++) {
    const KMcenter& c = ctrs[i];
    for (j = 0; j < ctrs.getDim(); j++) {
      (*centers)(i,j) = c[j];
    }
  }
  
  KMctrIdxArray closeCtr = new KMctrIdx[ctrs.getNPts()];
  double* sqDist = new double[ctrs.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist);

  BUserMat* umat_assigments = new BContensMat("Assignments",BMat(),
					   "Cluster assignments");
  BMat* assignments = &(umat_assigments->Contens());
  assignments->Alloc(ctrs.getNPts(), 2);
  
  
  for (i = 0; i < ctrs.getNPts(); i++) {
    (*assignments)(i,0) = closeCtr[i];
    (*assignments)(i,1) = sqDist[i];
  }
  BUserDat* av_distortion = new BContensDat("AverageDistortion",
				      ctrs.getDist(0)/double(ctrs.getNPts()),
				      "Average distortion");
  result.RobElement(Cons(av_distortion,
			 Cons(umat_centers,
			      Cons(umat_assigments, NIL))));
  delete [] closeCtr;
  delete [] sqDist;
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetKMeans);
DefExtOpr(1, BSetKMeans, "KMeans", 2, 2, "Matrix Real",
	  "(Matrix data, Real k)",
	  "KMeans",
          BOperClassify::DataMining_);
//--------------------------------------------------------------------
void BSetKMeans::CalcContens()
//--------------------------------------------------------------------
{
  BMat* mat_data;
  int K;
  BDat* dat_k;

  mat_data = &Mat(Arg(1));
  if (mat_data->Rows() <= 0 || mat_data->Columns() <= 0) {
    Error("Invalid data matrix: number of rows and columns must be greater than zero");
    return;
  }
  dat_k = &Dat(Arg(2));
  K = int(dat_k->Value());
  if (!dat_k->IsKnown() || K <= 0) {
    Error("Argument k must be a positive number");
    return;
  }
  
  KMdata dataPts(mat_data->Columns(), mat_data->Rows());
  
  for (int i = 0; i < dataPts.getNPts(); i++) {
    KMdataPoint& pt = dataPts[i];
    for (int j = 0; j < dataPts.getDim(); j++) {    
      pt[j] = (*mat_data)(i,j).Value();
    }
  }
  //----------------------------------------------------------------------
  //  Termination conditions
  //	These are explained in the file KMterm.h and KMlocal.h.  Unless
  //	you are into fine tuning, don't worry about changing these.
  //----------------------------------------------------------------------
  KMterm	term(100, 0, 0, 0,		// run for 100 stages
		     0.10,			// min consec RDL
		     0.10,			// min accum RDL
		     3,			// max run stages
		     0.50,			// init. prob. of acceptance
		     10,			// temp. run length
		     0.95);			// temp. reduction factor
  
  dataPts.buildKcTree(); // build filtering structure
  KMfilterCenters ctrs(K, dataPts); // allocate centers 
  KMlocalHybrid kmHybrid(ctrs, term);		// Hybrid heuristic
  ctrs = kmHybrid.execute();

  get_KM_output(contens_, ctrs);
}

/* TODO:
  
   - Parametrizar el metodo a utilizar.

   - Parametrizar los criterios de parada.

   - Parametrizar la salida deseada, ayudando asi a TOL a un mejor
     manejo de los recursos.

 */
