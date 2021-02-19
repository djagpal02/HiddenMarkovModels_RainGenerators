#include <iostream>
#include <ctime> // For time()
#include <random> // Random generators
#include "grando_head.h"

int main()
{
    // int grando_generator(int days,int states,int sites,double delta[],double gamma[],double lambda[],double xi[],double tau[]);
    // Set seed for random numbers 
    srand((unsigned int)time(NULL));
    std::default_random_engine generator;

    // Set Number of states, days and sites
    int states = 3, days = 365, sites = 30;
    
    // Set delta, initial distribution
    double delta[states];

    
}