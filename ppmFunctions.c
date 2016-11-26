#include<stdio.h>
#include<stdlib.h>

typedef struct {
     int red, green, blue;
} ppmPixel;

typedef struct {
     int x, y;
     ppmPixel *data;
} ppmImage;

#define CREATOR "RPFELGUEIRAS" /* Modified for our own purposes*/
#define RGB_COMPONENT_COLOR 255

static ppmImage *ppmReader(const char *filename)
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

int main(){
    ppmImage *image;
    image = ppmReader("test.ppm");
    changeColorPPM(image);
    ppmWriter("can_bottom2.ppm",image);
    printf("Press any key...");
    getchar();
}
