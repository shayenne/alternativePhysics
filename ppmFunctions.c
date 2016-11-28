#include<stdio.h>
#include<stdlib.h>
#include "ppmFunctions.h"

#define CREATOR "RPFELGUEIRAS" /* Modified for our own purposes*/

ppmImage *ppmReader(const char *filename)
{
         char buff[16];
         ppmImage *img;
         FILE *fp;
         int c, rgb_comp_color;
         //open PPM file for reading
         fp = fopen(filename, "r");
         if (!fp) {
              fprintf(stderr, "Unable to open file '%s'\n", filename);
              exit(1);
         }

         //read image format
         if (!fgets(buff, sizeof(buff), fp)) {
              perror(filename);
              exit(1);
         }

    //check the image format
    if (buff[0] != 'P' || buff[1] != '3') {
         fprintf(stderr, "Invalid image format (must be 'P3')\n");
         exit(1);
    }

    //alloc memory form image
    img = (ppmImage *)malloc(sizeof(ppmImage));
    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
    while (getc(fp) != '\n') ;
         c = getc(fp);
    }

    ungetc(c, fp);
    //read image size information
    if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

    while (fgetc(fp) != '\n') ;
    //memory allocation for pixel data
    img->data = (ppmPixel*)malloc(img->x * img->y * sizeof(ppmPixel));

    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //read pixel data from file
    int i;
    for (i = 0; i < img->x*img->y; i++)
      if (fscanf(fp, "%d %d %d", &img->data[i].red,&img->data[i].green,&img->data[i].blue) != 3) {
	fprintf(stderr, "Error loading image '%s'\n", filename);
	exit(1);
      }

    fclose(fp);
    return img;
}

void ppmWriter(const char *filename, ppmImage *img)
{
    FILE *fp;
    //open file for output
    fp = fopen(filename, "w");
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", filename);
         exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P3\n");

    //image size
    fprintf(fp, "%d %d\n",img->x,img->y);

    // rgb component depth
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // pixel data
    int i;
    for (i = 0; i < img->x*img->y; i++)
      fprintf(fp, "%d %d %d ", img->data[i].red, img->data[i].green, img->data[i].blue);
    fclose(fp);
}

void changeColorPPM(ppmImage *img)
{
  int i;
  if(img){
    for(i=0;i<img->x*img->y;i++){
      img->data[i].red=img->data[i].red;
      img->data[i].green=img->data[i].green;
      img->data[i].blue=img->data[i].blue;
    }
  }
}


float ***convertFloatImage(ppmImage *img) {
  int i;

  float ***image;
  
  if(img){
    
    //image = (float ***)malloc(sizeof(float)*img->x*img->y*3);
    image = (float ***) malloc (img->x * sizeof(float **));

    int k, l;
    for (k = 0; k < img->x; k++) {
      image[k] = (float **) malloc (img->y * sizeof(float *));
      for (l = 0; l<img->y;l++)
	image[k][l] = (float *) malloc (3* sizeof(float));
    }
	  
    for(i = 0; i < img->x * img->y; i++){
      image[i/img->y][i%img->y][0] = img->data[i].red / 255.0;
      image[i/img->y][i%img->y][1] = img->data[i].green / 255.0;
      image[i/img->y][i%img->y][2] = img->data[i].blue / 255.0;
    }

    return image;
  }
  return NULL;
}

ppmImage *convertIntPPM(float ***image, int M, int N) {
  //alloc memory form image
  ppmImage *img;
  img = (ppmImage *)malloc(sizeof(ppmImage));
  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }
  img->x = M;
  img->y = N;
  img->data = (ppmPixel*)malloc(M * N * sizeof(ppmPixel));
  
  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  //read pixel data from file
  int i, j, k;

  k = 0;
  for (i = 0; i < M; i++)
    for (j = 0; j < N; j++) {
      img->data[k].red   = (int) (image[i][j][0] * 255);
      img->data[k].green = (int) (image[i][j][1] * 255);
      img->data[k].blue  = (int) (image[i][j][2] * 255);
      k++; 
    }
  
  return img; 
}

void freeImage(float ***img, int M, int N) {
  int i,j;
  
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++)
      free(img[i][j]);
    free(img[i]);
  }
  free(img);
}


void freePPM(ppmImage *img) {
  free(img->data);
  free(img);
}
/* int main(){ */
/*     ppmImage *image; */
/*     image = ppmReader("test.ppm"); */
/*     changeColorPPM(image); */
/*     ppmWriter("can_bottom2.ppm",image); */
/*     printf("Press any key..."); */
/*     getchar(); */
/*     float ***img = convertFloatImage("test.ppm"); */
/*     /\*MAKE FUNCTIONS TO FREE MEMORY*\/ */
/* } */
