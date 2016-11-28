#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include <string.h>

#include "ep.h"
#include "ppmFunctions.h"


/* Image dimensions */
int  M = 5; /*rows*/
int  N = 5;/*columns*/

/*-----------------------------------------------------*
           ALTERNATIVE PHYSICS - MAIN FUNCTION
 *-----------------------------------------------------*/
int main(int argc, char *argv[]) {
  
  /* Parameters in argv: 
     [1] input file
     [2] output file
     [3] # of iterations
     [4] # of processors
  */
  char *input, *output;
  int nIter, nProc;
  
  if (argc == 5) {
    input  = argv[1];
    output = argv[2];
    nIter  = atoi(argv[3]);
    nProc  = atoi(argv[4]);
  }
  else {
    printf("You need pass all arguments:\n \
	   /alterPhysics <input> <output> <n_iter> <n_proc>\n");
    return 0;
  }
  
  /* R, G, B in [0,1[*/
  printf("Parâmetros recebidos: %s %s %d %d\n", input, output, nIter, nProc);
  omp_set_num_threads(nProc);

  /* Get PPM data */
  ppmImage *ppm, *newPPM;
  ppm = ppmReader(input);
  
  float ***image = convertFloatImage(ppm);
  float ***temp  = convertFloatImage(ppm);

  /* Refresh image dimensions */
  M = ppm->x;
  N = ppm->y;
  
  //printMatrix(temp);
  
#pragma omp parallel shared(temp, image)
  {
    int id;
    id = omp_get_thread_num();
    printf("id: %d \n", id);
    
    // #pragma omp parallel for schedule(dynamic)
    for (int k = 0; k < nIter; k++) {
	clean(temp);
	/* First try: apply blurry in every pixel*/
	for (int i = 1; i < M-1; i++) {
	  for (int j = 1; j < N-1; j++) {
	    blurry(i, j, image, temp);
	  }
	}
	
	/*
	  for (int i = 1; i < M-1; i++) 
	  for (int j = 1; j < N-1; j++) 
	  image[i][j][B] += temp[i][j][B];
	  
	  for (int i = 1; i < M-1; i++) 
	  for (int j = 1; j < N-1; j++)
	  image[i][j][R] += temp[i][j][R];
	*/
	
	for (int i = 1; i < M-1; i++) 
	  for (int j = 1; j < N-1; j++) {
	    image[i][j][R] += temp[i][j][R];
	    image[i][j][B] += temp[i][j][B];
	  }
	
	greenRefresh(image);
	newPPM = convertIntPPM(image, M, N);
	char str[15];
	sprintf(str, "res/out%d.ppm", k);
	ppmWriter(str, newPPM);
	
    }
  }
  
  
  printMatrix(image);
  
  newPPM = convertIntPPM(image, M, N);
  ppmWriter(output, newPPM);
  
  /* Free memory used */
  freeImage(image, M, N);
  freeImage(temp, M, N);
  freePPM(ppm);
  freePPM(newPPM);
  return 0;
}

/*-----------------------------------------------------*
             DEBUG - PRINT MATRIX FUNCTION
 *-----------------------------------------------------*/

void printMatrix(float ***mtx) {
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
void copyResult(float ***orig, float ***dest) {

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
void blurry(int i, int j, float ***img, float ***tmp) {
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

  //printf("SIGN X: %d   Y: %d\n",signX,signY);
  //printf("Rx: %.3f   Ry: %.3f\n",rx,ry);
  /* Calculate the amount of RED color to be transfered */
  deltaRx = (1 - img[i][j + signX][R]) * rx * img[i][j][R] * 0.25;
  deltaRy = (1 - img[i - signY][j][R]) * ry * img[i][j][R] * 0.25;
  //printf("deltaRx: %.3f   deltaRy: %.3f\n",deltaRx, deltaRy);

  /* Verify the neighbour: can receive the RED color? */
  //if ((i + signX) > 0 && (i + signX) < M-1)
  tmp[i][j + signX][R] += signX * deltaRx;

  //if ((j + signY) > 0 && (j + signY) < N-1)
  tmp[i - signY][j][R] += signY * deltaRy;

  /* Calculate the amount of BLUE color to be transfered */
  deltaBx = (1 - img[i][j - signX][B]) * rx * img[i][j][B] * 0.25;
  deltaBy = (1 - img[i + signY][j][B]) * ry * img[i][j][B] * 0.25;
  
  /* Verify the neighbour: can receive the BLUE color? */
  if (deltaBx > 0 && (j - signX) > 0 && (j - signX) < M-1)
  tmp[i][j - signX][B] += (signX) * deltaBx;

    //if (deltaBy > 0 && (i + signY) > 0 && (i + signY) < N-1)
  tmp[i + signY][j][B] += (signY) * deltaBy;
 
 
  /* Refresh self value */ 
  tmp[i][j][R] = tmp[i][j][R] - sqrt(deltaRx*deltaRx + deltaRy*deltaRy);
  tmp[i][j][B] = tmp[i][j][B] - sqrt(deltaBx*deltaBx + deltaBy*deltaBy);

  //  printf("MATRIZ TMP EM BLURRY INDEX %d, %d   ",i, j);
  //printMatrix(tmp);

}  


/*-----------------------------------------------------*
               REFRESH GREEN COLOR FUNCTION
 *-----------------------------------------------------*/
void greenRefresh(float ***temp){
  int i, j;
  float eps = 1E-5;   //eps: Avoid division by zero
  
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
void clean(float ***mat) {
  int i, j;
  for (int i = 0; i < M; i++)
    for (int j = 0; j < N; j++){
      mat[i][j][R] = 0.0;
      mat[i][j][G] = 0.0;
      mat[i][j][B] = 0.0;
    }
}
