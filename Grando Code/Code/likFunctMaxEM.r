library("spatstat")
library(e1071)
library(neldermead)
library(REdaS)
library(lme4)
library("optimsimplex")
library(gtools)

#Here, we use the fuctions defined in "likFunctEM.r" to perform the Expectation Maximization algorithm to find the parameters of the HMM defined (delta, Gamma, lambda, xi, tau). (see §4.1.2 e §5.1.2)
#We use our data from Germany 

ptm <- proc.time()
m = 3 #number of states 
T = 365 #number of days observed
Tmc = 5000 #number of simulated data via MonteCarlo algorithm 
#nbreaks = c(10, 30, 30)


coord = read.csv("CoordGermany.csv", header = TRUE)
tabPioggia = read.csv("GermanData.csv", header = TRUE)
#Let's read our data
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
siteXtot = coord[1, 2:31]/1000
siteYtot = coord[2, 2:31]/1000
lx = min(siteXtot) - 200
hx = max(siteXtot) + 200
ly = min(siteYtot) - 200
hy = max(siteYtot) + 200

#Data pooling 1
#l = 3
annimax = T/365 #years considered 
rain = vector(mode = "numeric", T)
siteXa = vector(mode = "numeric", annimax) 
siteYa = vector(mode = "numeric", annimax) 
for (anni in 1:annimax)
{
  l = sample(1:L, size = 1)
  rain[(((anni-1)*365)+1):(anni*365)] = matPioggia[(((anni-1)*365)+1):(anni*365) ,l]
  siteXa[anni] = coord[1, (l+1)]/1000
  siteYa[anni] = coord[2, (l+1)]/1000
}

siteX = mean(siteXa)
siteY = mean(siteYa)

#Data pooling 2
#rain = t(matPioggia[1:T, l])
#siteX = runif(1, lx, hx)
#siteY = runif(1, ly, hy)
#rain = matrix(0, T, 1)

#for (n in 1:T)
#{
#  rain[n] = mean(matPioggia[n, 1:L])
#}


#Random chosen initial values

deltacurr = runif(m)
deltacurr = deltacurr/sum(deltacurr) #initial probability of the chain 
Gammacurr = matrix(runif(m^2), ncol = m) #transition matrix probability of the chain 
for (j in 1:m)
{
  Gammacurr[j, ] = Gammacurr[j, ]/sum(Gammacurr[j, ])
}


lambdacurr =  c(runif(1, 4, 12), runif(1, 25, 35), runif(1, 38, 45)) #initial value for lambda
xicurr = c(runif(1, 0.04, 0.1), runif(1, 0.005, 0.01), runif(1, 0.0008, 0.004)) #initial value for xi
taucurr = c(runif(1, 0.8, 1.4), runif(1, 0.09, 0.3), runif(1, 0.04, 0.1)) #initial value for tau

Gamma0 = Gammacurr
delta0 = deltacurr
lambda0 =lambdacurr
xi0 = xicurr
tau0 = taucurr

maxIt = 3 #max number of iteration of the EM algorithm 

#EM algorithm 

for (it in 1:maxIt)
{
  MC = MonteCarlo(Tmc, nbreaks, lambdacurr, xicurr, taucurr)
  hh1 = MC$hh1
  hh2 = MC$hh2
  hh3 = MC$hh3
  dens1 = approxfun(hh1$mids, hh1$density, rule = 2:1)
  dens2 = approxfun(hh2$mids, hh2$density, rule = 2:1)
  dens3 = approxfun(hh3$mids, hh3$density, rule = 2:1)
  
  lalpha = logforward(T, deltacurr, Gammacurr, dens1, dens2, dens3, rain)
  lbeta = logbackward(T, deltacurr, Gammacurr, dens1, dens2, dens3, rain)
  #E step: we compute u and v
  EstepResults = Estep(lalpha, lbeta, T, Gammacurr, deltacurr, dens1, dens2, dens3, rain) 
  hatU = EstepResults$hatU
  hatV1 = EstepResults$hatV1
  hatV2 = EstepResults$hatV2
  hatV3 = EstepResults$hatV3
  #M step
  #We have better estimates for delta and Gamma, so we update them
  deltacurr = hatU[1, ]
  for (j in 1:m)
  {
    Gammacurr[j, 1] = sum(hatV1[, j])/((sum(hatV1[, j])+sum(hatV2[, j]))+sum(hatV3[, j]))
    Gammacurr[j, 2] = sum(hatV2[, j])/((sum(hatV1[, j])+sum(hatV2[, j]))+sum(hatV3[, j]))
    Gammacurr[j, 3] = sum(hatV3[, j])/((sum(hatV1[, j])+sum(hatV2[, j]))+sum(hatV3[, j]))
  }
  print(deltacurr)
  print(Gammacurr)
  #By Nelder Mead algorthm we obtain better estimates for lambda, xi, tau, so we update them
  nd = Nelder_Mead(thirdLik, par = c(lambdacurr, xicurr, taucurr), lower = c(0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001))
  lambdacurr = nd$par[1:3]
  xicurr = nd$par[4:6]
  taucurr = nd$par[7:9]
}
time = (proc.time()-ptm)[3]
time/60


