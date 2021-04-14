#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>  
#include <time.h>
#include <bits/stdc++.h>

double arr_sum(long double arr[], int len)
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

void bw(int N, int M, int batch_size ,int iter , char filename[], std::string month)
{
    long double A[N][N]; // Transition Matrix
    long double B[N][M]; // Observation Probability Matrix
    long double pi[N]; // Initial Distribution

    // Fill A B and pi with random values ensuring each row of matrix adds to 1
        
    // pi
    for (int i = 0; i < N; i++)
    {
        pi[i] = unif_real(0,1);
    }
    // Get sum of states
    double sum = arr_sum(pi, N);
    // Divive by sum to get distribution
    for (int i = 0; i < N; i++)
    {
        pi[i] = pi[i]/sum;
    }


    // A
    // Set values of matrix equal to unif RV
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = unif_real(0,1);
        }
    }
    // Ensure Right stochastic by dividing by row sums
    for (int i = 0; i < N; i++)
    {
        // Sum of each row
        double row_sum = arr_sum(A[i],N);
        // Change vaues for each element of the row
        for (int j = 0; j < N; j++)
        {
            A[i][j] = A[i][j]/row_sum;
        }
    }

    // B
    // Set values of matrix equal to unif RV
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            B[i][j] = unif_real(0,1);
        }
    }
    // Ensure Right stochastic by dividing by row sums
    for (int i = 0; i < N; i++)
    {
        // Sum of each row
        double row_sum = arr_sum(B[i],M);
        // Change vaues for each element of the row
        for (int j = 0; j < M; j++)
        {
            B[i][j] = B[i][j]/row_sum;
        }
    }



    // Import Observation data
    int observations[batch_size];

    std::string string;
    std::ifstream data(filename);

    for (int j = 0; j < batch_size; j++)
    {
        getline(data,string,'\n');
        observations[j] = std::stoi(string);
    }
    data.close();


    ////////////////////////////////////////////////////////////////////////////
    long double alpha[N][batch_size]; // Alpha 
    long double beta[N][batch_size];  // Beta
    long double gamma[N][batch_size]; // Gamma
    long double eta[N][N][batch_size-1]; //Eta NxN for i to j, with 3rd dimension of time
    long double pi_bar[N]; // Updated intital distribution
    long double A_bar[N][N]; // Updated Transition matrix
    long double B_bar[N][M]; // Updated Observation matrix

    std::string conv = "convergence_log.csv";
    std::ofstream convergence;
    convergence.open (month + conv);

    // Performs baum-welch algorithm on hidden Markov chain for n iterations
    for (int n = 0; n < iter; n++)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Alpha 

        // Alpha_1
        for (int i = 0; i < N; i++)
        {
            alpha[i][0] = pi[i]*B[i][observations[1]];
        }

        // Inductive step
        for (int j = 1; j < batch_size; j++)
        {
            for (int i = 0; i < N; i++)
            {
                long double sum_alpha = 0; // Sum of previous state alphas times transition
                for (int k = 0; k < N; k++)
                {
                    sum_alpha += alpha[k][j-1]*A[k][i];
                }
                alpha[i][j] = sum_alpha*B[i][observations[j]];
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Beta 

        // Beta_{batch_size}
        for (int i = 0; i < N; i++)
        {
            beta[i][batch_size-1] = 1;
        }

        // Inductive step
        for (int j = batch_size-2; j >= 0; j--)
        {
            for (int i = 0; i < N; i++)
            {
                beta[i][j] = 0; // Set current beta to 0, ready for summing
                for (int k = 0; k < N; k++)
                {
                    beta[i][j] += A[i][k]*beta[k][j+1]*B[k][observations[j+1]];
                }
            }
        }


        ///////////////////////////////////////////////////////////////////////////////
        // Gamma
        for (int i =0; i < batch_size;i++)
        {
            long double gamma_denom_sum = 0;
            for (int j = 0; j < N; j++)
            {
                gamma_denom_sum +=  alpha[j][i]*beta[j][i];
            }
            for (int j = 0; j < N; j++)
            {
                gamma[j][i] = (alpha[j][i]*beta[j][i])/gamma_denom_sum;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Eta

        for (int k = 0; k < batch_size-1; k++)
        {
            long double eta_sum = 0;
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    eta_sum += alpha[i][k]*A[i][j]*B[j][observations[k+1]]*beta[j][k+1];
                }
            }


            for (int i = 0;i < N; i++) // From state
            {
                for (int j = 0; j < N; j++) // To state
                {
                    eta[i][j][k] = (alpha[i][k]*A[i][j]*B[j][observations[k+1]]*beta[j][k+1])/eta_sum;
                }
            }
        }


        ///////////////////////////////////////////////////////////////////////////////
        // BAUM WELCH


        // Pi
        for (int i = 0; i < N; i++)
        {
            pi_bar[i] = gamma[i][1];
        }
        // Get sum of states
        long double sum = arr_sum(pi_bar, N);
        // Divive by sum to get distribution
        for (int i = 0; i < N; i++)
        {
            pi_bar[i] = pi_bar[i]/sum;
        }

        // A
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                long double eta_time_sum = 0;
                long double gamma_time_sum = 0;
                for (int k = 0; k < batch_size-1; k++)
                {
                    eta_time_sum += eta[i][j][k];
                    gamma_time_sum += gamma[i][k];

                }
                A_bar[i][j] = eta_time_sum/gamma_time_sum;
            }
        }
        // Ensure Right stochastic by dividing by row sums
        for (int i = 0; i < N; i++)
        {
            // Sum of each row
            long double row_sum = arr_sum(A_bar[i],N);
            // Change vaues for each element of the row
            for (int j = 0; j < N; j++)
            {
                A_bar[i][j] = A_bar[i][j]/row_sum;
            }
        }


        // B
        for (int i = 0; i < N; i++)
        {
            for (int m = 0; m < M; m++)
            {
                B_bar[i][m] = 0;
                long double gamma_sum_b = 0;
                for (int k = 0; k < batch_size; k++)
                {
                    gamma_sum_b += gamma[i][k];

                    if (observations[k] == m)
                    {
                        B_bar[i][m] += gamma[i][k];
                    } 
                }
                B_bar[i][m] = B_bar[i][m]/gamma_sum_b;
            }
        }
        // Ensure Right stochastic by dividing by row sums
        for (int i = 0; i < N; i++)
        {
            // Sum of each row
            long double row_sum = arr_sum(B_bar[i],M);
            // Change vaues for each element of the row
            for (int j = 0; j < M; j++)
            {
                B_bar[i][j] = B_bar[i][j]/row_sum;
            }
        }


        /////////////////////////////////////////////////////////////////////
        // Convergence Log

        long double residual = 0 ;

        // A - A_bar
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                residual += (A[i][j]-A_bar[i][j])*(A[i][j]-A_bar[i][j]);
            }
        }

        // B - B_bar
        for (int i = 0; i < N; i++)
        {
            for (int m = 0; m < M; m++)
            {
                residual += (B[i][m] - B_bar[i][m])*(B[i][m] - B_bar[i][m]);
            }
        }

        // pi - pi_bar
        for (int i = 0; i < N; i++)
        {
            residual += (pi[i] - pi_bar[i])*(pi[i] - pi_bar[i]);
        }

        convergence << residual << '\n';
        

        ////////////////////////////////////////////////////////////////////
        // Reassignment

        // A = A_bar
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = A_bar[i][j];
            }
        }

        // B = B_bar
        for (int i = 0; i < N; i++)
        {
            for (int m = 0; m < M; m++)
            {
                B[i][m] = B_bar[i][m];
            }
        }
            

        // pi = pi_bar
        for (int i = 0; i < N; i++)
        {
            pi[i] = pi_bar[i];
        }
        
    }   





    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Outputs

    // Convergence Log

    convergence.close();


    // Transition Matrix
    std::string tran = "A_transitionMat.csv"; 
    std::ofstream transition;
    transition.open (month + tran);
    for (int i =0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            transition << A[i][j] << ',';
        }
        transition << "\n";
    }
    transition.close();


    // Observation Matrix
    std::string observ = "B_ObservationMat.csv";
    std::ofstream obs;
    obs.open (month + observ);
    for (int i =0; i < M; i++)
    {
        for (int j=0; j < N; j++)
        {
            obs << B[j][i] << ',';
        }
        obs << "\n";
    }
    obs.close();

    // Pi Vector
    std::string ppi = "pi.csv";
    std::ofstream pi_out;
    pi_out.open (month + ppi);

    for (int i =0; i < N; i++)
    {
        pi_out << pi[i] << ',';
    }
    pi_out.close();

}