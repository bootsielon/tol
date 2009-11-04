################################################################################
## FILE: _tolcoda.R
## PURPOSE: API between TOL and R for CODA functions
## AUTHOR: josp@tol-project.org
## ACKNOWLEDGEMENTS:
##
##  R Development Core Team (2007). R: A language and environment for 
##  statistical computing. R Foundation for Statistical Computing, "
##  Vienna, Austria. ISBN 3-900051-07-0, \n"
##  URL http://www.R-project.org.\n"};
##
##  Martyn Plummer, Nicky Best, Kate Cowles and Karen Vines (2006). coda:
##  Output analysis and diagnostics for MCMC. R package version 0.10-7.
##  URL http://www-fis.iarc.fr/coda/
##
################################################################################
require(coda)

################################################################################
read.bbm <-
################################################################################
function(fbbm) {
  fd = file(fbbm, open='rb')
  dim <- readBin(fd,integer(), n=2)
  data<-readBin(fd,double(),n=dim[1]*dim[2])
  close(fd)
  matrix(data, nrow=dim[1], ncol=dim[2], byrow=TRUE)
}

################################################################################
write.bbm <-
################################################################################
function(fbbm, mat) {
  fd = file(fbbm, open='wb')
  if (is.matrix(mat)) {
    writeBin(dim(mat), fd)
    writeBin(as.numeric(t(mat)), fd)
  } else {
    writeBin(c(0,0), fd)
  }
  close(fd)
}

################################################################################
get.mcmc.bbm <-
################################################################################
function(fin, verbose=FALSE, varByCol=TRUE, title) {
  chain <- if (varByCol)
    read.bbm(fin)
  else
    t(read.bbm(fin))
  if(verbose)
  {
    print(paste("[",title,"]Loading Markov Chain with",
                dim(chain)[2], " variables and",
                dim(chain)[1], " simulations."), 
          quote=FALSE) 
  }
  chain
}

################################################################################
raftery.bbm <-
################################################################################
function(fin, fout, verbose=FALSE, varByCol=TRUE, 
         q=0.025, r=0.005, s=0.95, eps=0.001) {
  mcmc_item <- get.mcmc.bbm(fin,verbose,varByCol,"raftery.diag")
  result <- raftery.diag(mcmc_item,q,r,s,eps)
  if(verbose)
  {
    print(result)
  }
  write.bbm(fout, result$resmatrix)
}


################################################################################
gelman.bbm <-
################################################################################
function(mcmc_fin, fout, verbose=FALSE, varByCol=TRUE, 
         confidence=0.95, transform=FALSE, autoburnin=TRUE) {
  mcmc_items <- list()
  for(i in 1:length(mcmc_fin)) {
    X <- mcmc(get.mcmc.bbm(mcmc_fin[i],verbose,varByCol,"gelman.diag"))
    mcmc_items[[i]] <- X
  }
  mcmc_list = mcmc.list(mcmc_items);
  result <- gelman.diag(mcmc_list,confidence,transform,autoburnin)
  if(verbose)
  {
    print(result)
  }
  mpsrf <- matrix(result$mpsrf,1,1)
  write.bbm(paste(fout,"_psrf", sep=""), result$psrf )
  write.bbm(paste(fout,"_mpsrf",sep=""), mpsrf)
}

################################################################################
### RGA This is a wrapper for spectrum0 which returns NA if
### spectrum0 crashes.  This has happened to me several times when
### there was bug in my MCMC algorithm.
"safespec0" <-
################################################################################
function (x) {
  result <- try(spectrum0(x)$spec)
  ## R
  if (class(result) == "try-error") result <- NA
  ## S-Plus
  if (class(result) == "try") result <- NA
  result
}

################################################################################
summary.mcmc.bbm <-
################################################################################
function(fin.bbm, fstat.bbm, fquant.bbm, verbose=FALSE, varByCol=TRUE, 
         quantiles=c(0.025, 0.25, 0.5, 0.75, 0.975)) {
  mcmc <- get.mcmc.bbm(fin.bbm,verbose,varByCol,"summary.mcmc")

  x <- as.mcmc(mcmc)
  statnames <- c("Mean", "SD", "Naive SE", "Time-series SE")
  varstats  <- matrix(nrow = nvar(x), ncol = length(statnames), 
                      dimnames = list(varnames(x), statnames))

  xmean    <- apply(x, 2, mean)
  xvar     <- apply(x, 2, var)
  xtsvar   <- apply(x, 2, safespec0)
  varquant <- t(apply(x, 2, quantile, quantiles))

  varstats[, 1] <- xmean
  varstats[, 2] <- sqrt(xvar)
  varstats[, 3] <- sqrt(xvar/niter(x))
  varstats[, 4] <- sqrt(xtsvar/niter(x))

  if(verbose)
  {
    print(xmean)
  }

  write.bbm(fstat.bbm,varstats)
  write.bbm(fquant.bbm,varquant)
}
