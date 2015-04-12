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
int extractFrames(char *fileName){
    // TODO: extractFrames from movie here
    return 0;
}

int freeFrame(frame_t *frame){
    // TODO: Free the rest of the frames structure here.
    free(frame);
    return 0;
}

int readImageFrame(frame_t *frame, char *fileName){
    //TODO: Initialize frame structure from img
    //See for reference - http://numberduck.com/Blog/?nPostId=2
    Image_t *img = (Image_t *)(malloc(sizeof(struct Image_s))); 
    if (loadJpg(fileName, img) == 1){
        printf("Unable to read image at [%s]\n", fileName);
        return -1;
    }
    frame->image = img;
    
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
