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
#include "../peopleCounter2.h"
#include "../CycleTimer.h"

int main(int argc, char *argv[]){

    if (argc < 3) {
        printf("USAGE: ./fullTest <first_image_filename> <second_image_filename>\n");
        return 1;
    }

    double startTime = CycleTimer::currentSeconds();
    double deltaTime;

    char *filename1 = argv[1];
    char *filename2 = argv[2];

    frame_t *frame1 = (frame_t *) malloc(sizeof(struct frame_s));
    frame_t *frame2 = (frame_t *) malloc(sizeof(struct frame_s));    
    frame_t *res1 = (frame_t *) malloc(sizeof(struct frame_s));
    frame_t *res2 = (frame_t *) malloc(sizeof(struct frame_s));
    frame_t *res3 = (frame_t *) malloc(sizeof(struct frame_s));
    frame_t *res4 = (frame_t *) malloc(sizeof(struct frame_s));

    readImageFrame(frame1, filename1);
    readImageFrame(frame2, filename2);
    res1 = copyFrame(frame1);
    res2 = copyFrame(frame1);
    res3 = copyFrame(frame2);
    res4 = copyFrame(frame2);
    
    deltaTime = CycleTimer::currentSeconds() - startTime;
    
    printf("CopyFrame finished, elapsed time = %f seconds\n", deltaTime);


    frameToJPG(frame1, "full1.jpg");
    frameToJPG(frame2, "full2.jpg");

    startTime = CycleTimer::currentSeconds();

    if (frameSubtraction(frame1, frame2, res1) != 0) {
        printf("Error in frame subtraction\n");
    }
    
    deltaTime = CycleTimer::currentSeconds() - startTime;

    printf("FrameSubtraction finished, time = %f seconds\n", deltaTime);
    
    frameToJPG(res1, "full3.jpg");

    startTime = CycleTimer::currentSeconds();
    
    if (thresholdImage(res1, res2) != 0) {
        printf("Error in thresholdImage\n");
    }

    deltaTime = CycleTimer::currentSeconds() - startTime;

    printf("thresholdImage finished, time = %f seconds\n", deltaTime);
    
    frameToJPG(res2, "full4.jpg");

    startTime = CycleTimer::currentSeconds();
    
    if (blobDetection(res2) != 0) {
        printf("Error in blob detection\n");
    }

    deltaTime = CycleTimer::currentSeconds() - startTime;

    printf("BlobDetection Finished, time = %f seconds\n", deltaTime);

//    res2 = copyFrame(res1);

    if (drawBoxOnImage(res2, res3) != 0) {
        printf("Error in draw box on image\n");
        exit(1);
    }
    printf("Writing frame\n");    
    frameToJPG(res3, "full5.jpg");
    printf("Write to frame complete, please check file = full5.jpg\n");

    startTime = CycleTimer::currentSeconds();

    // merge boxes
    if (mergeBoxes(res2) != 0) {
        printf("Error in mergeBoxes\n");
        exit(1);
    }
    deltaTime = CycleTimer::currentSeconds() - startTime;
    printf("mergeBoxes Finished, time = %f seconds\n", deltaTime);

    
    if (drawBoxOnImage(res2, res4) != 0) {
        printf("Error in draw box on image\n");
        exit(1);
    }
    printf("Writing frame\n");    
    frameToJPG(res4, "full6.jpg");
    printf("Write to frame complete, please check file = full6.jpg\n");
    


    printf("Freeing frames\n");
    int err = freeFrame(frame1);
    err +=  freeFrame(frame2);
    err += freeFrame(res1);
    err += freeFrame(res2);
    err += freeFrame(res3);
    err += freeFrame(res4);
    if (err != 0) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
