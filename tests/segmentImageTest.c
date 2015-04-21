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

    thresholdImage(frame, temp);
    if (frameToJPG(temp, "threshOut.jpg") != 0) {
        printf("Could not save thresholded frame\n");
        return 1;
    }
    
    
    unsigned long largestLabel;
    segmentImage(frame, frame, &largestLabel);    

    pixel_t p;
    int i,j;
    int fH = frame->image->height;
    int fW = frame->image->width;

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
    

#if 0
    pixel_t *P;
    srand(time(NULL));
    for (i = 0; i < fH; i++) {
        for (j = 0; j < fW; j++) {
            P = &frame->image->data[i*fW+j];
            if (P->L > 1) {
                P->L = rand() % 255+1;
                P->A = rand() % 255+1;
                P->A = rand() % 255+1; 
            } else {
                P->L = 0;
                P->A = 0;
                P->B = 0;
            }
        }
    }

    if (frameToJPG(frame, "segOut.jpg") != 0) {
        printf("Unable to write image\n");
    } else {
        printf("Segmented Image can be found at [segOut.jpg]\n");
    } 
 #endif
    printf("Freeing frame\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
