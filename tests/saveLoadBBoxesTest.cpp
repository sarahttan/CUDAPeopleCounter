#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../CycleTimer.h"
#include "../peopleCounter2.h"

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("USAGE: ./saveLoadBBoxes <filename>\n");
        printf("        where filename - file to save bboxes to/from\n");
        printf("        requires blobDetection to work\n");
        return 1;
    }
    
    const char *saveFile = argv[1];

    const char *filename = "frames/diff/frame_1.jpg";
    frame_t *frame = (frame_t *)malloc(sizeof(struct frame_s));
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
  
    if (blobDetection(frame) == 1) {
        printf("Unable to detect blobs\n");
        return 1;
    }

    printf("----Saving Bounding Boxes to file----\n");
    if (saveBBoxes(frame, saveFile) == 1) {
        printf("FAILURE! Unable to save bounding boxes to file\n");
        return 1;
    } 
    printf("SUCCESS! Saved bounding boxes to file [%s]\n", saveFile);

    printf("----Loading Bounding Boxes from file---\n");
    int numBoxes = 0;
    box_t *newBoxes = loadBBoxes(saveFile, &numBoxes);
    if ((newBoxes == NULL) && (frame->arBoxes != NULL)){
        printf("FAILURE! No bounding boxes found in file\n");
        return 1;
    }
    printf("SUCCESS! Loaded bounding boxes from file [%s]\n", saveFile);
    
    printf("Checking if the bounding boxes match original bounding boxes in frame\n");

    if (frame->arBoxes == NULL) {
        printf("No boxes found in original array\n");
        if (newBoxes != NULL) {
            printf("FAILURE!  Boxes found after save and load\n");
            return 1;
        }
    } else {
        int i;
        box_t *pFbox = frame->arBoxes;
        for (i = 0; i < frame->numBoxes; i++) {
            box_t *box = &pFbox[i];
            box_t *rbox = &newBoxes[i];
            if (box->startx != rbox->startx) {
                printf("box %d startx not initialized correctly\n", i);
                return 1;
            } 
            if (box->starty != rbox->starty) {
                printf("box %d starty not initialized correctly\n", i);
                return 1;
            }
            if (box->centroid_x != rbox->centroid_x) {
                printf("box %d centroid_x not initialized correctly (original, copied) -> (%d, %d)\n", i, box->centroid_x, rbox->centroid_x);
                return 1;
            }
            if (box->centroid_y != rbox->centroid_y) {
                printf("box %d centroid_y not initialized correctly\n", i);
                return 1;
            }
            if (box->numPixels != rbox->numPixels) {
                printf("box %d center_x not initialized correctly\n", i);
                return 1;
            }
            if (box->timeLastSeen != rbox->timeLastSeen) {
                printf("box %d center_y not initialized correctly\n", i);
                return 1;
            }
            if (box->height != rbox->height) {
                printf("box %d height not initialized correctly\n", i);
                return 1;
            }
            if (box->width != rbox->width) {
                printf("box %d width not initialized correctly\n", i);
                return 1;
            }
            if (box->dir != rbox->dir) {
                printf("box %d dir not initialized correctly\n", i);
                return 1;
            }
            if (box->tag != rbox->tag) {
                printf("box %d tag not initialized correctly\n", i);
                return 1;
            }
            if (box->isValid != rbox->isValid) {
                printf("box %d isValid not initialized correctly\n", i);
                return 1;
            }
        }
    }

    printf("Box data after save and load is correct\n");

    printf("----Freeing frame----\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
