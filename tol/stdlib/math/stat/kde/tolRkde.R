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
## The (S3) generic function density computes kernel density estimates. Its 
## default method does so with the given kernel and bandwidth for univariate 
## observations. 
## URL http://stat.ethz.ch/R-manual/R-patched/library/stats/html/density.html
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
  verbose      <- get.tol.arg(Rkde_args$verbose,      FALSE)
  n_           <- get.tol.arg(Rkde_args$n,            512)
  adjust_      <- get.tol.arg(Rkde_args$ajust,        1)
  leftBounded  <- get.tol.arg(Rkde_args$leftBounded,  FALSE)
  from_        <- get.tol.arg(Rkde_args$from,         -1/0)
  rightBounded <- get.tol.arg(Rkde_args$rightBounded, FALSE)
  to_          <- get.tol.arg(Rkde_args$to,           +1/0)
  cut_         <- get.tol.arg(Rkde_args$cut,          3)
  if(verbose)  { print(Rkde_args) }
  if(leftBounded & rightBounded)
  {
    dens <- density(sample,bw = "nrd0", adjust = adjust_,
        kernel = c("gaussian", "epanechnikov", "rectangular",
                   "triangular", "biweight", "cosine", "optcosine"),
        window = kernel, 
        give.Rkern = FALSE,n=n_,from=from_,to=to_,cut=cut_)
  } 
  else if(!leftBounded & rightBounded)
  {
    dens <- density(sample,bw = "nrd0", adjust = adjust_,
        kernel = c("gaussian", "epanechnikov", "rectangular",
                   "triangular", "biweight", "cosine", "optcosine"),
        window = kernel, 
        give.Rkern = FALSE,n=n_,from=from_,to=to_,cut=cut_)
  }
  else if(leftBounded & !rightBounded)
  {
    dens <- density(sample,bw = "nrd0", adjust = adjust_,
        kernel = c("gaussian", "epanechnikov", "rectangular",
                   "triangular", "biweight", "cosine", "optcosine"),
        window = kernel, 
        give.Rkern = FALSE,n=n_,from=from_,cut=cut_)  
  }
  else if(!leftBounded & !rightBounded)
  {
    dens <- density(sample,bw = "nrd0", adjust = adjust_,
        kernel = c("gaussian", "epanechnikov", "rectangular",
                   "triangular", "biweight", "cosine", "optcosine"),
        window = kernel, 
        give.Rkern = FALSE,n=n_,cut=cut_)
  }
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
