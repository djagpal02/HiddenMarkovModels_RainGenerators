#include <iostream> // Output
#include <ctime> // For time()
#include <random> // Random generators
#include <math.h> //Sqrt
#include "grando_gen_head.h" // Created Functions

int grando_generator(int days,int states,int sites,double delta[],double gamma[],double lambda[],double xi[],double tau[])
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
    int east[sites], north[sites], height[sites];
    // Pull co-ordinate data from csv
    loc_reader(east, north, height, sites);


    // Set Variables
    int x_low, x_high, y_low, y_high;
    // Get values for highs and lows
    x_low = min(east, 30) - 200;
    x_high = max(east, 30) + 200;
    y_low = min(north, 30) - 200;
    y_high = max(north, 30) + 200;


    // Create rain intensity matrix
    double intensity[days][sites];
    // Populate intensity matrix with 0. Need base value to add further rain intensities on.
    for (int i=0;i < days; i++)
    {
        for (int j=0; j< sites; j++)
        {
            intensity[i][j] = 0;
        }
    }


    
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

