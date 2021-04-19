#include <iostream>
#include <fstream>

double arr_sum(long double arr[], int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum += arr[i];
    }
    return sum;
}

int main ()
{

    int N = 3, M = 2, iter = 1, obs_seq_length = 5;
    long double A[N][N] = {{0.4,0.4,0.2}, {0.3,0.3,0.4},{0.2,0.5,0.3}}; // Transition Matrix
    long double B[N][M] = {{0.8,0.2},{0.7,0.3},{0.6,0.4}};              // Observation Probability Matrix
    long double pi[N]   = {0.5,0.3,0.2};                                // Initial Distribution

    // Boolean, 0 for Happy 1 for Sad
    int observations[obs_seq_length] = {0, 1, 0, 1, 1};


    ////////////////////////////////////////////////////////////////////////////
    long double alpha[N][obs_seq_length];   // Alpha 
    long double beta[N][obs_seq_length];    // Beta
    long double gamma[N][obs_seq_length];   // Gamma
    long double eta[N][N][obs_seq_length - 1];    //Eta NxN for i to j, with 3rd dimension of time
    long double pi_bar[N];                  // Updated intital distribution
    long double A_bar[N][N];                // Updated Transition matrix
    long double B_bar[N][M];                // Updated Observation matrix


    // Performs baum-welch algorithm on hidden Markov chain for n iterations
    for (int n = 0; n < iter; n++)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Alpha 

        // Alpha_1
        for (int i = 0; i < N; i++)
        {
            alpha[i][0] = pi[i]*B[i][observations[0]];
        }

        // Inductive step
        for (int j = 1; j < obs_seq_length; j++)
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

        std::cout << "ALPHA" << std::endl;
        for (int i = 0; i < N; i++)
        {
            for(int j = 0; j < obs_seq_length; j++)
            {
                std::cout << alpha[i][j] << " , ";
            }
            std::cout << "\n";
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Beta 

        // Beta_{batch_size}
        for (int i = 0; i < N; i++)
        {
            beta[i][obs_seq_length-1] = 1;
        }

        // Inductive step
        for (int j = obs_seq_length-2; j >= 0; j--)
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

        std::cout << "BETA" << std::endl;
        for (int i = 0; i < N; i++)
        {
            for(int j = 0; j < obs_seq_length; j++)
            {
                std::cout << beta[i][j] << " , ";
            }
            std::cout << "\n";
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Gamma
        for (int i =0; i < obs_seq_length;i++)
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

        std::cout << "GAMMA" << std::endl;
        for (int i = 0; i < N; i++)
        {
            for(int j = 0; j < obs_seq_length; j++)
            {
                std::cout << gamma[i][j] << " , ";
            }
            std::cout << "\n";
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Eta

        for (int k = 0; k < obs_seq_length-1; k++)
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
        
        std::cout << "ETA" << std::endl;
        for (int k = 0; k < obs_seq_length-1; k++)
        {
            for (int i = 0; i < N; i++)
            {
                for(int j = 0; j < N; j++)
                {
                    std::cout << eta[i][j][k] << " , ";
                }
                std::cout << "\n";
            }
            std::cout << "\n \n \n ";
        }


        ///////////////////////////////////////////////////////////////////////////////
        // BAUM WELCH


        // Pi
        for (int i = 0; i < N; i++)
        {
            pi_bar[i] = gamma[i][0];
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
                for (int k = 0; k < obs_seq_length-1; k++)
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
                for (int k = 0; k < obs_seq_length; k++)
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

    // Transition Matrix
    std::string tran = "A_transitionMat.csv"; 
    std::ofstream transition;
    transition.open (tran);
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
    obs.open (observ);
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
    pi_out.open (ppi);

    for (int i =0; i < N; i++)
    {
        pi_out << pi[i] << ',';
    }
    pi_out.close();

}