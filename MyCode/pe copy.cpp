#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <cstdlib>
#include <cmath> // For pow
#include <random> // Random generators
#include <math.h> //Sqrt
#include "pe.h"


int main()
{
    clock_t tStart = clock();
    
    int test_iter = 10;
    int N = 3;
    int M = 750;
    int iterations = 1000000;

    std::default_random_engine generator(std::random_device{}()); 
    std::uniform_real_distribution<double> unif_lambda(0,10);
    std::uniform_real_distribution<long double> unif_tau(0,10);
    Bsampler(N,M,iterations);
    std::cout << '\n' << (double)(clock() - tStart)/(CLOCKS_PER_SEC) << '\n';
    std::ofstream PE;
    PE.open ("Param_estimation.csv");

    for (int i = 0; i < test_iter; i++ )
    {
        double lambda = unif_lambda(generator);
        long double tau =unif_tau(generator);
        PE << lambda << ',' << tau << ',';

        MSample(M,iterations,lambda,tau);
        std::cout << '\n' << (double)(clock() - tStart)/(CLOCKS_PER_SEC) << '\n';
        PE << compare(iterations) << '\n';
        std::cout << '\n' << (double)(clock() - tStart)/(CLOCKS_PER_SEC) << '\n';
    }
    PE.close();



    std::cout << '\n' << (double)(clock() - tStart)/(CLOCKS_PER_SEC) << '\n';
}