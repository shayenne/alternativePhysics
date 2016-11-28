#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <omp.h>
//#include"ep.h" 


/* Assumes page size PAD = 4096. Is a machine-dependend info */
#define PAD 4096
#define eps 1E-5   //eps: Avoid division by zero

#define PI 3.141592
#define NMAX 40000
#define R 0
#define G 1
#define B 2

/* Image dimensions */
#define  M 7 /*rows*/
#define  N 7 /*columns*/

/*
float image[M][N][3] = { 
   {{0,0,0},{0,0,0},{0,0,0}},
   {{0,0,0},{1,0,0},{0,0,0}},
   {{0,0,0},{0,0,0},{0,0,0}},
 }; 
*/
/* float temp[M][N][3] = { 
   {{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{1,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0}},
 }; */

/*
float image[M][N][3] = {
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{1,0,1},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
*/


float image[M][N][3] = {
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}, 
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}, 
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}, 
  {{0,0,0},{0,0,0},{0,0,0},{1,0,1},{0,0,0},{0,0,0},{0,0,0}}, 
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}, 
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};

float temp[M][N][3];  //initialize matrix with zeros with static

/*
float temp[M][N][3] = {
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{1,0,1},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
*/

void blurry(int i, int j, float img[M][N][3], float tmp[M][N][3]);
void printMatrix(float mtx[M][N][3]);
void copyResult(float orig[M][N][3], float dest[M][N][3]);
void greenRefresh();
void clean(float mat[M][N][3]);

/*-----------------------------------------------------*
           ALTERNATIVE PHYSICS - MAIN FUNCTION
 *-----------------------------------------------------*/
int main(int argc, char *argv[]) {
  static long num_steps = 4000000;
  double step;
  
  /* Parameters in argv: 
     [1] input file
     [2] output file
     [3] # of iterations
     [4] # of processors
  */
  char *input, *output;
  int nIter, nProc;
  
  if (argc == 5) {
    nIter = atoi(argv[3]);
    nProc = atoi(argv[4]);
  }

  else {
    printf("You need pass all arguments:\n \
	   /alterPhysics <input> <output> <n_iter> <n_proc>\n");
    return 0;
  }
  
  /* R, G, B in [0,1[*/

  //double total_begin = omp_get_wtime(); //time
  //double total_end = omp_get_wtime();
  //return double(total_end - total_begin);
  
  omp_set_num_threads(nProc);
  
  #pragma omp parallel shared(temp, image)
  {
    int i, id;
    float tmp;
    id = omp_get_thread_num();
    printf("id: %d \n", id);

    #pragma omp parallel for schedule(dynamic)
    for (int k = 0; k < nIter; k++) {
      clean(temp);
      /* First try: apply blurry in every pixel*/
      for (int i = 1; i < M-1; i++) {
	for (int j = 1; j < N-1; j++) {
	  blurry(i, j, image, temp);
	  
	}
      }

      for (int i = 1; i < M-1; i++) 
	for (int j = 1; j < N-1; j++) 
	  image[i][j][B] += temp[i][j][B];
       
      for (int i = 1; i < M-1; i++) 
	for (int j = 1; j < N-1; j++)
	  image[i][j][R] += temp[i][j][R];
      
      greenRefresh(image);   
    }
  }

  printMatrix(image);
    
  return 0;
}

/*-----------------------------------------------------*
             DEBUG - PRINT MATRIX FUNCTION
 *-----------------------------------------------------*/
void printMatrix(float mtx[M][N][3]) {
  printf("Print Matrix function\n");

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

  if(rx < 0)    signX = -1;
  if(ry < 0)    signY = -1;

  float red = img[i][j][R];
  float blue = img[i][j][B];

  //printf("SIGN X: %d   Y: %d\n",signX,signY);
  //printf("Rx: %.3f   Ry: %.3f\n",rx,ry);
  /* Calculate the amount of RED color to be transfered */
  deltaRx = (1 - img[i][j + signX][R]) * rx * img[i][j][R] * 0.25;
  deltaRy = (1 - img[i - signY][j][R]) * ry * img[i][j][R] * 0.25;
  //printf("deltaRx: %.3f   deltaRy: %.3f\n",deltaRx, deltaRy);

  tmp[i][j + signX][R] += signX * deltaRx;
  tmp[i - signY][j][R] += signY * deltaRy;

  /* Calculate the amount of BLUE color to be transfered */
  deltaBx = (1 - img[i][j - signX][B]) * rx * img[i][j][B] * 0.25;
  deltaBy = (1 - img[i + signY][j][B]) * ry * img[i][j][B] * 0.25;

  tmp[i][j - signX][B] -= (-signX) * deltaBx;
  tmp[i + signY][j][B] -= (-signY) * deltaBy;
 
  /* Refresh self value */ 
  tmp[i][j][R] = tmp[i][j][R] - sqrt(deltaRx*deltaRx + deltaRy*deltaRy);
  tmp[i][j][B] = tmp[i][j][B] - sqrt(deltaBx*deltaBx + deltaBy*deltaBy);
}  


/*-----------------------------------------------------*
               REFRESH GREEN COLOR FUNCTION
 *-----------------------------------------------------*/
void greenRefresh(float temp[M][N][3]){
  int i, j;
  
  for(i = 1; i < M-1 ; i++)
    for(j = 1; j < N-1 ; j++) {    //angle between two vectors
      float red = temp[i][j][R];
      float blue = temp[i][j][B];
      float theta = temp[i][j][G];
      float newTheta = 0.0f;
      float norm = sqrt(red*red + blue*blue);
      
      if(norm > eps) {
        float angle = blue / (norm + eps);
        newTheta = acos(angle) / (2 * PI);
      }
      //   printf("GREEN red: %.2f  blue: %.2f  newTheta:%.3f angle:%.4f \n",red,blue, newTheta, angle);
      theta += newTheta;
      if(theta > 1) theta -= 1.0;

      temp[i][j][G] = theta;
    }
}

/*-----------------------------------------------------*
               MATRIX CLEANER FUNCTION
 *-----------------------------------------------------*/
void clean(float mat[M][N][3]) {
  int i, j;
  for (int i = 0; i < M; i++)
    for (int j = 0; j < N; j++){
      mat[i][j][R] = 0.0;
      mat[i][j][G] = 0.0;
      mat[i][j][B] = 0.0;
    }

}
