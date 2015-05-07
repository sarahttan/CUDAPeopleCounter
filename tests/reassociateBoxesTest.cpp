#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../CycleTimer.h"
#include "../peopleCounter2.h"

int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("USAGE: ./reassociateBoxes <old_box_datafile> <new_box_datafile>\n");
        printf("        Requires loadBBoxes, frameToJpg, copyBBoxes, drawBoxOnImage\n");
        return 1;
    }

    const char *origBoxFile = argv[1];
    const char *newBoxFile = argv[2];

    int nOrig = 0;
    int nNew = 0;
    int nRes = 0;

    box_t *origBox = loadBBoxes(origBoxFile, &nOrig);
    box_t *newBox = loadBBoxes(newBoxFile, &nNew);
    box_t *resBox;

    if (origBox == NULL) {
        printf("Could not load boxes from file [%s]\n", origBoxFile);
        return 1;
    } 
    
    if (newBox == NULL) {
        printf("Could not load boxes from file [%s]\n", newBoxFile);
        return 1;
    }

    frame_t *tempFrame = (frame_t *)malloc(sizeof(struct frame_s));
    tempFrame->arBoxes = origBox;
    box_t *copyOrigBox = copyBBoxes(tempFrame);

    if (copyOrigBox == NULL) {
        printf("Could not copy boxes from frame\n");
        return 1;
    }

    // ----- Testing the reassociation ---- //
    printf("---- Testing the reassociation function ----\n");
    printf("Test 1: Testing reassociation on the same input and output boxes\n");
    nRes = nOrig;
    if (reassociateBoxes(origBox, origBox, nOrig, nOrig, &nRes) == 1){
        printf("FAILURE! ReassociateBoxes failed\n");
        return 1;
    }
    if (nOrig != nRes) {
        printf("FAILURE! Number of output bboxes (%d) different from number of input (%d)\n", nOrig, nRes);
        return 1;
    }

    int i;
    for (i = 0; i < nOrig; i ++) {
        box_t *box = &copyOrigBox[i];
        box_t *rbox = &origBox[i];
        if (((rbox->isValid ==0) && (box->isValid != 0))
            || ((rbox->isValid !=0) && (box->isValid == 0))){
            printf("---box %d failed\n", i);
            continue;
        } 
         if (box->startx != rbox->startx) {
                printf("box %d startx not initialized correctly\n", i);
                //return 1;
            }
            if (box->starty != rbox->starty) {
                printf("box %d starty not initialized correctly\n", i);
                //return 1;
            }
            if (box->centroid_x != rbox->centroid_x) {
                printf("box %d centroid_x not initialized correctly (original, copied) -> (%d, %d)\n", i, box->centroid_x, rbox->centroid_x);
                //return 1;
            }
            if (box->centroid_y != rbox->centroid_y) {
                printf("box %d centroid_y not initialized correctly\n", i);
                ///return 1;
            }
            if (box->numPixels != rbox->numPixels) {
                printf("box %d center_x not initialized correctly\n", i);
                //return 1;
            }
            if (box->timeLastSeen != rbox->timeLastSeen) {
                printf("box %d center_y not initialized correctly\n", i);
                //return 1;
            }
            if (box->height != rbox->height) {
                printf("box %d height not initialized correctly\n", i);
                //return 1;
            }
            if (box->width != rbox->width) {
                printf("box %d width not initialized correctly\n", i);
                //return 1;
            }
            if (box->dir != rbox->dir) {
                printf("box %d dir not initialized correctly\n", i);
                //return 1;
            }
            if (box->tag != rbox->tag) {
                printf("box %d tag not initialized correctly\n", i);
                //return 1;
            }
            if (box->isValid != rbox->isValid) {
                printf("box %d isValid not initialized correctly\n", i);
                //return 1;
            }    
    }
    resBox = origBox;
    printf("End of test 1\n\n");


    // draw boxes on an image so we can view it.
    const char *filename = "frames/diff/frame_1.jpg";
    frame_t *frame = (frame_t *)malloc(sizeof(struct frame_s));
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
    frame->arBoxes = resBox;
    
    if (drawBoxOnImage(frame, frame) == 1) {
        printf("Error in draw Box on image\n");
        return 1;
    }

    if (frameToJPG(frame, "diff_frame1_boxTest.jpg") == 1){
        printf("Unable to save frame\n");
        return 1;
    }
    printf("----Displaying boxes on file diff_frame1_boxTest.jpg----\n"); 
 
    printf("----Freeing frame----\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }

    return 0;
}
