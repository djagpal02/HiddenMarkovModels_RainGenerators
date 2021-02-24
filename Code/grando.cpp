#include <iostream>
#include <ctime> // For time()
#include <random> // Random generators
#include "grando_head.h"
#include <string>
#include <cmath>
#include <fstream>

double arr_sum(double arr[], int len)
{
    int sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum += arr[i];
    }
    return sum;
}

double unif_real(double a, double b)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<double> unif(a,b);
    return unif(generator);
}

int main()
{
    clock_t tStart = clock();

    // int grando_generator(int days,int states,int sites,double delta[],double gamma[],double lambda[],double xi[],double tau[]);
    // Set seed for random numbers 
    srand((unsigned int)time(NULL));

    // data file
    char filename[27] = "/Data_in_months/month0.csv";

    // Set Number of states, days and sites
    int states = 3, days = 365*1, sites = 30;
    
    // Set delta, initial distribution
    double delta[states];
    for (int i = 0; i < states; i++)
    {
        // We take mod to ensure the sum will most likely be below RAND_MAX, 1000 is an arbitrary value chosen since this allows up to around 30 states
        delta[i] = rand()% 1000;
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
            gamma[i][j] = rand()% 1000;
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
    double intensity_data[sites][365*1];
    inten_reader(intensity_data,sites,days, filename);
    

    // Calculate means 
    double means[sites];
    for (int i = 0; i < sites; i++)
    {
        means[i] = arr_sum(intensity_data[i], days)/days;
    } 

    ////////////////////////////////////////////////// ABC ESTIMATION ////////////////////////////////////////////////////////


    // Set maximum number of iterations
    int maxit = 1000;

    // Create output intensity matrix 
    double intensity_sim[sites][365*1];
    
    // create an output simulated mean matrix
    double sim_mean[maxit][sites];

    // mean differences 
    double diffMeanSq[maxit][sites], diffMeanSqAss[maxit][sites], distSit[maxit], distSitAss[maxit];

    
    // Set output file paramters
    std::ofstream lambda_output;
    std::ofstream xi_output;
    std::ofstream tau_output;



    ///////////////////////////////////////////////////////////  LAMBDA  ///////////////////////////////////////////////
    lambda_output.open ("lambda_avg.csv");
    double x = 0; 
    // Iterations
    for (int iter = 0; iter < maxit; iter++)
    {
        x += 0.01;
        // intialize parameters used to estimate
        double lambdaE[3] = {x, x, x};
        double xiE[3] = {unif_real(0,0.003), unif_real(0.003,0.006), unif_real(0.006,0.009)};
        double tauE[3] = {unif_real(0,0.0004), unif_real(0.0004,0.0008), unif_real(0.0008,0.0012)};

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
        lambda_output << x << ',' << distSit[iter] << ',' << distSitAss[iter] << "\n";
    }

    lambda_output.close();


    /////////////////////////////////////////////////////////  XI  /////////////////////////////////////////////////////////
    xi_output.open ("xi_avg.csv");
    x = 0; 
    // Iterations
    for (int iter = 0; iter < maxit; iter++)
    {
        x += 0.0001;
        // intialize parameters used to estimate
        double lambdaE[3] = {unif_real(0,100), unif_real(0,100), unif_real(0,100)};
        double xiE[3] = {x, x, x};
        double tauE[3] = {unif_real(0,1), unif_real(0,1), unif_real(0,1)};

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
        xi_output << x << ',' << distSit[iter] << ',' << distSitAss[iter] << "\n";
    }

    xi_output.close();


    ///////////////////////////////////////////////////  TAU  //////////////////////////////////////////////////
    tau_output.open ("tau_avg.csv");
    x = 0; 
    // Iterations
    for (int iter = 0; iter < maxit; iter++)
    {
        x += 0.0001;
        // intialize parameters used to estimate
        double lambdaE[3] = {unif_real(0,100), unif_real(0,100), unif_real(0,100)};
        double xiE[3] = {unif_real(0,1), unif_real(0,1), unif_real(0,1)};
        double tauE[3] = {x, x, x};

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
        tau_output << x << ',' << distSit[iter] << ',' << distSitAss[iter] << "\n";
    }

    tau_output.close();





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