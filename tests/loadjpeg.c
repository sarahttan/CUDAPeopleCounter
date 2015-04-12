#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "../image.h"

int main(int argc, char *argv[])
{
    Image_t myImage;
    int result;

    char *filename = "frames/frame01.jpg";

    result = loadJpg(filename, &myImage);

    printf("Loading image %s, result=%d\n",filename,result);
    printf("Image height = %d, width = %d\n",myImage.height, myImage.width);

    pixel_t p = myImage.rgbImage[0];
    printf("Pixel[0] has values (%u, %u, %u)\n", p.L, p.A, p.B);
    // free the memory
    free( myImage.rgbImage );

    return 0;
}
