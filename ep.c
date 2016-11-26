#include<stdio.h>
#include<stdlib.h>
#include<math.h>
/* #include"ep.h" */

#define PI 3.141592
#define NMAX 40000
#define R 0
#define G 1
#define B 2

/* Image dimensions */
#define  M  5 /*rows*/
#define  N  5 /*columns*/


/* float image[M][N][3] = { */
/*   {{0,0,0},{0,0,0},{0,0,0}}, */
/*   {{0,0,0},{1,0,0},{0,0,0}}, */
/*   {{0,0,0},{0,0,0},{0,0,0}}, */
/* }; */

/* float temp[M][N][3] = { */
/*   {{0,0,0},{0,0,0},{0,0,0}}, */
/*   {{0,0,0},{1,0,0},{0,0,0}}, */
/*   {{0,0,0},{0,0,0},{0,0,0}}, */
/*  }; */


float image[M][N][3] = {
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{1,0,1},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};

float temp[M][N][3] = {
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{1,0,1},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};

void blurry(int i, int j, float img[M][N][3], float tmp[M][N][3]);

void printMatrix(float mtx[M][N][3]);

void copyResult(float orig[M][N][3], float dest[M][N][3]);

int main(int argc, char *argv[]) {
  
  /* Parameters in argv: 
     [1] input file
     [2] output file
     [3] # of iterations
     [4] # of processors
  */
  char *input, *output;
  int nIter, nProc;
  
  if (argc == 5) 
    nIter = atoi(argv[3]);
  else {
    printf("You need pass all arguments:\n ./alterPhysics <input> <output> <n_iter> <n_proc>\n");
    return 0;
  }
  
  /* R, G, B in [0,1[*/

  for (int k = 0; k < nIter; k++) {
    /* First try: apply blurry in every pixel*/
    for (int i = 1; i < M-1; i++) {
      for (int j = 1; j < N-1; j++) {
	blurry(i, j, image, temp);
      }
    }
    copyResult(temp, image);
    printMatrix(image);
  }

  return 0;
}

/*-----------------------------------------------------*
             DEBUG - PRINT MATRIX FUNCTION
 *-----------------------------------------------------*/
void printMatrix(float mtx[M][N][3]) {
  for(int i = 0; i < M; i++) { 
    for(int j = 0; j < N; j++) {
      printf("[%.2f, %.2f, %.2f]  ", mtx[i][j][R], mtx[i][j][G], mtx[i][j][B]);
    }
    printf("\n");
  }
  printf("\n");
}

/*-----------------------------------------------------*
             COPY - MAKE MATRIX EQUALS
 *-----------------------------------------------------*/
void copyResult(float orig[M][N][3], float dest[M][N][3]) {

  for(int i = 0; i < M; i++) {
    for(int j = 0; j < N; j++) {
      dest[i][j][R] = orig[i][j][R];
      dest[i][j][G] = orig[i][j][G];
      dest[i][j][B] = orig[i][j][B];
    }
  }

}

/*-----------------------------------------------------*
               TRANSPORT COLORS FUNCTION
 *-----------------------------------------------------*/
void blurry(int i, int j, float img[M][N][3], float tmp[M][N][3]) {
  
  float theta = 2 * PI * img[i][j][G];
  float rx = cos(PI/2 - theta);
  float ry = sin(PI/2 - theta);

  /* Define the neighbour that will receive the color */
  int signX = 1;
  int signY = 1;

  float deltaRx, deltaRy;
  float deltaBx, deltaBy;

  if(rx < 0) {
    signX = -1;
    rx *= -1;
  }
  if(ry < 0) {
    signY = -1;
    ry *= -1;
  }

  /* Calculate the amount of RED color to be transfered */
  deltaRx = (1 - img[i][j + signX][R]) * rx * img[i][j][R] * 0.25;
  deltaRy = (1 - img[i - signY][j][R]) * ry * img[i][j][R] * 0.25;

  /* Verify the neighbour: can receive the RED color? */
  if (deltaRx > 0 && (j + signX) > 0 && (j + signX) < M-1)
    tmp[i][j + signX][R] = img[i][j + signX][R] + deltaRx;

  if (deltaRy > 0 && (i - signY) > 0 && (i - signY) < N-1)
    tmp[i - signY][j][R] = img[i - signY][j][R] + deltaRy;

 
  /* Calculate the amount of BLUE color to be transfered */
  deltaBx = (1 - img[i][j - signX][B]) * rx * img[i][j][B] * 0.25;
  deltaBy = (1 - img[i + signY][j][B]) * ry * img[i][j][B] * 0.25;
  
  /* Verify the neighbour: can receive the BLUE color? */
  if (deltaBx > 0 && (j - signX) > 0 && (j - signX) < M-1)
    tmp[i][j - signX][B] = img[i][j - signX][B] + deltaBx;

  if (deltaBy > 0 && (i + signY) > 0 && (i + signY) < N-1)
    tmp[i + signY][j][B] = img[i + signY][j][B] + deltaBy;
 
 
  /* Refresh self value */ 
  tmp[i][j][R] = tmp[i][j][R] - (deltaRx + deltaRy);
  tmp[i][j][B] = tmp[i][j][B] - (deltaBx + deltaBy);

  /* Refresh GREEN color */
  float eps = 10e-5;             /* eps: Avoid division by zero*/
  float newTheta = asin( tmp[i][j][R] /
			 ( sqrt( pow(tmp[i][j][R], 2) + pow(tmp[i][j][B], 2))
			   + eps));
  tmp[i][j][G] = (theta + newTheta) / (2 * PI);

  /* Adjust the range */
  if (tmp[i][j][G] > 1)
    tmp[i][j][G] -= 1;

  printMatrix(tmp);
}  


