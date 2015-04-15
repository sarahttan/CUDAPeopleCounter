#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){

    if (argc != 3) {
        printf("USAGE: ./boxCreateDelete <num_boxes_create> <num_boxes_delete>\n");
        printf("       num_boxes: number of boxes to create/delete\n");
        return 1;
    }

    int num_boxes_c = atoi(argv[1]);
    int num_boxes_d = atoi(argv[2]);

    frame_t *frame = (frame_t *)malloc(sizeof(struct frame_s));
    frame->boxes = NULL;
    frame->image = (Image_t *)malloc(sizeof(struct Image_s));
    frame->image->height = 999;
    frame->image->width = 499;

    int option = 0;
    int i, randDel;
    int cx, cy, w, h;
    int nc = num_boxes_c;
    box_t *temp;   
 
    while ((num_boxes_c > 0) || num_boxes_d >0) {
        switch(option) {
        // create boxes case
        case 0:
            //printf("In creating box case\n");
            srand (time(NULL));
            cx = (rand() % 1000);
            cy = (rand() % 1000);
            h = (rand() % 1000);
            w = (rand() % 500);
            printf("Creating box with centroid (%d, %d), width = %d, height = %d",cx, cy, h, w);
            createNewBox(frame, cx, cy, h, w);
            printf(": tag = %d\n", frame->boxes->tag);
            //printf("Subtracting from num_boxes_c\n");
            num_boxes_c--;
            break;
            
        //delete boxes case
        case 1:
            //printf("In deleting box case\n");
            temp = frame->boxes;
            // get total number of boxes
            srand(time(NULL));
            randDel = rand() % nc;
            printf("Deleting box %d in linked list\n", randDel);
            while(randDel > 0) {
                if (temp != NULL) {
                    temp = temp->next;
                } else {
                    printf("Not enough boxes remaining\n");
                    break; 
                }
                randDel--;
            }
            deleteBox(frame, temp);
            nc--;
            num_boxes_d--;
            break;

        default:
            break;        
        }

        if ((num_boxes_c > 0) && (num_boxes_d > 0)) {
            srand(time(NULL));
            option = rand() % 2;
        } else if (num_boxes_c > 0) {
            option = 0;
        } else {
            option = 1;
        }
        //printf("Remaining: creation - %d, deletion - %d, option=%d\n", num_boxes_c, num_boxes_d, option);
    }

    printf("Freeing frames\n");
    int err = freeFrame(frame);
    if (err != 0) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
