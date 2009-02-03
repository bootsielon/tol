/* api_tol_optimal_bw.cpp: Interface between TOL and optimal_bw, a fast 
                           optimal bandwidth selection for univariate 
                           kernel density estimation.

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

/*!
  \file   api_tol_optimal_bw.cpp
  \author vdebuen@tol-project.org
  \date   2007-09-13

  \brief  API of TOL for optimal_bw KDE functions  
*/


#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include "FastUnivariateDensityDerivative.h"
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>

BTraceInit("api_tol_optimal_bw.cpp");

#define dMat(arg) ((DMat&)Mat(arg))
#define b2dMat(M) ((DMat&)(M))


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatUniKerDnsEstOptBnw);
DefExtOpr(1, BMatUniKerDnsEstOptBnw, "UniKerDnsEstOptBnw", 2, 5, 
"Matrix Matrix Real Real Real",
I2("(Matrix sample,  Matrix targets [, Real order=0, Real numIter=0, "
   "Real epsilon=1.E-5])",
   "(Matrix muestra, Matrix objetivos [, Real orden=0, Real numIter=0, "
   "Real epsilon=1.E-5])"),
BText("UniKerDnsEstOptBnw ")+
I2("is an abbreviation for", "es abreviatura de" )+" "+
"Univariate Kernel Density Estimation with Optimal Bandwith "+
I2("A software developped by", "Un programa desarrollado por")+
   " Vikas C. Raykar "+
I2("under the Lessr GPL license and available at ",
   "bajo la licencia Lessr GPL y disponible en ")+"\n"+
"  http://www.umiacs.umd.edu/~vikas/Software/optimal_bw/optimal_bw_code.htm \n"+
I2("Given a vector <sample> of independent accomplishments of a univariate "
   "random variable, this function approximates, over the given targets, "
   "the <order>-th derivative of "
   "the density function, by means of the method called Kernel Density "
   "Estimation (KDE) with normal kernel function and using the optimal-AMISE "
   "criterion for bandwidth selection. If <order> is 0, it gives back the "
   "density and if it's greater, its successive derivatives. The argument "
   "<epsilon> indicates the tolerance to the error with respect to the exact "
   "method that it is desired to obtain, and the greater it is, the faster and "
   "lesser exact it will be the result.",
   "Dado un vector <muestra> de realizaciones independientes de una variable "
   "aleatoria univariante esta función aproxima, en los objetivos marcados, "
   "la derivada de <orden> dado "
   "de la función de densidad correspondiente mediante el método Kernel "
   "Density Estimation (KDE) con función de núcleo normal y usando el criterio "
   "de optimalidad AMISE para la selección del ancho de banda. Si <orden> es 0 "
   "devuelve la densidad y si es mayor sus derivadas sucesivas. El argumento "
   "<epsilon> indica la tolerancia al error con respecto al método exacto que "
   "se desea obtener y cuanto mayor sea más rápido y menos exacto será el "
   "resultado."),
BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BMatUniKerDnsEstOptBnw::CalcContens()
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE BMatUniKerDnsEstOptBnw 1"));
  DMat& sample = dMat(Arg(1));
  DMat& target = dMat(Arg(2));
  int order = 0;
  int numIter = 0;
  double epsilon = 1.E-5;
  if(Arg(3)) { order   = (int)Real(Arg(3)); }
  if(Arg(4)) { numIter = (int)Real(Arg(4)); }
  if(Arg(5)) { epsilon = Real(Arg(5)); }
  if(sample.Data().Size() &&
     target.Data().Size() &&
     (order >= 0)&&
     (epsilon > 0))
  {
    int i;
    int N = sample.GetData().Size();
    int M = target.GetData().Size();
    double *X = new double[N];
    double *T = new double[M];
// Scale the data to lie in the unit interval
    double shift_bottom = sample.GetData()[0];
    double shift_top    = sample.GetData()[0];
    for(i=1; i<N; i++) 
    { 
      double x = sample.GetData()[i];
      shift_bottom = min(x,shift_bottom); 
      shift_top    = max(x,shift_top); 
    }
    double scale = 1.0/(shift_top-shift_bottom);
    for(i=0; i<N; i++) 
    { 
      X[i] = (sample.GetData()[i]-shift_bottom)*scale;
    }
    for(i=0; i<M; i++) 
    { 
      T[i] = (target.GetData()[i]-shift_bottom)*scale;
    }
    contens_.Alloc(M,1);
//Std(BText("\nTRACE BMatUniKerDnsEstOptBnw 2"));
    FastUnivariateDensityDerivative::
    EvaluateWithOptimalBandwith(N, M, X, T, order, numIter, epsilon,
			                          b2dMat(contens_).GetData().GetBuffer());
//Std(BText("\nTRACE BMatUniKerDnsEstOptBnw 4"));
    contens_*=scale;
  }
}

