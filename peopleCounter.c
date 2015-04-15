#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "peopleCounter.h"

#include <jpeglib.h>
typedef struct { void * data; int pri; } q_elem_t;
typedef struct { q_elem_t *buf; int n, alloc; } pri_queue_t, *pri_queue;
 
#define priq_purge(q) (q)->n = 1
#define priq_size(q) ((q)->n - 1)
#define PI 3.14159265

#define DEBUG
#ifdef DEBUG
#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG_ERR(...)
#endif

// PRIORITY COMPLETION ORDER
//  (1) FrameToJPG
//  (2) FrameSubtraction (testing in progress)
//  (2) Test create boxes
//  (3) Test delete boxes
//  (4) Test print boxes
//  (3) BlobDetection
//  (4) MergeBlobs
//  (5) BlobDirection  (testing in progress)
//  (6) Test freeFrame on box linked list
//  (20) extractFrames

// Input: movie file name
//          - use imagemagick to get frames and put them in a folder.
int extractFrames(char *fileName){
    // TODO: extractFrames from movie here
    return 0;
}

int freeFrame(frame_t *frame){
    if (frame == NULL) {
        printf("freeFrame: Unable to free frame, frame pointer invalid\n");
        return 1;
    }
    if (frame->image != NULL) {
        if (frame->image->data != NULL){
            free(frame->image->data);
        }
        free(frame->image);
    }

    if (frame->boxes != NULL) {
        box_t *temp = frame->boxes->next;
        box_t *temp2;
        while(temp != NULL){
            temp2 = temp;
            temp = temp->next;
            free(temp2);
        }  
        free(frame->boxes);
    }
    free(frame);
    return 0;
}

int readImageFrame(frame_t *frame, char *fileName){
    // Load image into the structure
    Image_t *img = (Image_t *)(malloc(sizeof(struct Image_s))); 
    if (img == NULL) {
        printf("readImageFrame: Unable to malloc img pointer\n");
        return -1;
    }
    if (loadJpg(fileName, img) == 0){
        printf("readImageFrame: Unable to read image at [%s]\n", fileName);
        return -1;
    }
    frame->image = img;
   
    // Initialize the other variables in the structure
    frame->boxes = NULL;
    

    LOG_ERR("readImageFrame: Image height = %d, width = %d\n", img->height, img->width);
    return 0;
}

int frameSubtraction(frame_t *frame, frame_t *frame2, frame_t *res){
    //TODO: subtract two frames and give back the resulting frame
    //Frames are not the same size and we're screwed
    if((frame->image->width != frame2->image->width) || 
        (frame->image->height != frame2->image->height)){
        printf("The frame sizes are not the same!");
        return -1;
    }

    int frameWidth = frame->image->width;
    int frameHeight = frame->image->height;
    unsigned char frameL= 0;
    unsigned char frame2L = 0;
    unsigned char frameA = 0;
    unsigned char frame2A = 0;
    unsigned char frameB = 0;
    unsigned char frame2B = 0;

    //Subtract frames
    for(int i = 0; i < frameHeight; i++){
        for(int j = 0; i < frameWidth; j++){
            //Get L values
            frameL = frame->image->data[i * frameWidth + j].L;
            frame2L = frame2->image->data[i * frameWidth + j].L;
            //Get A values
            frameA = frame->image->data[i * frameWidth + j].A;
            frame2A = frame2->image->data[i * frameWidth + j].A;
            //Get B values
            frameB = frame->image->data[i * frameWidth + j].B;
            frame2B = frame2->image->data[i * frameWidth + j].B;
            //Set pixel values in res
            res->image->data[i * frameWidth + j].L = frame2L - frameL;
            res->image->data[i * frameWidth + j].A = frame2A - frameA;
            res->image->data[i * frameWidth + j].B = frame2B - frameB;
        }
    }
    return 0;
}

// Create a new box with centroid (c_x, c_y), height and width
int createNewBox(frame_t *frame, int c_x, int c_y, int height, int width) {
    //create a new bounding box in the frame

    if (height > frame->image->height) {
        LOG_ERR("createNewBox: unable to create box of height larger than image\n");
        return 1;
    }
    if (width > frame->image->width) {
        LOG_ERR("createNewBox: unable to create box of width larger than image\n");
        return 1;
    }

    if ((width == 0) || (height == 0)) {
        LOG_ERR("createNewBox: unable to create box of width/height of 0\n");
        return 1;
    }

    // initialize values
    box_t *newB = (box_t *)malloc(sizeof(box_t));
    newB->centroid_x = c_x;
    newB->centroid_y = c_y;
    newB->height = height;
    newB->width = width;

    // Update tag
    if (frame->boxes == NULL){
        newB->tag = 0;
    } else {
        newB->tag = frame->boxes->tag + 1;
    }

    // Update linked list
    newB->next = frame->boxes;
    frame->boxes = newB;
    return 0;
}

int deleteBox(frame_t *frame, box_t *b){
    box_t *tmp = frame->boxes;
    if (tmp == NULL) {
        printf("deleteBox: No boxes found\n");
        return 1;
    }

    // Check if its the first box in the list
    if (tmp == b) {
        frame->boxes = NULL;
        return 0;
    }

    // Check the remaining boxes in the list
    while (tmp->next != NULL) {
        if (tmp->next == b) {
            tmp->next = b->next;
            free(b);
            return 0;
        }
        tmp = tmp->next;
    }
    printf("deleteBox: Unable to find box to delete\n");
    return 1;
}

int blurImage(frame_t *frame) {
    //TODO: blurImage before use in segmentation
    return 0;
}

int randomNum(int last) {
    int n;
    n = rand() % last;
    return n;
}

/* first element in array not used to simplify indices */
pri_queue priq_new(int size)
{
  if (size < 4) size = 4;

  pri_queue q = malloc(sizeof(pri_queue_t));
  q->buf = malloc(sizeof(q_elem_t) * size);
  q->alloc = size;
  q->n = 1;
 
  return q;
}
 
void priq_push(pri_queue q, void *data, int pri)
{
  q_elem_t *b;
  int n, m;
 
  if (q->n >= q->alloc) {
    q->alloc *= 2;
    b = q->buf = realloc(q->buf, sizeof(q_elem_t) * q->alloc);
  } else
    b = q->buf;
 
  n = q->n++;
  /* append at end, then up heap */
  while ((m = n / 2) && pri < b[m].pri) {
    b[n] = b[m];
    n = m;
  }
  b[n].data = data;
  b[n].pri = pri;
}
 
/* remove top item. returns 0 if empty. *pri can be null. */
void * priq_pop(pri_queue q, int *pri)
{
  void *out;
  if (q->n == 1) return 0;
 
  q_elem_t *b = q->buf;
 
  out = b[1].data;
  if (pri) *pri = b[1].pri;
 
  /* pull last item to top, then down heap. */
  --q->n;
 
  int n = 1, m;
  while ((m = n * 2) < q->n) {
    if (m + 1 < q->n && b[m].pri > b[m + 1].pri) m++;
 
    if (b[q->n].pri <= b[m].pri) break;
    b[n] = b[m];
    n = m;
  }
 
  b[n] = b[q->n];
  if (q->n < q->alloc / 2 && q->n >= 16)
    q->buf = realloc(q->buf, (q->alloc /= 2) * sizeof(b[0]));
 
  return out;
}

int checkAndFill(frame_t *frame, frame_t *res, frame_t *visisted, int randWidth, int randHeight){
    int nextPixel;
    int frameWidth = frame->image->width;
    int frameHeight = frame->image->height;
    int widthNext;
    int heightNext;
    unsigned char currentVal = -1;

    //Left Most
    if(randWidth != 0){
        widthNext = randWidth - 1;
        //Left Top Corner
        if(randHeight != 0){
            heightNext = randHeight - 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 1){
                currentVal = res->image->data[heightNext * frameWidth + widthNext].L;
            }
        }
        //Left Bottom Corner
        if(randHeight != (frameHeight - 1)){
            heightNext = randHeight + 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 1){
                if ((currentVal == -1)
                    currentVal = res->image->data[heightNext * frameWidth + widthNext].L;
                else{
                    if(currentVal != res->image->data[heightNext * frameWidth + widthNext].L)
                        return -1;
                }
            }
        }
        //Left Middle
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 1){
            if ((currentVal == -1)
                currentVal = res->image->data[heightNext * frameWidth + widthNext].L;
            else{
                if(currentVal != res->image->data[heightNext * frameWidth + widthNext].L)
                    return -1;
            }
        }
    }
    //Right Most
    if(randWidth != frameWidth - 1){
        widthNext = randWidth + 1;
        //Right Top Corner
        if(randHeight != 0){
            heightNext = randHeight - 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 1){
                if ((currentVal == -1)
                    currentVal = res->image->data[heightNext * frameWidth + widthNext].L;
                else{
                    if(currentVal != res->image->data[heightNext * frameWidth + widthNext].L)
                        return -1;
                }
            }
        }
        //Right Bottom Corner
        if(randHeight != (frameHeight - 1)){
            heightNext = randHeight + 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 1){
                if ((currentVal == -1)
                    currentVal = res->image->data[heightNext * frameWidth + widthNext].L;
                else{
                    if(currentVal != res->image->data[heightNext * frameWidth + widthNext].L)
                        return -1;
                }
            }
        }
        //Right Middle
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
            visited->image->data[heightNext * frameWidth + widthNext].L == 1;
            priq_push(p, nextPixel, 0);
        }
    }
    //Middle Column
    widthNext = randWidth;
    //Middle Top
    if(randHeight != 0){
        heightNext = randHeight - 1;
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 1){
            if ((currentVal == -1)
                currentVal = res->image->data[heightNext * frameWidth + widthNext].L;
            else{
                if(currentVal != res->image->data[heightNext * frameWidth + widthNext].L)
                    return -1;
            }
        }
    }
    //Middle Bottom
    if(randHeight != (frameHeight - 1)){
        heightNext = randHeight + 1;
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 1){
            if ((currentVal == -1)
                currentVal = res->image->data[heightNext * frameWidth + widthNext].L;
            else{
                if(currentVal != res->image->data[heightNext * frameWidth + widthNext].L)
                    return -1;
            }
        }
    }
    return int(currentVal); 
}

int checkSurrounding(frame_t *frame, frame_t *res, frame_t *visisted, int randWidth, int randHeight, pri_queue q){
    int nextPixel;
    int frameWidth = frame->image->width;
    int frameHeight = frame->image->height;
    int widthNext;
    int heightNext;

    //Left Most
    if(randWidth != 0){
        widthNext = randWidth - 1;
        //Left Top Corner
        if(randHeight != 0){
            heightNext = randHeight - 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
                visited->image->data[heightNext * frameWidth + widthNext].L == 1;
                priq_push(p, nextPixel, 0);
            }
        }
        //Left Bottom Corner
        if(randHeight != (frameHeight - 1)){
            heightNext = randHeight + 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
                visited->image->data[heightNext * frameWidth + widthNext].L == 1;
                priq_push(p, nextPixel, 0);
            }
        }
        //Left Middle
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
            visited->image->data[heightNext * frameWidth + widthNext].L == 1;
            priq_push(p, nextPixel, 0);
        }
    }
    //Right Most
    if(randWidth != frameWidth - 1){
        widthNext = randWidth + 1;
        //Right Top Corner
        if(randHeight != 0){
            heightNext = randHeight - 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
                visited->image->data[heightNext * frameWidth + widthNext].L == 1;
                priq_push(p, nextPixel, 0);
            }
        }
        //Right Bottom Corner
        if(randHeight != (frameHeight - 1)){
            heightNext = randHeight + 1;
            nextPixel = heightNext * frameWidth + widthNext;
            if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
                visited->image->data[heightNext * frameWidth + widthNext].L == 1;
                priq_push(p, nextPixel, 0);
            }
        }
        //Right Middle
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
            visited->image->data[heightNext * frameWidth + widthNext].L == 1;
            priq_push(p, nextPixel, 0);
        }
    }
    //Middle Column
    widthNext = randWidth;
    //Middle Top
    if(randHeight != 0){
        heightNext = randHeight - 1;
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
            visited->image->data[heightNext * frameWidth + widthNext].L == 1;
            priq_push(p, nextPixel, 0);
        }
    }
    //Middle Bottom
    if(randHeight != (frameHeight - 1)){
        heightNext = randHeight + 1;
        nextPixel = heightNext * frameWidth + widthNext;
        if(visited->image->data[heightNext * frameWidth + widthNext].L == 0){
            visited->image->data[heightNext * frameWidth + widthNext].L == 1;
            priq_push(p, nextPixel, 0);
        }
    }
    return 0;
}

}
int thresholdImage(frame_t *frame, frame_t *res, frame_t *visisted) {
    //TODO: thresholdImage for use in segmentation - create a binary image
    int frameWidth = frame->image->width;
    int randWidth;
    int frameHeight = frame->image->height;
    int randHeight;
    int sigDiff = 10;
    int numStarting = 10;
    int pixelVal;
    int popPixel;
    unsigned char curPixel;
    pri_queue q = priq_new(0);

    for(int i = 0; i < numStarting; i++){
        randWidth = randomNum(frameWidth);
        randHeight = randomNum(frameHeight);
        curPixel = frame->image->data[randHeight * frameWidth + randWidth].L;
        if(visited->image->data[randHeight * frameWidth + randWidth].L == 0){ 
            visited->image->data[randHeight * frameWidth + randWidth].L == 1;    
            if(curPixel < sigDiff)
                res->image->data[randHeight * frameWidth + randWidth].L = 0;
            else
                res->image->data[randHeight * frameWidth + randWidth].L = 1;
            checkSurrounding(frame, res, visisted, randWidth, randHeight, q)
        }
    }

    while(priq_size(p) != 0){
        popPixel = priq_pop(q, 0);
        curPixel = frame->image->data[popPixel].L;
        pixelVal = checkAndFill(frame, res, visisted, randWidth, randHeight)
        if(pixelVal != -1)
            res->image->data[popPixel].L = pixelVal;
        else{
            if(curPixel < sigDiff)
                res->image->data[popPixel].L = 0;
            else
                res->image->data[popPixel].L = 1;
        }
        randWidth = popPixel % frameHeight;
        randHeight = popPixel / frameHeight;
        checkSurrounding(frame, res, visisted, randWidth, randHeight, q)
    }

    return 0;
}

int segmentImage(frame_t *frame) {
    //segment the image (label each connected component a different label)
    if (blurImage(frame) != 0) {
        printf("segmentImage: blurImage failure code\n");
        return 1;
    }
    if (thresholdImage(frame) != 0) {
        printf("segmentImage: thresholdImage failure code\n");
        return 1;
    }

    //TODO: add Segmentation code here - watershed

    return 0;
}


int blobDetection(frame_t *frame){
    //TODO: detect blobs in the current frame and fill out the box struct
    //      --- look into segmentation of images (blur the image first then segment)
    // don't add a blob smaller than a certain size.

    return 0;
}

int mergeBlobs(frame_t *frame){
    //TODO: given the image and the bounding boxes, merge close by ones
    //      update bounding box structure in img.
    return 0;
}

//TODO: Testing in progress
int findBlobDirection(frame_t *frame, frame_t *frame2, frame_t *res){
    //based on the distance and similarities between blobs in the two images, associate the closest blob to each other and then update the direction
    if ((frame == NULL) || (frame2 == NULL) || (res == NULL)){
        printf("findBlobDirection: Can't find Blob dir - frame is not initialized\n");
    }

    if ((frame->boxes == NULL) || (frame2->boxes == NULL) || (res->boxes == NULL)){
        printf("findBlobDirection: Can't find blob dir - frame->boxes not initialized\n");
    }

    int x1 = frame->boxes->centroid_x;
    int y1 = frame->boxes->centroid_y;
    int x2 = frame2->boxes->centroid_x;
    int y2 = frame2->boxes->centroid_y;

    res->boxes->dir = atan2((y2-y1),(x2-x1))* 180 / PI * -1;
    return 0;
}

//TODO: Testing in progress
box_t *getBoundingBoxes(frame_t *frame) {
    //based on the image, get the bounding boxes and return them
    if (frame == NULL){
        printf("getBoundingBoxes: Can't get bounding box, frame is NULL\n");
        return NULL;
    }
    if (frame->boxes == NULL){
        LOG_ERR("getBoundingBoxes: no bounding boxes found\n");
        return NULL;
    }

    //TODO: fix this to copy over all boxes and return completely new pointer
    box_t *newB = (box_t *)(malloc(sizeof(struct box_s)));
    box_t *tmp = frame->boxes;

    while(tmp != NULL) {
        
    }    


    return frame->boxes;
}

//TODO: Testing in progress
frame_t *copyFrame(frame_t *frame) {
    if (frame == NULL) {
        LOG_ERR("copyFrame: frame is NULL\n");
        return NULL;
    }
    frame_t *newF = malloc(sizeof(struct frame_s));
    if (frame->boxes == NULL) {
        newF->boxes = NULL;
    } else {
        newF->boxes = malloc(sizeof(struct box_s));
        newF->boxes = getBoundingBoxes(frame);
    }
    if (frame->image == NULL) {
        newF->image = NULL;
    } else {
        newF->image = malloc(sizeof(struct Image_s));
        newF->image->width = frame->image->width;
        newF->image->height = newF->image->height;
        if (frame->image->data == NULL) {
            newF->image->data = NULL;
        } else {
            // copy over all of the image data in single for loop
            //  - will be really slow
            int i;
            for (i = 0; i < frame->image->width*frame->image->height; i++){
                newF->image->data[i].L = frame->image->data[i].L;
                newF->image->data[i].A = frame->image->data[i].A;
                newF->image->data[i].B = frame->image->data[i].B;
            }
        }
    }
    return newF; 
}

//TODO: Testing in progress
int drawBoxOnImage(frame_t *frame, frame_t *res) {
    // draws a white box for every bounding box given the values of frame
    if ((frame == NULL) || (res == NULL)) {
        printf("drawBoxOnImage: Can't draw box, frame is NULL\n");
        return 1;
    }

    if ((frame->image == NULL) || (res->image == NULL)) {
        printf("drawBoxOnImage: Can't draw box, frame->image is NULL\n");
        return 1;
    }    

    if ((frame->image->data == NULL) || (res->image->data == NULL)) {
        printf("drawBoxOnImage: Can't draw box, image data is empty\n");
        return 1;
    }

    // initialize
    box_t *head = frame->boxes;
    box_t *tmp = head;
    int width;
    int height;
    int cx;
    int cy;
    char done = 0;

    // Check if there are boxes available
    if (head == NULL){
        LOG_ERR("drawBoxOnImage: No boxes found in frame\n");
        return 1;
    }
    while (tmp != NULL){
        //get width, height, and centroid of each box
        width = tmp->width;
        height = tmp->height;
        cx = tmp->centroid_x;
        cy = tmp->centroid_y;
    
        //draw box onto the result frame
        int i,j,cr1,cr2;
        cr1 = cx-width/2;
        cr2 = cx+width/2;
        for (j=cy-height/2; j < cy+height/2; j++){
        // draw in the left and right cols in the result image
            //Draw left column
            if ((cr1 < 0) || (cr1 >= frame->image->width)){
                // column 1 is out of bounds
                LOG_ERR("drawBoxOnImage: left column %d is out of bounds\n", cr1);
                done++;
            } else {
                // draw in the values
                res->image->data[j*width+cr1].L = 1;
                res->image->data[j*width+cr1].A = 0;
                res->image->data[j*width+cr1].B = 0;
            }
            
            // Draw right column
            if ((cr2 < 0) || (cr2 >= frame->image->width)) {
                LOG_ERR("drawBoxOnImage: right column %d is out of bounds\n",cr2);
                done++;
            } else {
                // draw in the values
                res->image->data[j*width+cr2].L = 1;
                res->image->data[j*width+cr2].A = 0;
                res->image->data[j*width+cr2].B = 0;   
            }
            if (done >= 2) {
                break;
            }
        }


        cr1 = cy-height/2;
        cr2 = cy+height/2;
        for (i=cx-width/2; i < cx+width/2; j++){
        // draw in the top and bottom rows in the result image
            //Draw top row
            if ((cr1 < 0) || (cr1 >= frame->image->height)){
                // column 1 is out of bounds
                LOG_ERR("drawBoxOnImage: top row %d is out of bounds\n", cr1);
                done++;
            } else {
                // draw in the values
                res->image->data[i*width+cr1].L = 1;
                res->image->data[i*width+cr1].A = 0;
                res->image->data[i*width+cr1].B = 0;
            }

            // Draw right column
            if ((cr2 < 0) || (cr2 >= frame->image->height)) {
                LOG_ERR("drawBoxOnImage: bottom row %d is out of bounds\n",cr2);
                done++;
            } else {
                // draw in the values
                res->image->data[i*width+cr2].L = 1;
                res->image->data[i*width+cr2].A = 0;
                res->image->data[i*width+cr2].B = 0;
            }
            if (done >= 2) {
                break;
            }
        }

        tmp = tmp->next;
    }
    return 0;
}

int frameToJPG(frame_t *frame){
    //TODO: save the given frame as a jpg image.  
    return 0;
}
