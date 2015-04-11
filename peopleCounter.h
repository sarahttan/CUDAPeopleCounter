#ifndef PPL_COUNTER
#define PPL_COUNTER

typedef char pixel;
typedef struct box_s {
    int centroid_x; //center x of bounding box (pixels)
    int centroid_y; //center y of bounding box (pixels)
    int height;     //height of bounding box (pixels)
    int width;      //width of bounding box (pixels)
    int dir;        //direction of box travel (degrees)
} box_t;

typedef struct frame_s {
    pixel *image;   //image this frame represents
    box_t *boxes;   //array of bounding boxes in frame
} frame_t;

void extractFrames(char *fileName);

void freeFrame(frame_t *img);

void readImageFrame(frame_t *img, char *fileName);

void frameSubtraction(frame_t *img, frame_t *img2, frame_t *res);

void blobDetection(frame_t *img);

void mergeBlobs(frame_t *img);

void findBlobDirection(frame_t *img, frame_t *img2, frame_t *res);

box_t *getBoundingBoxes(frame_t *img);

#endif
