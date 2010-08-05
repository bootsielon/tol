/* FUNCTIONS FOR PERFORMING UNIVARIATE SLICE SAMPLING.

  Implements, with slight modifications and extensions, the algorithm
  described in Figures 3 and 5 of the following paper:
 
  Neal, R. M (2003) "Slice sampling" (with discussion), Annals of Statistics,
  vol. 31, no. 3, pp. 705-767.

 See the documentation for the function uni.slice below for how to use it.


 GLOBAL VARIABLES FOR RECORDING PERFORMANCE.

 uni.slice.calls <- 0	# Number of calls of the slice sampling function
 uni.slice.evals <- 0	# Number of density evaluations done in these calls
*/

#include <tol/tol_bprdist_internal.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>

static int uni_slice_calls = 0;
static int uni_slice_evals = 0;

/*
  UNIVARIATE SLICE SAMPLING WITH STEPPING OUT AND SHRINKAGE.

  Performs a slice sampling update from an initial point to a new point that 
  leaves invariant the distribution with the specified log density function.

  Arguments:

    x0    Initial point
    g     Function returning the log of the probability density (plus constant)
    w     Size of the steps for creating interval (default 1)
    m     Limit on steps (default infinite)
    lower Lower bound on support of the distribution (default -Inf)
    upper Upper bound on support of the distribution (default +Inf)
    gx0   Value of g(x0), if known (default is not known)

  The log density function may return -Inf for points outside the support 
  of the distribution.  If a lower and/or upper bound is specified for the
  support, the log density function will not be called outside such limits.
 
  The value of this function is the new point sampled, the argument
  gx0 receive on return the value of the log density function, g, at
  this point.  Depending on the context, this log density might be
  passed as the gx0 argument of a future call of uni.slice.

  The global variable uni_slice_calls is incremented by one for each
  call of slice_sampler_1D. The global variable uni_slice_evals is
  incremented by the number of calls made to the g function passed.

  WARNING:  If you provide a value for g(x0), it must of course be correct!
  In addition to giving wrong answers, wrong values for gx0 may result in
  the slice_sampler_1D function going into an infinite loop.
*/

#define _SS_ZERO_ 1.0e-10
#define _SS_MAXITER_ 5000

double slice_sampler_1D( const void /*gsl_rng*/ *_rng,
                         /* initial point */
                         double x0,
                         /* log of the probability density (plus constant) */
                         double (*g)(double,void*),
                         /* client data for g */
                         void *gdata,
                         /* gx0 = g(x0,gdata) */
                         double *gx0,
                         /* step's size for creating interval (default 1) */
                         double w,
                         /* Limit on steps (negative means infinite) */
                         int m,
                         /* lower bound on support of the distribution */
                         double lower,
                         /* upper bound on support of the distribution */   
                         double upper )
{
  gsl_rng *rng = getGslRng();
  double _gx0, _gx;

  ++uni_slice_calls;

  /* Find the log density at the initial point, if not already known. */

  if ( !gx0 ) {
    ++uni_slice_evals;
    _gx0 = ( *g )( x0, gdata );
  } else _gx0 = *gx0;

  if ( !gsl_finite( _gx0 ) ) {
    /* we can not start from undefined density value */
    return x0;
  }

  /* Determine the slice level, in log terms. */
  double logy = _gx0 - gsl_ran_exponential( rng, 1.0 );

  /* Find the initial interval to sample from. */

  double u = gsl_rng_uniform( rng )*w;
  double L = x0 - u;
  double R = x0 + ( w - u );  /*should guarantee that x0 is in [L,R], even with roundoff */

  if ( L > lower ) {
    _gx = ( *g )( L, gdata );
    if ( !gsl_finite( _gx ) ) {
      /* found undefined density value while expanding interval */
      return x0;
    }
  }
  if ( R < upper ) {
    _gx = ( *g )( R, gdata );
    if ( !gsl_finite( _gx ) ) {
      /* found  undefined density value while expanding interval */
      return x0;
    }
  }
  
  /*
     Expand the interval until its ends are outside the slice, or until
     the limit on steps is reached.
  */

  if ( m < 0 ) { /* no limit on number of steps */
    while( 1 ) {
      if ( L <= lower ) break;
      ++uni_slice_evals;
      _gx = ( *g )( L, gdata );
      if ( !gsl_finite( _gx ) ) {
        /* found  undefined density value while expanding interval */
        L += w;
        break;
      }
      if ( _gx <= logy ) break;
      L -= w;
    }

    while( 1 ) {
      if ( R >= upper ) break;
      ++uni_slice_evals;
      _gx = ( *g )( R, gdata );
      if ( !gsl_finite( _gx ) ) {
        /* found  undefined density value while expanding interval */
        R -= w;
        break;
      }
      if ( _gx <= logy ) break;
      R += w;
    }
  } else if ( m > 1 ) { /* limit on steps, bigger than one */
    size_t J = gsl_rng_uniform_int( rng, m );
    size_t K = ( m-1 ) - J;

    while ( J > 0 ) {
      if ( L <= lower ) break;
      ++uni_slice_evals;
      _gx = ( *g )( L, gdata );
      if ( !gsl_finite( _gx ) ) {
        /* found  undefined density value while expanding interval */
        L += w;
        break;
      }
      if ( _gx <= logy ) break;
      L -= w;
      --J;
    }

    while ( K > 0 ) {
      if ( R >= upper ) break;
      ++uni_slice_evals;
      _gx = ( *g )( R, gdata );
      if ( !gsl_finite( _gx ) ) {
        /* found  undefined density value while expanding interval */
        R -= w;
        break;
      }
      if ( _gx <= logy ) break;
      R += w;
      --K;
    }
  }

  /* Shrink interval to lower and upper bounds. */

  if ( L < lower ) {
    L = lower;
  }
  if ( R > upper ) {
    R = upper;
  }

  /* Sample from the interval, shrinking it on each rejection. */

  double x1 = x0, gx1 = _gx0;
  register unsigned int niter = 0;
  
  do {
    double W = R - L;
    if ( W < _SS_ZERO_ ) break;
    x1 = gsl_rng_uniform( rng ) * W + L;

    ++uni_slice_evals;
    gx1 = ( *g )( x1, gdata );
    if ( !gsl_finite( gx1 ) ) {
      /* found undefined density value while sampling from interval */
      x1 = x0;
      gx1 = _gx0;
      break;
    }

    if ( gx1 >= logy ) break;

    if ( x1 > x0 ) { R = x1; }
    else           { L = x1; }
  } while ( ++niter < _SS_MAXITER_ );

  /* Return the point sampled, with its log density attached */

  if ( gx0 ) *gx0 = gx1;
  return x1;
}

#ifdef _SLICE_SAMPLER_MAIN_

/* g++ -D_SLICE_SAMPLER_MAIN_ -O2 slice_sampler.cpp -lgsl -lgslcblas */

double lognorm(double x, void *data)
{
  return -x*x/2;
}

double dnorm(double x, double m, double s)
{
  return gsl_ran_gaussian_pdf(x-m, s);
}

int test_normal()
{
}

struct npar {
  double mean;
  double sigma;
};

double g_mix_normal(double x, void *params)
{
  npar *p1, *p2;

  p1 = (npar *)params;
  p2 = p1+1;
  double v1 = dnorm(x,p1->mean,p1->sigma);
  double v2 = dnorm(x,p2->mean,p2->sigma);
  return log(v1+v2);
}

int test_mix_normal(gsl_rng * rng)
{
  npar mix_params[] = {
    {-1.0, 0.7},
    {1.0, 0.5}
  };
  
  const size_t N1 = 10000;
  const size_t N2 = 2;
  double x0 = 3;

  printf("x<-c(");
  double s = x0;
  for (size_t i = 0; i < N1; i++) {
    
    for (size_t j = 0; j < N2; j++) {
      s = slice_sampler_1D(rng, s, &g_mix_normal, mix_params);
    }
    printf("%g", s);
    if (i<N1-1) printf(",");
  }
  printf(")");
  return 0;
}

int main( )
{
  gsl_rng * rng = gsl_rng_alloc( gsl_rng_default );

  test_mix_normal( rng );
  return 0;
}

#endif

/*
  TODO:

  - Comparar con gsl_fcmp

  - Verificar que cada evaluacion de la densidad de un valor
    finito. Decidir que hacer cuando de un valor no finito (infinito o
    NAN)
  
 */
