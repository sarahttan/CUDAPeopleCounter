// 
// reading a JPG image into memory
//
// Adapted from http://stackoverflow.com/questions/694080/how-do-i-read-jpeg-and-png-pixels-in-c-on-linux
//
// added saving a JPG image
// Adapted from http://stackoverflow.com/questions/16390783/how-to-save-yuyv-raw-data-to-jpeg-using-libjpeg
// and http://stackoverflow.com/questions/16740165/saving-rgb-char-into-jpeg-image-using-libjpeg


#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "image.h"


#define LAB JCS_YCbCr
#define RGB JCS_RGB
#define CMYK JCS_CMYK
#define YCCK JCS_YCCK
#define GRAYSCALE JCS_GRAYSCALE

#define COLOR_SPACE LAB


int saveJpg(const char *Name, Image_t *pImage)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error( &jerr );

    jpeg_create_compress(&cinfo);

    FILE *outfile;
    outfile = fopen(Name,"wb");
    if (outfile == NULL) {
        printf("Error: Cannot open %s file for writing!\n",Name);
        return 0;
    }
    
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = pImage->width;
    cinfo.image_height = pImage->height;
    cinfo.input_components = 3;
    cinfo.in_color_space = COLOR_SPACE;
    cinfo.jpeg_color_space = COLOR_SPACE;
 
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo,100,TRUE);

    unsigned char *pBuf;
    pBuf = (unsigned char *) malloc(sizeof(char) * pImage->width * 3);
    if (pBuf == NULL) {
        printf("Error: Cannot allocate memory for buffer\n");
        fclose(outfile);
        return 0;
    }
    
    jpeg_start_compress( &cinfo, TRUE );
    
    int i;
    int row_offset;
    row_offset = 0;
    // now copy the info into the buffer
    while (cinfo.next_scanline < pImage->height) {
     
        for (i=0;i<cinfo.image_width; i++) {
            pBuf[i*3]   = pImage->data[i + row_offset].L;
            pBuf[i*3+1] = pImage->data[i + row_offset].A;
            pBuf[i*3+2] = pImage->data[i + row_offset].B;
        }
        JSAMPROW row_pointer;
        row_pointer = (JSAMPROW) pBuf;
        jpeg_write_scanlines( &cinfo, &row_pointer, 1);
        row_offset += pImage->width;
    }
    jpeg_finish_compress( &cinfo );
    jpeg_destroy_compress( &cinfo );
    
    fclose(outfile);
    
    return 1;    
}


int loadJpg(const char* Name, Image_t *pImage)
{
    unsigned char r,g,b;
    int width, height;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE * infile;        /* source file */
    JSAMPARRAY pJpegBuffer;       /* Output row buffer */
    int row_stride;       /* physical row width in output buffer */

    int x;

    if ((infile = fopen(Name, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", Name);
        return 0;
    }
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = COLOR_SPACE;
    (void) jpeg_start_decompress(&cinfo);
    width = cinfo.output_width;
    height = cinfo.output_height;

    //unsigned char * pDummy = (unsigned char *) malloc (width*height*4*sizeof(char));
    pixel_t *pDummy = (pixel_t *) malloc (sizeof(struct pixel_s)*width*height);
    if (pDummy == NULL){
        printf("NO MEM FOR JPEG CONVERT!\n");
        return 0;
    }
    pImage->data = pDummy;


    row_stride = width * cinfo.output_components ;
    pJpegBuffer = (*cinfo.mem->alloc_sarray)
             ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        (void) jpeg_read_scanlines(&cinfo, pJpegBuffer, 1);
        for (x=0;x<width;x++) {
//           a = 0; // alpha value is not supported on jpg
           r = pJpegBuffer[0][cinfo.output_components*x];
           if (cinfo.output_components>2) {
               g = pJpegBuffer[0][cinfo.output_components*x+1];
               b = pJpegBuffer[0][cinfo.output_components*x+2];
           } else {
               g = r;
               b = r;
           }
           pDummy->B = b;
           pDummy->A = g;
           pDummy->L = r;
           //*(pDummy++) = a;
           pDummy++;
       }
   }
   fclose(infile);

   (void) jpeg_finish_decompress(&cinfo);
   jpeg_destroy_decompress(&cinfo);
   
   pImage->width = width;
   pImage->height = height;
   
   return 1;

}

/*
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
*/
