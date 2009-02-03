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
// File    : FastUnivariateDensityDerivative.cpp
// Purpose : Implementation for for FastUnivariateDensityDerivative
// Author  : Vikas C. Raykar (vikas@cs.umd.edu)
// Date    : September 17, 2005
//-------------------------------------------------------------
#include "FastUnivariateDensityDerivative.h"
#include <assert.h>
#include <math.h>

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bfsmem.h>
#include <tol/tol_brealfun.h>

#define  min(a,b) (((a)<(b))?(a):(b)) 
#define  max(a,b) (((a)>(b))?(a):(b)) 
#define  P_UL 500
#define  R 1.0

static double pi      = 3.14159265358979;
static double sqrt2pi = sqrt(2.0*pi);
static double sqrtpi  = sqrt(pi);
static double sqrt2   = sqrt(2.0);


//-------------------------------------------------------------------
void DoubleArray::Alloc(int size)
//-------------------------------------------------------------------
{
  if(size>size_)
  {
    Free();
    size_ = size;
    _bfsm_BaseSize__ = sizeof(double)*size_;

    buffer_ = (double*)BFixedSizeMemoryBase::
              Instance(_bfsm_BaseSize__)->New(_bfsm_BaseSize__,_bfsm_VectorPageNum__); 
//  buffer_ = new double[size_];
  }
  else
  {
    size_ = size;
  }
}

//-------------------------------------------------------------------
void DoubleArray::Free()
//-------------------------------------------------------------------
{
  if(buffer_)
  {
    BFixedSizeMemoryBase::Instance(_bfsm_BaseSize__)->Delete(buffer_,_bfsm_VectorPageNum__);
  //delete buffer_;
    buffer_ = NULL; 
    size_ = 0; 
  }
}

//-------------------------------------------------------------------
void IntArray::Alloc(int size)
//-------------------------------------------------------------------
{
  if(size>size_)
  {
    Free();
    size_ = size;
    _bfsm_BaseSize__ = sizeof(int)*size_;
    buffer_ = (int*)BFixedSizeMemoryBase::
              Instance(_bfsm_BaseSize__)->New(_bfsm_BaseSize__,_bfsm_VectorPageNum__); 
  //buffer_ = new int[size_];
  }
  else
  {
    size_ = size;
  }
}

//-------------------------------------------------------------------
void IntArray::Free()
//-------------------------------------------------------------------
{
  if(buffer_)
  {
    BFixedSizeMemoryBase::Instance(_bfsm_BaseSize__)->Delete(buffer_,_bfsm_VectorPageNum__);
  //delete buffer_;
    buffer_ = NULL; 
    size_ = 0; 
  }
}

//-------------------------------------------------------------------
// Constructor.
//
// PURPOSE                                                    
// -------   
// Initialize the class. 
// Read the parameters.
// Choose the parameter for the algorithm.
// Space subdivision.
// Compute the constant a.
// Compute B or all the clusters.
//
// PARAMETERS                                                      
// ----------
// NSources           --> number of sources, N.
// MTargets           --> number of targets, M.
// pSources           --> pointer to sources, px(N).
// pTargets           --> pointer to the targets, py(M).
// Order              --> order of the derivative, r.  
// epsilon            --> desired error, eps.
// pDensityDerivative --> pointer the the evaluated Density 
//-------------------------------------------------------------------


FastUnivariateDensityDerivative::
FastUnivariateDensityDerivative(int     NSources,
                                int     MTargets,
                                double *pSources,
                                double *pTargets,
                                int     Order,
                                double  epsilon,
                                double *pDensityDerivative)
{
  // Read the arguments.
  N=NSources;
  M=MTargets;
  px=pSources;
  r=Order;
  py=pTargets;
  pD=pDensityDerivative;
  eps=epsilon;
  pClusterIndex.Alloc(N);

//pClusterCenter=new double[maxK];
//l_constant=new double[((int)floor((double)r/2))+1];
//m_constant=new double[r+1];
//a_terms=new double[num_of_a_terms];
//B_terms=new double[num_of_B_terms];
//k_factorial=new double[p];
//temp3=new double[p+r];
  
}

//-------------------------------------------------------------------
// Destructor.
//-------------------------------------------------------------------

FastUnivariateDensityDerivative::~FastUnivariateDensityDerivative()
{
}




//-------------------------------------------------------------------
// Compute the factorial.
//-------------------------------------------------------------------

int
FastUnivariateDensityDerivative::factorial(int n){
  
  int fact=1;

  for ( int i = 1; i <= n; i++){
    fact=fact*i;
  }

  return fact;
}

//-------------------------------------------------------------------
// Choose the parameters
// 1. rx --> interval length.
// 2. K  --> number of intervals.
// 3. rr --> cutoff radius.
// 4. ry --> cluster cutoff radius.
// 5. p  --> truncation number.
//-------------------------------------------------------------------

void
FastUnivariateDensityDerivative::choose_parameters(){

  // 1. rx --> interval length.

  rx=h/2;

  // 2. K  --> number of intervals.

  K=(int)ceil(1.0/rx);
  rx=1.0/K;
  double rx_square=rx*rx;

  // 3. rr --> cutoff radius.

    //double r_term=pow(2.0,r/2)*sqrt((double)factorial(r));
  double r_term=sqrt((double)factorial(r));

  rr=min(R,2*h*sqrt(log(r_term/eps)));

  // 4. ry --> cluster cutoff radius.
  ry=rx+rr;

  // 5. p  --> truncation number.

  p=0;
  double error=1;
  double temp=1;
  double comp_eps=eps/r_term;
    
  while((error > comp_eps) & (p <= P_UL)){
    p++;
    double b=min(((rx+sqrt((rx_square)+(8*p*h_square)))/2),ry);
    double c=rx-b;
    temp=temp*(((rx*b)/h_square)/p);
    error=temp*(exp(-(c*c)/2*two_h_square));      
  }  
  p=p+1;

  //printf("h=%f r=%d eps=%f K=%d rx=%f rr=%f ry=%f p=%d\n",h,r,eps,K,rx,rr,ry,p);

}


//-------------------------------------------------------------------
// Space subdivision
//-------------------------------------------------------------------

void
FastUnivariateDensityDerivative::space_sub_division(){

  // 1. Cluster Centers

  pClusterCenter.Alloc(K);
  for(int i=0; i<K; i++){
    pClusterCenter[i]=(i*rx)+(rx/2);
    //printf("%f\n",pClusterCenter[i]);
  }

  //2. Allocate each source to the corresponding interval

  for(int i=0; i<N; i++){
    pClusterIndex[i]=min((int)floor(px[i]/rx),K-1);
    //printf("x=%f Cluster=%d\n",px[i],pClusterIndex[i]);
  }

}

//-------------------------------------------------------------------
// Compute the contant term a_{lm}.
// l=0...floor(r/2)
// m=0...r-2l 
//-------------------------------------------------------------------

void
FastUnivariateDensityDerivative::compute_a(){

  double r_factorial=(double)factorial(r);
  //printf("%f \n",r_factorial);

    int L = ((int)floor((double)r/2));
  l_constant.Alloc(L+1);
  l_constant[0]=1;
  for(int l=1; l <= L; l++){
    l_constant[l]=l_constant[l-1]*(-1.0/(2*l));
    //printf("%f \n",l_constant[l]);
  }

  m_constant.Alloc(r+1);
  m_constant[0]=1;
  for(int m=1; m <= r; m++){
    m_constant[m]=m_constant[m-1]*(-1.0/m);
    //printf("%f \n",m_constant[m]);
  }

  num_of_a_terms=0;
	for(int l=0; l <= (int)floor((double)r/2); l++){
		for(int m=0; m <= r-(2*l); m++){			
			num_of_a_terms++;
		}
	}

  //printf("r=%d num_of_a_terms=%d\n",r,num_of_a_terms);

  a_terms.Alloc(num_of_a_terms);
  int k=0;
  for(int l=0; l <= L; l++){
    for(int m=0; m <= r-(2*l); m++){
      a_terms[k]=(l_constant[l]*m_constant[m]*r_factorial)/((double)factorial(r-(2*l)-m));
      //printf("%f \n",a_terms[k]);
      k++;      
    }
  }
}

//-------------------------------------------------------------------
// Compute the contant term B^{n}_{km} for all the clusters.
// n=0...K-1
// k=0...p-1
// m=0...r 
//-------------------------------------------------------------------

void
FastUnivariateDensityDerivative::compute_B(){

  num_of_B_terms=K*p*(r+1);

  //printf("K=%d p=%d r=%d num_of_B_terms=%d\n",K,p,r,num_of_B_terms);

  B_terms.Alloc(num_of_B_terms);

  k_factorial.Alloc(p);

  k_factorial[0]=1;
  for(int i=1; i<p ;i++){
    k_factorial[i]=k_factorial[i-1]/i;
    //printf("%f \n",k_factorial[i]);
  }

  temp3.Alloc(p+r);

  for(int n=0; n<K; n++){
    //printf("Cluster %d ",n);
    for(int k=0; k<p; k++){
      for(int m=0; m< r+1; m++){
        B_terms[(n*p*(r+1))+((r+1)*k)+m]=0.0;;
        //printf("%f ",B_terms[(n*p*(r+1))+((r+1)*k)+m]);
      }
    }
    //printf("\n");
  }

  for(int i=0; i<N; i++){
    int cluster_number=pClusterIndex[i];
    double temp1=(px[i]-pClusterCenter[cluster_number])/h;
    double temp2=exp(-temp1*temp1/2);
    temp3[0]=1;
    for(int k=1; k<p+r; k++){
      temp3[k]=temp3[k-1]*temp1;
    }

    for(int k=0; k<p; k++){
      for(int m=0; m< r+1; m++){
        int mk =(cluster_number*p*(r+1))+((r+1)*k)+m;
        B_terms[mk]+=(temp2*temp3[k+m]);
      }
    }
  }

  for(int n=0; n<K; n++){
    //printf("Cluster %d ",n);
    for(int k=0; k<p; k++){
      for(int m=0; m< r+1; m++){
        B_terms[(n*p*(r+1))+((r+1)*k)+m]*=(k_factorial[k]*q);
        //printf("%f ",B_terms[(n*p*(r+1))+((r+1)*k)+m]);
      }
    }
    //printf("\n");
  }
}


//-------------------------------------------------------------------
// Actual function to evaluate the Univariate Density Derivative.
//-------------------------------------------------------------------

void
FastUnivariateDensityDerivative::Evaluate(double Bandwith)
{
  h =  Bandwith;
  h_square=h*h;
  two_h_square=2*h_square;

  q=(pow(-1.0,r))/(sqrt2pi*N*(pow(h,(r+1))));
  //printf("q=%f \n",q);

  // Choose the parameters for the algorithm.

  choose_parameters();

  // Space sub-division

  space_sub_division();

  // Compute the constant a

  compute_a();

  // Compute the constant B

  compute_B();

  //int num_of_influential_neighbors=(int)ceil(ry/rx);
  //printf("Num of influential right or left neighbors = %d\n",num_of_influential_neighbors);

  temp4.Alloc(p+r);


  for(int j=0; j<M; j++){
    pD[j]=0.0;
    
    int target_cluster_number=min((int)floor(py[j]/rx),K-1);
    double temp1=py[j]-pClusterCenter[target_cluster_number];
    double dist=fabs(temp1);
    while (dist <= ry && target_cluster_number <K && target_cluster_number >=0){

      //printf("j=%d y=%f Influential cluster=%d\n",j,py[j],target_cluster_number);
      //Do something
      double temp2=exp(-temp1*temp1/two_h_square);
      double temp1h=temp1/h;
      temp4[0]=1;
      for(int i=1; i<p+r; i++){
        temp4[i]=temp4[i-1]*temp1h;
      }

      for(int k=0; k<=p-1; k++){
        int dummy=0;
        for(int l=0; l <= (int)floor((double)r/2); l++){
          for(int m=0; m <= r-(2*l); m++){
            int mk = (target_cluster_number*p*(r+1))+((r+1)*k)+m;
            double& aux1 = a_terms[dummy];
            double& aux2 = B_terms[mk];
            double& aux3 = temp4[k+r-(2*l)-m];
            pD[j]+=(aux1*aux2*temp2*aux3);
            dummy++;
          }
        }
      }
      //
        
  
      target_cluster_number++;
      temp1=py[j]-pClusterCenter[target_cluster_number];
      dist=fabs(temp1);
    }

    target_cluster_number=min((int)floor(py[j]/rx),K-1)-1;
    if (target_cluster_number >=0){
      double temp1=py[j]-pClusterCenter[target_cluster_number];
      double dist=fabs(temp1);
      while (dist <= ry && target_cluster_number <K && target_cluster_number >=0){
        //printf("j=%d y=%f Influential cluster=%d\n",j,py[j],target_cluster_number);
        //Do something
        double temp2=exp(-temp1*temp1/two_h_square);
          double temp1h=temp1/h;
        temp4[0]=1;
        for(int i=1; i<p+r; i++){
          temp4[i]=temp4[i-1]*temp1h;
        }

        for(int k=0; k<=p-1; k++){
          int dummy=0;
          for(int l=0; l <= (int)floor((double)r/2); l++){
            for(int m=0; m <= r-(2*l); m++){
              pD[j]=pD[j]+(a_terms[dummy]*B_terms[(target_cluster_number*p*(r+1))+((r+1)*k)+m]*temp2*temp4[k+r-(2*l)-m]);
              dummy++;
            }
          }
        }
        //
        target_cluster_number--;
        temp1=py[j]-pClusterCenter[target_cluster_number];
        dist=fabs(temp1);
      }
    }

  }
}


//-------------------------------------------------------------------
void FastUnivariateDensityDerivative::Evaluate
(
  int    NSources,
  int    MTargets,
  double *pSources,
  double *pTargets,
  double Bandwidth,
  int    Order,
  double epsilon,
  double *pDensityDerivative
)
//-------------------------------------------------------------------
{
  FastUnivariateDensityDerivative pUDD
  (
    NSources,
    MTargets,
    pSources,
    pTargets,
    Order,
    epsilon,
    pDensityDerivative
  );
  pUDD.Evaluate(Bandwidth);
}

//-------------------------------------------------------------------
void FastUnivariateDensityDerivative::EvaluateWithOptimalBandwith
(
  int    NSources,
  int    MTargets,
  double *pSources,
  double *pTargets,
  int    Order,
  int    numIter,
  double epsilon,
  double *pDensityDerivative
)
//-------------------------------------------------------------------
{
  FastUnivariateBandwidthEstimate fube(NSources, pSources, numIter, epsilon);
  Evaluate
  (
    NSources,
    MTargets,
    pSources,
    pTargets,
    fube.h,
    Order,
    epsilon,
    pDensityDerivative
  );
}

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
  FastUnivariateBandwidthEstimate::
  FastUnivariateBandwidthEstimate(int N_, double* X_, int numIter, double epsil_)
//-------------------------------------------------------------------
: N(N_), X(X_), epsil(epsil_)
{
//Std(BText("\nTRACE FastUnivariateBandwidthEstimate 1"));
  int i = 0; 
// Reserve RAM for arrays
  D4.Alloc(N);
  D6.Alloc(N);

//Std(BText("\nTRACE FastUnivariateBandwidthEstimate 2"));
  double nu1=0, nu2=0;
  for(i=0; i<N; i++) 
  { 
    nu1 += X[i];
    nu2 += X[i]*X[i];
  }
  nu1/=N;
  nu2/=N;
  // Compute an estimate of the standard deriviation of the data
  sigma = sqrt(nu2-nu1*nu1);

  // Estimate the density functionals ${\Phi}_6$ and ${\Phi}_8$ using the normal scale rule.
  phi6=(-15.0/(16.0*sqrtpi))*pow(sigma,-7.0);
  phi8=(105.0/(32.0*sqrtpi))*pow(sigma,-9.0);
//Std(BText("\nTRACE FastUnivariateBandwidthEstimate 3"));

  // Estimate the density functionals ${\Phi}_4$ and ${\Phi}_6$ using the kernel density
  // estimators with the optimal bandwidth based on the asymptotic MSE.
  g1=pow(-6.0/(sqrt2pi*phi6*N),1.0/7.0);
  g2=pow(30.0/(sqrt2pi*phi8*N),1.0/9.0);

  FastUnivariateDensityDerivative::Evaluate(N,N,X,X,g1,4,epsil,D4.buffer_);
  phi4=0.0;
  for(i=0; i<N; i++) { phi4 += D4[i]; }
  phi4 /= N-1;

//Std(BText("\nTRACE FastUnivariateBandwidthEstimate 4"));
  FastUnivariateDensityDerivative::Evaluate(N,N,X,X,g2,6,epsil,D6.buffer_);
  phi6=0.0;
  for(i=0; i<N; i++) { phi6 += D6[i]; }
  phi6 /= N-1;

  constant1=pow(1.0/(2*sqrtpi*N),    1.0/5.0);
  constant2=pow(-6.0*sqrt2*phi4/phi6,1.0/7.0);

//Std(BText("\nTRACE FastUnivariateBandwidthEstimate 5"));
  D4_.Alloc(N);
  fudd = new FastUnivariateDensityDerivative(N,N,X,X,4,epsil,D4_.buffer_);

  BDat h_initial = constant1*pow(phi4,-1/5);
  BDat f_initial = AMISE_equation(h_initial.Value());
  h = h_initial.Value();
//Std(BText("\nTRACE FastUnivariateBandwidthEstimate 6"));
  
  //-------------------------------------------------------------------
    class BEval_AMISE_equation : public BRRFunction
  //-------------------------------------------------------------------
  {
   public:
    FastUnivariateBandwidthEstimate& handler_;
    BEval_AMISE_equation(FastUnivariateBandwidthEstimate& handler)
    : BRRFunction(),
      handler_(handler) 
    {}
    void Evaluate(BDat& r, const BDat& d)
    {
      r = handler_.AMISE_equation(d.Value());
    }
  } eval_AMISE_equation(*this);
  if(numIter)
  {
    h = fabs(eval_AMISE_equation.NewtonSolve(h_initial, f_initial, epsil, numIter).Value());
  }
  else
  {
    h = h_initial.Value();
  }
  Std(BText("\nFastUnivariateBandwidthEstimate(")+N_+","+numIter+","+epsil_+")="+h+"\n");
  double f = AMISE_equation(h);
}

//-------------------------------------------------------------------
  FastUnivariateBandwidthEstimate::
 ~FastUnivariateBandwidthEstimate()
//-------------------------------------------------------------------
{
  delete fudd;
}

//-------------------------------------------------------------------
  double FastUnivariateBandwidthEstimate::
  AMISE_equation(double h_)

// The bandwidth is the solution to the following equation.
//   AMISE_equation(h)==0
//-------------------------------------------------------------------
{
  int i;
  h_ = fabs(h_);
  double lambda=constant2*pow(h_,5.0/7.0);
  fudd->Evaluate(lambda);
  double phi4_=0.0;
  for(i=0; i<N; i++) { phi4_ += D4_[i]; }
  phi4_ /= N-1;
  double dif = h_ - constant1*pow(phi4_,-1.0/5.0);
  return(dif);
}


//-------------------------------------------------------------------
  double FastUnivariateBandwidthEstimate::
  AMISE_bisection(double a, double b, double fa, double fb)
//-------------------------------------------------------------------
{
  assert((fa<0)&&(fb>0));
  double c  = (b+a)/2;
  if((c-a<epsil)||(b-c<epsil)) { return(c); }
  double fc = AMISE_equation(c);
       if(fc==0) { return(c); }
  else if(fc<0)  { return(AMISE_bisection(c,b,fc,fb)); }
  else           { return(AMISE_bisection(a,c,fa,fc)); }
}




