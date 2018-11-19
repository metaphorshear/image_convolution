#include <stdio.h>
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

int copy_image(image *src, image *dst){
    dst->w = src->w;
    dst->h = src->h;
    dst->c = src->c;
    dst->data = malloc(sizeof(char)*src->w*src->h*src->c);
    if (dst->data != NULL){
        memcpy(dst->data, src->data, sizeof(char)*src->w*src->h*src->c);
        return 1;
    }
    else{
        return 0;
    }
}

void delete_matrix(fmatrix* m){
    int i;
    if (m != NULL){
        if (m->array != NULL){
            for (i = 0; i < m->rows; ++i){
                if( m->array[i] != NULL){
                    free(m->array[i]);
                    m->array[i] = NULL;
                }
            }
            free(m->array);
            m->array = NULL;
        }
        free(m);
        m = NULL;
    }
}

fmatrix* new_matrix(int rows, int cols){
    fmatrix* m = (fmatrix*)malloc(sizeof(fmatrix));
    char bad = 0;
    if (m != NULL){
        m->rows = rows;
        m->cols = cols;
        m->array = (float**)malloc(rows * sizeof(float*));
        if (m->array != NULL){
            int i, j;
            for (i = 0; i < rows; ++i){
                m->array[i] = (float*)malloc(cols * sizeof(float));
                if (m->array[i] == NULL){
                    bad = 1;
                    break;
                }
            }
        }
        else { bad = 1; }
    }
    else { bad = 1;}
    if (bad){
        delete_matrix(m);
        return NULL;
    }
    return m;
}

kernel* new_psf(int rows, int cols, int divisor){
    if (rows < 3 || cols < 3){
        rows = 3; cols = 3;
    }
    if (rows % 2 == 0){
        ++rows;
    }
    if (cols % 2 == 0){
        ++cols;
    }
    kernel* psf = (kernel*)malloc(sizeof(kernel));
    if (psf == NULL){
        puts("Failed to allocate kernel!");
        return NULL;
    }
    psf->psf = new_matrix(rows, cols);
    if (psf->psf == NULL){
        free(psf);
        puts("Failed to allocate psf!");
        return NULL;
    }
    psf->divisor = divisor;
    return psf;
}

void free_psf(kernel* psf){
    delete_matrix(psf->psf);
    free(psf);
}

kernel* square_3x3(float n, int div){
    kernel* k = new_psf(3, 3, div);
    int i;
    for (i = 0; i < 9; ++i){
        k->psf->array[i/3][i%3] = n;
    }
    return k;
}

kernel* square_blur_3x3(){
    return square_3x3(1.0, 9);
}
kernel* shift_subtract_3x3(){
    kernel* k = square_3x3(0.0, 1);
    k->psf->array[1][1] = 1.0;
    k->psf->array[2][2] = -1.0;
    return k;
}
kernel* edge_detect_3x3(){
    kernel* k = square_3x3(-1.0/8.0, 1);
    k->psf->array[1][1] = 1.0;
    return k;
}
kernel* edge_enhance_3x3(){
    kernel* k = square_3x3(-2.0/8.0, 1);
    k->psf->array[1][1] = 3.0;
    return k;
}

int convolve_sq(image* src, image* dst, kernel* psf, int pixel){
    int size = psf->psf->rows * psf->psf->cols;
    float tmp[size];
    int n, m; //for psf
    int x, y, x0, y0, cur; //for image

    y0 = pixel / (src->w * src->c);
    x0 = (pixel / src->c) % src->w;

    for (n = 0; n < psf->psf->rows; ++n){
        for (m = 0; m < psf->psf->cols; ++m){
            y = n - (psf->psf->rows / 2);
            x = m - (psf->psf->cols / 2);
            if ((y + y0) < 0 || (y + y0) >= src->h || (x + x0) < 0 || (x + x0) >= src->w){
                tmp[n*psf->psf->rows+m] = 255 * psf->psf->array[n][m];
            }
            else{
                cur = (pixel + y * src->w * src->c + x * src->c);
                tmp[n*psf->psf->rows+m] = src->data[cur] * psf->psf->array[n][m];
            }
        }
    }
    m = 0;
    for (n = 0; n < size; ++n){
        m += (int) tmp[n];
    }
    m /= psf->divisor;
    /*while (m < 0){
        m += 256;
    }
    if (m > 255) m %= 256;
    */
    if (m < 0) m = 0;
    if (m > 255) m = 255;
//    dst->data[pixel] = m;
    return m;
}

void convolve_image(image* src, image* dst, kernel* psf){
    int i, j, k, m, pixel;
    int n = 0;
    unsigned char buffer[2048];
    for (i = 0; i < src->h; ++i){
        for (j = 0; j < src->w; ++j){
            for (k = 0; k < src->c; ++k){
                pixel = i * src->w * src->c + j * src->c + k;
                buffer[n] = (unsigned char) convolve_sq(src, dst, psf, pixel );
                ++n;
                if (n == 2048){
                    memcpy((dst->data)+(pixel-n+1), buffer, n);
                    n = 0;
                }
            }
        }
    }
    if (n != 0){
        memcpy((dst->data)+(pixel-n+1), buffer, n);
    }
}

int main(int argc, char** argv){
    if (argc <4){
        printf("Usage: %s <input file> <output file> <effect>\n", argv[0]);
        return -1;
    }
    char * infile = argv[1];
    char * outfile = argv[2];
    char * effect = argv[3];
    image src, dst;
    src.data = stbi_load(infile, &(src.w), &(src.h), &(src.c), 3);
    printf("Image width: %d, height: %d, channels: %d\n", src.w, src.h, src.c);
    if (src.data != NULL){
        if (copy_image(&src, &dst)){
            kernel* psf;
            if (strcmp(effect, "blur") == 0){
                psf = square_blur_3x3();
            }
            else if (strcmp(effect, "emboss") == 0){
                psf = shift_subtract_3x3();
            }
            else if (strcmp(effect, "edge_detect")==0 || strcmp(effect, "edge")==0){
                psf = edge_detect_3x3();
            }
            else {
                psf = edge_enhance_3x3();
            }
            convolve_image(&src, &dst, psf);
            free_psf(psf);
            stbi_write_png(outfile, dst.w, dst.h, 3, dst.data, dst.w*3);
            //free(dst.data);
            stbi_image_free(dst.data);
        }
        stbi_image_free(src.data);
    }
}
