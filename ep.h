
#define twoPI  6.283184
#define PIhalf 1.570796
#define R 0
#define G 1
#define B 2

void blurry(int i, int j, float ***img, float ***tmp);
void printMatrix(float ***mtx);
void copyResult(float ***orig, float ***dest);
void greenRefresh(float ***mat);
void clean(float ***mat);
