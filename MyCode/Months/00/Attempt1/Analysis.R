library(readr)
library(plot3D)

Param_estimation <- read_csv("Desktop/MMathProject/MyCode/Months/01/Attempt3/Param_estimation.csv", col_names = FALSE)

scatter3D(x=Param_estimation[[1]], y=Param_estimation[[2]], z = Param_estimation[[3]], phi = 0, bty = "g", pch = 20, cex = 2)
scatter3D(x=Param_estimation[[2]], y=Param_estimation[[1]], z = Param_estimation[[3]], phi = 0, bty = "g", pch = 20, cex = 2)
plot(Param_estimation[[1]],Param_estimation[[3]], xlab = "Lambda", ylab = "Residual", main = "Residual against Lambda")
plot(Param_estimation[[2]],Param_estimation[[3]], xlab = "Tau", ylab = "Residual", main = "Residual against Tau")
