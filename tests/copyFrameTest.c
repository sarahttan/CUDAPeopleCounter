#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){
    if (argc != 1) {
        printf("USAGE: ./copyFrameTest\n");
        return 1;
    }

    char *filename = "frames/frame01.jpg";
    frame_t *frame = malloc(sizeof(struct frame_s));
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
  
// Comment number of boxes we want out
    if (createNewBox(frame, 100, 200, 100, 200,10, 20) != 0) {
        printf("Unable to create box\n");
        return 1;
    }

    if (createNewBox(frame, 100, 500, 100,500,80, 20) != 0) {
        printf("Unable to create box\n");
        return 1;
    }

    if (createNewBox(frame, 100, 800, 100, 800, 10, 500) != 0) {
        printf("Unable to create box\n");
        return 1;
    }

    printf("Copying frame\n");
    frame_t *res = copyFrame(frame);
    if (res == NULL) {
        printf("Unable to copy frame\n");
        return 1;
    }

    printf("Checking all values in frame are initialized\n");
    
    int i,j;
    pixel_t p;
    for (i = 0; i < res->image->width*res->image->height; i++) {
        p = res->image->data[i];
        if ((p.L < 0) || (p.A < 0) || (p.B < 0)){
            printf("Pixel[%d] not valid\n", i);
            return 1;
        }
    }

    int null = 0;

    if (frame->boxes == NULL) {
        if (res->boxes != NULL) {
            printf("Boxes not initialized correctly\n");
            return 1;
        }
        null = 0;
    } else {
        box_t *box = frame->boxes;
        box_t *rbox = res->boxes;
        while(box != NULL) {
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
            box = box->next;
            rbox = rbox->next;
            null++;
        }
    }


    printf("Freeing frame 1\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }

    box_t *tmp = res->boxes;
    int count = 0;
    if (null == 0) {
        if (tmp != NULL) {
            printf("Box copy failure after free\n");
            return 1;
        }
    } else {
        while(tmp != NULL) {
            count ++;
            tmp = tmp->next;
        }
        if (count != null) {
            printf("Box copy failure - not same num boxes after free\n");
        }
    }   

    printf("Frames copied correctly\n"); 
 

    printf("Freeing copy frame\n");
    if (freeFrame(res) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
