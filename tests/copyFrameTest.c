#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../peopleCounter2.h"

int main(int argc, char *argv[]){
    if (argc != 1) {
        printf("USAGE: ./copyFrameTest\n");
        printf("            only works if blobDetection works\n");
        return 1;
    }

    char *filename = "frames/diff/frame_1.jpg";
    frame_t *frame = malloc(sizeof(struct frame_s));
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
  
    if (blobDetection(frame) == 1) {
        printf("Unable to detect blobs\n");
        return 1;
    }

    printf("Copying frame\n");
    frame_t *res = copyFrame(frame);
    if (res == NULL) {
        printf("Unable to copy frame\n");
        return 1;
    }

    printf("Checking all values in frame are initialized\n");
    
    int i;
    pixel_t p;
    for (i = 0; i < res->image->width*res->image->height; i++) {
        p = res->image->data[i];
        if ((p.L < 0) || (p.A < 0) || (p.B < 0)){
            printf("Pixel[%d] not valid\n", i);
            return 1;
        }
    }


    if (frame->arBoxes == NULL) {
        if (res->arBoxes != NULL) {
            printf("Boxes not initialized correctly\n");
            return 1;
        }
    } else {
        box_t *pFbox = frame->boxes;
        box_t *pRbox = res->boxes;
        if (frame->numBoxes != res->numBoxes) {
            printf("Boxes are not equal in number\n");
            return 1;
        }
        for (i = 0; i < frame->numBoxes; i++) {
            box_t *box = &pFbox[i];
            box_t *rbox = &pRbox[i];
            if (box->centroid_x != rbox->centroid_x) {
                printf("box centroid_x not initialized correctly\n");
                return 1;
            }
            if (box->centroid_y != rbox->centroid_y) {
                printf("box centroid_y not initialized correctly\n");
                return 1;
            }
            if (box->height != rbox->height) {
                printf("box height not initialized correctly\n");
                return 1;
            }
            if (box->width != rbox->width) {
                printf("box width not initialized correctly\n");
                return 1;
            }
            if (box->dir != rbox->dir) {
                printf("box dir not initialized correctly\n");
                return 1;
            }
            if (box->tag != rbox->tag) {
                printf("box tag not initialized correctly\n");
                return 1;
            }
        }
    }


    printf("Freeing frame 1\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }

    box_t *tmp = res->boxes;
    if ((res->numBoxes == 0) && (tmp != NULL)) {
            printf("Box copy failure after free\n");
    } else if ((tmp == NULL) || (sizeof(tmp)/sizeof(tmp[0]) != res->numBoxes)) {
            printf("Box copy failure - not same num boxes after free\n");
    }  else { 
        printf("Frames copied correctly\n"); 
    }

    printf("Freeing copy frame\n");
    if (freeFrame(res) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
