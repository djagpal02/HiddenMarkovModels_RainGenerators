#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <cstdlib>
#include <cmath> // For pow
#include <random> // Random generators
#include <math.h> //Sqrt
#include <algorithm> // for max
int max(int arr[], int length)
{
    int max = 0;
    for (int i = 0; i < length; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        } 
    }
    return max;
}

int count(int num, int arr[], int length)
{
    int count = 0;
    for (int i = 0; i < length; i++)
    {
        if (arr[i] == num)
        {
            count += 1;
        }
    }
    return count;
}


long unsigned int compare(int iterations)
{
    int B[iterations];
    int M[iterations];

    // Pull B Sample
    std::string B_string;
    std::ifstream dataB("Samples.csv");
    for (int i = 0; i < iterations; i++ )
    {
           getline(dataB,B_string,'\n');
           B[i] = std::stold(B_string);
    }
    dataB.close();


    // Pull Model Sample
    std::string M_string;
    std::ifstream dataM("MSamples.csv");
    for (int i = 0; i < iterations; i++ )
    {
           getline(dataM,M_string,'\n');
           M[i] = std::stold(M_string);
    }
    dataM.close();


    int max_B = max(B,iterations);
    int max_M = max(M,iterations);
    int max;
    if (max_B < max_M)
    {
        max = max_M;
    }else
    {
        max = max_B;
    }
    
    int freq_B, freq_M;

    long unsigned int residual = 0;
    for (int i = 0; i < max+1;i++)
    {
        freq_B = count(i,B,iterations);
        freq_M = count(i,M,iterations);
        residual += abs((freq_B - freq_M));
    }

    return residual;

}