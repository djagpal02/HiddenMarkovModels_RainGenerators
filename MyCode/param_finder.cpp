#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <cstdlib>
#include <cmath> // For pow
#include <random> // Random generators
#include <math.h> //Sqrt

int factorial(int n)    
{   
    if (n == 0)
    {
        return 1;
    }else
    {
        int r = 1;
        for (int i = 1; i < n; i++)
        {
            r *= i;
        }
        return r;
    }
}

int main()
{
    clock_t tStart = clock();

    int N = 3;
    int M = 750;
    int iterations = 1000;
    long double B[N][M]; // Observation Probability Matrix
    

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

    std::default_random_engine generator(std::random_device{}()); 
    std::uniform_real_distribution<double> unif_lambda(0,10);
    std::uniform_real_distribution<long double> unif_tau(0,10);


    std::ofstream PE;
    PE.open ("Param_estimation.csv");
    
    for (int iter = 0; iter < iterations; iter++)
    {
        int lambda = unif_lambda(generator);
        std::poisson_distribution<int> pois(lambda);
        int n = pois(generator);
        long double tau = unif_tau(generator);
        long double residual[N];


        PE << lambda << ',' << tau << ',';
        for (int n = 0; n < N; n++)
        {
            residual[n] = 0;
            for (int x = 0; x < M; x++)
            {
                long double E = exp(-tau*x)*pow(x,n-1)*pow(tau,n)/factorial(n-1);
                long double P = exp(-lambda)*pow(lambda,n)/factorial(n);
                long double G = E*P;
                residual[n] += (G-B[n][x])*(G-B[n][x]);
            }
            PE << residual[n] << ',';
        }
        PE << '\n';
    }
    
    PE.close();


    std::cout << '\n' << (double)(clock() - tStart)/(CLOCKS_PER_SEC) << '\n';

}