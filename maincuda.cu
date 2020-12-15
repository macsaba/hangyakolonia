#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
// Kernel function to add the elements of two arrays

#define ALPHA 5.1
#define BETA 0.15
#define RHO 0.6
#define Q 5.0

__global__
void calcDenom(int N, float *p, float *tau, float *dist,float *denominator, int *connections)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for(int r = index; r < N; r += stride)
  {
    //probability denominator
    denominator[r] = 0;
    for(int s = 0; s < N; s++)
    {
        denominator[r] += pow(tau[r*N + s], ALPHA)*pow((1/dist[r*N + s]), BETA)*connections[r*N + s];
    }
  }
}
__global__
void calcProb(int N, float *p, float *tau, float *dist,float *denominator, int *connections)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for(int r = index; r < N*N; r += stride)
  {
    if(denominator[(int) r/N] != 0)
    {
      p[r] = (pow(tau[r], ALPHA)*pow((1/dist[r]), BETA)*connections[r])/denominator[(int) r/N];
    }
    else
    {
      p[r] = -1;
    }

  }
}
__global__
void setZeros(int N, float *p, float *tau,float *dtau, float *dist,float *denominator, int *connections)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for(int r = index; r < N*N; r += stride)
  {
    connections[r] = 0;
    tau[r] = 1.0;
    dtau[r] = 0;
    dist[r] = 1.0;
    p[r] = 0;
  }
}

__global__
void clearTau(int N, float *tau,float *dtau)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for(int r = index; r < N*N; r += stride)
  {
    tau[r] = dtau[r] + (1-RHO)*tau[r] +1;
    dtau[r] = 0;
  }
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

void readEdges(int N, int row, int col, char *filename, int *connections, float *dist){
  double **dataEdges;
  dataEdges = (double **)malloc(row * sizeof(double *));
  for (int i = 0; i < row; ++i){
    dataEdges[i] = (double *)malloc(col * sizeof(double));
  }

  read_csv(row, col, filename, dataEdges);

  for(int i = 0; i < row; i++)
  {
      connections[((int) dataEdges[i][1])*N + (int) dataEdges[i][2]] = 1;
      dist[(int) dataEdges[i][1] *N + (int) dataEdges[i][2]] = dataEdges[i][3];
  }

  free(dataEdges);

}

int main(void)
{
  //CONSTANTS
  const int NR_OF_DATAPOINTS = 1363;
  const int NR_OF_EDGES = 3977;
  const int NR_OF_ITERATIONS = 5;
  const int NR_OF_ANTS = 20;

  const int NR_OF_BLOCKS = 4096;
  const int NR_OF_THREADS = 512;
  const int start = 1;
  const int destination = 250;

  //MEMORY ALLOCATION
  float *tau, *dtau, *dist, *p;
  int *connections, *numOfConnections;
  float *denominator;
  cudaMallocManaged(&tau, NR_OF_DATAPOINTS*NR_OF_DATAPOINTS*sizeof(float));
  cudaMallocManaged(&dtau, NR_OF_DATAPOINTS*NR_OF_DATAPOINTS*sizeof(float));
  cudaMallocManaged(&dist, NR_OF_DATAPOINTS*NR_OF_DATAPOINTS*sizeof(float));
  cudaMallocManaged(&p, NR_OF_DATAPOINTS*NR_OF_DATAPOINTS*sizeof(float));
  cudaMallocManaged(&connections, NR_OF_DATAPOINTS*NR_OF_DATAPOINTS*sizeof(int));
  cudaMallocManaged(&denominator, NR_OF_DATAPOINTS*sizeof(float));
  cudaMallocManaged(&numOfConnections, NR_OF_DATAPOINTS*sizeof(int));

  setZeros<<<NR_OF_BLOCKS, NR_OF_THREADS>>>(NR_OF_DATAPOINTS, p, tau, dtau, dist,denominator, connections);
  cudaDeviceSynchronize();

  int route [NR_OF_ANTS][200];
  float length [NR_OF_ANTS];
  float lengthMin [NR_OF_ANTS];
  //READ EDGES
  int col     = 6;
  char fname[256] = "dist.csv";

  readEdges(NR_OF_DATAPOINTS, NR_OF_EDGES, col, fname, connections, dist);

  //count the connections of nodes
  for(int r = 0; r < NR_OF_DATAPOINTS; r++)
  {
      numOfConnections[r] = 0;
      for(int s = 0; s < NR_OF_DATAPOINTS; s++)
      {
          numOfConnections[r] += connections[NR_OF_DATAPOINTS*r + s];
      }
  }

  //iteration
  float roulette = 0;
  float sumP = 0;
  int selection = 0;
  int current = 0;
  int step = 0;


  for(int i = 0; i < NR_OF_ITERATIONS; i++)
  {
    clearTau<<<NR_OF_BLOCKS, NR_OF_THREADS>>>(NR_OF_DATAPOINTS, tau, dtau);
    cudaDeviceSynchronize();
    //"create ants"
    for(int ant = 0; ant < NR_OF_ANTS; ant++)
    {
        route[ant][0] = start;
        length[ant] = 0;
        lengthMin[ant] = FLT_MAX;

        calcDenom<<<NR_OF_BLOCKS, NR_OF_THREADS>>>(NR_OF_DATAPOINTS, p, tau, dist,denominator, connections);
        cudaDeviceSynchronize();

        //calculate probabilities
        calcProb<<<NR_OF_BLOCKS, NR_OF_THREADS>>>(NR_OF_DATAPOINTS, p, tau, dist,denominator, connections);
        cudaDeviceSynchronize();

        //start an ant
        step = 0;
        while(step < 200)
        {
            //ROULETTE
            current = route[ant][step];

            do
            {
              roulette = (double) rand() / (double) RAND_MAX;
              sumP = 0;
              selection = -1;
              for(int s = 0; s < NR_OF_DATAPOINTS; s++)
              {

                  sumP += p[route[ant][step]*NR_OF_DATAPOINTS + s];
                if(roulette < sumP)
                  {
                      selection = s;
                      break;
                  }
              }
            }while (selection == route[ant][step - 1] && numOfConnections[current] != 1);

            if(selection == destination)
            {
                printf("\ntalalat\n");

                for(int ii = 0; ii <=step ; ii++)
                {
                  printf("-> %d ", route[ant][ii]);
                }
                printf("\n\n");
                step++;
                route[ant][step] = selection;
                length[ant] += dist[route[ant][step - 1]*NR_OF_DATAPOINTS + route[ant][step]];
                break;
            }

            if(numOfConnections[selection] <= 1)
            {
                connections[current*NR_OF_DATAPOINTS + selection] = 0;
                connections[selection*NR_OF_DATAPOINTS + current] = 0;
                numOfConnections[current] = numOfConnections[current] - numOfConnections[selection];
                numOfConnections[selection] = 0;
            }
            else
            {
                //step forward
                step++;
                route[ant][step] = selection;
                length[ant] += dist[route[ant][step - 1]*NR_OF_DATAPOINTS + route[ant][step]];
                //feromon
                dtau[current*NR_OF_DATAPOINTS + selection] += Q/length[ant];

            }
        }
    }
  }

  cudaFree(denominator);
  cudaFree(tau);
  cudaFree(dtau);
  cudaFree(dist);
  cudaFree(p);
  cudaFree(connections);
  cudaFree(numOfConnections);

}
