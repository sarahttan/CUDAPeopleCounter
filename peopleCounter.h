#ifndef PPL_COUNTER
#define PPL_COUNTER
#include "image.h"

typedef struct box_s {
    int centroid_x; //center x of bounding box (pixels)
    int centroid_y; //center y of bounding box (pixels)
    int height;     //height of bounding box (pixels)
    int width;      //width of bounding box (pixels)
    int dir;        //direction of box travel (degrees)
    int tag;        //tag of the bounding box
} box_t;

typedef struct frame_s {
    Image_t *image;   //image this frame represents
    box_t *boxes;   //array of bounding boxes in frame
} frame_t;

// extract frames from movie and save them in a folder
// INPUT: fileName - filename of movie
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int extractFrames(char *fileName);

// free the frame structure
// INPUT: frame - frame structure
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int freeFrame(frame_t *frame);

// read image frame from memory
// INPUT: frame - frame structure to load image into
//        fileName - filename of image to read from
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int readImageFrame(frame_t *frame, char *fileName);

// subtract two frames and save them in the resulting frame
// INPUT: frame - 1st image
//        frame2 - 2nd image
//        res - result to save 1st - 2nd image into
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int frameSubtraction(frame_t *frame, frame_t *frame2, frame_t *res);

// detect blobs in the image
// INPUT: frame - image to detect blobs
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int blobDetection(frame_t *frame);

// merge similar+close blobs together
// INPUT: frame - frame to merge blobs
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int mergeBlobs(frame_t *frame);

// find the direction the blob is travelling in
// INPUT: frame - original image frame
//        frame2 - next image frame
//        res - resulting frame with updated blob directions
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int findBlobDirection(frame_t *frame, frame_t *frame2, frame_t *res);

// get the bounding box information from the frame
// INPUT: frame - image frame
// OUTPUT: boxes array
box_t *getBoundingBoxes(frame_t *frame);

// print the frame to a jpg image
// INPUT: frame - image frame
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int frameToJPG(frame_t *frame);

#endif
