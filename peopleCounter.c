#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "peopleCounter.h"

#include <jpeglib.h>

#define PI 3.14159265

#define DEBUG
#ifdef DEBUG
#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG_ERR(...)
#endif

// PRIORITY COMPLETION ORDER
//  (1) FrameToJPG
//  (2) FrameSubtraction (testing in progress)
//  (2) Test create boxes
//  (3) Test delete boxes
//  (4) Test print boxes
//  (3) BlobDetection
//  (4) MergeBlobs
//  (5) BlobDirection  (testing in progress)
//  (6) Test freeFrame on box linked list
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
    //TODO: subtract two frames and give back the resulting frame
    //Frames are not the same size and we're screwed
    if((frame->image->width != frame2->image->width) || 
        (frame->image->height != frame2->image->height)){
        printf("The frame sizes are not the same!");
        return -1;
    }

    int frameWidth = frame->image->width;
    int frameHeight = frame->image->height;
    unsigned char frameL= 0;
    unsigned char frame2L = 0;
    unsigned char frameA = 0;
    unsigned char frame2A = 0;
    unsigned char frameB = 0;
    unsigned char frame2B = 0;

    //Subtract frames
    for(int i = 0; i < frameWidth; i++){
        for(int j = 0; i < frameHeight; j++){
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
            res->image->data[i * frameWidth + j].L = frame2L - frameL;
            res->image->data[i * frameWidth + j].A = frame2A - frameA;
            res->image->data[i * frameWidth + j].B = frame2B - frameB;
        }
    }
    return 0;
}


//TODO: Testing in progress
// Create a new box with centroid (c_x, c_y), height and width
int createNewBox(frame_t *frame, int c_x, int c_y, int height, int width) {
    //create a new bounding box in the frame
    
    // initialize values
    box_t *newB = (box_t *)malloc(sizeof(box_t));
    newB->centroid_x = c_x;
    newB->centroid_y = c_y;
    newB->height = height;
    newB->width = width;

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

//TODO: Testing in progress
int deleteBox(frame_t *frame, box_t *b){
    box_t *tmp = frame->boxes;
    if (tmp == NULL) {
        printf("deleteBox: No boxes found\n");
        return 1;
    }
    while (tmp != NULL) {
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
    //TODO: blurImage before use in segmentation
    return 0;
}

int thresholdImage(frame_t *frame) {
    //TODO: thresholdImage for use in segmentation - create a binary image
    return 0;
}

int segmentImage(frame_t *frame) {
    //segment the image (label each connected component a different label)
    if (blurImage(frame) != 0) {
        printf("segmentImage: blurImage failure code\n");
        return 1;
    }
    if (thresholdImage(frame) != 0) {
        printf("segmentImage: thresholdImage failure code\n");
        return 1;
    }

    //TODO: add Segmentation code here - watershed

    return 0;
}


int blobDetection(frame_t *frame){
    //TODO: detect blobs in the current frame and fill out the box struct
    //      --- look into segmentation of images (blur the image first then segment)
    // don't add a blob smaller than a certain size.

    return 0;
}

int mergeBlobs(frame_t *frame){
    //TODO: given the image and the bounding boxes, merge close by ones
    //      update bounding box structure in img.
    return 0;
}

//TODO: Testing in progress
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

//TODO: Testing in progress
box_t *getBoundingBoxes(frame_t *frame) {
    //based on the image, get the bounding boxes and return them
    if (frame == NULL){
        printf("getBoundingBoxes: Can't get bounding box, frame is NULL\n");
        return NULL;
    }

    return frame->boxes;
}

//TODO: Testing in progress
int drawBoxOnImage(frame_t *frame, frame_t *res) {
    // draws a white box for every bounding box given the values of frame
    if ((frame == NULL) || (res == NULL)) {
        printf("drawBoxOnImage: Can't draw box, frame is NULL\n");
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

int frameToJPG(frame_t *frame){
    //TODO: save the given frame as a jpg image.  
    return 0;
}
