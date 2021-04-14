#include <iostream> // Output
#include <fstream> // File output
#include <ctime> // For time()
#include <random> // Random generators


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
int max_non_zero(long double arr[], int length)
{
        int index = length -1;
        long double placeholder = arr[index];
        while (placeholder == 0)
        {
            index -= 1;
            placeholder = arr[index];
        }
        return index;
}

long unsigned int compare(int N, int M, int iterations, double lambda, double tau, int cs)
{
    int Model[iterations];
    long double B[N][M]; // Observation Probability Matrix
    
    std::default_random_engine generator(std::random_device{}()); 

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



    // Generate Model Sample
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
        Model[iter] = rain;
    }


    int max_B = max_non_zero(B[cs],M);
    int max_M = max(Model,iterations);
    int max;
    if (max_B < max_M)
    {
        max = max_M;
    }else
    {
        max = max_B;
    }
    
    int freq_B;
    int freq_M;


    long unsigned int residual = 0;
    for (int i = 0; i < max+1;i++)
    {
        if (i >= M)
        {
            freq_B = 0;
        }else
        {
            freq_B = (B[cs][i]*iterations);
        }
        freq_M = count(i,Model,iterations);
        residual += abs((freq_B - freq_M));
    }

    return residual;

}