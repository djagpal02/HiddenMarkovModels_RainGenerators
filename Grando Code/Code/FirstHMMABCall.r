library("spatstat")
#Here we use the Approximate Bayesian Computation to find an estimation of lambda, ta and xi for fixed value of Gamma and delta (§4.2.2 and 5.2)


#As in lkFunctMaxEm, we set tha data used 
coord = read.csv("CoordGermany.csv", header = TRUE)
tabPioggia = read.csv("GermanData.csv", header = TRUE)
matPioggia = as.matrix(tabPioggia)
days = dim(tabPioggia)[1] #total number of days considered
L = dim(tabPioggia)[2] #total number of sites 
missingData = vector(mode = "numeric", L)
#missing data in our file are indicated as "-9999", we set them as NA
for(i in 1:days)
{
  for(k in 1:L)
  {
    if(matPioggia[i, k] == -9999)
    {
      matPioggia[i, k] = NA
      missingData[k] = missingData[k]+1
    }
  }
}
missingDataPerc = (missingData/days)*100

#The region in which data are simulated is rectangular: [lx, ly]x[lx, hy]x[hx, hy]x[hx, ly]
l = sample(1:L, size = 1)
siteXtot = coord[1, 2:31]/1000
siteYtot = coord[2, 2:31]/1000
lx = min(siteXtot) - 200
hx = max(siteXtot) + 200
ly = min(siteYtot) - 200
hy = max(siteYtot) + 200

#This functions emulates our rain intensity process. Given the parameters (lambda, xi, tau) and for fixed delta and Gamma
#it gives the rain intensity for every site l = 1,..., L for a period of time T


HMMmodel <- function(lambda, xi, tau)
{
  states = vector(mode = "numeric", T) #states of the hidden Markov chain
  intens = matrix(0, L, T) #matrix per the rain intensity for each day (columns), for each sites (rows)
  diffMean = vector(mode = "numeric", L)
  for(l in 1:L)
  {
    for (n in 1:T)
    {
      if(n == 1)
      {
        states[n] = sample(x = 1:m, size = 1, prob = delta)
      }
      else
      {
        states[n] = sample(x = 1:m, size = 1, prob = Gamma[(states[n-1]), ])
      }
      j = states[n]
      N = rpois(1, lambda[j])
      for (i in 1:N)
      {
        x = runif(1, lx, hx)
        y = runif(1, ly, hy)
        R = rexp(1, xi[j])
        sigma = rexp(1, tau[j])
        D = disc(centre = c(x, y), radius = R)
        if(inside.owin(siteXtot[l], siteYtot[l], D) == TRUE)
        {
          intens[l, n] = intens[l, n] + sigma  
        }
      }
    }
    #diffMean[l] = (abs(mean(matPioggia[1:T, l]) - mean(intens[l, 1:T])))
  }
  #diff = (max(diffMean))
  return(intens)
}
ptm <- proc.time()
#1° step:
#We simulate our HMM with  ad hoc parameters
m = 3 #number of states
T = 365*5 #days
delta = runif(m)
delta = delta/sum(delta) #initial probability of the chain  
Gamma = matrix(runif(m^2), ncol = m) #transition matrix probability
for (j in 1:m)
{
  Gamma[j, ] = Gamma[j, ]/sum(Gamma[j, ])
}
intens = t(matPioggia[1:T, ])

#2° step: ABC method to estimate lambda, xi, tau
ptm <- proc.time()
maxIt = 100 #number of iterations
distPar = matrix(0, maxIt, ((3*m)+2))
for (it in 1:maxIt)
{
  lambdaE =  c(runif(1, 5, 15), runif(1, 16, 25), runif(1, 26, 35))
  xiE = c(runif(1, 0.03, 0.07), runif(1, 0.008, 0.02), runif(1, 0.001, 0.007))
  tauE = c(runif(1, 0.8, 1.4), runif(1, 0.3, 0.7), runif(1, 0.07, 0.2))
  #Parameters used to estimate only the third component of lambda, xi, tau with ABC

  #lambdaE =  c(10, 20,  runif(1, 40, 60)) 
  #xiE = c(0.05, 0.01, 0.005)
  #tauE = c(1.1, 0.5, 0.1)
  
  #lambdaE =  c(10, 20,  30) 
  #xiE = c(0.05, 0.01, 0.005) 
  #tauE = c(1.1, 0.5, c(1, 0.008, 0.1)) 

  #lambdaE =  c(10, 20,  30)
  #xiE = c(0.05, 0.01, runif(1, 0.001, 0.007)) 
  #tauE = c(1.1, 0.5, 0.1)


  intensE = HMMmodel(lambdaE, xiE, tauE)
  diffMeanSq = vector(mode = "numeric", L)
  diffMeanSqAss = vector(mode = "numeric", L)
  for(l in 1:L)
  {
    #Difference between mean values
    diffMeanSq[l] = ((mean(intensE[l, ], na.rm = TRUE) - mean(intens[l, ]))/(mean(intens[l, ])))^2
    diffMeanSqAss[l] = (mean(intensE[l, ], na.rm = TRUE) - mean(intens[l, ]))^2
  }
  distSit = sqrt(sum(diffMeanSq))
  distSitAss = sqrt(sum(diffMeanSqAss))
  distPar[it, ] = c(lambdaE, xiE, tauE, distSit, distSitAss)
}
time = (proc.time()-ptm)[3]
time/3600
distPar[ , ((3*m)+1)]
distPar
write.csv(distPar, "ABCall.csv")


