//-------------------------------------------------------------------
// The code was written by Vikas C. Raykar 
// and is copyrighted under the Lessr GPL: 
//
// Copyright (C) 2006 Vikas C. Raykar
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 or later.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU Lesser General Public License for more details. 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, 
// MA 02111-1307, USA.  
//
// The author may be contacted via email at: vikas(at)cs(.)umd(.)edu 
//-------------------------------------------------------------------

//-------------------------------------------------------------
// File    : FastUnivariateDensityDerivative.h
// Purpose : Header file for FastUnivariateDensityDerivative.cpp
// Author  : Vikas C. Raykar (vikas@cs.umd.edu)
// Date    : September 17, 2005
//-------------------------------------------------------------
// Fast implementation of the r^{th} kernel density derivative
// estimate based on the Gaussian kernel.
// [HANDLES ONLY UNIVARIATE CASE]
// 
// Data is assumed to be scaled to the unit interval [0 1].
//
// Implementation based on:
//
// V. C. Raykar and R. Duraiswami 'Very fast optimal bandwidth 
// selection for univariate kernel density estimation'
// Technical Report CS-TR-4774, Dept. of Computer 
// Science, University of Maryland, College Park.
// ------------------------------------------------------------
//
// INPUTS [7] 
// ----------------
// NSources      --> number of sources, N.
// MTargets      --> number of targets, M.
// pSources      --> pointer to sources, px(N).
// pTargets       --> pointer to the targets, py(M).
// Bandwidth    --> the source bandwidth, h.
// Order          --> order of the derivative, r.
// epsilon        --> desired error, eps.
//
// OUTPUTS [1]
// ----------------
// pDensityDerivative --> pointer the the evaluated Density 
//             Derivative, pD(M).
//-------------------------------------------------------------------


#ifndef UNIVARIATE_DENSITY_DERIVATIVE_H
#define UNIVARIATE_DENSITY_DERIVATIVE_H

class DoubleArray
{
 public: 
  double* buffer_;
  int     size_;
  int            _bfsm_BaseSize__;
  unsigned short _bfsm_VectorPageNum__;

  DoubleArray() : buffer_(0), size_(0) { }
 ~DoubleArray() { Free(); }
  void Alloc(int size);
  void Free();
  const double& operator[] (int i) const { return(buffer_[i]); }
        double& operator[] (int i)       { return(buffer_[i]); }
};

class IntArray
{
 public: 
  int* buffer_;
  int  size_;
  int            _bfsm_BaseSize__;
  unsigned short _bfsm_VectorPageNum__;

  IntArray() : buffer_(0), size_(0) { }
 ~IntArray() { Free(); }
  void Alloc(int size);
  void Free();
  const int& operator[] (int i) const { return(buffer_[i]); }
        int& operator[] (int i)       { return(buffer_[i]); }
};

class FastUnivariateDensityDerivative{
  public:
    //constructor 
    FastUnivariateDensityDerivative(int     NSources,
                                    int     MTargets,
                                    double *pSources,
                                    double *pTargets,
                                    int     Order,
                                    double  epsilon,
                                    double *pDensityDerivative);

    //destructor
    ~FastUnivariateDensityDerivative();


    //function to evaluate the Density Derivative
    void Evaluate(double Bandwidth);

    static void Evaluate(int     NSources,
                         int     MTargets,
                         double *pSources,
                         double *pTargets,
                         double  Bandwidth,
                         int     Order,
                         double  epsilon,
                         double *pDensityDerivative);

    static void EvaluateWithOptimalBandwith(int     NSources,
                                            int     MTargets,
                                            double *pSources,
                                            double *pTargets,
                                            int     Order,
                                            int     numIter,
                                            double  epsilon,
                                            double *pDensityDerivative);
    
    //function to evaluate the Hermite polynomial.
    double hermite(double x, int r);

  private:
    int    N;        //number of sources.
    int    M;        //number of targets.
    double *px;      //pointer to sources, (N).
    double *py;      //pointer to the targets, (M).
    double  h;       //the source bandwidth.
    int     r;       //the rth density derivative.
    double eps;      //the desired error
    double *pD;      //pointer the the evaluated Density Derivative, (M).

    double rx;
    double rr;
    double ry;
    int    K;
    int    p;
    double h_square;
    double two_h_square;

    IntArray    pClusterIndex;
    DoubleArray pClusterCenter;
    DoubleArray l_constant;
    DoubleArray m_constant;
    DoubleArray k_factorial;
    DoubleArray temp3;
    DoubleArray temp4;
    DoubleArray a_terms;
    DoubleArray B_terms;

    int    num_of_a_terms;
    int    num_of_B_terms;
    double pi;
    double q;

    int  factorial(int n);
    void choose_parameters();
    void space_sub_division();
    void compute_a();
    void compute_B();
  
};

/*
%     AMISE optimal bandwidth estimation for univariate kernel
%     density estimation. [FAST METHOD]
%     [Shether Jones Solve-the-equation plug-in method.]
%     --------------------------------------------------------------------------
%     INPUTS
%     --------------------------------------------------------------------------
%     N                 --> number of source points.
%     X                 --> 1 x N matrix of N source points.
%     epsil           --> accuracy parametr for fast density derivative
%                              estimation [1e-3 to 1e-6]
%     --------------------------------------------------------------------------
%     OUTPUTS
%     --------------------------------------------------------------------------
%     h                 --> the optimal bandiwdth estimated using the
%                              Shether Jones Solve-the-equation plug-in
%                              method.
%     --------------------------------------------------------------------------
%     Implementation based on:
%
%     V. C. Raykar and R. Duraiswami 'Very fast optimal bandwidth 
%     selection for univariate kernel density estimation'
%     Technical Report CS-TR-4774, Dept. of Computer 
%     Science, University of Maryland, College Park.
%
%    S.J. Sheather and M.C. Jones. 'A reliable data-based
%    bandwidth selection method for kernel density estimation'
%    J. Royal Statist. Soc. B, 53:683-690, 1991
%    ---------------------------------------------------------------------------
*/

//-------------------------------------------------------------------
class FastUnivariateBandwidthEstimate
//-------------------------------------------------------------------
{
public:
  FastUnivariateBandwidthEstimate(int N_, double* X_, int numIter, double epsil_);
 ~FastUnivariateBandwidthEstimate();

public:
  double h;

private:
  int    N;
  double *X;
  double epsil;

  double sigma;
  double phi4;
  double phi6;
  double phi8;
  double g1;
  double g2;
  DoubleArray D4;
  DoubleArray D4_;
  DoubleArray D6;
  double constant1;
  double constant2;
  FastUnivariateDensityDerivative* fudd;
  double AMISE_equation(double h);
  double AMISE_bisection(double a, double b, double fa, double fb);
};



#endif