################################################################################
## FILE: test_gelman.R
## PURPOSE: test for gelman.diag R function of package CODA
## AUTHOR: vdebuen@tol-project.org
## ACKNOWLEDGEMENTS:
##
##  Patrick Lam 
##  Department of Government, Harvard University
##  http://www.people.fas.harvard.edu/~plam/
##  
##  Is the author of a demo about using CODA 
##
## www.people.fas.harvard.edu/~plam/teaching/methods/convergence/convergence.pdf
##
################################################################################

require(coda)
require(mvtnorm)

NormalMHExample <- function(n.sim, n.burnin) {
 library(mvtnorm)
 mu.vector <- c(3, 1)
 variance.matrix <- cbind(c(1, -0.5), c(-0.5, 2))
 theta.mh <- matrix(NA, nrow = n.sim, ncol = 2)
 theta.current <- rnorm(n = 2, mean = 0, sd = 4)
 theta.update <- function(index, theta.current, ...) {
 theta.star <- rnorm(n = 1, mean = theta.current[index], sd = 2)
 if (index == 1)
 theta.temp <- c(theta.star, theta.current[2])
 else theta.temp <- c(theta.current[1], theta.star)
 r <- dmvnorm(theta.temp, mu.vector, variance.matrix)/dmvnorm(theta.current,
 mu.vector, variance.matrix)
 r <- min(r, 1, na.rm = T)
 if (runif(1) < r)
 theta.star
 else theta.current[index]
 }
 for (i in 1:n.sim) {
 theta.current[1] <- theta.mh[i, 1] <- theta.update(1, theta.current,
 mu.vector, variance.matrix)
 theta.current[2] <- theta.mh[i, 2] <- theta.update(2, theta.current,
 mu.vector, variance.matrix)
 }
 theta.mh <- theta.mh[(n.burnin + 1):n.sim, ]
 }

mh.draws1 <- mcmc(NormalMHExample(n.sim = 5000, n.burnin = 0))
mh.draws2 <- mcmc(NormalMHExample(n.sim = 5000, n.burnin = 0))
mh.draws3 <- mcmc(NormalMHExample(n.sim = 5000, n.burnin = 0))
mh.draws4 <- mcmc(NormalMHExample(n.sim = 5000, n.burnin = 0))
mh.draws5 <- mcmc(NormalMHExample(n.sim = 5000, n.burnin = 0))

mh.list <- mcmc.list(list(mh.draws1, mh.draws2, mh.draws3, mh.draws4, mh.draws5))
diag <- gelman.diag(mh.list,0.95,0,1)

gelman.plot(mh.list)