################################################################################
## FILE: _tolRquadprog.R
## PURPOSE: API between TOL and R for quadprog functions
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
## URL http://cran.r-project.org/web/packages/quadprog/index.html
##
################################################################################
require(quadprog)

Rquadprog_solve_QP.TOL <-
function(D.bbm, d.bbm, A.bbm, b0.bbm, args.R)
{
  D  <- read.bbm(D.bbm)
  d  <- read.bbm(d.bbm)
  A  <- read.bbm(A.bbm)
  b0 <- read.bbm(b0.bbm)

  # Eval non-binary arguments
  source(args.R, local=TRUE)

  # Extract optional arguments
  verbose    <- get.tol.arg(Rquadprog_args$verbose,   FALSE)
  factorize  <- get.tol.arg(Rquadprog_args$factorize, FALSE)
  meq        <- get.tol.arg(Rquadprog_args$meq,       0)
  if(verbose)  { print(Rquadprog_args) }
  
  res <- solve.QP(D, d, A, b0, meq, factorize)
  matres <- res$solution
  if(verbose)  
  { 
    print(res);
    print(matres);
  }
  # Write the result
  write.bbm(Rquadprog_args$result.bbm, matres)
}
