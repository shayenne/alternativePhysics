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
  

  /* Get PPM data */
  ppmImage *ppm, *newPPM;
  ppm = ppmReader(input);
  
  float ***image = convertFloatImage(ppm);
  float ***temp  = convertFloatImage(ppm);

  /* Refresh image dimensions */
  M = ppm->x;
  N = ppm->y;

  float angle, rx, ry, percentage;
  int signX, signY;
  float deltaRx, deltaRy;
  float deltaBx, deltaBy;
  
  for (int k = 0; k < nIter; k++) {
    //clean(temp);
    
    /* First try: apply blurry in every pixel*/
#pragma omp parallel for shared(temp, image) num_threads(nProc)
    for (int i = 1; i < M-1; i++) {
      for (int j = 1; j < N-1; j++) {
	/* blurry(i, j, image, temp); */
	angle = PIhalf - (twoPI * image[i][j][G]);
	rx = cos(angle);
	ry = sin(angle);
	
	/* Define the neighbour that will receive the color */
	signX = 1;
	signY = 1;
	
	if(rx < 0)    signX = -1;
	if(ry < 0)    signY = -1;
	
	/* Calculate the amount of RED color to be transfered */
	percentage = image[i][j][R] * 0.25;
	deltaRx = (1 - image[i][j + signX][R]) * rx * percentage;
	deltaRy = (1 - image[i - signY][j][R]) * ry * percentage;
	
	temp[i][j + signX][R] += signX * deltaRx;
	temp[i - signY][j][R] += signY * deltaRy;
	
	/* Calculate the amount of BLUE color to be transfered */
	percentage = image[i][j][B] * 0.25;
	deltaBx = (1 - image[i][j - signX][B]) * rx * percentage;
	deltaBy = (1 - image[i + signY][j][B]) * ry * percentage;
	
	temp[i][j - signX][B] += (signX) * deltaBx;
	temp[i + signY][j][B] += (signY) * deltaBy;

	/* Refresh self value */ 
	temp[i][j][R] -= sqrt(deltaRx*deltaRx + deltaRy*deltaRy);
	temp[i][j][B] -= sqrt(deltaBx*deltaBx + deltaBy*deltaBy);
      }
    }
    
#pragma omp parallel for shared(temp, image) num_threads(nProc)
    for (int i = 1; i < M-1; i++)
      for (int j = 1; j < N-1; j++) {
	/* Updating values of image */
	image[i][j][R] += temp[i][j][R];
	image[i][j][B] += temp[i][j][B];

	/* Cleaning temp matrix */
	temp[i][j][R] = 0;
	temp[i][j][G] = 0;
	temp[i][j][B] = 0;

	/* Green Refresh optimized */
	{
	  float theta = 0.0f;
	  float norm  = sqrt(image[i][j][R]*image[i][j][R] +
			     image[i][j][B]*image[i][j][B]);

	  /* eps: Avoid division by zero */
	  if(norm > 1E-5) 
	    theta = acos(image[i][j][B] / (norm + 1E-5)) / (twoPI);
	  
	  image[i][j][G] += theta;
	  
	  if(image[i][j][G] > 1)
	    image[i][j][G] -= 1.0;
	}
      }
    
    //greenRefresh(image);
    
    /*
      newPPM = convertIntPPM(image, M, N);
      char str[15];
      sprintf(str, "res/out%d.ppm", k);
      ppmWriter(str, newPPM);
    */
  }
    
  newPPM = (ppmImage *) convertIntPPM(image, M, N);
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
  float angle = PIhalf - (twoPI * img[i][j][G]);
  float rx = cos(angle);
  float ry = sin(angle);

  /* Define the neighbour that will receive the color */
  int signX = 1;
  int signY = 1;

  float deltaRx, deltaRy;
  float deltaBx, deltaBy;

  if(rx < 0)    signX = -1;
  if(ry < 0)    signY = -1;


  /* Calculate the amount of RED color to be transfered */
  float percentage = img[i][j][R] * 0.25;
  deltaRx = (1 - img[i][j + signX][R]) * rx * percentage;
  deltaRy = (1 - img[i - signY][j][R]) * ry * percentage;

  tmp[i][j + signX][R] += signX * deltaRx;
  tmp[i - signY][j][R] += signY * deltaRy;

  /* Calculate the amount of BLUE color to be transfered */
  percentage = img[i][j][B] * 0.25;
  deltaBx = (1 - img[i][j - signX][B]) * rx * percentage;
  deltaBy = (1 - img[i + signY][j][B]) * ry * percentage;
  
  tmp[i][j - signX][B] += (signX) * deltaBx;
  tmp[i + signY][j][B] += (signY) * deltaBy;


 
  /* Refresh self value */ 
  tmp[i][j][R] -= sqrt(deltaRx*deltaRx + deltaRy*deltaRy);
  tmp[i][j][B] -= sqrt(deltaBx*deltaBx + deltaBy*deltaBy);

}  


/*-----------------------------------------------------*
               REFRESH GREEN COLOR FUNCTION
 *-----------------------------------------------------*/
void greenRefresh(float ***temp){
#pragma omp parallel for shared(temp)
  for(int i = 1; i < M-1 ; i++)
    for(int j = 1; j < N-1 ; j++) {    /* angle between two vectors */
      float newTheta = 0.0f;
      float norm     = sqrt(temp[i][j][R]*temp[i][j][R] + temp[i][j][B]*temp[i][j][B]);

      /* eps: Avoid division by zero */
      if(norm > 1E-5) 
        newTheta = acos(temp[i][j][B] / (norm + 1E-5)) / (twoPI);

      temp[i][j][G] += newTheta;

      if(temp[i][j][G] > 1)
	temp[i][j][G] -= 1.0;
    }
}

/*-----------------------------------------------------*
               MATRIX CLEANER FUNCTION
 *-----------------------------------------------------*/
void clean(float ***mat) {
#pragma omp parallel for
  for (int i = 0; i < M; i++)
    for (int j = 0; j < N; j++){
      mat[i][j][R] = 0.0;
      mat[i][j][G] = 0.0;
      mat[i][j][B] = 0.0;
    }
}

/*-----------------------------------------------------*
               CREATE EMPTY MATRIX FUNCTION
 *-----------------------------------------------------*/
float *** createEmptyMatrix(int M, int N) {

   float ***image;
    
   image = (float ***) malloc (M * sizeof(float **));
   
   for (int k = 0; k < M; k++) {
     image[k] = (float **) malloc (N * sizeof(float *));
     for (int l = 0; l < N; l++)
       image[k][l] = (float *) malloc (3* sizeof(float));
   }
   
   for (int k = 0; k < M; k++) 
     for (int l = 0; l < N; l++) {  
       image[k][l][0] = 0;
       image[k][l][1] = 0;
       image[k][l][2] = 0;
     }
   
   return image;  
}
