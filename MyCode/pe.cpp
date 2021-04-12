#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <cstdlib>
#include <cmath> // For pow
#include <random> // Random generators
#include <math.h> //Sqrt
#include "pe.h"


int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        std::cout <<  "This Program requires 4 Arguments in this order \n  Lower Bound Lambda \n Upper Bound Lambda \n Lower Bound Tau \n Upper Bound Tau";
    }else
    {
        clock_t tStart = clock();

        double L_lam = std::stold(argv[1]); 
        double U_lam = std::stold(argv[2]); 
        double L_tau = std::stold(argv[3]);
        double U_tau = std::stold(argv[4]);
        

        
        int test_iter = 1000;
        int N = 3;
        int M = 750;
        int iterations = 100000;

        std::default_random_engine generator(std::random_device{}()); 
        std::uniform_real_distribution<double> unif_lambda(L_lam,U_lam);
        std::uniform_real_distribution<long double> unif_tau(L_tau,U_tau);
        Bsampler(N,M,iterations);
        std::ofstream PE;
        PE.open ("Param_estimation.csv");

        for (int i = 0; i < test_iter; i++ )
        {
            double lambda = unif_lambda(generator);
            long double tau =unif_tau(generator);
            PE << lambda << ',' << tau << ',';

            MSample(M,iterations,lambda,tau);
            PE << compare(iterations) << '\n';
        }
        PE.close();



        std::cout << '\n' << (double)(clock() - tStart)/(CLOCKS_PER_SEC) << '\n';
    }
}