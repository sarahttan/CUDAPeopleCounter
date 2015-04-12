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

// extract frames from movie and save them in a folder
// INPUT: fileName - filename of movie
void extractFrames(char *fileName);

// free the frame structure
// INPUT: img - frame structure
void freeFrame(frame_t *img);

// read image frame from memory
// INPUT: img - frame structure to load image into
//        fileName - filename of image to read from
void readImageFrame(frame_t *img, char *fileName);

// subtract two frames and save them in the resulting frame
// INPUT: img - 1st image
//        img2 - 2nd image
//        res - result to save 1st - 2nd image into
void frameSubtraction(frame_t *img, frame_t *img2, frame_t *res);

// detect blobs in the image
// INPUT: img - image to detect blobs
void blobDetection(frame_t *img);

// merge similar+close blobs together
// INPUT: img - img to merge blobs
void mergeBlobs(frame_t *img);

// find the direction the blob is travelling in
// INPUT: img - original image frame
//        img2 - next image frame
//        res - resulting frame with updated blob directions
void findBlobDirection(frame_t *img, frame_t *img2, frame_t *res);

// get the bounding box information from the frame
// INPUT: img - image frame
box_t *getBoundingBoxes(frame_t *img);

#endif
