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