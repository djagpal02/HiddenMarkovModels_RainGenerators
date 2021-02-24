#include <fstream>
#include <string>

void inten_reader(double e[][365*1], int sites, int readings, char filename[])
{
    // This requires the CSV to contain only numerical data.
    // The end of each line must contain a ',' after the final numerical value.
    // To do so simply add a header when in excel or equvialent and then save. Trailing Commas should apair on the text version of the file.
    // Now you can remove the header from the text view. 

    std::string temp;

    std::ifstream data(filename);
    for (int i =0; i < readings; i++ )
    {
        for (int j = 0; j < sites; j++)
        {
           getline(data,temp,',');
           // Transposed to match other intestity matrix
           e[j][i] = std::stoi(temp);
        }

    }

    data.close();
}