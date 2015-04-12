#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "peopleCounter.h"

#include <jpeglib.h>

#define DEBUG
#ifdef DEBUG
#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG_ERR(...)
#endif

// PRIORITY COMPLETION ORDER
//  (1) ReadImageFrame - testing in progress 
//  (2) FrameSubtraction
//  (3) BlobDetection
//  (4) MergeBlobs
//  (5) FreeFrame - testing in progress

// Input: movie file name
//          - use imagemagick to get frames and put them in a folder.
//        output frames directory - malloc here

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
        if (frame->image->rgbImage != NULL){
            free(frame->image->rgbImage);
        }
        free(frame->image);
    }

    if (frame->boxes != NULL) {
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
    //TODO: subtract two grames and give back the resulting frame
    return 0;
}

int blobDetection(frame_t *frame){
    //TODO: detect blobs in the current frame and fill out the box struct
    //      --- look into segmentation of images (blur the image first then segment)

    return 0;
}

int mergeBlobs(frame_t *frame){
    //TODO: given the image and the bounding boxes, merge close by ones
    //      update bounding box structure in img.
    return 0;
}

int findBlobDirection(frame_t *frame, frame_t *frame2, frame_t *res){
    //TODO: based on the distance and similarities between blobs in the two images, associate the closest blob to each other and then update the direction 
    return 0;
}

box_t *getBoundingBoxes(frame_t *frame) {
    //TODO: based on the image, get the bounding boxes and return them
    return NULL;
}
