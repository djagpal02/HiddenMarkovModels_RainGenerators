#include "bw.h"
#include <iostream>
#include <ctime> // For time()
#include <stdlib.h>  
#include <thread>

void bw_plus_sim(int N,int M,int batch_size,int iter,char filename[],std::string month)
{
    bw(N, M, batch_size, iter, filename, month);
    simulator(N, M, batch_size, month);
}

int main(int argc, char* argv[])
{


    if (argc < 5)
    {
        std::cout <<  "This Program requires 4 Arguments in this order \n N - Number of hidden states \n M - Number of observables \n Iter - Number of iterations \n batch_size - Total number of observations";
    }else{
        clock_t tStart = clock();


        const int N = std::stoi(argv[1]); // Number of Hidden States
        const int M = std::stoi(argv[2]); // Number of observables
        const int iter = std::stoi(argv[3]); //Number of iterations
        const int batch_size = std::stoi(argv[4]); //Total number of observations/Time steps

        srand (time(NULL));

        // Data Paths
        char filename_0[17] = "month0.csv";
        char filename_1[17] = "month1.csv";
        char filename_2[17] = "month2.csv";
        char filename_3[17] = "month3.csv";
        char filename_4[17] = "month4.csv";
        char filename_5[17] = "month5.csv";
        char filename_6[17] = "month6.csv";
        char filename_7[17] = "month7.csv";
        char filename_8[17] = "month8.csv";
        char filename_9[17] = "month9.csv";
        char filename_10[18] = "month10.csv";
        char filename_11[18] = "month11.csv";

        // Month Extention
        std::string month_0 = "00/";
        std::string month_1 = "01/";
        std::string month_2 = "02/";
        std::string month_3 = "03/";
        std::string month_4 = "04/";
        std::string month_5 = "05/";
        std::string month_6 = "06/";
        std::string month_7 = "07/";
        std::string month_8 = "08/";
        std::string month_9 = "09/";
        std::string month_10 = "10/";
        std::string month_11 = "11/";


        std::thread T_0 (bw_plus_sim ,N , M, batch_size, iter, filename_0,month_0);
        std::thread T_1 (bw_plus_sim ,N , M, batch_size, iter, filename_1,month_1);
        std::thread T_2 (bw_plus_sim ,N , M, batch_size, iter, filename_2,month_2);
        std::thread T_3 (bw_plus_sim ,N , M, batch_size, iter, filename_3,month_3);
        std::thread T_4 (bw_plus_sim ,N , M, batch_size, iter, filename_4,month_4);
        std::thread T_5 (bw_plus_sim ,N , M, batch_size, iter, filename_5,month_5);
        std::thread T_6 (bw_plus_sim ,N , M, batch_size, iter, filename_6,month_6);
        std::thread T_7 (bw_plus_sim ,N , M, batch_size, iter, filename_7,month_7);
        std::thread T_8 (bw_plus_sim ,N , M, batch_size, iter, filename_8,month_8);
        std::thread T_9 (bw_plus_sim ,N , M, batch_size, iter, filename_9,month_9);
        std::thread T_10 (bw_plus_sim ,N , M, batch_size, iter, filename_10,month_10);
        std::thread T_11 (bw_plus_sim ,N , M, batch_size, iter, filename_11,month_11);


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

        std::cout << '\n' << (double)(clock() - tStart)/(CLOCKS_PER_SEC*12) << '\n';
    }


}