#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <random> // Random generators
#include "pe.h"
#include <thread>

void resiudal(int start, int end, int N, int M, int iterations, int cs, double lambda[], long double tau[], long unsigned int residual[])
{
    for (int i = start; i < end; i++)
    {
        residual[i] = compare(N, M, iterations, lambda[i], tau[i], cs);
    }
}
int main(int argc, char* argv[])
{
    if (argc < 6)
    {
        std::cout <<  "This Program requires 5 Arguments in this order \n  Lower Bound Lambda \n Upper Bound Lambda \n Lower Bound Tau \n Upper Bound Tau \n current state";
    }else
    {
        clock_t tStart = clock();

        double L_lam = std::stold(argv[1]); 
        double U_lam = std::stold(argv[2]); 
        double L_tau = std::stold(argv[3]);
        double U_tau = std::stold(argv[4]);
        int cs = std::stoi(argv[5]);
        

        
        int test_iter = 1000;
        int N = 3;
        int M = 750;
        int iterations = 100000;
        int diff = test_iter/24;

        double lambda[1000];
        long double tau[1000];
        long unsigned int residual[1000];

        std::default_random_engine generator(std::random_device{}()); 
        std::uniform_real_distribution<double> unif_lambda(L_lam,U_lam);
        std::uniform_real_distribution<long double> unif_tau(L_tau,U_tau);
        
        for (int i = 0; i < test_iter; i++ )
        {
            lambda[i] = unif_lambda(generator);
            tau[i] = unif_tau(generator);
        }
        

        std::thread T_0 (resiudal, 0, diff, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_1 (resiudal, diff, diff*2, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_2 (resiudal, diff*2, diff*3, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_3 (resiudal, diff*3, diff*4, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_4 (resiudal, diff*4, diff*5, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_5 (resiudal, diff*5, diff*6, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_6 (resiudal, diff*6, diff*7, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_7 (resiudal, diff*7, diff*8, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_8 (resiudal, diff*8, diff*9, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_9 (resiudal, diff*9, diff*10, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_10(resiudal, diff*10, diff*11, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_11(resiudal, diff*11, diff*12, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_12 (resiudal, diff*12, diff*13, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_13 (resiudal, diff*13, diff*14, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_14 (resiudal, diff*14, diff*15, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_15 (resiudal, diff*15, diff*16, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_16 (resiudal, diff*16, diff*17, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_17 (resiudal, diff*17, diff*18, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_18 (resiudal, diff*18, diff*19, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_19 (resiudal, diff*19, diff*20, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_20 (resiudal, diff*20, diff*21, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_21 (resiudal, diff*21, diff*22, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_22(resiudal, diff*22, diff*23, N, M, iterations, cs, lambda, tau, residual);
        std::thread T_23(resiudal, diff*23, test_iter, N, M, iterations, cs, lambda, tau, residual);


        T_0.join();
        T_1.join();
        T_2.join();
        T_3.join();
        T_4.join();
        T_5.join();
        T_6.join();
        T_7.join();
        T_8.join();
        T_9.join();
        T_10.join();
        T_11.join();
        T_12.join();
        T_13.join();
        T_14.join();
        T_15.join();
        T_16.join();
        T_17.join();
        T_18.join();
        T_19.join();
        T_20.join();
        T_21.join();
        T_22.join();
        T_23.join();





        std::ofstream PE;
        PE.open ("Param_estimation.csv");
        for (int i = 0; i < test_iter; i++ )
        {
            PE << lambda[i] << ',' << tau[i] << ',' << residual[i] << '\n';
        }
        PE.close();


        std::cout << '\n' << (double)(clock() - tStart)/(24*CLOCKS_PER_SEC) << '\n';
    }
}