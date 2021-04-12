#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <cstdlib>
#include <cmath> // For pow
#include <random> // Random generators
#include <math.h> //Sqrt

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

int Bsampler(int N, int M, int iterations)
{

    double B[N][M]; // Observation Probability Matrix
    

    // Pull Matrix B
    std::string B_string;
    std::ifstream dataB("B_ObservationMat.csv");
    for (int m = 0; m < M; m++ )
    {
        for (int i = 0; i < N; i++)
        {
           getline(dataB,B_string,',');
           B[i][m] = std::stold(B_string);
        }
    }
    dataB.close();



    std::ofstream S;
    S.open ("Samples.csv");
    
    for (int i = 0; i < iterations; i++)
    {
        S << sampler(B[2],M);
        S << '\n';
    }
    
    S.close();



}