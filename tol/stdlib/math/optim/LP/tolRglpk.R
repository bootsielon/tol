################################################################################
## FILE: _tolRglpk.R
## PURPOSE: API between TOL and R for Rglpk functions
## AUTHOR: josp@tol-project.org
##
## ACKNOWLEDGEMENTS:
##
##  R Development Core Team (2007). R: A language and environment for 
##  statistical computing. R Foundation for Statistical Computing, "
##  Vienna, Austria. ISBN 3-900051-07-0, \n"
##  URL http://www.R-project.org.\n"};
##
##  Kurt Hornik and Stefan Theussl. Rglpk: R interface to the GNU Linear 
##  Programing Kit (GLPK version 4.25). GLPK is open source software for 
##  solving large-scale linear programming (LP), mixed integer linear
##  programming (MILP) and other related problems.
##  URL 	http://www.gnu.org/software/glpk/
##
################################################################################
require(Rglpk)

Rglpk_solve_LP.TOL <-
function(objmat.bbm, rhs.bbm, args.R)
{
  objmat <- read.bbm(objmat.bbm)
  obj <- as.numeric(objmat[1,])
  mat <- objmat[2:dim(objmat)[1],]
  rhs <- as.numeric(read.bbm(rhs.bbm))

  # Eval non-binary arguments
  source(args.R, local=TRUE)

  dir     <- get.tol.arg(Rglpk_args$dir,     rep("<=", dim(mat)[1]))
  # Extract optional arguments
  verbose <- get.tol.arg(Rglpk_args$verbose, FALSE)
  max     <- get.tol.arg(Rglpk_args$max,     FALSE)
  lower.bbm  <- get.tol.arg(Rglpk_args$lower,  NULL)
  upper.bbm  <- get.tol.arg(Rglpk_args$upper,  NULL)
  if (is.null(lower.bbm)) {
    bounds <- NULL
  } else {
    mlower <- read.bbm(lower.bbm)
    bounds <- list(lower=list(ind=as.integer(mlower[,1]), val=mlower[,2]))
  }                    
  if (!is.null(upper.bbm)) {
    mupper <- read.bbm(upper.bbm)
    bounds <- c(list(upper=list(ind=as.integer(mupper[,1]), val=mupper[,2])),bounds)
  }
  print(bounds)
  res <- Rglpk_solve_LP(obj, mat, dir, rhs, Rglpk_args$int, max=max,
                        bounds=bounds, verbose=verbose)
  matres <- matrix(c(res$status,res$optimum,res$solution,recursive=TRUE),nrow=1)

  # Write the result
  write.bbm(Rglpk_args$result.bbm, matres)
}
