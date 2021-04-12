#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <cstdlib>
#include <cmath> // For pow
#include <random> // Random generators
#include <math.h> //Sqrt


int main()
{
    int m = 750;
    int iterations = 1000;
    double lambda = 0.62;
    double tau = 0.05;

    std::default_random_engine generator(std::random_device{}()); 

    std::ofstream S;
    S.open ("Model.csv");
    
    
    for (int iter = 0; iter < iterations; iter++)
    {
        std::poisson_distribution<int> pois(lambda);
        int n = pois(generator);
        std::exponential_distribution<long double> exp(tau);

        long double rain = 0;
        for (int i = 0; i < n; i++)
        {
            rain += exp(generator);
        }
        int rain_int = rain;
        S << rain_int << '\n';
    }
    
    S.close();


}