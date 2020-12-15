#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
//#include "csvreader.h"
//gcc main.c -o main -g -lm
//cd BME/onlab1/hangyakoloniaM

//#define NR_OF_DATAPOINTS 10;
//#define ALPHA 0.5
#define ALPHA 5.1
#define BETA 0.15
//#define RHO 0.8
#define RHO 0.6
#define Q 5.0

int** randomCoords(int n);
float** getDistances(int n, int** coords, int* connections);
void listNetwork(int n, int** coords, int* connections);
void destroyArray(int** arr);
float** malloc2DFloat(int N);
int** malloc2DInt(int N);
void read_csv(int row, int col, char *filename, double **data);

int main() {
  const int NR_OF_DATAPOINTS = 1363;
  const int NR_OF_EDGES = 3977;
  const int NR_OF_ITERATIONS = 5;
  const int NR_OF_ANTS = 20;
  const int start = 1;
  const int destination = 250;

    //READ EDGES
    int row     = NR_OF_EDGES;
	  int col     = 6;
	  char fname2[256] = "dist.csv";

  	double **dataEdges;
  	dataEdges = (double **)malloc(row * sizeof(double *));
  	for (int i = 0; i < row; ++i){
  		dataEdges[i] = (double *)malloc(col * sizeof(double));
  	}

  	read_csv(row, col, fname2, dataEdges);

    float **tau = malloc2DFloat(NR_OF_DATAPOINTS);
    float **dtau = malloc2DFloat(NR_OF_DATAPOINTS);
    float **dist = malloc2DFloat(NR_OF_DATAPOINTS);
    float **p = malloc2DFloat(NR_OF_DATAPOINTS);
    int route [NR_OF_ANTS][200];
    float length [NR_OF_ANTS];
    float lengthMin [NR_OF_ANTS];
    int **connections = malloc2DInt(NR_OF_DATAPOINTS);
    int numOfConnections [NR_OF_DATAPOINTS];


    for(int r = 0; r < NR_OF_DATAPOINTS; r++)
    {
      for(int s = 0; s < NR_OF_DATAPOINTS; s++)
      {
          connections[r][s] = 0;
          tau[r][s] = 1;
          dtau[r][s] = 0;
          dist[r][s] = 1;
      }
    }

    for(int i = 0; i < NR_OF_EDGES; i++)
    {
        connections[(int) dataEdges[i][1]][(int) dataEdges[i][2]] = 1;
        dist[(int) dataEdges[i][1]][(int) dataEdges[i][2]] = dataEdges[i][3];
    }

    //calculate the number of connections
    for(int r = 0; r < NR_OF_DATAPOINTS; r++)
    {
        numOfConnections[r] = 0;
        for(int s = 0; s < NR_OF_DATAPOINTS; s++)
        {

            numOfConnections[r] += connections[r][s];
        }
    }
    printf("ITERATION BEGIN\n" );
    float roulette = 0;
    float sumP = 0;
    int selection = 0;
    int current = 0;
    int loop = 0;
    int step = 0;
    int testInteger;


    //ITERATION
    for(int i = 0; i < NR_OF_ITERATIONS; i++)
    {


      //clear dtau
      for( int r = 0; r < NR_OF_DATAPOINTS; r++)
      {
        for(int s = 0; s < NR_OF_DATAPOINTS; s++)
        {
          tau[r][s] = dtau[r][s] + (1-RHO)*tau[r][s] +1;
          dtau[r][s] = 0;

        }
      }

      //"create ants"
      for(int ant = 0; ant < NR_OF_ANTS; ant++)
      {
          route[ant][0] = start;
          length[ant] = 0;
          lengthMin[ant] = FLT_MAX;
          //TODO: a két for ciklust össze lehetne vonni, ciklusváltozók eggyel eltolva, az első nevező számítást elvégezni itt
          //PROBABILITY, calculates probabilities
          for(int r = 0; r < NR_OF_DATAPOINTS; r++)
          {
            //probability denominator
            float pDenominator = 0;
            for(int s = 0; s < NR_OF_DATAPOINTS; s++)
            {
                pDenominator += pow(tau[r][s], ALPHA)*pow((1/dist[r][s]), BETA)*connections[r][s];
            }
            //probability matrix
            for(int s = 0; s < NR_OF_DATAPOINTS; s++)
            {
              //Ha a nevező 0, akkor a csp. kiesett a hálózatból, mert zsákutca és nem cél
              if(pDenominator != 0)
              {
                p[r][s] = (pow(tau[r][s], ALPHA)*pow((1/dist[r][s]), BETA)*connections[r][s])/pDenominator;
              }
              else
              {
                p[r][s] = 0;
              }
            }
          }
          step = 0;
          while(step < 200)
          {
              //ROULETTE

              current = route[ant][step];
              loop = 0;
              do
              {
                roulette = (double) rand() / (double) RAND_MAX;
                sumP = 0;
                if(selection == 66)
                {
                  int aaaa=0;
                }
                selection = -1;

                for(int s = 0; s < NR_OF_DATAPOINTS; s++)
                {
                    sumP += p[route[ant][step]][s];
                    if(roulette < sumP)
                    {
                        selection = s;
                        break;
                    }
                }
              }while (selection == route[ant][step - 1] && numOfConnections[current] != 1);

              //check finish
              if(selection == destination)
              {
                  printf("\ntalalat\nstep: %d, ant: %d, i: %d\n", step, ant, i);

                  for(int ii = 0; ii <=step ; ii++)
                  {
                    printf("-> %d ", route[ant][ii]);
                  }

                  step++;
                  route[ant][step] = selection;
                  length[ant] += dist[route[ant][step - 1]][route[ant][step]];
                  break;
              }


              if(numOfConnections[selection] <= 1)
              {
                  connections[current][selection] = 0;
                  connections[selection][current] = 0;
                  numOfConnections[current] = numOfConnections[current] - numOfConnections[selection];
                  numOfConnections[selection] = 0;

              }
              else
              {
                  //step forward
                  step++;
                  route[ant][step] = selection;
                  length[ant] += dist[route[ant][step - 1]][route[ant][step]];
                  //feromon
                  dtau[current][selection] += Q/length[ant];
              }
          }
      }
    }
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

float** malloc2DFloat(int N)
{
  float **a = malloc(N * sizeof *a);
  for (int i = 0; i < N; i++)
    a[i] = malloc(N * sizeof *a[i]);

    return a;
}

int** malloc2DInt(int N)
{
  int **a = malloc(N * sizeof *a);
  for (int i = 0; i < N; i++)
    a[i] = malloc(N * sizeof *a[i]);

    return a;
}

void read_csv(int row, int col, char *filename, double **data){
	FILE *file;
	file = fopen(filename, "r");

	int i = 0;
  char line[100];

	while (fgets(line, 4098, file) && (i < row))
	  {
	    char* tmp = strdup(line);
	    int j = 0;

	    char *token;

	    /* get the first token */
	    token = strtok(line, " ");

	    /* walk through other tokens */
	    while( token != NULL ) {
	      data[i][j] = atof(token);
	      token = strtok(NULL, " ");
	      j++;
	    }
	    free(tmp);
	    i++;
	  }

}
