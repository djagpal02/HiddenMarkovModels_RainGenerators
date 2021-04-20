#For example purposes we will use a generate the posterior as the observations.

# Posterior is pois(5), prior is pois(10)
iter = 1000;


obs = rpois(10000,5)
obs_mean = mean(obs)
obs_var = var(obs)


posterior_dataset = rep(0,iter)
sampled = 1

while (sampled < iter+1)
{
  theta_dash = rpois(1,10)
  
  sim_dataset = rpois(10000,theta_dash)
  
  # Only mean and variance for summary statistics
  # eplison = 1
  
  sim_mean = mean(sim_dataset)
  sim_var = var(sim_dataset)
  if (abs(sim_mean-obs_mean)<1 && abs(sim_var-obs_var)<1)
  {
    posterior_dataset[sampled] = theta_dash
    sampled = sampled + 1
  }
}

library(arm)
discrete.histogram(posterior_dataset, main = "Posterior Distribution")

x = rpois(1000,10)
discrete.histogram(x, main = "Prior Distribution")

discrete.histogram(obs, main = "Observed Data Distribution")
