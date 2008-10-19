/* @(#)rtnorm.c
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include "gsl_ext.h"

#ifdef _MSC_VER
#define finite _finite
#define __PRETTY_FUNCTION__ ""
#endif

#define rnorm(r, m, s) (m + gsl_ran_gaussian(r,s))

#define runif(r) gsl_rng_uniform_pos(r)

#define pnorm2(x) gsl_cdf_gaussian_P(x, 1.0)

#define qnorm1(p) gsl_cdf_gaussian_Pinv(p, 1.0)

double gsl_rtnorm(const gsl_rng * r, double m, double sd,
              double below, double above)
{  
  double FA = 0.0;
  double FB = 0.0;
  double term, draw;

  if (below > above) {
    GSL_ERROR("Truncation bound not logically consistent", GSL_EINVAL);
  }
		
  /* double sd = ::sqrt(v); */
  if ((fabs((above-m)/sd) < 8.2) && (fabs((below-m)/sd) < 8.2)){
    FA = pnorm2((above-m)/sd); /*, true, false);*/
    FB = pnorm2((below-m)/sd); /*, true, false);*/
  }
  if ((((above-m)/sd) < 8.2)  && (((below-m)/sd) <= -8.2) ){ 
    FA = pnorm2((above-m)/sd); /*, true, false);*/
    FB = 0.0;
  }
  if ( (((above-m)/sd) >= 8.2)  && (((below-m)/sd) > -8.2) ){ 
    FA = 1.0;
    FB = pnorm2((below-m)/sd); /*, true, false);*/
  } 
  if ( (((above-m)/sd) >= 8.2) && (((below-m)/sd) <= -8.2)){
    FA = 1.0;
    FB = 0.0;
  }
  term = runif(r)*(FA-FB)+FB;
  if (term < 5.6e-17)
    term = 5.6e-17;
  if (term > (1 - 5.6e-17))
    term = 1 - 5.6e-17;
  draw = m + sd * qnorm1(term);
  if (draw > above)
    draw = above;
  if (draw < below)
    draw = below;
  
  return draw;
}

double gsl_rtnorm_combo(const gsl_rng * r, double m, double s,
             double below, double above)
{
  const double eps = 1.0e-3;
  double FA, FB, x, term;

  if (below > above) {
    if (below-above > eps)
      GSL_ERROR("Truncation bound not logically consistent", GSL_EINVAL);
    return above;
  }
  /* double s = sqrt(v); */
  
  if (( ((above-m)/s > 0.5) && ((m-below)/s > 0.5)) ||
      ( ((above-m)/s > 2.0) && ((below-m)/s < 0.25)) ||
      ( ((m-below)/s > 2.0) && ((above-m)/s > -0.25)) ){ 
    x = rnorm(r, m, s);
    while ((x > above) || (x < below))
      x = rnorm(r, m,s);
    return x;
  } else {
    /* use the inverse cdf method */
    FA = 0.0;
    FB = 0.0;
    if ((fabs((above-m)/s) < 8.2) && (fabs((below-m)/s) < 8.2)){
      FA = pnorm2((above-m)/s); /*, true, false);*/
      FB = pnorm2((below-m)/s); /*, true, false);*/
    }
    if ((((above-m)/s) < 8.2)  && (((below-m)/s) <= -8.2) ){ 
      FA = pnorm2((above-m)/s); /*, true, false);*/
      FB = 0.0;
    }
    if ( (((above-m)/s) >= 8.2)  && (((below-m)/s) > -8.2) ){ 
      FA = 1.0;
      FB = pnorm2((below-m)/s); /*, true, false);*/
    } 
    if ( (((above-m)/s) >= 8.2) && (((below-m)/s) <= -8.2)){
      FA = 1.0;
      FB = 0.0;
    }
    term = runif(r)*(FA-FB)+FB;
    
    if (term < 5.6e-17)
      term = 5.6e-17;
    if (term > (1 - 5.6e-17))
      term = 1 - 5.6e-17;
    x = m + s * qnorm1(term);
    if (x > above)
      x = above;
    if (x < below)
      x = below;
    return x;
  }
}

double gsl_rtbnorm_combo (const gsl_rng * r, double m, double s,
                      double below, int iter)
{
  double x, above, z, v;
  int i;

  if (s <= 0){
    GSL_ERROR("Sigma non-positive", GSL_EINVAL);
  }
  
  /* double s = ::sqrt(v); */
  /* do rejection sampling and return value */
  /*if (m >= below){*/
  if ((m/s - below/s ) > -0.5){
    x = rnorm(r, m, s);
    while (x < below)
      x = rnorm(r, m,s);
    return x; 
  } else if ((m/s - below/s ) > -5.0 ){
    /*use the inverse cdf method*/
    above = (m+30.0)*s;
    x = gsl_rtnorm(r, m, s, below, above);
    return x;
  } else {
    /* do slice sampling and return value*/
    z = 0;
    x = below + .00001;
    v = s*s;
    
    for (i=0; i<iter; ++i){
      z = runif(r)*exp(-1*pow((x-m),2)/(2*v));
      x = runif(r)*( (m + sqrt(-2*v*log(z))) - below) + below;
    }
    if (! finite(x)) {
      fprintf(stderr, "WARNING in %s, %s, %d: "
              "Mean extremely far from truncation point. "
              "Returning truncation point\n", __FILE__, __PRETTY_FUNCTION__,
              __LINE__);
      return below; 
    }
    return x;
  }
}

double gsl_rtanorm_combo (const gsl_rng * r, double m, double s,
                      double above, int iter)
{
  double x, below, z, newmu, v;
  int i;

  if (s <= 0){
    GSL_ERROR("Sigma non-positive", GSL_EINVAL);
  }
  /* double s = ::sqrt(v); */
  /* do rejection sampling and return value*/
  if ((m/s - above/s ) < 0.5){
    x = rnorm(r, m, s);
    while (x > above)
      x = rnorm(r, m,s);
    return x;
  } else if ((m/s - above/s ) < 5.0 ){
    /* use the inverse cdf method*/
    below = (m-30.0)*s;
    x = gsl_rtnorm(r, m, s, below, above);
    return x;
  } else {
    /* do slice sampling and return value*/
    below = -1*above;
    newmu = -1*m;
    z = 0;
    x = below + .00001;
    v = s*s;
    
    for (i=0; i<iter; ++i){
      z = runif(r)*exp(-1*pow((x-newmu),2)/(2*v));
      x = runif(r)*( (newmu + sqrt(-2*v*log(z))) - below) + below;
    }
    if (! finite(x)) {
      fprintf(stderr, "WARNING in %s, %s, %d: "
              "Mean extremely far from truncation point. "
              "Returning truncation point\n", __FILE__, __PRETTY_FUNCTION__,
              __LINE__);
      return above;
    }
    return -1*x;
  }
}

double gsl_rtabnorm_combo(const gsl_rng * r, double m, double s,
             double below, double above, int iter)
{
  if(below>above)
  {
    GSL_ERROR("Truncation bound not logically consistent", GSL_EINVAL);
    return(GSL_NAN);
  }
  else if(below==above)
  {
    return(below);
  }
  else
  {
    int hasBelow = gsl_finite(below);
    int hasAbove = gsl_finite(above);
    if(!hasBelow && !hasAbove)
    {
      return(m + gsl_ran_gaussian(r, s));
    }
    else if(!hasBelow && hasAbove)
    {
      return(gsl_rtanorm_combo(r, m, s, above, iter));
    }
    else if(hasBelow && !hasAbove)
    {
      return(gsl_rtbnorm_combo(r, m, s, below, iter));
    }
    else//if(hasBelow && hasAbove)
    {
      return(gsl_rtnorm_combo(r, m, s, below, above));
    }
  }
}
