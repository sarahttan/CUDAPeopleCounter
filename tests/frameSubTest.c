#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../peopleCounter.h"
#include "../image.h"

int main(int argc, char *argv[]){

    frame_t *frame = malloc(sizeof(struct frame_s));
    frame_t *frame2 = malloc(sizeof(struct frame_s));    
    frame_t *res = malloc(sizeof(struct frame_s));

    readImageFrame(frame, "./frames/frame01.jpg");
    readImageFrame(frame2, "./frames/frame04.jpg");
    if (frameSubtraction(frame, frame2, res) != 0) {
        printf("Error in frame subtraction\n");
    }
    
    int i;
    pixel_t rP, fP, f2P;

    for (i = 0; i < frame->image->height*frame->image->width; i++){
        rP = res->image->data[i];
        fP = frame->image->data[i];
        f2P = frame2->image->data[i];
        if (rP.L != (fP.L - f2P.L)){
            printf("Mismatch in pixel %d L channel\n", i);
            return 1;
        }
        if (rP.A != (fP.A - f2P.A)){
            printf("Mismatch in pixel %d A channel\n", i);
            return 1;
        }
        if (rP.B != (fP.B - f2P.B)){
            printf("Mismatch in pixel %d B channel\n", i);
            return 1;
        }
    }
    
    printf("Frame subtraction worked!\n");

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
