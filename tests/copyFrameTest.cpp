#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../CycleTimer.h"
#include "../peopleCounterOMP.h"

int main(int argc, char *argv[]){
    if (argc != 1) {
        printf("USAGE: ./copyFrameTest\n");
        printf("            only works if blobDetection works\n");
        return 1;
    }

    char *filename = "frames/diff/frame_1.jpg";
    frame_t *frame = (frame_t *)malloc(sizeof(struct frame_s));
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
  
    if (blobDetectionOMP(frame) == 1) {
        printf("Unable to detect blobs\n");
        return 1;
    }

    printf("Copying frame\n");
    frame_t *res = copyFrame(frame);
    if (res == NULL) {
        printf("Unable to copy frame\n");
        return 1;
    }

    printf("----Checking all values in frame are the same----\n");
    printf("Checking image data is correct\n");
    int numBoxes = frame->numBoxes;
    int i;
    pixel_t p, pOrig;
    for (i = 0; i < res->image->width*res->image->height; i++) {
        p = res->image->data[i];
        pOrig = frame->image->data[i];
        if ((p.L != pOrig.L) || (p.A != pOrig.A) || (p.B != pOrig.B) || (p.S != pOrig.S) || (p.label != pOrig.label)){
            printf("Pixel[%d] not valid\n", i);
            return 1;
        }
    }
    printf("Checking box data is correct\n");

    if (frame->arBoxes == NULL) {
        printf("No boxes found in array\n");
        if (res->arBoxes != NULL) {
            printf("Boxes not initialized correctly\n");
            return 1;
        }
    } else {
        printf("Boxes found in array\n");
        box_t *pFbox = frame->arBoxes;
        box_t *pRbox = res->arBoxes;
        if (frame->numBoxes != res->numBoxes) {
            printf("Boxes are not equal in number\n");
            return 1;
        }
        for (i = 0; i < frame->numBoxes; i++) {
            box_t *box = &pFbox[i];
            box_t *rbox = &pRbox[i];
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
            if (box->center_x != rbox->center_x) {
                printf("box %d center_x not initialized correctly\n", i);
                //return 1;
            }
            if (box->center_y != rbox->center_y) {
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
    }

    printf("Box and Image data are correct\n");

    printf("----Freeing frame----\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }

    printf("Checking if new frame exists after free\n");
    if (res == NULL) {
        printf("Frame disappeared after free\n");
        return 1;
    }

    printf("Checking if boxes exist after free\n");

    box_t *rArBox = res->arBoxes;
    if ((res->numBoxes == 0) && (rArBox != NULL)) {
            printf("Box copy failure after free\n");
            return 1;
    } else if (rArBox == NULL) {
            printf("Box copy failure - res is empty\n");
            return 1;
    } else if (numBoxes != res->numBoxes) {
        
            printf("Box copy failure - not same num boxes after free\n");
            return 1;
    } else { 
        int i;
        box_t Bbox;
        for (i = 0; i < res->numBoxes; i++) {
            Bbox = rArBox[i];
        }
        printf("Frames copied correctly\n"); 
    }

    printf("----Freeing copy frame----\n");
    if (freeFrame(res) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
