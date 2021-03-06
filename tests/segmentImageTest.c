#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("USAGE: ./segmentImageTest <image_filename>\n");
        return 1;
    }

    char *filename = argv[1];
    frame_t *frame = malloc(sizeof(struct frame_s));
    printf("Reading frame\n");
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
    frame_t *temp = malloc(sizeof(struct frame_s));
    readImageFrame(temp, filename);

    unsigned long largestLabel;

    pixel_t p;
    int i,j;
    int fH = frame->image->height;
    int fW = frame->image->width;
 
    thresholdImage(frame, temp);
    
    for (i=0; i<fH; i++) {
        for (j=0; j < fW; j++){
            p = frame->image->data[i*fW+j];
            if (p.L == 1) {
                p.L = 1;
                p.A = 0;
                p.B = 0;
            }
        }
    }

    if (frameToJPG(temp, "threshOut.jpg") != 0) {
        printf("Could not save thresholded frame\n");
        return 1;
    }
    
    segmentImage(frame, frame, &largestLabel);
    
    for (i = 0; i < fH; i++) {
        for (j = 0; j < fW; j++) {
            p = frame->image->data[i*fW+j];
            if ((p.label == 0) && (p.L != 0)) {
                printf("Missed a pixel during segmentation at (y,x) -> (%d,%d):(%d,%d,%d,%d,%lu)\n", i,j,p.L,p.A,p.B,p.S,p.label);
            }
            if (p.label >= 1) {
                if (i - 1 >= 0) 
                    if (frame->image->data[(i-1)*fW+j].label != p.label)
                        if (frame->image->data[(i-1)*fW+j].L > 0)
                            printf("Pixel (%d, %d) with val %lu not same as neighbor (y,x): (%d, %d)->%lu\n", i,j,p.label,i-1, j, frame->image->data[(i-1)*fW+j].label);
                if (i + 1 < fH) 
                    if (frame->image->data[(i+1)*fW+j].label != p.label) 
                        if (frame->image->data[(i+1)*fW+j].L > 0) 
                            printf("Pixel (%d, %d) with val %lu not same as neighbor (y,x): (%d, %d)->%lu\n", i,j,p.label,i+1, j, frame->image->data[(i+1)*fW+j].label);
                if (j-1 >= 0)
                    if (frame->image->data[i*fW+(j-1)].label != p.label) 
                        if (frame->image->data[i*fW+(j-1)].L > 0) 
                            printf("Pixel (%d, %d) with val %lu not same as neighbor (y,x): (%d,%d)->%lu\n", i,j,p.label,i, j-1, frame->image->data[i*fW+(j-1)].label);
                if (j+1 < fW)
                    if (frame->image->data[i*fW+(j+1)].label != p.label)
                        if (frame->image->data[i*fW+(j+1)].L > 0)
                            printf("Pixel (%d, %d) with val %lu not same as neighbor (y,x): (%d,%d)->%lu\n", i,j,p.label,i, j+1, frame->image->data[i*fW+(j+1)].label);
            }
        } 
    }
    printf("Segment Image Success!!\n");
    printf("Largest label number is %lu\n", largestLabel);
    

    pixel_t *P;
    srand(time(NULL));
    for (i = 0; i < fH; i++) {
        for (j = 0; j < fW; j++) {
            P = &frame->image->data[i*fW+j];
            if (P->label > 1) {
                P->L = rand() % 255+1;
                P->A = rand() % 255+1;
                P->B = rand() % 255+1; 
            } else {
                P->L = 0;
                P->A = 0;
                P->B = 0;
            }
        }
    }

    if (frameToJPG(frame, "segOut1.jpg") != 0) {
        printf("Unable to write image\n");
    } else {
        printf("Segmented Image can be found at [segOut1.jpg]\n");
    } 
    
    // Testing simple white square in center
    
    // clear the screen to black
    for (i=0; i<frame->image->height; i++) {
        for (j=0; j<frame->image->width; j++) {
            int idx;
            idx = i*frame->image->width + j;
            P = &frame->image->data[idx];
            // (16,128,128) is black because we
            // are actually using YCrCb, not LAB colorspace
            P->L = 16;
            P->A = 128;
            P->B = 128;
            
        }
    }
    
    // draw a white square in the middle
    int sq_size;
    int startx;
    int starty;
    
    // size of the square = 100x100    
    sq_size = 100;
    
    // calculate where we start drawing it
    startx = (frame->image->width / 2) - (sq_size / 2);
    starty = (frame->image->height / 2) - (sq_size / 2);
    
    for ( i=starty; i < (starty + sq_size); i++ ) {
        for ( j=startx; j < (startx + sq_size); j++ ) {
            int idx;
            idx = i*frame->image->width + j;
            P = &frame->image->data[idx];
        
            // draw white (235,128,128) in YCrCb colorspace
            P->L = 235;
            P->A = 128;
            P->B = 128;
        }
    }
     
    if (frameToJPG(frame, "segOut2before.jpg") != 0) {
        printf("Unable to write image\n");
    } else {
        printf("Before Segmented Image can be found at [segOut2before.jpg]\n");
    } 
            
    segmentImage(frame, frame, &largestLabel);
        
    printf("Largest label number is %lu (should be 2)\n", largestLabel);
        
    srand(time(NULL));
    for (i = 0; i < fH; i++) {
        for (j = 0; j < fW; j++) {
            P = &frame->image->data[i*fW+j];
            if (P->label > 1) {
                P->L = rand() % 255+1;
                P->A = rand() % 255+1;
                P->B = rand() % 255+1; 
            } else {
                P->L = 0;
                P->A = 0;
                P->B = 0;
            }
        }
    }
    
    if (frameToJPG(frame, "segOut2after.jpg") != 0) {
        printf("Unable to write image\n");
    } else {
        printf("Segmented Image can be found at [segOut2after.jpg]\n");
    } 
    
    
    printf("Freeing frame\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
