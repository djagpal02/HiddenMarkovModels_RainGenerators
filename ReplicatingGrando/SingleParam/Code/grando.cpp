#include <iostream>
#include <ctime> // For time()
#include <random> // Random generators
#include "grando_head.h"
#include <string>
#include <cmath>
#include <fstream>
#include <thread>

double arr_sum(double arr[], int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum += arr[i];
    }
    return sum;
}

double unif_real(double a, double b)
{
    double r = ((rand()*(b-a))/RAND_MAX) + a;
    return r;
}

void data_mean(double means[],double intensity_data[][25568],int sites)
{
    for (int k = 0; k < sites; k++)
    {
        int sum = 0, size = 0;
        for (int i = 0; i < 25568; i++)
        {
            if (intensity_data[k][i] != -9999)
            {
                size += 1;
                sum += intensity_data[k][i];
            }
        }
        means[k] = (sum*1.0)/size;
    }
}

void output(int var, int sites,int days,int states,double delta[],double gamma[][3], double means[] )
{
    ////////////////////////////////////////////////// ABC ESTIMATION ////////////////////////////////////////////////////////


    // Set maximum number of iterations
    int maxit = 100;

    // Create output intensity matrix 
    double intensity_sim[sites][365*5];
    
    // create an output simulated mean matrix
    double sim_mean[maxit][sites];

    // mean differences 
    double diffMeanSq[maxit][sites], diffMeanSqAss[maxit][sites], distSit[maxit], distSitAss[maxit];

    
    // Set output file paramters
    std::ofstream output;

    // Set third parameters
    double x = 30, y = 0.005, z = 0.1;

    
    // Open output file
    if (var == 1) // testing lambda
    {
        output.open ("lam_avg.csv");
    }
    else if (var == 2) // testing xi
    {
        output.open ("xi_avg.csv");
    }
    else // testing tau
    {
        output.open ("tau_avg.csv");
    }
    
    // Iterations
    for (int iter = 0; iter < maxit; iter++)
    {
        if (var == 1) // testing lambda
        {
            x = unif_real(26,35);
        }
        else if (var == 2) // testing xi
        {
            y = unif_real(0.001,0.007);
        }
        else // testing tau
        {
            z = unif_real(0.07,0.2);
        }

        // intialize parameters used to estimate
        double lambdaE[3] = {10, 20, x};
        double xiE[3] = {0.05, 0.01, y};
        double tauE[3] = {1.1, 0.5, z};

        // run generator
        grando_generator(days, states, sites, delta, gamma, lambdaE, xiE, tauE, intensity_sim);

        for (int i = 0; i < sites; i++)
        {
            sim_mean[iter][i] = arr_sum(intensity_sim[i], days)/days;
            diffMeanSq[iter][i] = ((sim_mean[iter][i] - means[i])/means[i])*((sim_mean[iter][i] - means[i])/means[i]);
            diffMeanSqAss[iter][i] = (sim_mean[iter][i] - means[i])*(sim_mean[iter][i] - means[i]);

        }

        distSit[iter] = sqrt(arr_sum(diffMeanSq[iter],sites));
        distSitAss[iter] = sqrt(arr_sum(diffMeanSqAss[iter],sites));
        output << lambdaE[0] << ',' << lambdaE[1] << ',' <<  lambdaE[2] << ',' <<  xiE[0] << ',' <<  xiE[1] << ',' <<  xiE[2] << ',' <<  tauE[0] << ',' <<  tauE[1] << ',' <<  tauE[2] << ',' << distSit[iter] << ',' << distSitAss[iter] << "\n";

    }

    output.close();

}

int main()
{
    clock_t tStart = clock();

    // int grando_generator(int days,int states,int sites,double delta[],double gamma[],double lambda[],double xi[],double tau[]);
    // Set seed for random numbers 
    srand((unsigned int)time(NULL));

    // data file
    char filename[7] = "gd.csv";

    // Set Number of states, days and sites
    const int states = 3, days = 365*5, sites = 30;
    
    // Set delta, initial distribution
    double delta[states];
    for (int i = 0; i < states; i++)
    {
        // We take mod to ensure the sum will most likely be below RAND_MAX, 1000 is an arbitrary value chosen since this allows up to around 30 states
        delta[i] = unif_real(0,1);
    }
    // Get sum of states
    double sum = arr_sum(delta, states);
    // Divive by sum to get distribution
    for (int i = 0; i < states; i++)
    {
        delta[i] = delta[i]/sum;
    }


    // Set Gamma, Transition Matrix for states - MxM matrix
    double gamma[states][3];
    // Set values of matrix equal to unif RV
    for (int i = 0; i < states; i++)
    {
        for (int j = 0; j < states; j++)
        {
            gamma[i][j] = unif_real(0,1);
        }
    }
    // Ensure Right stochastic by dividing by row sums
    for (int i = 0; i < states; i++)
    {
        // Sum of each row
        double row_sum = arr_sum(gamma[i],states);
        // Change vaues for each element of the row
        for (int j = 0; j < states; j++)
        {
            gamma[i][j] = gamma[i][j]/row_sum;
        }
    }


    // Import data into Intestity Matrix - rows = sites - Cols = number of days
    double intensity_data[sites][25568];
    inten_reader(intensity_data,sites,25568, filename);
    

    // Calculate means 
    double means[sites];
    data_mean(means, intensity_data, sites);

    std::thread first (output,1, sites, days, states, delta, gamma, means);
    std::thread second (output,2,sites, days, states, delta, gamma, means);
    std::thread third (output,3, sites, days, states, delta, gamma, means);
    
    first.join();
    second.join();
    third.join();



    std::cout << '\n' << (double)(clock() - tStart)/CLOCKS_PER_SEC << '\n';
/*
    std::cout << "\n \n Results - row = site, column = day \n  \n";
    for (int i =0; i < sites; i++)
    {
        for (int j=0; j < days; j++)
        {
            std::cout << intensity_sim[i][j] << ' ';
        }
        std::cout << '\n';
    }

    for (int i = 0; i < sites; i++)
    {
        std::cout << sim_mean[1][i] - means[i] << ' ';
    }
*/

}   