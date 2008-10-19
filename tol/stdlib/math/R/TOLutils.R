################################################################################
## FILE: TOLutils.R
## PURPOSE: Communication API between TOL and R 
## AUTHOR: josp@tol-project.org
##
## ACKNOWLEDGEMENTS:
##
##  R Development Core Team (2007). R: A language and environment for 
##  statistical computing. R Foundation for Statistical Computing, "
##  Vienna, Austria. ISBN 3-900051-07-0, \n"
##  URL http://www.R-project.org.\n"};
##
################################################################################

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
  } else if (is.vector(mat)) {
    Z <- matrix(mat,length(mat), 1)
    writeBin(dim(Z), fd)
    writeBin(as.numeric(t(Z)), fd)
  } else {
    writeBin(c(0,0), fd)
  }
  close(fd)
}

################################################################################
get.tol.arg <-
################################################################################
function(value, def)
{
  if (is.null(value))
    def
  else
    value
}
