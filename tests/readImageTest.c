#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "peopleCounter.h";

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("USAGE: ./readImageTest <image_filename>\n");
        return 1;
    }

    char *filename = argv[1];
    frame_t *img;
    readImageFrame(img, filename);
    return 0;
}
