
typedef struct {
     int red, green, blue;
} ppmPixel;

typedef struct {
     int x, y;
     ppmPixel *data;
} ppmImage;

#define RGB_COMPONENT_COLOR 255


ppmImage *ppmReader(const char *filename);
void ppmWriter(const char *filename, ppmImage *img);
void changeColorPPM(ppmImage *img);
float ***convertFloatImage(ppmImage *img);
ppmImage *convertIntPPM(float ***img, int M, int N);
void freeImage(float ***img, int M, int N);
void freePPM(ppmImage *img);
