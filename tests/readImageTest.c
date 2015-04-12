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
    
    pixel_t p = frame->image->rgbImage[0];
    printf("Pixel[0] has values (%u, %u, %u)\n", p.L, p.A, p.B);

 
    printf("Freeing frame\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
