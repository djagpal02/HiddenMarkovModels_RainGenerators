#include <iostream>
#include <fstream>
#include <ctime> // For time()
#include "cstdlib" // For rand() and srand()
#include <random>
#include <math.h> 

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

void loc_reader(int e[], int n[], int h[],int sites)
{
    // This requires the CSV to contain only numerical data.
    // The end of each line must contain a ',' after the final numerical value.

    std::string temp;

    std::ifstream data("cg.csv");
    
    for (int i = 0; i < sites; i++)
    {
       getline(data,temp,',');
       e[i] = std::stoi(temp)/1000;
    }
    
    for (int i = 0; i < sites; i++)
    {
       getline(data,temp,',');
       n[i] = std::stoi(temp)/1000;
    }

    for (int i = 0; i < sites; i++)
    {
       getline(data,temp,',');
       h[i] = std::stoi(temp);
    }
    data.close();
}

int max(int arr[], int len)
{
    int max = arr[0];
    for (int i = 0; i < len; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        } 
    }
    return max;
}

int min(int arr[], int len)
{
    int min = arr[0];
    for (int i = 0; i < len; i++)
    {
        if (arr[i] < min)
        {
            min = arr[i];
        } 
    }
    return min; 
}


int main()
{

    /*
    From Grando: 
    L = Sites
    T = days
    M = states
    */

    // Set seed for random numbers 
    srand((unsigned int)time(NULL));
    std::default_random_engine generator;

    // Set vectors containing co-ordinates
    int east[30], north[30], height[30];
    // Set Variables
    int days = 5, states = 3, sites = 30, x_low, x_high, y_low, y_high;
    double intensity[days][sites], delta[3] = {0.3,0.4,0.3}, gamma[3] = {0.2,0.5,0.3}, lambda[3] = {4,25,38}, xi[3] = {0.04,0.005,0.0008}, tau[3] = {0.8,0.09,0.004};
    // Populate intensity matrix with 0. Need base value to add further rain intensities on.
    for (int i=0;i < days; i++)
    {
        for (int j=0; j< sites; j++)
        {
            intensity[i][j] = 0;
        }
    }
    // Pull co-ordinate data from csv
    loc_reader(east, north, height, sites);
    // Get values for highs and lows
    x_low = min(east, 30) - 200;
    x_high = max(east, 30) + 200;
    y_low = min(north, 30) - 200;
    y_high = max(north, 30) + 200;

    
    // For each site
    for (int i = 0; i < sites; i++)
    {
        int curr_state, prev_state;

        // For each timestop
        for (int j = 0; j < days; j++)
        {
            // Update prev state before new curr state to allow for gamma dist calculation
            prev_state = curr_state;

            if (j < 1)
            {
                // Get first state using initial distribution
                curr_state = sampler(delta,states);
            }else
            {
                curr_state = sampler(gamma,states);
            }

            // Generate number of storm discs
            std::poisson_distribution<int> pois(lambda[curr_state]);
            int N = pois(generator);

            std::cout << N << "\n";
            // For each storm disc
            for (int k = 0; k < N; k++)
            {
                // Get size of disc in x and y
                std::uniform_real_distribution<double> unif_x(x_low,x_high);
                std::uniform_real_distribution<double> unif_y(y_low,y_high);
                double x = unif_x(generator);
                double y = unif_y(generator);

                
                // Get Radius
                std::exponential_distribution<double> exp_r(xi[curr_state]);
                double R = exp_r(generator);

                // Get local intesity sigma
                std::exponential_distribution<double> exp_sig(tau[curr_state]);
                double sigma = exp_sig(generator);

                // Get Distance
                double dist = sqrt((east[i] - x)*(east[i] - x) + (north[i] - y)*(north[i] - y));

                
                std::cout << dist << ',' << R << '\n'; 
                // Check if region is within storm disc
                if (dist <= R)
                {
                    intensity[j][i] += sigma; 
                }
            }
        }
    }


    std::cout << "\n \n Results - row = site, column = day \n  \n";
    for (int i =0; i < sites; i++)
    {
        for (int j=0; j < days; j++)
        {
            std::cout << intensity[j][i] << ' ';
        }
        std::cout << '\n';
    }
}

