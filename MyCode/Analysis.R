library(readr)
library(plot3D)

P <- read_csv("Desktop/MMath/HiddenMarkovModels_RainGenerators/MyCode/Param_estimation.csv", col_names = FALSE)

scatter3D(x=P[[1]], y=P[[2]], z = P[[3]], phi = 0, bty = "g", pch = 20, cex = 2)
scatter3D(x=P[[2]], y=P[[1]], z = P[[3]], phi = 0, bty = "g", pch = 20, cex = 2)
plot(P[[1]],P[[3]])
plot(P[[2]],P[[3]])
