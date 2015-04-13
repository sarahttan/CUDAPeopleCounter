#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../peopleCounter.h"

int main(int argc, char *argv[]){

    if (argc != 5) {
        printf("USAGE: ./blobDirTest <X1> <Y1> <X2> <Y2>\n");
        return 1;
    }

    int X1 = atoi(argv[1]);
    int Y1 = atoi(argv[2]);
    int X2 = atoi(argv[3]);
    int Y2 = atoi(argv[4]);

    frame_t *frame = malloc(sizeof(struct frame_s));
    frame_t *frame2 = malloc(sizeof(struct frame_s));    
    frame_t *res = malloc(sizeof(struct frame_s));

    frame->boxes = malloc(sizeof(struct box_s));
    frame2->boxes = malloc(sizeof(struct box_s));
    res->boxes = malloc(sizeof(struct box_s));

    frame->boxes->centroid_x = X1;
    frame->boxes->centroid_y = Y1;
    frame2->boxes->centroid_x = X2;
    frame2->boxes->centroid_y = Y2;

    if (findBlobDirection(frame, frame2, res) != 0){
        printf("Error finding blob direcction\n");
    }
 
    printf("Direction is %d\n", res->boxes->dir);

    printf("Freeing frames\n");
    int err = freeFrame(frame);
    err +=  freeFrame(frame2);
    err += freeFrame(res);
    if (err != 0) {
        printf("Unable to free frame\n");
        return 1;
    }
    
    return 0;
}
