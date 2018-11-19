#include <stdio.h>
#include <omp.h>
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct fmatrix{
    int rows;
    int cols;
    float** array;
} fmatrix;

typedef struct image{
    unsigned char* data;
    int w;
    int h;
    int c;
} image;

typedef struct kernel{
    fmatrix* psf;
    int divisor;
} kernel;

kernel* new_psf(int rows, int cols, int divisor);
void free_psf(kernel*);
void convolve_image(image* src, image* dst, kernel* psf, int threads);
