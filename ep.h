
#define PI 3.141592
#define MMAX 5
#define NMAX 5
#define R 0
#define G 1
#define B 2

void blurry(int i, int j, float img[MMAX][NMAX][3], float tmp[MMAX][NMAX][3]);
void printMatrix(float mtx[MMAX][NMAX][3]);
void copyResult(float orig[MMAX][NMAX][3], float dest[MMAX][NMAX][3]);
