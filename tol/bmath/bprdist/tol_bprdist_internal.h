#include <gsl/gsl_rng.h>
#include <tol/tol_bprdist.h>

inline gsl_rng* getGslRng()
{
  return reinterpret_cast<gsl_rng*>(BProbDist::rng());
}
