//
// Do a full test,
// 1. subtract image
// 2. threshold
// 3. blob detect
// 4. draw boxes around blobs
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){

    if (argc < 3) {
        printf("USAGE: ./drawBoxOnImageTest <first_image_filename> <second_image_filename>\n");
        return 1;
    }

    char *filename1 = argv[1];
    char *filename2 = argv[2];

    frame_t *frame1 = malloc(sizeof(struct frame_s));
    frame_t *frame2 = malloc(sizeof(struct frame_s));    
    frame_t *res1 = malloc(sizeof(struct frame_s));
    frame_t *res2 = malloc(sizeof(struct frame_s));
    frame_t *res3 = malloc(sizeof(struct frame_s));

    readImageFrame(frame1, filename1);
    readImageFrame(frame2, filename2);
    res1 = copyFrame(frame1);
    res2 = copyFrame(frame1);
    res3 = copyFrame(frame1);
    
    printf("CopyFrame finished\n");

    frameToJPG(frame1, "full1.jpg");
    frameToJPG(frame2, "full2.jpg");

    if (frameSubtraction(frame1, frame2, res1) != 0) {
        printf("Error in frame subtraction\n");
    }
    
    printf("FrameSubtraction finished\n");
    
    frameToJPG(res1, "full3.jpg");
    
    if (thresholdImage(res1, res2) != 0) {
        printf("Error in thresholdImage\n");
    }
    printf("thresholdImage finished\n");
    
    frameToJPG(res2, "full4.jpg");
    
    if (blobDetection(res2) != 0) {
        printf("Error in blob detection\n");
    }

    printf("BlobDetection Finished\n");

//    res2 = copyFrame(res1);


    if (drawBoxOnImage(res2, res3) != 0) {
        printf("Error in draw box on image\n");
        exit(1);
    }

    printf("Writing frame\n");    
    frameToJPG(res3, "full5.jpg");
    printf("Write to frame complete, please check file = full5.jpg\n");

    printf("Freeing frames\n");
    int err = freeFrame(frame1);
    err +=  freeFrame(frame2);
    err += freeFrame(res1);
    err += freeFrame(res2);
    err += freeFrame(res3);
    if (err != 0) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
