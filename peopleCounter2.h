#ifndef PPL_COUNTER
#define PPL_COUNTER
#include "image.h"
#include "stack.h"
#include "CycleTimer.h"

typedef struct box_s {
    int startx;	    // left most coordinate of the box
    int starty;     // top most coordinate of the box 
    int centroid_x; // center of mass x-component of bounding box (pixels)
    int centroid_y; // center of mass y-component of bounding box (pixels)
    int center_x;   // center x of bounding box (pixels)
    int center_y;   // center y of bounding box (pixels)
    int height;     // height of bounding box (pixels)
    int width;      // width of bounding box (pixels)
    int dir;        // direction of box travel (degrees)
    int tag;        // tag of the bounding box
    int isValid;    // flag to indicate box is valid
    struct box_s *next;
} box_t;

typedef struct frame_s {
    Image_t *image;  	// image this frame represents
    box_t *boxes;  	// linked list of bounding boxes in frame
    box_t *arBoxes; 	// array of boxes
    int numBoxes;	// number of arBoxes
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

int frameSubtractionOmp(frame_t *frame, frame_t *frame2, frame_t *res);

int thresholdImage(frame_t *frame, frame_t *res);

int segmentImage(frame_t *frame, frame_t *res, unsigned long *largestLabel);

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

// create a new box given the centroid, height and width
// INPUT: frame - frame to add image to
//        cx - x component of centroid
//        cy - y component of centroid
//        center_x - x component of center
//        center_y - y component of center
//        height - height of bounding box
//        width - width of bounding box
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int createNewBox(frame_t *frame, int cx, int cy, int center_x, int center_y, int height, int width);

// delete a given box from the frame
// INPUT: frame - frame to delete box from
//        b - box to delete
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int deleteBox(frame_t *frame, box_t *b);

// copy and return the bounding box information from the frame
// INPUT: frame - image frame to copy boxes from
// OUTPUT: boxes linked list (malloced)
box_t *copyBoundingBoxes(frame_t *frame);

// copy and return a new frame from the given frame
// INPUT: frame - image frame to copy
// OUTPUT: new malloced frame
frame_t *copyFrame(frame_t *frame);

int drawBoxOnImage(frame_t *frame, frame_t *res);

// print the frame to a jpg image
// INPUT: frame - image frame
// OUTPUT: 1 - if an error occurred, 0 - otherwise
int frameToJPG(frame_t *frame, const char *filename);


// merge intersecting boxes
// INPUT: frame - image frame
// OUTPUT: 1 - if error, 0 - success
int mergeBoxes(frame_t *frame);

#endif
