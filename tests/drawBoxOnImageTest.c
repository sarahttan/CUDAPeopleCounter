#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){

    if (argc < 2) {
        printf("USAGE: ./drawBoxOnImageTest <new_image_filename>\n");
        return 1;
    }

    char *filename = argv[1];

    frame_t *frame = malloc(sizeof(struct frame_s));
    frame_t *frame2 = malloc(sizeof(struct frame_s));    
    frame_t *res1 = malloc(sizeof(struct frame_s));
    frame_t *res2 = malloc(sizeof(struct frame_s));

    readImageFrame(frame, "./frames/frame13.jpg");
    readImageFrame(frame2, "./frames/frame16.jpg");
    res1 = copyFrame(frame);
    res2 = copyFrame(frame);
    
    printf("CopyFrame finished\n");

    if (frameSubtraction(frame, frame2, res1) != 0) {
        printf("Error in frame subtraction\n");
    }
    
    printf("FrameSubtraction finished\n");
    
    frameToJPG(res1, "drawBox1.jpg");
    
    if (blobDetection(res1) != 0) {
        printf("Error in blob detection\n");
    }

    printf("BlobDetection Finished\n");

//    res2 = copyFrame(res1);


    if (drawBoxOnImage(res1, res2) != 0) {
        printf("Error in draw box on image\n");
        exit(1);
    }

    printf("Writing frame\n");    
    frameToJPG(res2, filename);
    printf("Write to frame complete, please check file = %s\n", filename);

    printf("Freeing frames\n");
    int err = freeFrame(frame);
    err +=  freeFrame(frame2);
    err += freeFrame(res1);
    err += freeFrame(res2);
    if (err != 0) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
