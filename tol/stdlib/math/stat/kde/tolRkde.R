################################################################################
## FILE: _tolkde.R
## PURPOSE: API between TOL and R KDE (Kernel Density Estimation)
## AUTHOR: vdebuen@tol-project.org
##
## ACKNOWLEDGEMENTS:
##
##  R Development Core Team (2007). R: A language and environment for 
##  statistical computing. R Foundation for Statistical Computing, "
##  Vienna, Austria. ISBN 3-900051-07-0, \n"
##  URL http://www.R-project.org.\n"};
##
## S original by Berwin A. Turlach R port by Andreas Weingessel. Rquadprog:
## Functions to solve Quadratic Programming Problems
## URL http://cran.r-project.org/src/contrib/Descriptions/quadprog.html
##
################################################################################
require(stats)

Rkde_density.TOL <-
function(sample.bbm, args.R)
{
  sample  <- read.bbm(sample.bbm)

  # Eval non-binary arguments
  source(args.R, local=TRUE)

  # Extract optional arguments
  verbose    <- get.tol.arg(Rkde_args$verbose, FALSE)
  n          <- get.tol.arg(Rkde_args$n,       512)
  if(verbose)  { print(Rkde_args) }
  
  dens <- density(sample)
  kdeX <- dens$x
  kdeY <- dens$y
  if(verbose)  
  { 
    print(dens);
    print(kdeX);
    print(kdeY);
  }
  # Write the result
  write.bbm(Rkde_args$X.bbm, kdeX)
  write.bbm(Rkde_args$Y.bbm, kdeY)
}
