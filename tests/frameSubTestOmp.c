#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../peopleCounter.h"
#include "../image.h"

#define S(x) x < 0 ? abs(x) : x

int main(int argc, char *argv[]){

    clock_t begin, end;
    double time_spent;
    frame_t *frame = malloc(sizeof(struct frame_s));
    frame_t *frame2 = malloc(sizeof(struct frame_s));    
    frame_t *res = malloc(sizeof(struct frame_s));

    readImageFrame(frame, "./frames/frame01.jpg");
    frame2 = copyFrame(frame);
    //readImageFrame(frame2, "./frames/frame04.jpg");
    readImageFrame(res, "./frames/frame01.jpg");

    begin = clock();
    if (frameSubtractionOmp(frame, frame2, res) != 0) {
        printf("Error in frame subtraction\n");
    }
    end = clock();
    time_spent = (double)(end - begin) * 1000.0 / CLOCKS_PER_SEC;

    printf("time = %f\n", time_spent);  
    int i;
    pixel_t rP, fP, f2P;
    unsigned char sL, sA, sB;

    printf("Starting comparison for frame subtraction\n");
    for (i = 0; i < frame->image->height*frame->image->width; i++){
        rP = res->image->data[i];
        fP = frame->image->data[i];
        f2P = frame2->image->data[i];
        sL = S(fP.L - f2P.L);
        sA = S(fP.A - f2P.A);
        sB = S(fP.B - f2P.B);
        if (rP.L != sL){
            printf("Mismatch in pixel %d L channel [res, f1, f2] -> [%d, %d, %d], expected %d\n", i, rP.L, fP.L, f2P.L, sL);
            return 1;
        }
        if (rP.A != sA){
            printf("Mismatch in pixel %d A channel [res, f1, f2] -> [%d, %d, %d], expected %d\n", i, rP.A, fP.A, f2P.A, sA);
            return 1;
        }
        if (rP.B != sB){
            printf("Mismatch in pixel %d B channel [res, f1, f2] -> [%d, %d, %d], expected %d\n", i, rP.B, fP.B, f2P.B, sB);
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
