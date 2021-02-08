library("spatstat")
library(e1071)
library(neldermead)
library(REdaS)
library(lme4)
library("optimsimplex")
library(gtools)

#We define here some functions useful in the estimation process via the Expectation Maximization algorithm 
#See paragraphs 4.2.1 and 5.1.2 of the thesis (Tesi.pdf)

#m = 3 is the fixed number of states of the Markov chain

#Given the number Tmc of intensities we want to generate, the number nbreaks to use in hist function (if we want to plot histograms) and the parameters of the model lambda, xi, tau, the Montecarlo function returns the MonteCarlo distribution of rain intensity given the chain is in state 1, 2 or 3. 

MonteCarlo <- function(Tmc, nbreaks, lambda, xi, tau)
{
  intens = matrix(0, m, Tmc)
  for (j in 1:m)
  {
    for(n in 1:Tmc)
    {
      N = rpois(1, lambda[j])
      for (i in 1:N)
      {
        x = runif(1, lx, hx)
        y = runif(1, ly, hy)
        R = rexp(1, xi[j])
        sigma = rexp(1, tau[j])
        D = disc(centre = c(x, y), radius = R)
        if(inside.owin(siteX, siteY, D) == TRUE)
        {
          intens[j, n] = intens[j, n] + sigma  
        }
      }
    }
    if(j == 1)
    {
      hh1 = hist(intens[1, ], plot = FALSE)#, breaks = nbreaks[1])
    }
    if(j == 2)
    {
      hh2 = hist(intens[2, ], plot = FALSE)#, breaks = nbreaks[2])
    }
    if(j == 3)
    {
      hh3 = hist(intens[3, ], plot = FALSE)#, breaks = nbreaks[3])
    }
  }
  ll = list("hh1" = hh1, "hh2" = hh2, "hh3" = hh3)
  return(ll)
}

#This is a simple function used to set to zero NA values in a Matrix A

subNA <- function(A)
{
  n = dim(A)[1]
  for (i in 1:n)
  {
    for (j in 1:n)
    {
      if(is.na(A[i, j]) == TRUE)
      {
        A[i, j] = 0
      }
    }
  }
  return(A)
}

#logforward and logbackward are functions that compute respectively the logarithm of the forward and the backward proability of a Hidden Markov chain given the initial distribution delta of the chain, the transition probability matrix Gamma and the state-dependent densities for each states of the chain dens1, dens2, dens3 

logforward <- function(T, delta, Gamma, dens1, dens2, dens3, rain)
{
  lalpha = matrix(0, T, m)
  P = subNA(diag(c(dens1(rain[1]), dens2(rain[1]), dens3(rain[1])), m, m))
  pp = c(P[1, 1], P[2, 2], P[3, 3])
  foo = delta*pp
  sumfoo = sum(foo)
  lscale = log(sumfoo)
  foo = foo/sumfoo
  lalpha[1, ] = lscale + log(foo)
  for(t in 2:T)
  {
    P = subNA(diag(c(dens1(rain[t]), dens2(rain[t]), dens3(rain[t])), m, m))
    pp = c(P[1, 1], P[2, 2], P[3, 3])
    foo = foo%*%Gamma*pp
    sumfoo = sum(foo)
    lscale = lscale + log(sumfoo)
    foo = foo/sumfoo
    lalpha[t, ] = log(foo) + lscale
  }
  return(lalpha)
}

logbackward <- function(T, delta, Gamma, dens1, dens2, dens3, rain)
{
  lbeta = matrix(0, T, m)
  lbeta[T, ] = rep(0, m)
  foo = rep(1/m, m)
  lscale = log(m)
  for(t in (T-1):1)
  {
    P = subNA(diag(c(dens1(rain[t+1]), dens2(rain[t+1]), dens3(rain[t+1])), m, m))
    pp = c(P[1, 1], P[2, 2], P[3, 3])
    for(j in 1:m)
    {
      foo = Gamma%*%(pp*foo)
      lbeta[t, ] = log(foo) + lscale
      sumfoo = sum(foo)
      foo = foo/sumfoo
      lscale = lscale + log(sumfoo)
    }
  }
  return(lbeta)
}


#This function performs the E step of the Expectation Maximization algorithm
 
Estep <- function(lalpha, lbeta, T, Gamma, delta, dens1, dens2, dens3, rain)
{
  ca = apply(lalpha, 1, max)
  cb = apply(lbeta, 1, max)
  alpha = matrix(0, T, 3)
  beta = matrix(0, T, 3)
  for(t in 1:T)
  {
    alpha[t, ] = exp(lalpha[t, ] - ca[t])
    beta[t, ] = exp(lbeta[t, ] - cb[t])
  }
  hatU = matrix(0, T, 3)
  hatV1 = matrix(0, T, 3)
  hatV2 = matrix(0, T, 3)
  hatV3 = matrix(0, T, 3)
  for(t in 1:T)
  {
    P = subNA(diag(c(dens1(rain[t]), dens2(rain[t]), dens3(rain[t])), m, m))
    for(j in 1:3)
    {
      hatU[t, j] = (alpha[t, j]*beta[t, j])/(alpha[t, ]%*%beta[t, ])
      if(t > 1)
      {
        hatV1[t, j] = (alpha[(t-1), j]*Gamma[j, 1]*P[1, 1]*beta[t, 1])/(alpha[t, ]%*%beta[t, ])
        hatV2[t, j] = (alpha[(t-1), j]*Gamma[j, 2]*P[2, 2]*beta[t, 2])/(alpha[t, ]%*%beta[t, ])
        hatV3[t, j] = (alpha[(t-1), j]*Gamma[j, 3]*P[3, 3]*beta[t, 3])/(alpha[t, ]%*%beta[t, ])
      }
    }
  }
  return(list("hatU" = hatU, "hatV1" = hatV1, "hatV2" = hatV2, "hatV3" = hatV3))
}

#thirdLik computes the logLikelihood given the vector of parameters parametri = (lambda1, lambda2, lambda3, xi1, xi2, xi3, tau1, tau2, tau3)
thirdLik <- function(parametri)
{
  lambda = parametri[1:3]
  xi = parametri[4:6]
  tau = parametri[7:9]
  ret = 0
  MC = MonteCarlo(Tmc, nbreaks, lambda, xi, tau)
  hh1 = MC$hh1
  hh2 = MC$hh2
  hh3 = MC$hh3
  dens1 = approxfun(hh1$mids, hh1$density, rule = 2:1)
  dens2 = approxfun(hh2$mids, hh2$density, rule = 2:1)
  dens3 = approxfun(hh3$mids, hh3$density, rule = 2:1)
  for(t in 1:T)
  {
    P = subNA(diag(c(dens1(rain[t]), dens2(rain[t]), dens3(rain[t])), m, m))
    for (j in 1:m)
    {
      if(P[j, j] != 0)
      {
        ret = ret+ hatU[t, j]*log(P[j, j])
      }
    }
  }
  return(-ret)
}



