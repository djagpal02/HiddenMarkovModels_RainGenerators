iter = 100

lambda = runif(iter,0,100)
tau = runif(iter,0,10)
Tresid = rep(0,iter)

for ( i in 1:iter)
{
  residuals = rep(0,750)
  for (j in 1:750)
  {
    Fun = (((lambda*j)^(lambda-1))*(tau^lambda)*exp(-lambda-tau*j))/2*factorial(lambda)
    residuals[j] = (Fun - B[[1]][j])
  }
  Tresid[i] = sum(residuals)
}

B[[1]][1]
temp = Tresid*10000000
hist(temp)

x = P[[1]]
y = P[[2]]
z = P[[4]]*10000000


library(plot3D)

scatter3D(x, y, z, phi = 0, bty = "g", pch = 20, cex = 2)
