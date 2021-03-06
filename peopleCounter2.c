#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "peopleCounter2.h"
#include <time.h>
#include "CycleTimer.h"

#include <jpeglib.h>
#define PI 3.14159265
#define THRESHOLD_INIT_VALUE 150

#define DEBUG
#ifdef DEBUG
#define LOG_ERR(...) fprintf(stdout, __VA_ARGS__)
#else
#define LOG_ERR(...)
#endif

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))


// PRIORITY COMPLETION ORDER
//  (1) FrameToJPG
//  (4) Test print boxes
//  (3) BlobDetection
//  (4) MergeBlobs
//  (5) BlobDirection  (testing in progress)
//  (20) extractFrames

// Input: movie file name
//          - use imagemagick to get frames and put them in a folder.
int extractFrames(const char *fileName){
    // TODO: extractFrames from movie here
    return 0;
}

int freeFrame(frame_t *frame){
    if (frame == NULL) {
        printf("freeFrame: Unable to free frame, frame pointer invalid\n");
        return 1;
    }
    if (frame->image != NULL) {
        if (frame->image->data != NULL){
            free(frame->image->data);
        }
        free(frame->image);
    }
    
    if (frame->arBoxes != NULL) {
        free(frame->arBoxes);
    }

//    if (frame->boxes != NULL) {
//        box_t *temp = frame->boxes->next;
//        box_t *temp2;
//        while(temp != NULL){
//            temp2 = temp;
//            temp = temp->next;
//            free(temp2);
//        }  
//        free(frame->boxes);
//    }

    free(frame);
    return 0;
}

int readImageFrame(frame_t *frame, const char *fileName){
    // Load image into the structure
    Image_t *img = (Image_t *)(malloc(sizeof(struct Image_s))); 
    if (img == NULL) {
        printf("readImageFrame: Unable to malloc img pointer\n");
        return -1;
    }
    if (loadJpg(fileName, img) == 0){
        printf("readImageFrame: Unable to read image at [%s]\n", fileName);
        return -1;
    }
    frame->image = img;
   
    // Initialize the other variables in the structure
    frame->arBoxes = NULL;

    LOG_ERR("readImageFrame: Image height = %d, width = %d\n", img->height, img->width);
    return 0;
}

int frameSubtraction(frame_t *frame, frame_t *frame2, frame_t *res){
    //subtract two frames and give back the resulting frame
    //Frames are not the same size and we're screwed
    if ((frame == NULL) || (frame2 == NULL) || (res == NULL)) {
        printf("frameSubtraction: frames are not initialized\n");
        return 1;
    }
    
    if ((frame->image == NULL) || (frame2->image == NULL) || (res->image == NULL)) {
        printf("frameSubtraction: frame->image not initialized\n");
        return 1;
    }

    if((frame->image->width != frame2->image->width) || 
        (frame->image->height != frame2->image->height)){
        printf("frameSutraction: The frame sizes are not the same!");
        return 1;
    }

    int frameWidth = frame->image->width;
    int frameHeight = frame->image->height;
    unsigned char frameL= 0;
    unsigned char frame2L = 0;
    unsigned char frameA = 0;
    unsigned char frame2A = 0;
    unsigned char frameB = 0;
    unsigned char frame2B = 0;

    //printf("Frame->image->data length = %lx\n", sizeof(frame->image->data)/(sizeof(struct pixel_s)));

    //Subtract frames
    for(int i = 0; i < frameHeight; i++){
        for(int j = 0; j < frameWidth; j++){
            if ((&frame->image->data[i*frameWidth+j] == NULL) || (&frame2->image->data[i*frameWidth+j] == NULL) || (&res->image->data[i*frameWidth+j] == NULL)) {
                printf("frameSubtraction: pixel is null (%d, %d)\n", i, j);
                return 1;
            }
            //Get L values
            frameL = frame->image->data[i * frameWidth + j].L;
            frame2L = frame2->image->data[i * frameWidth + j].L;
            //Get A values
            frameA = frame->image->data[i * frameWidth + j].A;
            frame2A = frame2->image->data[i * frameWidth + j].A;
            //Get B values
            frameB = frame->image->data[i * frameWidth + j].B;
            frame2B = frame2->image->data[i * frameWidth + j].B;
            //Set pixel values in res
            res->image->data[i * frameWidth + j].L = abs(frame2L - frameL);
            res->image->data[i * frameWidth + j].A = abs(frame2A - frameA);
            res->image->data[i * frameWidth + j].B = abs(frame2B - frameB);
            //printf("(i,j) = (%d, %d)\n", i,j);
        }
    }
    return 0;
}

int frameSubtractionOmp(frame_t *frame, frame_t *frame2, frame_t *res){
    //subtract two frames and give back the resulting frame
    //Frames are not the same size and we're screwed
    if ((frame == NULL) || (frame2 == NULL) || (res == NULL)) {
        printf("frameSubtraction: frames are not initialized\n");
        return 1;
    }

    if ((frame->image == NULL) || (frame2->image == NULL) || (res->image == NULL)) {
        printf("frameSubtraction: frame->image not initialized\n");
        return 1;
    }

    if((frame->image->width != frame2->image->width) || 
        (frame->image->height != frame2->image->height)){
        printf("frameSutraction: The frame sizes are not the same!");
        return 1;
    }

    int frameWidth = frame->image->width;
    int frameHeight = frame->image->height;

    //printf("Frame->image->data length = %lx\n", sizeof(frame->image->data)/(sizeof(struct pixel_s)));

    #pragma omp parallel for 
    for(int i = 0; i < frameHeight; i++){
        for(int j = 0; j < frameWidth; j++){
            if ((&frame->image->data[i*frameWidth+j] == NULL) || (&frame2->image->data[i*frameWidth+j] == NULL) || (&res->image->data[i*frameWidth+j] == NULL)) {
                printf("frameSubtraction: pixel is null (%d, %d)\n", i, j);
                // return 1;
            }
            //Get L values
            // frameL = frame->image->data[i * frameWidth + j].L;
            // frame2L = frame2->image->data[i * frameWidth + j].L;
            // //Get A values
            // frameA = frame->image->data[i * frameWidth + j].A;
            // frame2A = frame2->image->data[i * frameWidth + j].A;
            // //Get B values
            // frameB = frame->image->data[i * frameWidth + j].B;
            // frame2B = frame2->image->data[i * frameWidth + j].B;
            //Set pixel values in res
            res->image->data[i * frameWidth + j].L = abs(frame2->image->data[i * frameWidth + j].L - frame->image->data[i * frameWidth + j].L);
            res->image->data[i * frameWidth + j].A = abs(frame2->image->data[i * frameWidth + j].A - frame->image->data[i * frameWidth + j].A);
            res->image->data[i * frameWidth + j].B = abs(frame2->image->data[i * frameWidth + j].B - frame->image->data[i * frameWidth + j].B);
            // printf("(i,j) = (%d, %d)\n", i,j);
        }
    }
    return 0;
}

//
// Return 1 if boxes intersect, 0 if they do not
// code is based on http://gamemath.com/2011/09/detecting-whether-two-boxes-overlap/
// tolerance is the number of pixels to expand the boxes so that boxes close to each
// other are still considered overlapping
//
int boxesIntersect(box_t *box1, box_t *box2, int tolerance)
{
    if (box1->startx + box1->width + tolerance < box2->startx) {
        // box1 is left of box2
        return 0;
    }
    if (box1->startx > box2->startx + box2->width + tolerance) {
        // box1 is right of box2
        return 0;
    }
    if (box1->starty + box1->height + tolerance < box2->starty) {
        // box1 is above box2
        return 0;
    }
    if (box1->starty > box2->starty + box2->height + tolerance) {
        // box1 is below box2
        return 0;
    }
    // yup, they overlap
    return 1;

}





/*
// Create a new box with centroid (c_x, c_y), and center (center_x, center_y) height and width
int createNewBox(frame_t *frame, int c_x, int c_y, int start_x, int start_y, int width, int height) {
    //create a new bounding box in the frame

    if (height > frame->image->height) {
        LOG_ERR("createNewBox: unable to create box of height larger than image\n");
        return 1;
    }
    if (width > frame->image->width) {
        LOG_ERR("createNewBox: unable to create box of width larger than image\n");
        return 1;
    }

    if ((width <= 5) || (height <= 5)) {
        LOG_ERR("createNewBox: unable to create box of width/height less than 5\n");
        return 1;
    }
    

    // initialize values
    box_t *newB = (box_t *)malloc(sizeof(box_t));
    newB->centroid_x = c_x;
    newB->centroid_y = c_y;
    newB->startx = start_x;
    newB->starty = start_y;
    newB->center_x = start_x + width / 2;
    newB->center_y = start_y + height / 2;
    newB->height = height;
    newB->width = width;
    newB->dir = 0; 

    // Update tag
    if (frame->boxes == NULL){
        newB->tag = 0;
    } else {
        newB->tag = frame->boxes->tag + 1;
    }

    // Update linked list
    newB->next = frame->boxes;
    frame->boxes = newB;
    return 0;
}

int deleteBox(frame_t *frame, box_t *b){
    box_t *tmp = frame->boxes;
    if (tmp == NULL) {
        printf("deleteBox: No boxes found\n");
        return 1;
    }

    // Check if its the first box in the list
    if (tmp == b) {
        frame->boxes = NULL;
        return 0;
    }

    // Check the remaining boxes in the list
    while (tmp->next != NULL) {
        if (tmp->next == b) {
            tmp->next = b->next;
            free(b);
            return 0;
        }
        tmp = tmp->next;
    }
    printf("deleteBox: Unable to find box to delete\n");
    return 1;
}
*/

//
// mergeBoxes - merge overlapping boxes
//
// return 0 on success
//
int mergeBoxes(frame_t *frame)
{
    if (frame == NULL) {
        printf("ERROR: mergeBoxes called with frame == NULL\n");
        return 1;
    }
    if (frame->arBoxes == NULL) {
        printf("ERROR: mergeBoxes called with frame->arBoxes == NULL\n");
        return 1;
    }
    
    box_t *temp1;
    box_t *temp2;
    int boxUpdated;
    boxUpdated = 1;
    
    
    int i,j;

    while (boxUpdated) {    
        boxUpdated = 0;
        for ( i=0; i<frame->numBoxes; i++ ) {
            temp1 = &frame->arBoxes[i];
            if (temp1->isValid == 0) {
                continue;
            }
            for ( j=i+1; j<frame->numBoxes; j++ ) {
                temp2 = &frame->arBoxes[j];
                if (temp2->isValid == 0) {
                    continue;
                }
                if (boxesIntersect(temp1, temp2, 10)) {
                    // boxes intersect, merge them
                    int endy, endy1, endy2;
                    int endx, endx1, endx2;
                    endx1 = temp1->startx + temp1->width;
                    endx2 = temp2->startx + temp2->width;
                    endy1 = temp1->starty + temp1->height;
                    endy2 = temp2->starty + temp2->height;
                    
                    endx = MAX(endx1, endx2);
                    endy = MAX(endy1, endy2);
                    
                    int startx, starty;
                    startx = MIN(temp1->startx, temp2->startx);
                    starty = MIN(temp1->starty, temp2->starty);
                    
                    // expand the first box
                    temp1->startx = startx;
                    temp1->starty = starty;
                    temp1->width = endx - startx;
                    temp1->height = endy - starty;

                    // delete the 2nd box
                    temp2->isValid = 0;

                    // set the update flag so we rescan the boxes
                    boxUpdated = 1;
                }
            }
        }
    }
    return 0;
}






int blurImage(frame_t *frame) {
    //blurImage before thresholding
    return 0;
}

//TODO: testing in progress
int thresholdImage(frame_t *frame, frame_t *res) {
    int frameWidth = frame->image->width;
    int frameHeight = frame->image->height;
    int sigDiff = THRESHOLD_INIT_VALUE;

    // Add blurring before thresholding 
    //      --- equivalent to imfill in Matlab

    if ((frame == NULL) || (res == NULL)) {
        printf("thresholdImage: failure because frame not initialized\n");
        return 1;
    }
    if ((frame->image == NULL) || (res->image == NULL)) {
        printf("thresholdImage: failure because frame->image not initialized\n");
        return 1;
    }
    if ((res->image->width != frame->image->width) || (res->image->height != frame->image->height)) {
        printf("thresholdImage: failure because res and frame image sizes are not the same\n");
        return 1;
    } 


    int value;
    pixel_t *P; 
    for(int i = 0; i < frameHeight; i++){
        for(int j = 0; j < frameWidth; j++){
            // for now we work in RGB, 
            P = &frame->image->data[i * frameWidth + j];
            value = P->L + P->A + P->B;
        
            if(value > sigDiff)
                res->image->data[i * frameWidth + j].L = 255;
            else
                res->image->data[i * frameWidth + j].L = 0;          
            res->image->data[i*frameWidth+j].A = 0;
            res->image->data[i*frameWidth+j].B = 0;
            res->image->data[i*frameWidth+j].S = 0;
            res->image->data[i*frameWidth+j].label = 0;
        }
    }
    return 0;
}

int segmentImage(frame_t *frame, frame_t *res, unsigned long *largestLabel)  {
    //segment the image (label each connected component a different label)
    if (thresholdImage(frame, res) != 0) {
        printf("segmentImage: thresholdImage failure code\n");
        return 1;
    }

    // DISCLAIMERS: L channel - binary map after thresholding image
    //                        - contains segmented image following this func
    //              A channel - must be 0s after threshold
    //                        - "in stack" binary map use in this func only
    //              B channel - must be 0s after threshold

    // Segmentation code here - watershed
    //      START LABELS AT 1 (non-labeled remains at 0)
    int i, j, pValW, pValH;
    unsigned long label = 1;
    int rWidth = res->image->width;
    int rHeight = res->image->height;
    
    pixel_t *P;
    pixel_t *nP;
    int x, y;
    createStack();
    for (i = 0; i < rHeight; i++) {
        for (j = 0; j < rWidth; j++) {
        pValH = i;
        pValW = j;
        // Using pVal, we'll segment surrounding pixels with the same label.
        if (res->image->data[pValH*rWidth + pValW].L == 0) {
            // Pixel did not have a value
            //LOG_ERR("segmentImage: Continuing with seeds, pixel off at (w,h) -> (%d, %d)\n", pValW, pValH);
        } else {
            if (res->image->data[pValH*rWidth + pValW].label >= 1) {
            //LOG_ERR("segmentImage: Continuing with seeds, pixel already labeled at (w,h) -> (%d, %d)\n", pValW, pValH);
            } else {
            //LOG_ERR("segmentImage: Labeling connected pixels starting at (w,h) -> (%d, %d)\n", pValW, pValH);
                // Add pixels to stack 
                push(&res->image->data[pValH * rWidth + pValW], pValW, pValH);
                res->image->data[pValH*rWidth+pValW].S = 1; 
                while(isEmpty() != 0) {
                    P = pop(&x, &y);
                    if (((P->label != label) && (P->L != 0))) {
                        P->label = label;
                        //printf("\nAdding label to (%d, %d) to get (%d, %d, %d, %d, %lu)\n", y, x, P->L,P->A,P->B,P->S, P->label);
                    // Add neighboring pixels within the bounds to the stack
                        if (y-1 >= 0) {
                            nP = &res->image->data[(y-1)*rWidth+x];
                            // Check if the pixel has been in the stack
                            if (nP->S != 1) {
                                // Check if the pixel has a value
                                if(nP->L != 0 && nP->label != label){
                                    push(nP, x, y-1);
                                    nP->S = 1;
                                }
                            }
                        }
                        if (y+1 < rHeight) {
                            nP = &res->image->data[(y+1)*rWidth+x];
                            if (nP->S != 1) {
                                if(nP->L != 0 && nP->label != label){
                                    push(nP, x, y+1);
                                    nP->S = 1;
                                }
                            } 
                        }   
                        if (x-1 >= 0) {
                            nP = &res->image->data[y*rWidth+(x-1)];
                            if (nP->S != 1) {
                                if(nP->L != 0 && nP->label != label){
                                    push(nP, x-1, y);
                                    nP->S = 1;
                                }
                            }    
                        }
                        if (x+1 < rWidth) {
                            nP = &res->image->data[y*rWidth+(x+1)];
                            if (nP->S != 1) {
                                if(nP->L != 0 && nP->label != label){
                                    push(nP, x+1, y);
                                    nP->S = 1;
                                }
                            }
                        }
                    }
                }
            }

        label++;
        }
    }
}

// Other method of labelling pixels - sequential algo
#if 0
    // segment remaining pixels by looking for neighbor nearby or creating new label
    int val1, val2, val3, val4; 
    for (i = 0; i <rHeight; i++){
        for (j = 0; j < rWidth; j++) {
            val1 = 0;
            val2 = 0;
            val3 = 0;
            val4 = 0;
            // pixel has not been labelled yet
            if (res->image->data[i*rWidth+j].L == 1) {
                // give the current pixel the label of its neighbor or new label
                if (i-1 >= 0) 
                    val1 = res->image->data[(i-1)*rWidth+j].L;
                if (i+1 < rHeight)
                    val2 = res->image->data[(i+1)*rWidth+j].L;
                if (j-1 >= 0)
                    val3 = res->image->data[i*rWidth+(j-1)].L;
                if (j+1 < rWidth)
                    val4 = res->image->data[i*rWidth+(j+1)].L;
                if (val1 > 1){
                    res->image->data[i*rWidth+j].L = val1;
                } else if (val2 > 1) {
                    res->image->data[i*rWidth+j].L = val2;
                } else if (val3 > 1) {
                    res->image->data[i*rWidth+j].L = val3;
                } else if (val4 > 1) {
                    res->image->data[i*rWidth+j].L = val4;
                } else {
                    res->image->data[i*rWidth+j].L = label;
                    label++;
                }
            }
        }
    }
#endif

    *largestLabel = label;
    return 0;
}


int minBlob(int width, int height, int cx, int cy){
    // for a given centroid position, check if bbox is within min size

    int minW = 200;
    int minH = 400;

    if (width < minW) {
        //LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] < min dim of [%d, %d]\n", cx, cy, width, height, minW, minH);
        return 1;
    }
    if (height < minH) {
        //LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] < min dim of [%d, %d]\n", cx, cy, width, height, minW, minH);
        return 1; 
    }
    return 0;
}

int maxBlob(int width, int height, int cx, int cy){
    // for a given centroid position, check if bbox is within max size
    int maxW = 400;
    int maxH = 800;

    if (height > maxH) {
        //LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] > max dim of [%d, %d]\n", cx, cy, width, height, maxW, maxH);
        return 1;
    }

    if (width > maxW) {
        //LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] > max dim of [%d, %d]\n", cx, cy, width, height, maxW, maxH);
        return 1;
    }

    return 0;
}


//TODO: testing in progress
int blobDetection(frame_t *frame){
    //detect blobs in the current frame and fill out the box struct
    //      --- look into segmentation of images (blur the image first then segment)
    // don't add a blob smaller than a certain size.

    // Sanity checks
    if (frame == NULL) {
        printf("ERROR: frame is NULL in blobDetection\n");
        return 1;
    }

    if (frame->image == NULL) {
        printf("ERROR: frame->image is NULL in blobDetection\n");
        return 1;
    }

    if (frame->image->data == NULL) {
        printf("ERROR: frame->image->data is NULL in blobDetection\n");
        return 1;
    }
    
    if (frame->arBoxes != NULL) {
        printf("ERROR: frame->arBoxes is NOT NULL in blobDetection\n");
        return 1;
    }


    unsigned long largestLabel;
    
    printf("at blobDetection, about to segment Image\n");
    double sTime = CycleTimer::currentSeconds(); 
    if (segmentImage(frame, frame, &largestLabel) != 0) {
        printf("blobDetection: segmentImage failed\n");
        return 1;
    }
    double dTime = CycleTimer::currentSeconds() - sTime;
    printf("at blobDetection, segmentImage finished, time = %f\n", dTime);

//    box_t *box = frame->boxes;

//    if (box != NULL) {
//        printf("blobDetection: frame already has bounding boxes filled in!!\n");
//        printf("                free boxes before calling this function\n");
//        return 1;
//    }
    
    // first build a list of labels found in the image
    // we build it in a character array bitmap instead of a 
    // linked list so that we can implemenet parallel algorithms on it
    // e.g. prefix sum
    
    // get the current time to time the algorithm
    sTime = CycleTimer::currentSeconds(); 
    
    unsigned char *map;
    // allocate memory for it
    // we already have the maximum number of elements in 
    // largest label
    map = (unsigned char *) malloc( sizeof(char) * (largestLabel+1) );
    if (map == NULL) {
        printf("ERROR: out of memory, cannot allocate map in blobDetection\n");
        return 1;
    }
    // zero it
    memset(map, 0, largestLabel+1);
    
    // allocated memory for map reduction
    unsigned int *map2;
    map2 = (unsigned int *) malloc( sizeof(unsigned int) * (largestLabel+1) );
    if (map2 == NULL) {
        printf("ERROR: out of memory, cannot allocate map2 in blobDetection\n");
        free(map);
        return 1;
    }

    
    // loop through the image
    int label;
    int i, j;

    for (i = 0; i < frame->image->height; i++){
        for (j = 0; j < frame->image->width; j++){
            label = frame->image->data[i*frame->image->width+j].label;
            map[label] = 1;
        }
    }
    
    dTime = CycleTimer::currentSeconds() - sTime;
    
    //TODO: change this to a parallel prefix sum
    // collapse the map using sequential methods for now
    unsigned int map2maxIdx;
    unsigned int k;
    map2maxIdx = 0;
    for (k=1; k< largestLabel+1; k++) {
        if (map[k] == 1) {
            map2[map2maxIdx] = k;
            map2maxIdx++;
        }
    }
    
    double d2Time = CycleTimer::currentSeconds() - sTime - dTime;    

    // Allocate memory for arBoxes (array of boxes)
    frame->arBoxes = (box_t *) calloc(sizeof(box_t), map2maxIdx);
    if (frame->arBoxes == NULL) {
        printf("ERROR: cannot allocate arBoxes in blobDetection\n");
        free(map);
        free(map2);
        return 1;
    }
    // set the number of boxes
    frame->numBoxes = map2maxIdx;
    
    // set all the valid flags to 0
    // TODO: change this to memset?
    // NVM, used calloc instead based on 
    // http://stackoverflow.com/questions/2688466/why-mallocmemset-is-slower-than-calloc?lq=1
    // 
    unsigned int idx;

//    for ( idx=0; idx<map2maxIdx; idx++ ) {
//        frame->arBoxes[idx].isValid = 0;
//    }
    
    double d3Time = CycleTimer::currentSeconds() - sTime - d2Time;
    
    printf("at blobDetection, map finished, time = %f\n",dTime);    
    printf("at blobDetection, map2 finished, map2maxIdx = %d, time = %f\n",
                map2maxIdx, d2Time);    
    printf("at blobDetection, arBoxes malloc finished, time = %f\n",d3Time);    
    
    int left, right, up, down; 
    int x=0,y=0,count=0;
    pixel_t p;
    int w, h, cx, cy;
    //detect blobs based on size - mean of pixels connected together
    // Check the segmented pixels and create a bounding box for each segment
    for (idx = 0; idx < map2maxIdx; idx++) {
        // Add blobs based on the segment - we're done when we've looked 
        //  through the whole list of segmentations
        // Based on segmentation, decide what the centroid, width, height        
        //      We're going to go through the image by each tag to find the 
        //          corresponding left, right, up, and down of the box.
        //      THIS IS COMPUTATIONALLY EXPENSIVE
        // TODO: change this operation to look around the area instead of
        //          the entire image.

        left = frame->image->width;
        right = 0;
        up = frame->image->height;
        down = 0;
            
        count = 0;
        x = 0;
        y = 0;


        for (i = 0; i < frame->image->height; i++){
            for (j = 0; j < frame->image->width; j++){
                
                p = frame->image->data[i*frame->image->width+j];

                if (p.label == map2[idx]) {
                    // The pixel has label we're looking for, so we include it
                    //  Find the left, right, up, and down most values for the label
                    if (j < left) {
                        left = j;
                    }
                    if (j > right) {
                        right = j;
                    }
                    if (i < up) {
                        up = i;
                    }
                    if (i > down) {
                        down = i;
                    }
                    x+=j;
                    y+=i;
                    count++;
                }
            }
        }
        

        if (count == 0) {
            continue;
        }        
//        printf("loop done, tag = %d, count = %d\n",tag, count);

        // update the corresponding values for the blob
        cx = x/count;
        cy = y/count;
        w = abs(right - left);
        h = abs(up - down);

        // very simple noise remover, just count blobs with more
        // than 30 pixels
        if ((count > 30) && (w > 5) && (h > 5)) {
//            printf("adding new box at (%d,%d) centroid = (%d,%d) (w,h) = (%d,%d)\n",
//                left,up, centerx,centery, w, h);
//            createNewBox(frame, cx, cy, left, up, w, h);
            //printf("BlobDetection: x = %d, y = %d,  count = %d, left = %d, right = %d, up = %d, down = %d\n", x,y,count,left,right,up,down);

            box_t *pB;
            pB = &frame->arBoxes[idx];
            // set it to valid
            pB->isValid = 1;  
            // save the coordinates          
            pB->startx = left;
            pB->starty = up;
            pB->centroid_x = cx;
            pB->centroid_y = cy;
            pB->width = w;
            pB->height = h;
            pB->dir = 0;
            pB->tag = idx;
            pB->timeLastSeen = 0;
            pB->numPixels = count;
        }
    }
    
    // clean up memory
    free(map);
    free(map2);

    return 0;
}






/*
//TODO: testing and writing
int mergeBlobs(frame_t *frame){
    // given the image and the bounding boxes, merge close by ones
    //      update bounding box structure in img.
    if (frame == NULL) {
        printf("mergeBlobs: frame does not exist\n");
        return 1;
    }
    int tolX = 10;// tolerance is 10 pixels
    int tolY = 10;// tolerance is 10 pixels
    box_t *temp = frame->boxes;
    box_t *temp2 = frame->boxes;
    while(temp != NULL) {
        // TODO: Algorithm in O(n^2) because each box looks at every other box
        //   to see if should merge - should update this to become more efficient
        while (temp2 != NULL) {
            temp2 = temp2->next;
            // if the current box is within some tolerance of another, merge boxes
            // TODO: fix the data in which to merge blobs together
            if (tolX <= abs(temp->centroid_x - temp2->centroid_x)){
                if (tolY <= abs(temp->centroid_y - temp2->centroid_y)){
                    LOG_ERR("mergeBlobs: merging blobs within tolerance\n");   
                    // Create bounding box with new data
                    temp->centroid_x = (temp->centroid_x + temp2->centroid_x)/2;
                    temp->centroid_y = (temp->centroid_y + temp2->centroid_y)/2;
                    
                        //TODO: update the height and width of the box
                    // Remove old bounding box
                    if(deleteBox(frame, temp2)!= 0) {
                        printf("mergeBlobs: Could not delete box from frame\n");
                        return 1;
                    }
                }
            }
        }
        temp = temp->next;
        temp2 = frame->boxes;
    }
    return 0;
}
*/
int findBlobDirection(frame_t *frame, frame_t *frame2, frame_t *res){
    //based on the distance and similarities between blobs in the two images, associate the closest blob to each other and then update the direction
    if ((frame == NULL) || (frame2 == NULL) || (res == NULL)){
        printf("findBlobDirection: Can't find Blob dir - frame is not initialized\n");
    }

    //x1,y1,y2,x2 need to be updated

    // formula for dir = atan2((y2-y1),(x2-x1))* 180 / PI * -1;
    return 0;
}

box_t *copyBBoxes(frame_t *frame) {
    //based on the image, get the bounding boxes and return them
    //mallocs a new set of boxes for the array of boxes
    if (frame == NULL){
        printf("copyBBoxes: Can't get bounding box, frame is NULL\n");
        return NULL;
    }
    if (frame->arBoxes == NULL){
        LOG_ERR("copyBBoxes: no bounding boxes found\n");
        return NULL;
    }

    int numBoxes = frame->numBoxes;
    box_t *tmp = frame->arBoxes;
    //LOG_ERR("Creating %d boxes\n", numBoxes);
    box_t *res = (box_t *) malloc(sizeof(struct box_s)*numBoxes);
    /*
    int i;
    for (i = 0; i < numBoxes; i++) {
        res[i].startx = tmp[i].startx;
        res[i].starty = tmp[i].starty;
        res[i].centroid_x = tmp[i].centroid_x;
        res[i].centroid_y = tmp[i].centroid_y;
        res[i].center_x = tmp[i].center_x;
        res[i].center_x = tmp[i].center_x;
        res[i].height = tmp[i].height;
        res[i].width = tmp[i].width;
        res[i].dir = tmp[i].dir;
        res[i].tag = tmp[i].tag;
        res[i].isValid = tmp[i].isValid;
    }
    */
    memcpy((void *) res, (const void *) tmp, sizeof(struct box_s)*numBoxes);
    //LOG_ERR("Copied over boxes\n");

    return res;
}


frame_t *copyFrame(frame_t *frame) {
    if (frame == NULL) {
        LOG_ERR("copyFrame: frame is NULL\n");
        return NULL;
    }

    LOG_ERR("copyFrame: Creating new frame\n");
    frame_t *newF = (frame_t *)malloc(sizeof(struct frame_s));
    if (frame->arBoxes == NULL) {
        newF->arBoxes = NULL;
        newF->numBoxes = 0;
    } else {
        LOG_ERR("copyFrame: Copying over existing boxes\n");
        newF->arBoxes = copyBBoxes(frame);
        newF->numBoxes = frame->numBoxes;
    }
    if (frame->image == NULL) {
        newF->image = NULL;
    } else {
        newF->image = (Image_t *)malloc(sizeof(struct Image_s));
        newF->image->width = frame->image->width;
        newF->image->height = frame->image->height;
        if (frame->image->data == NULL) {
            newF->image->data = NULL;
        } else {
            LOG_ERR("copyFrame: Copying over all image data\n");
            // copy over all of the image data in single for loop
            //  - will be really slow
            // **fixed using memcpy
            
            int imageSizeInPixels;
            imageSizeInPixels = (frame->image->width)*(frame->image->height);
            int imageSizeInBytes;
            imageSizeInBytes = imageSizeInPixels * sizeof(pixel_t);
            
            newF->image->data = (pixel_t *)(malloc(imageSizeInBytes));
            /*
            int i;
            for (i = 0; i < frame->image->width*frame->image->height; i++){
                newF->image->data[i].L = frame->image->data[i].L;
                newF->image->data[i].A = frame->image->data[i].A;
                newF->image->data[i].B = frame->image->data[i].B;
            }
            */
            memcpy(newF->image->data, frame->image->data, imageSizeInBytes);
        }
    }
    return newF; 
}


//
// Draw a horizontal line in the image inside frame starting
// at (startx, starty) and going right by width
//
void drawHLine(frame_t *frame, int startx, int starty, int width, 
                unsigned char L, unsigned char A, unsigned char B)
{
    // Sanity checks
    if (frame == NULL) {
        printf("ERROR: drawHLine called with frame == NULL\n");
        return;
    }
    if (frame->image == NULL) {
        printf("ERROR: drawHLine called with frame->image == NULL\n");
        return;
    }
    if (frame->image->data == NULL) {
        printf("ERROR: drawHLine called with frame->image->data == NULL\n");
        return;
    }
    if ((frame->image->width == 0) || (frame->image->height == 0)) {
        printf("ERROR: drawHLine called with invalid image width/height\n");
        return;
    }
    // do some clipping
    if (startx >= frame->image->width) {
        printf("Warning: drawHline called with startx (%d) >= image width (%d)\n",
            startx, frame->image->width);
        return;
    }
    if (starty >= frame->image->height) {
        printf("Warning: drawHline called with starty (%d) >= image height (%d)\n",
            startx, frame->image->height);
        return;
    }
    if (startx < 0) {
        startx = 0;
    }
    if (starty < 0) {
        starty = 0;
    }
    if (width < 0) {
        printf("Warning: drawHLine called with invalid width (%d)\n",
                width);
        return;                
    }


    int endx;
    endx = startx + width;
    if (endx >= frame->image->width) {
        endx = frame->image->width - 1;
    }
    int idx;
    pixel_t *P;
    int x;
    for (x=startx; x<=endx; x++) {
        idx = starty*frame->image->width + x;
        P = &frame->image->data[idx];
        P->L = L;
        P->A = A;
        P->B = B;
    }
}



//
// Draw a vertical line in the image inside frame starting
// at (startx, starty) and going down by height
//
void drawVLine(frame_t *frame, int startx, int starty, int height, 
                unsigned char L, unsigned char A, unsigned char B)
{
    // Sanity checks
    if (frame == NULL) {
        printf("ERROR: drawVLine called with frame == NULL\n");
        return;
    }
    if (frame->image == NULL) {
        printf("ERROR: drawVLine called with frame->image == NULL\n");
        return;
    }
    if (frame->image->data == NULL) {
        printf("ERROR: drawVLine called with frame->image->data == NULL\n");
        return;
    }
    if ((frame->image->width == 0) || (frame->image->height == 0)) {
        printf("ERROR: drawVLine called with invalid image width/height\n");
        return;
    }
    // do some clipping
    if (startx >= frame->image->width) {
        printf("Warning: drawVline called with startx (%d) >= image width (%d)\n",
            startx, frame->image->width);
        return;
    }
    if (starty >= frame->image->height) {
        printf("Warning: drawVline called with starty (%d) >= image height (%d)\n",
            startx, frame->image->height);
        return;
    }
    if (startx < 0) {
        startx = 0;
    }
    if (starty < 0) {
        starty = 0;
    }
    if (height < 0) {
        printf("Warning: drawVLine called with invalid height (%d)\n",
                height);
        return;                
    }
    
    int endy;
    endy = starty + height;
    if (endy >= frame->image->height) {
        endy = frame->image->height - 1;
    }
    int idx;
    pixel_t *P;
    int y;
    for (y=starty; y<=endy; y++) {
        idx = y*frame->image->width + startx;
        P = &frame->image->data[idx];
        P->L = L;
        P->A = A;
        P->B = B;
    }
}


//
// Draw a box in the image in the frame of width, height
// starting at startx, starty, and of thickness pixels 
//
void drawBox(frame_t *frame, int startx, int starty, int width, int height,
             int thickness, unsigned char L, unsigned char A, unsigned char B)
{
    int i;
    for (i=0;i<thickness;i++) {
        drawVLine(frame, startx+i, starty, height, L, A, B);
        drawVLine(frame, startx+width-i, starty, height, L, A, B);
        drawHLine(frame, startx, starty+i, width, L, A, B);
        drawHLine(frame, startx, starty+height-i, width, L, A, B);
    }
}
             


//
// Draw the boxes from frame into res (frame is not touched)
// 
//
int drawBoxOnImage(frame_t *frame, frame_t *res) {
    // draws a white box for every bounding box given the values of frame
    if ((frame == NULL) || (res == NULL)) {
        printf("drawBoxOnImage: Can't draw box, frame is NULL\n");
        return 1;
    }

    if ((frame->image == NULL) || (res->image == NULL)) {
        printf("drawBoxOnImage: Can't draw box, frame->image is NULL\n");
        return 1;
    }    

    if ((frame->image->data == NULL) || (res->image->data == NULL)) {
        printf("drawBoxOnImage: Can't draw box, image data is empty\n");
        return 1;
    }

    // initialize
//    box_t *head = frame->boxes;
//    box_t *tmp = head;
    int fWidth = frame->image->width;
    int fHeight = frame->image->height;
    int cx;
    int cy;
    
    int i;
    box_t *pB;
    
    for ( i=0; i<frame->numBoxes; i++) {

        // point to the box
        pB = &frame->arBoxes[i];

        // check if it is a valid box        
        if (pB->isValid == 0) {
            continue;
        }
        
        //get width, height, and centroid of each box
        cx = pB->centroid_x;
        cy = pB->centroid_y;
        
        printf("drawing box at (%d,%d) (w,h)=(%d,%d)\n",
            pB->startx, pB->starty, pB->width, pB->height);
  
        // draw centroid into the result frame 
        //  box should be at least a 5 square pixels in order to view
        int pCX, pCY;
        for (pCY = 0; pCY < 5; pCY++) {
            for (pCX = 0; pCX < 5; pCX++) {
                if ((cx+3-pCX > 0) && (cx+3-pCX < fWidth)){
                    if ((cy+3-pCY) && (cy+3-pCY < fHeight)){
                        res->image->data[(cy+3-pCY)*fWidth+(cx+3-pCX)].L = 142;
                        res->image->data[(cy+3-pCY)*fWidth+(cx+3-pCX)].A = 105;
                        res->image->data[(cy+3-pCY)*fWidth+(cx+3-pCX)].L = 196;
                    }   
                }
            }
        }
        
        // draw the box!        
        drawBox(res, pB->startx, pB->starty, pB->width, pB->height, 4,
                    117, 196, 117);
    }
    return 0;
}

int frameToJPG(frame_t *frame, const char *filename){
    if (saveJpg(filename, frame->image) != 0) {
        return 0;
    } else {
        return 1;
    }
}


//TODO: add timing code here
int saveBBoxes(frame_t *frame, const char *filename) {
    double sTime = CycleTimer::currentSeconds();
    double dTime, nTime, cTime;

    FILE *pFBox = fopen(filename, "w");
    if (pFBox == NULL) {
        LOG_ERR("saveBBoxes: Unable to open file for writing bounding boxes\n");
        return 1;
    }
    dTime = CycleTimer::currentSeconds() - sTime;
    
    printf("at saveBBoxes, opened bbox file for writing in = %f\n", dTime);
    char line[1000]; //each line will be a maximum of 1000 bytes long
    const char *fLine = line;
    box_t *arBoxes = frame->arBoxes;
    int nBoxes = frame->numBoxes;

    // Write a line containing how many lines (bounding boxes) 
    //  are contained in the file
    sprintf(line, "%d BBoxes %s", nBoxes, "[startx,starty,centroid_x,centroid_y,numPixels,height,width,dir,tag,isValid,timeLastSeen]\n");
    if (fputs(fLine, pFBox) < 0) {
        LOG_ERR("saveBBoxes: Unable to write starter line to file\n");
        fclose(pFBox);
        return 1;
    }

    int i;
    // Copy data for each bounding box to a new line in the file
    for (i = 0; i < nBoxes; i++) {
        sprintf(line, "%d: %d %d %d %d %d %d %d %d %d %d %d\n", i, 
                arBoxes[i].startx, arBoxes[i].starty, arBoxes[i].centroid_x, 
                arBoxes[i].centroid_y, arBoxes[i].numPixels,
                arBoxes[i].height, arBoxes[i].width, arBoxes[i].dir, 
                arBoxes[i].tag, arBoxes[i].isValid, arBoxes[i].timeLastSeen);
        if (fputs(fLine, pFBox) < 0) {
            LOG_ERR("saveBBoxes: Unable to write line to file\n");
            fclose(pFBox);
            return 1;
        }
    }

    nTime = CycleTimer::currentSeconds() - sTime - dTime;
    printf("at saveBBoxes, wrote all boxes in = %f\n", nTime);

    fclose(pFBox);
    cTime = CycleTimer::currentSeconds() - sTime - dTime - nTime;
    printf("at saveBBoxes, closed file in = %f\n", cTime);
    return 0;
}

box_t *loadBBoxes(const char *filename, int *numBoxes) {
    double sTime = CycleTimer::currentSeconds();
    double dTime, nTime, cTime;

    FILE *pFBox = fopen(filename, "r");
    if (pFBox == NULL) {
        LOG_ERR("loadBBoxes: Unable to open file for reading bounding boxes\n");
        return NULL;
    }
    dTime = CycleTimer::currentSeconds() - sTime;
    printf("at loadBBoxes, opened bbox file for reading in = %f\n", dTime);

    const char *lineFormat = "%d: %d %d %d %d %d %d %d %d %d %d %d\n";
    int nBoxes=0;
    char tempL[1000];

    // read the first line of the file to see how many lines we'll be reading
    if (fscanf(pFBox, "%d %s %s\n", &nBoxes, tempL, tempL)!=3) {
        LOG_ERR("loadBBoxes: Unable to read first line\n");
        return NULL;
    }
    LOG_ERR("Found %d boxes, read %s before new line\n", nBoxes, tempL);
    *numBoxes = nBoxes;

    // allocate enough memory to hold all the boxes
    box_t *arBoxes = (box_t *)malloc(sizeof(struct box_s)*nBoxes);

    // read each line of the file to the specified number of boxes
    int i, temp=0, temp2=0;
    for(i=0; i < nBoxes; i++) {
        temp2 =fscanf(pFBox, lineFormat, &temp, 
                &arBoxes[i].startx, &arBoxes[i].starty, &arBoxes[i].centroid_x,
                &arBoxes[i].centroid_y, &arBoxes[i].numPixels, 
                &arBoxes[i].height, &arBoxes[i].width, &arBoxes[i].dir,
                &arBoxes[i].tag, &arBoxes[i].isValid, &arBoxes[i].timeLastSeen
            );
        if (temp2 != 12) {
            LOG_ERR("loadBBoxes: Unable to read line %d at box %d, got %d/12 args\n", i+1, temp, temp2);
            return NULL;
        }
    }
    nTime = CycleTimer::currentSeconds() - sTime - dTime;
    printf("at loadBBoxes, read all boxes in = %f\n", nTime);

    fclose(pFBox);
    cTime = CycleTimer::currentSeconds() - sTime - dTime - nTime;
    printf("at loadBBoxes, closed file in = %f\n", cTime);
   
    return arBoxes;
}


//TODO: testing
int addBoxNonValid(box_t *box, box_t *arBoxes, int *arNonValid, int *arSize, int *numBoxes) {
    int index = -1;
    
    // find the first nonValid box 
    int i;
    for (i = 0; i < *arSize; i++) {
        if (arNonValid[i] > 0) {
            index = i;
            break;
        }
    }
    
    // if we couldn't find the first nonValid box, we have to allocate a newBox array
    if (index == -1) {
        LOG_ERR("addBoxNonValid: not enough available boxes for storage, adding new boxes\n");
        // Using unbounded array style and allocating 10 more mem than needed
        int addPaddingBoxes = 11;
        box_t *newBoxes = (box_t *)malloc(sizeof(struct box_s)*(*numBoxes+addPaddingBoxes));
        memcpy(newBoxes, arBoxes, sizeof(struct box_s)*(*numBoxes));

        // set the new box to the next available index in the new array
        index = *numBoxes+1;

        // free old boxes array and reassociate to new boxes array
        free(arBoxes);
        arBoxes = newBoxes;
        *numBoxes = *numBoxes +addPaddingBoxes;
    
        // Update the non-valid tag bits in the new boxes
        int j;
        for (j = *numBoxes; j < addPaddingBoxes; j++) {
            arBoxes[j].isValid = 0;
        }
        
        LOG_ERR("addBoxNonValid: updating the nonValid array for new allocated boxes arr\n");
        // update non-valid array
        int newArSize = *numBoxes;
        int *newAr = (int *) malloc(sizeof(int)*(newArSize));
        memcpy(newAr, arNonValid, sizeof(int)*(*arSize));
        for (j = *arSize; j < newArSize; j++) {
            newAr[j] = j;
        }
        *arSize = newArSize;
        free(arNonValid);
        arNonValid = newAr;
    }
    LOG_ERR("addBoxNonValid: adding new box to tag %d\n", index);
    // Add the box to the found index
    box->isValid = 1;
    box->tag = index;
    memcpy(&arBoxes[index], box, sizeof(box)*1);

    return 0;
}


//TODO: testing
// reassociate boxes that are close to each other as the same boxes
//  once that is done, apply the direction of travel onto the boxes
int reassociateBoxes(box_t *origBox, box_t *newBox, int nOrigBoxes, int nNewBoxes, int *numBoxesAfter){
    // Do error checking
    if ((origBox == NULL) || (newBox == NULL)) {
        LOG_ERR("reassociateBoxes: input boxes are empty and non-allocated\n");
        return 1;
    }

    if ((nOrigBoxes < 0) || (nNewBoxes < 0)) {
        LOG_ERR("reassociateBoxes: no boxes found\n");
        return 1;
    }

    // --- PRE-DEFINED CONSTANTS
    int L1_maxDist = 50; // maximum L1 distance should be apart from another box (pixels)
    int max_timeLastSeen = 4; // how many frames it's ok to go without seeing the person
    int max_areaPerson = 200; // maximum area a person should take up
    int min_areaPerson = 50;  // minimum area a person should take up

    int arrNonValidSize = nNewBoxes;
    int *arrNonValid = (int *)malloc(sizeof(int)*arrNonValidSize); 
        //keep track of all non valid indicies of newBox
    // Update the non-valid array
    LOG_ERR("reassociateBoxes: creating a non-valid array \n");
    int ind;
    for (ind = 0; ind < arrNonValidSize; ind++) {
        if (newBox[ind].isValid == 0) {
            // if original box is not valid
            arrNonValid[ind] = ind;
        } else {
            arrNonValid[ind] = 0;
        }
    }

    int boxFound = 0;  // flag if we've found a box which is closeby
    
    int i;
    for (i=0; i<nOrigBoxes; i++) {
        box_t box = origBox[i];
        int minJ = 0;
        int minDiff = L1_maxDist;
        int distDiff = minDiff;
        if (box.isValid == 0) {
            // original box is not valid
            continue;
        }
        if (box.timeLastSeen > max_timeLastSeen) {
            // original box is stale on the screen
            LOG_ERR("reassociateBoxes: original box exceed max time of %d between frames\n", max_timeLastSeen);
            continue;
        }

        if (box.width*box.height > max_areaPerson) {
            //the original box is too large to be the size of the person
            //  so we'll just ignore the box
            // DO WE NEED TO FIX THIS?
            LOG_ERR("reassociateBoxes: original box larger than size of person\n");
            continue;
        }

        LOG_ERR("reassociateBoxes: looking at box %d\n", i);
         // associate each of the original boxes with a new box 
         //     and label them with a tag
         // If the original box isn't close to any of the new boxes, 
         //     increment the timeLastSeen and add the box to the newBox pointer
        int j;
        for (j=0; j<nNewBoxes; j++) {
            box_t nBox = newBox[j];
            
            distDiff = (nBox.centroid_x-box.centroid_x)
                            +(nBox.centroid_y-box.centroid_y);
            if (distDiff < minDiff) {
                // find the closest new box nearby the original box
                minJ = j;
                minDiff = distDiff;
                boxFound = 1;
            } 
        }

        // Nearby box was found for merging
        if (boxFound == 1) {
            box_t nBox = newBox[minJ];
            // TODO: check if box was heading in a direction that would
            //          lead the box off screen, if it is and associated box
            //          is a smaller size, then we should update bounding box to
            //          take new bounding box values.  If the associated box is a
            //          larger or equal size, we should leave the two boxes separate.
            
            LOG_ERR("reassociateBoxes: original box %d and new box %d to be merged\n",
                        i,minJ);
                // ---- Update new bounding box values ----
            // Calculate bounding box new bounding box coordinates
            int endy, endy1, endy2;
            int endx, endx1, endx2;
            endx1 = box.startx + box.width;
            endx2 = nBox.startx + nBox.width;
            endy1 = box.starty + box.height;
            endy2 = nBox.starty + nBox.height;

            endx = MAX(endx1, endx2);
            endy = MAX(endy1, endy2);

            int startx, starty;
            startx = MIN(box.startx, nBox.startx);
            starty = MIN(box.starty, nBox.starty);

            // Calculate new bounding box width and height    
            int width, height;
            width = endx - startx;
            height = endy - starty;

            // expand the new box       
                // Check if box size would make box larger than size of person
                //          If so, add the box instead of merging new box
            if ((width*height) > max_areaPerson) {
                LOG_ERR("reassociateBoxes: newBox is too large adding seperate boxes\n");
                box.timeLastSeen++;
                nBox.timeLastSeen++;
                addBoxNonValid(&box, newBox, arrNonValid, &arrNonValidSize, &nNewBoxes);
            } else if ((width*height) < min_areaPerson) {
                //box is too small to be a person, so we don't add the box 
                //  and remove both boxes by making the boxes invalid in new arr
                LOG_ERR("reassociateBoxes: newBox is too small to be a person, removing box\n");
                nBox.isValid = 0;
                arrNonValid[minJ] = minJ;
            } else {
                //box is within allowed size constraints
                LOG_ERR("reassociateBoxes: found movement of newBox %d, updating\n", minJ);

                // Update new position and size
                nBox.startx = startx;
                nBox.starty = starty;
                nBox.width = width;
                nBox.height = height;

                // Update new centroid and nPixels
                int nPixels=nBox.numPixels;
                int oPixels=box.numPixels;
                nBox.centroid_x = ((nBox.centroid_x*nPixels)
                                    + (box.centroid_x*oPixels))/(nPixels+oPixels);
                nBox.centroid_y = ((nBox.centroid_y*nPixels)
                                    + (box.centroid_y*oPixels))/(nPixels+oPixels);
                nBox.numPixels = nPixels+oPixels;

                // Update time last seen of box 
                nBox.timeLastSeen = 0;

                // TODO: Update direction of travel of box
                
            }
            // we've done our computation with the boxes
            boxFound = 0;
         } else {
                //TODO: check if the box was heading in a direction that would lead box 
                //          off screen.  If so, then we shouldn't add a bounding box

                // no box nearby was found, add box to the newBox pointer
                //  and increment lastSeen variable
                LOG_ERR("reassociateBoxes: no box nearby to associate box with, adding box seperately\n");
                box.timeLastSeen++;
                addBoxNonValid(&box, newBox, arrNonValid, &arrNonValidSize, &nNewBoxes);
         }        
        
    } 
    *numBoxesAfter = nNewBoxes; 
    free(arrNonValid);     
    return 0;
}
