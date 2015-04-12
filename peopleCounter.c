#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "peopleCounter.h"
#include <jpeglib.h>

// PRIORITY COMPLETION ORDER
//  (1) ReadImageFrame
//  (2) FrameSubtraction
//  (3) BlobDetection
//  (4) MergeBlobs
//

// Input: movie file name
//          - use imagemagick to get frames and put them in a folder.
//        output frames directory - malloc here
void extractFrames(char *fileName){
    // TODO: extractFrames from movie here

}

void freeFrame(frame_t *img){
    // TODO: Free the rest of the frames structure here.
    free(img);
}

void readImageFrame(frame_t *img, char *fileName){
    //TODO: Initialize frame structure from img
    //See for reference - http://numberduck.com/Blog/?nPostId=2
}

void frameSubtraction(frame_t *img, frame_t *img2, frame_t *res){
    //TODO: subtract two grames and give back the resulting frame
}

void blobDetection(frame_t *img){
    //TODO: detect blobs in the current frame and fill out the box struct
    //      --- look into segmentation of images (blur the image first then segment)

}

void mergeBlobs(frame_t *img){
    //TODO: given the image and the bounding boxes, merge close by ones
    //      update bounding box structure in img.
}

void findBlobDirection(frame_t *img, frame_t *img2, frame_t *res){
    //TODO: based on the distance and similarities between blobs in the two images, associate the closest blob to each other and then update the direction 
}

box_t *getBoundingBoxes(frame_t *img) {
    //TODO: based on the image, get the bounding boxes and return them
    return NULL;
}
