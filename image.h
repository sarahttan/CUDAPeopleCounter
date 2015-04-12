#ifndef IMAGE
#define IMAGE
typedef struct pixel_s {
    unsigned char L;
    unsigned char A;
    unsigned char B;
} pixel_t;

typedef struct Image_s {
    int width;
    int height;
    pixel_t *data;
} Image_t;

// loadJpg function found in readjpeg.c 
int loadJpg(const char *Name, Image_t *pImage);

#endif
