library(stats)

# Month data
m = month11[[1]]

x1 = m[1:1000]
x2 = m[1001:2138] 
y1 = simulation[[1]]
y2 = PEsimulation[[1]]

# Training Set HMM Simulation
ks.test(x1,y1)

# Training Set Model Simulation
ks.test(x1,y2)

# Test Set HMM Simulation
ks.test(x2,y1)


# Test set Model Simulation
ks.test(x2,y2)


# CDF of actual (test + training) vs simulated data
plot(ecdf(m), xlab = "Rainfall", main = "CDF of Recorded Rainfall" )
plot(ecdf(y1), xlab = "Rainfall", main = "CDF of HMM Simulated Rainfall")
plot(ecdf(y2), xlab = "Rainfall", main = "CDF of Model simulated Rainfall")
