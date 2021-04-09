#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>    

int sampler(long double delta[], int states )
{
    long double cdf[states];
    
    // Create cumulative distribution function
    cdf[0] = delta[0];
    for (int i = 1; i < states; i++)
    {
        cdf[i] = cdf[i-1] + delta[i];
    }
    
    // generate uniform random variable
    long double decider = (rand()*1.0/RAND_MAX);

    // Find which group RV fits into
    
    for (int i = 0; i <= states; i++)
    {
        if (decider <= cdf[i])
        {
            return i;
        }
    }
}

void simulator(int N, int M, int max_time , std::string month)
{

    srand (time(NULL));
    
    long double A[N][N]; // Transition Matrix
    long double B[N][M]; // Observation Probability Matrix
    long double pi[N]; // Initial Distribution
    int sim_observations[max_time]; // Array to hold Simulation
    int state_seq[max_time]; // Array of state sequence


    // Pull Matrix A
    std::string tran = "A_transitionMat.csv"; 
    std::string A_string;
    std::ifstream dataA(month + tran);
    for (int i =0; i < N; i++ )
    {
        for (int j = 0; j < N; j++)
        {
           getline(dataA,A_string,',');
           A[i][j] = std::stold(A_string);
        }
    }
    dataA.close();

    // Pull Matrix B
    std::string observ = "B_ObservationMat.csv";
    std::string B_string;
    std::ifstream dataB(month + observ);
    for (int m =0; m < M; m++ )
    {
        for (int i = 0; i < N; i++)
        {
           getline(dataB,B_string,',');
           B[i][m] = std::stold(B_string);
        }
    }
    dataB.close();

    // Pull Vector Pi
    std::string ppi = "pi.csv";
    std::string pi_string;
    std::ifstream datapi(month + ppi);
    for (int i =0; i < N; i++ )
    {
        getline(datapi,pi_string,',');
        pi[i] = std::stold(pi_string);
    }
    datapi.close();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Simulation

    // State sequence
    state_seq[0] = sampler(pi,N);
    for (int n = 1; n < max_time; n++)
    {
        state_seq[n] = sampler(A[state_seq[n-1]],N);
    }

    // Observations
    for (int n = 0; n < max_time; n++)
    {
        sim_observations[n] = sampler(B[state_seq[n]],M);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Output 

    std::string sim = "simulation.csv";
    std::ofstream simulation;
    simulation.open (month + sim);

    for (int n = 0; n < max_time; n++)
    {
        simulation << sim_observations[n] << '\n';
    }
    simulation.close();


}