#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("USAGE: ./writeImageTest <new_image_filename>\n");
        return 1;
    }

    char *filename = argv[1];
    frame_t *frame = malloc(sizeof(struct frame_s));
    printf("Reading frame\n");
    if (readImageFrame(frame, "./frames/frame01.jpg") == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
    
    printf("Checking all values in frame are initialized\n");
    int i;
    pixel_t p;
    for (i = 0; i < frame->image->width*frame->image->height; i++) {
        p = frame->image->data[i];
        if ((p.L < 0) || (p.A < 0) || (p.B < 0)){
            printf("Pixel[%d] not initialized\n", i);
            return 1;
        }
    }

    printf("Writing frame\n");    
    frameToJPG(frame, filename);
    printf("Write to frame complete, please check file = %s\n", filename);
 
    printf("Freeing frame\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
