#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>


//#define NR_OF_DATAPOINTS 10;
#define ALPHA 1.0
#define BETA 1.0

int** randomCoords(int n);
float** getDistances(int n, int** coords, int* connections);
void destroyArray(int** arr);


int main() {
    const int NR_OF_DATAPOINTS = 10;
    const int NR_OF_ITERATIONS = 1;
    const int NR_OF_ANTS = 1;
    const int start = 1;
    const int destination = 9;

    float tau [NR_OF_DATAPOINTS][NR_OF_DATAPOINTS];
    float p [NR_OF_DATAPOINTS][NR_OF_DATAPOINTS];
    int route [NR_OF_ANTS][NR_OF_DATAPOINTS*10];
    float length [NR_OF_ANTS];
    float lengthMin [NR_OF_ANTS];
    int connections [NR_OF_DATAPOINTS][NR_OF_DATAPOINTS];
    //TODO: értelmes taut adni
    for(int r = 0; r < NR_OF_DATAPOINTS; r++)
    {
      for(int s = 0; s < NR_OF_DATAPOINTS; s++)
      {
          connections[r][s] = 0;
          tau[r][s] = 1;
      }
    }
    //generate node coords
    int** coords = randomCoords(NR_OF_DATAPOINTS);
    //float dist [NR_OF_DATAPOINTS][NR_OF_DATAPOINTS];

    //set connections btw nodes
    
    connections[0][1]=1; connections[0][3]=1; connections[1][0]=1; connections[1][2]=1; connections[1][5]=1; connections[2][1]=1; connections[2][3]=1; connections[2][4]=1; connections[3][0]=1; connections[3][2]=1;
    connections[3][8]=1; connections[4][2]=1; connections[4][5]=1; connections[5][1]=1; connections[5][4]=1; connections[5][8]=1; connections[6][7]=1; connections[7][6]=1; connections[7][8]=1; connections[7][9]=1;
    connections[8][3]=1; connections[8][9]=1; connections[8][5]=1; connections[8][7]=1; connections[9][7]=1; connections[9][8]=1;

    float** dist = getDistances(NR_OF_DATAPOINTS, coords, &connections[0][0]);
    //iteration
    for(int i = 0; i < NR_OF_ITERATIONS; i++)
    {
      //TODO: clear dtau

      //"create ants"
      for(int ant = 0; ant < NR_OF_ANTS; ant++)
      {
          route[ant][0] = start;
          length[ant] = 0;
          lengthMin[ant] = FLT_MAX;
          //TODO: a két for ciklust össze lehetne vonni, ciklusváltozók eggyel eltolva, az első nevező számítást elvégezni itt
          for(int r = 0; r < NR_OF_DATAPOINTS; r++)
          {
            //probability denominator
            float pDenominator = 0;
            for(int s = 0; s < NR_OF_DATAPOINTS; s++)
            {
                pDenominator += pow(tau[r][s], ALPHA)*pow((1/dist[r][s]), BETA)*connections[r][s];
            }
            //probability matrix
            printf("%f\n", pDenominator);
            for(int s = 0; s < NR_OF_DATAPOINTS; s++)
            {
                int a = 1;
            }
          }
      }
    }
    destroyArray(coords);
    return 0;
}


//generates random coordinates btw 0 and 100(not included)
int** randomCoords(int n)
{
    int* values = calloc(n*2, sizeof(int));
    int** rows = malloc(n*sizeof(int*));

    //generate random numbers
    for (int i=0; i<n*2; ++i)
    {
        values[i] = rand()%100;
    }
    //fill row pointers with the adresses
    for (int i=0; i<n; ++i)
    {
        rows[i] = values + i*2;
    }
    return rows;
}

//calculates the distances between the nodes
float** getDistances(int n, int** coords, int* connections)
{
    float* values = calloc(n*n, sizeof(float));
    float** rows = malloc(n*sizeof(float*));

    for(int i = 0; i < n*n; i++)
    {
        if(connections[i] == 1)
        {
              values[i] = sqrt((double)pow((coords[i/10][0] - coords[i%10][0]),2) + (double)pow((coords[i/10][1] - coords[i%10][1]),2));
        }
        else values[i] =1;
    }

    //fill row pointers with the adresses
    for (int i=0; i<n; ++i)
    {
        rows[i] = values + i*n;
    }

    return rows;
}

void destroyArray(int** arr)
{
    free(*arr);
    free(arr);
}

//lists the node pairs and their coordinates
void listNetwork(int n, int** coords, int* connections)
{
    for(int i = 0; i < n*n; i++)
    {
        if(connections[i] == 1)
        {
              printf("%d, %d  ",i/10, i%10 );
              printf("(%d, %d), (%d, %d)\n", coords[i/10][0], coords[i/10][1], coords[i%10][0], coords[i%10][1]);
        }
    }
}
