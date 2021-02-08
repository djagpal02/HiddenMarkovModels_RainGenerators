library("spatstat")

#This function emulates our rain intensity process. Given the parameters (Gamma, delta, lambda, xi, tau),
#it gives the rain intensity for every site l = 1,..., L for a period of time T

#m is the number of states of the chain

HMMmodel <- function(T, Gamma, delta, lambda, xi, tau)
{
  states = vector(mode = "numeric", T) #states of the hidden Markov chain
  intens = matrix(0, L, T) #matrix per the rain intensity for each day (columns), for each sites (rows)
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
        dist = sqrt((siteXtot[l]-x)^2 + (siteYtot[l]-y)^2)
        if(dist <= R)
        {
          intens[l, n] = intens[l, n] + sigma  
        }
      }
    }
  }
  return(intens)
}
