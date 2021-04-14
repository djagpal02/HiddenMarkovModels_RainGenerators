#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>    
#include <random>

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

int main(int argc, char* argv[])
{
    if (argc < 7)
    {
        std::cout <<  "This Program requires 6 Arguments in this order \n lambda_1 \n lambda_2 \n lambda_3 \n tau_1 \n tau_2 \n tau_3 \n";
    }else{
        clock_t tStart = clock();

        double lam_1 = std::stold(argv[1]); 
        double lam_2 = std::stold(argv[2]);
        double lam_3 = std::stold(argv[3]);
        double tau_1 = std::stold(argv[4]);
        double tau_2 = std::stold(argv[5]);
        double tau_3 = std::stold(argv[6]);

        int N = 3;
        int M = 750;
        int max_time = 1000;
        double lambda[3] = {lam_1,lam_2,lam_3};
        double tau[3] = {tau_1,tau_2,tau_3};


        srand (time(NULL));
        std::default_random_engine generator(std::random_device{}()); 

        long double A[N][N]; // Transition Matrix
        long double pi[N]; // Initial Distribution
        int sim_observations[max_time]; // Array to hold Simulation
        int state_seq[max_time]; // Array of state sequence


        // Pull Matrix A
        std::string tran = "A_transitionMat.csv"; 
        std::string A_string;
        std::ifstream dataA(tran);
        for (int i =0; i < N; i++ )
        {
            for (int j = 0; j < N; j++)
            {
            getline(dataA,A_string,',');
            A[i][j] = std::stold(A_string);
            }
        }
        dataA.close();

        // Pull Vector Pi
        std::string ppi = "pi.csv";
        std::string pi_string;
        std::ifstream datapi(ppi);
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
            std::poisson_distribution<int> pois(lambda[state_seq[n]]);
            int N = pois(generator);
            std::exponential_distribution<long double> exp(tau[state_seq[n]]);
            long double rain = 0;
            for (int i = 0; i < N; i++)
            {
                rain += exp(generator);
            }
            sim_observations[n] = (int) rain;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////
        // Output 

        std::string sim = "PEsimulation.csv";
        std::ofstream simulation;
        simulation.open (sim);

        for (int n = 0; n < max_time; n++)
        {
            simulation << sim_observations[n] << '\n';
        }
        simulation.close();

    }
}