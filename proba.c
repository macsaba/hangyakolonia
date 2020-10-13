#include <stdio.h>

int** createArray(int m, int n)
{
    int* values = calloc(m*n, sizeof(int));
    int** rows = malloc(n*sizeof(int*));
    for (int i=0; i<n*m; ++i)
    {
        //values[i] = rand()%100;
        values[i] = i;
    }
    for (int i=0; i<n; ++i)
    {
        rows[i] = values + i*m;
    }
    return rows;
}

void destroyArray(int** arr)
{
    free(*arr);
    free(arr);
}


int main(void)
{
    int** arr = createArray(2,2);

    printf("*aa %d\n", **arr);
    printf("*aa %d\n", *(*arr+1));
    printf("*aa %d\n", *(*arr+2));
    printf("*aa %d\n", arr[1][1]);
    destroyArray(arr);
}
