#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("USAGE: ./readImageTest <image_filename>\n");
        return 1;
    }

    char *filename = argv[1];
    frame_t *frame = malloc(sizeof(struct frame_s));
    printf("Reading frame\n");
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
    
    pixel_t p = frame->image->data[0];
    printf("Pixel[0] has values (%u, %u, %u)\n", p.L, p.A, p.B);

    printf("Checking all values in frame are initialized\n");
    printf("Data array is size %ld\n", sizeof(frame->image->data)/sizeof(frame->image->data[0]));
    
    int i,j;
    for (i = 0; i < frame->image->width; i++) {
        for (j = 0; j < frame->image->height; j++) {
        p = frame->image->data[i*frame->image->width+j];
        printf("Pixel[%d, %d] has vals L = %d, A = %d, B = %d\n", i,j, p.L, p.A, p.B);
        #if 0
        printf("At pixel %d\n", i);
        if ((p.L < 0) || (p.A < 0) || (p.B < 0)){
            printf("Pixel[%d] not initialized\n", i);
            return 1;
        }
        #endif
        }
    }

 
    printf("Freeing frame\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
