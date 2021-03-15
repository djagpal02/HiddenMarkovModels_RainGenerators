#include <cstdlib>

int sampler(double delta[], int states )
{
    double cdf[states];
    int group;
    
    // Create cumulative distribution function
    cdf[0] = delta[0];
    for (int i = 1; i < states; i++)
    {
        cdf[i] = cdf[i-1] + delta[i];
    }
    
    // generate uniform random variable
    double decider = (rand()*1.0/RAND_MAX);

    // Find which group RV fits into
    
    for (int i = 0; i <= states; i++)
    {
        if (decider <= cdf[i])
        {
            return i;
        }
    }
}