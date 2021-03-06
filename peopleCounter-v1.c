#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "peopleCounter.h"
#include <time.h>

#include <jpeglib.h>
#define PI 3.14159265
#define THRESHOLD_INIT_VALUE 150

#define DEBUG
#ifdef DEBUG
#define LOG_ERR(...) fprintf(stdout, __VA_ARGS__)
#else
#define LOG_ERR(...)
#endif

// PRIORITY COMPLETION ORDER
//  (1) FrameToJPG
//  (4) Test print boxes
//  (3) BlobDetection
//  (4) MergeBlobs
//  (5) BlobDirection  (testing in progress)
//  (20) extractFrames

// Input: movie file name
//          - use imagemagick to get frames and put them in a folder.
int extractFrames(char *fileName){
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

    if (frame->boxes != NULL) {
        box_t *temp = frame->boxes->next;
        box_t *temp2;
        while(temp != NULL){
            temp2 = temp;
            temp = temp->next;
            free(temp2);
        }  
        free(frame->boxes);
    }
    free(frame);
    return 0;
}

int readImageFrame(frame_t *frame, char *fileName){
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
    frame->boxes = NULL;
    

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

// Create a new box with centroid (c_x, c_y), height and width
int createNewBox(frame_t *frame, int c_x, int c_y, int height, int width) {
    //create a new bounding box in the frame

    if (height > frame->image->height) {
        LOG_ERR("createNewBox: unable to create box of height larger than image\n");
        return 1;
    }
    if (width > frame->image->width) {
        LOG_ERR("createNewBox: unable to create box of width larger than image\n");
        return 1;
    }

    if ((width == 0) || (height == 0)) {
        LOG_ERR("createNewBox: unable to create box of width/height of 0\n");
        return 1;
    }

    // initialize values
    box_t *newB = (box_t *)malloc(sizeof(box_t));
    newB->centroid_x = c_x;
    newB->centroid_y = c_y;
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


    for(int i = 0; i < frameHeight; i++){
        for(int j = 0; j < frameWidth; j++){
            if(frame->image->data[i * frameWidth + j].L > sigDiff)
                res->image->data[i * frameWidth + j].L = 1;
            else
                res->image->data[i * frameWidth + j].L = 0;          
            res->image->data[i*frameWidth+j].A = 0;
            res->image->data[i*frameWidth+j].B = 0;
        }
    }
    return 0;
}

int segmentImage(frame_t *frame, frame_t *res, int *largestLabel)  {
    //segment the image (label each connected component a different label)
    if (thresholdImage(frame, res) != 0) {
        printf("segmentImage: thresholdImage failure code\n");
        return 1;
    }

    // Segmentation code here - watershed
    //      START LABELS AT 2 (non-labeled remains at 0)
    int i, j, pValW, pValH, label = 2;
    int rWidth = res->image->width;
    int rHeight = res->image->height;
    
    pixel_t *P;
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
        } else if (res->image->data[pValH*rWidth + pValW].L > 1) {
            //LOG_ERR("segmentImage: Continuing with seeds, pixel already labeled at (w,h) -> (%d, %d)\n", pValW, pValH);
        } else {
            LOG_ERR("segmentImage: Labeling connected pixels starting at (w,h) -> (%d, %d)\n", pValW, pValH);
            // Add pixels to stack 
            push(&res->image->data[pValH * rWidth + pValW], pValW, pValH); 
            while(isEmpty() != 0) {
                P = pop(&x, &y);
                if (P->L == 1) {
                    P->L = label;
                    // Add neighboring pixels within the bounds to the stack
                    if (y-1 >= 0) {
                        if(res->image->data[(y-1)*rWidth+x].L == 1){
                            push(&res->image->data[(y-1)*rWidth+x], x, y-1);
                        }
                    }
                    if (y+1 < rHeight) {
                        if (res->image->data[(y+1)*rWidth+x].L == 1) {
                            push(&res->image->data[(y+1)*rWidth+x], x, y+1);
                        }   
                    }   
                    if (x-1 >= 0) {
                        if(res->image->data[y*rWidth+(x-1)].L == 1) {
                            push(&res->image->data[y*rWidth+(x-1)], x-1, y);
                        }
                    }
                    if (x+1 < rWidth) {
                        if(res->image->data[y*rWidth+(x+1)].L == 1) {
                            push(&res->image->data[y*rWidth+(x+1)], x+1, y);
                        }
                    }
                }
            }
        }

        label++;
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
        LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] < min dim of [%d, %d]\n", cx, cy, width, height, minW, minH);
        return 1;
    }
    if (height < minH) {
        LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] < min dime of [%d, %d]\n", cx, cy, width, height, minW, minH);
        return 1; 
    }
    return 0;
}

int maxBlob(int width, int height, int cx, int cy){
    // for a given centroid position, check if bbox is within max size
    int maxW = 400;
    int maxH = 800;

    if (height > maxH) {
        LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] > max dim of [%d, %d]\n", cx, cy, width, height, maxW, maxH);
        return 1;
    }

    if (width > maxW) {
        LOG_ERR("blob at (%d, %d) with size [w,h] -> [%d,%d] > max dim of [%d, %d]\n", cx, cy, width, height, maxW, maxH);
        return 1;
    }

    return 0;
}

int blobDetection(frame_t *frame){
    //detect blobs in the current frame and fill out the box struct
    //      --- look into segmentation of images (blur the image first then segment)
    // don't add a blob smaller than a certain size.
    int largestLabel;
    if (segmentImage(frame, frame, &largestLabel) != 0) {
        printf("blobDetection: segmentImage failed\n");
        return 1;
    }

    box_t *box = frame->boxes;

    if (box != NULL) {
        printf("blobDetection: frame already has bounding boxes filled in!!\n");
        printf("                free boxes before calling this function\n");
        return 1;
    }

    int i, j;
    int left, right, up, down; 
    int tag=1;
    pixel_t p;
    int w, h, cx, cy;
    int done = 0;
    //detect blobs based on size - mean of pixels connected together
    // Check the segmented pixels and create a bounding box for each segment
    while(done == 0) {
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
        tag++;       
 
        for (i = 0; i < frame->image->height; i++){
            for (j = 0; j < frame->image->width; j++){
                p = frame->image->data[i*frame->image->width+j];
                if (p.L == tag) {
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
                }
                if (tag > largestLabel) {
                    // If we looked through the largest tag value, then we're done
                    done = 1;
                }
            }
        }

        // update the corresponding values for the blob
        cx = (right-left)/2 + left;
        cy = (up - down)/2 - up;
        w = abs(right - left);
        h = abs(up - down);

        // Remove all blobs which do not fit within the constraints. 
        // Update the centroid and get min and max width and height of blob
        if (minBlob(w,h,cx,cy) != 0) {
            LOG_ERR("Blob too small, Removing blob at (cx, cy) -> (%d, %d)\n", cx, cy);
        } else if (maxBlob(w,h,cx,cy) != 0) {
            LOG_ERR("Blob too large, Splitting blob at (cx,cy) -> (%d, %d)\n", cx, cy);
            // TODO: Check if we can split the blob into multiple boxes or not
            //  for now, we'll just add the box to the list
            createNewBox(frame, cx, cy, h, w);         
        } else {
            createNewBox(frame, cx, cy, h, w);
        }
    }

    return 0;
}

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

int findBlobDirection(frame_t *frame, frame_t *frame2, frame_t *res){
    //based on the distance and similarities between blobs in the two images, associate the closest blob to each other and then update the direction
    if ((frame == NULL) || (frame2 == NULL) || (res == NULL)){
        printf("findBlobDirection: Can't find Blob dir - frame is not initialized\n");
    }

    if ((frame->boxes == NULL) || (frame2->boxes == NULL) || (res->boxes == NULL)){
        printf("findBlobDirection: Can't find blob dir - frame->boxes not initialized\n");
    }

    int x1 = frame->boxes->centroid_x;
    int y1 = frame->boxes->centroid_y;
    int x2 = frame2->boxes->centroid_x;
    int y2 = frame2->boxes->centroid_y;

    res->boxes->dir = atan2((y2-y1),(x2-x1))* 180 / PI * -1;
    return 0;
}

box_t *copyBoundingBoxes(frame_t *frame) {
    //based on the image, get the bounding boxes and return them 
    //mallocs a new set of boxes
    if (frame == NULL){
        printf("getBoundingBoxes: Can't get bounding box, frame is NULL\n");
        return NULL;
    }
    if (frame->boxes == NULL){
        LOG_ERR("getBoundingBoxes: no bounding boxes found\n");
        return NULL;
    }

    box_t *tmp = frame->boxes;
    box_t *head;
    box_t *newB;

    while(tmp != NULL) {
        newB = (box_t *)(malloc(sizeof(struct box_s)));
        newB->centroid_x = tmp->centroid_x;
        newB->centroid_y = tmp->centroid_y;
        newB->height = tmp->height;
        newB->width = tmp->width;
        newB->dir = tmp->dir;
        newB->tag = tmp->tag;
        newB->next = tmp->next;

        if (tmp == frame->boxes){
            head = newB;    
        }
        tmp = tmp->next;
    }    

    return head;
}

// TODO: frames are not the same size - TO BE FIXED?
frame_t *copyFrame(frame_t *frame) {
    if (frame == NULL) {
        LOG_ERR("copyFrame: frame is NULL\n");
        return NULL;
    }

    LOG_ERR("copyFrame: Creating new frame\n");
    frame_t *newF = (frame_t *)malloc(sizeof(struct frame_s));
    if (frame->boxes == NULL) {
        newF->boxes = NULL;
    } else {
        LOG_ERR("copyFrame: Copying over existing boxes\n");
        newF->boxes = copyBoundingBoxes(frame);
    }
    if (frame->image == NULL) {
        newF->image = NULL;
    } else {
        newF->image = (Image_t *)malloc(sizeof(struct Image_s));
        newF->image->width = frame->image->width;
        newF->image->height = newF->image->height;
        if (frame->image->data == NULL) {
            newF->image->data = NULL;
        } else {
            LOG_ERR("copyFrame: Copying over all image data\n");
            // copy over all of the image data in single for loop
            //  - will be really slow
            newF->image->data = (pixel_t *)(malloc(sizeof(struct pixel_s) *frame->image->width*frame->image->height));
            int i;
            for (i = 0; i < frame->image->width*frame->image->height; i++){
                newF->image->data[i].L = frame->image->data[i].L;
                newF->image->data[i].A = frame->image->data[i].A;
                newF->image->data[i].B = frame->image->data[i].B;
            }
        }
    }
    return newF; 
}

//TODO: Testing in progress
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
    box_t *head = frame->boxes;
    box_t *tmp = head;
    int width;
    int height;
    int cx;
    int cy;
    char done = 0;

    // Check if there are boxes available
    if (head == NULL){
        LOG_ERR("drawBoxOnImage: No boxes found in frame\n");
        return 1;
    }
    while (tmp != NULL){
        //get width, height, and centroid of each box
        width = tmp->width;
        height = tmp->height;
        cx = tmp->centroid_x;
        cy = tmp->centroid_y;
    
        //draw box onto the result frame
        int i,j,cr1,cr2;
        cr1 = cx-width/2;
        cr2 = cx+width/2;
        for (j=cy-height/2; j < cy+height/2; j++){
        // draw in the left and right cols in the result image
            //Draw left column
            if ((cr1 < 0) || (cr1 >= frame->image->width)){
                // column 1 is out of bounds
                LOG_ERR("drawBoxOnImage: left column %d is out of bounds\n", cr1);
                done++;
            } else {
                // draw in the values
                res->image->data[j*width+cr1].L = 1;
                res->image->data[j*width+cr1].A = 0;
                res->image->data[j*width+cr1].B = 0;
            }
            
            // Draw right column
            if ((cr2 < 0) || (cr2 >= frame->image->width)) {
                LOG_ERR("drawBoxOnImage: right column %d is out of bounds\n",cr2);
                done++;
            } else {
                // draw in the values
                res->image->data[j*width+cr2].L = 1;
                res->image->data[j*width+cr2].A = 0;
                res->image->data[j*width+cr2].B = 0;   
            }
            if (done >= 2) {
                break;
            }
        }


        cr1 = cy-height/2;
        cr2 = cy+height/2;
        for (i=cx-width/2; i < cx+width/2; j++){
        // draw in the top and bottom rows in the result image
            //Draw top row
            if ((cr1 < 0) || (cr1 >= frame->image->height)){
                // column 1 is out of bounds
                LOG_ERR("drawBoxOnImage: top row %d is out of bounds\n", cr1);
                done++;
            } else {
                // draw in the values
                res->image->data[i*width+cr1].L = 1;
                res->image->data[i*width+cr1].A = 0;
                res->image->data[i*width+cr1].B = 0;
            }

            // Draw right column
            if ((cr2 < 0) || (cr2 >= frame->image->height)) {
                LOG_ERR("drawBoxOnImage: bottom row %d is out of bounds\n",cr2);
                done++;
            } else {
                // draw in the values
                res->image->data[i*width+cr2].L = 1;
                res->image->data[i*width+cr2].A = 0;
                res->image->data[i*width+cr2].B = 0;
            }
            if (done >= 2) {
                break;
            }
        }

        tmp = tmp->next;
    }
    return 0;
}

//TODO: testing
int frameToJPG(frame_t *frame, char *filename){
    if (saveJpg(filename, frame->image) != 0) {
        return 0;
    } else {
        return 1;
    }
}
