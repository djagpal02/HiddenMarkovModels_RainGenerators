#include <fstream>
#include <cstdlib> 

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