//
//  trackBoxes reads all the files in a given directory/folder 
//  looking for box_nn.txt files (for now we will limit to 2 digits)
//  It then generates a tracking file (a directed graph) as follows
//
//  sourceFrameNumber, sourceBoxNumber, nextFrameNumber, nextBoxNumber, deltaDistance
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#include <string>
#include <vector>
#include <algorithm>


#include "../CycleTimer.h"
#include "../peopleCounter2.h"


int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("USAGE: ./trackBoxes <box_directory> <result_file>\n");
        printf("        Requires loadBBoxes, frameToJpg, copyBBoxes, drawBoxOnImage\n");
        return 1;
    }




    // load the filenames from the directory
    //
    // adpated from
    // http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
    //
    
    DIR *d;
    struct dirent *dir;
    
    std::vector<std::string> arFilenames;
    
    d = opendir(argv[1]);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
//                printf("%s\n", dir->d_name);
                arFilenames.push_back(dir->d_name);
                
            }
            
        }
        closedir(d);
    } else {
        printf("Error: Could not open directory %s\n",argv[1]);
    
    }
    
    printf("----------------\n");
    printf("sorted %ld files\n", arFilenames.size());
    printf("----------------\n");
    
    std::sort(arFilenames.begin(), arFilenames.end());
    
    unsigned int i;
    
    for(i = 0; i< arFilenames.size(); i++) {
        printf("%s\n", arFilenames[i].c_str());
    }

    // create the output file
    FILE *pOutFile;
    pOutFile = fopen(argv[2],"w");
    if (pOutFile == NULL) {
        printf("Error - cannot open output file %s\n",argv[2]);
        exit(1);
    }

    
    unsigned int numFiles;
    numFiles = arFilenames.size();
    
    box_t *boxes[numFiles];
    int numBoxes[numFiles];
    std::string boxFilename;
    std::string boxPath;
    boxPath = argv[1];
    boxPath += "/";
    int maxNumBoxes;
    maxNumBoxes = 0;

    for (i=0; i<numFiles; i++) {
        // load all the boxes in
        boxFilename = boxPath + arFilenames[i];
        boxes[i] = loadBBoxes(boxFilename.c_str(), &numBoxes[i]);
        if (boxes[i] == NULL) {
            printf("Error - cannot load box[%d] file %s\n",i, arFilenames[i].c_str());
            exit(1);
        }
        // find the largest number of boxes we need
        if (numBoxes[i] > maxNumBoxes) {
            maxNumBoxes = numBoxes[i];
        }
    }
    
    printf("loaded all boxes\n");
    
    
    // Now use the box centers to find the nearest boxes 
    // TODO: filter by nearest size
    
    // do the first two files first
    
    // flag to inidicate that box has been used
    int boxUsed[maxNumBoxes];
    int j,k;

    for (j=0;j<maxNumBoxes;j++) {
        boxUsed[j] = 0;
    }

    printf("First frame (%d):\n", numBoxes[0]);
    for (j=0;j<numBoxes[0];j++) {
        if (boxes[0][j].isValid == 1) {
            printf("%d:(%d,%d) (%d,%d)\n",
                    j, boxes[0][j].startx, boxes[0][j].starty,
                       boxes[0][j].width, boxes[0][j].height);
        }                       
    }
    printf("Second frame (%d):\n", numBoxes[1]);
    for (j=0;j<numBoxes[1];j++) {
        if (boxes[1][j].isValid == 1) {
            printf("%d:(%d,%d) (%d,%d)\n",
                    j, boxes[1][j].startx, boxes[1][j].starty,
                       boxes[1][j].width, boxes[1][j].height);
        }                       
    }
    printf("-------------\n");

    for (i=0;i<numFiles-1;i++) {
        for (j=0;j<numBoxes[i];j++) {
            int matchBox;
            int center1x;
            int center1y;
            
            if (boxes[i][j].isValid == 0) {
                continue;
            }
            
            matchBox = -1;
            center1x = boxes[i][j].startx + (boxes[i][j].width / 2);
            center1y = boxes[i][j].starty + (boxes[i][j].height / 2);
            int minDistance;
            minDistance = INT_MAX;
            for (k=0;k<numBoxes[i+1];k++) {
                int center2x;
                int center2y;
                int distance2;

                if (boxes[i+1][k].isValid == 0) {
                    continue;
                }
                center2x = boxes[i+1][k].startx + (boxes[i+1][k].width / 2);
                center2y = boxes[i+1][k].starty + (boxes[i+1][k].height / 2);
                // calculate the distance squared
                distance2 = ((center2x - center1x)*(center2x - center1x)) +
                            ((center2y - center1y)*(center2y - center1y));
                if (minDistance > distance2) {
                    minDistance = distance2;
                    matchBox = k;
                }                        
            }
            printf("%d, %d, %d, %d, %d\n", i, j, i+1, matchBox, minDistance);
            fprintf(pOutFile,"%d, %d, %d, %d, %d\n", i, j, i+1, matchBox, minDistance);
            
        
        }
    }
    fclose(pOutFile);


    
    return 0;
/*


    const char *origBoxFile = argv[1];
    const char *newBoxFile = argv[2];

    int nOrig = 0;
    int nNew = 0;
    int nRes = 0;

    box_t *origBox = loadBBoxes(origBoxFile, &nOrig);
    box_t *newBox = loadBBoxes(newBoxFile, &nNew);
    box_t *resBox;

    if (origBox == NULL) {
        printf("Could not load boxes from file [%s]\n", origBoxFile);
        return 1;
    } 
    
    if (newBox == NULL) {
        printf("Could not load boxes from file [%s]\n", newBoxFile);
        return 1;
    }

    frame_t *tempFrame = (frame_t *)malloc(sizeof(struct frame_s));
    tempFrame->arBoxes = origBox;
    box_t *copyOrigBox = copyBBoxes(tempFrame);

    if (copyOrigBox == NULL) {
        printf("Could not copy boxes from frame\n");
        return 1;
    }

    // ----- Testing the reassociation ---- //
    printf("---- Testing the reassociation function ----\n");
    printf("Test 1: Testing reassociation on the same input and output boxes\n");
    nRes = nOrig;
    if (reassociateBoxes(origBox, origBox, nOrig, nOrig, &nRes) == 1){
        printf("FAILURE! ReassociateBoxes failed\n");
        return 1;
    }
    if (nOrig != nRes) {
        printf("FAILURE! Number of output bboxes (%d) different from number of input (%d)\n", nOrig, nRes);
        return 1;
    }

//    unsigned int i;
    for (i = 0; i < nOrig; i ++) {
        box_t *box = &copyOrigBox[i];
        box_t *rbox = &origBox[i];
        if (((rbox->isValid ==0) && (box->isValid != 0))
            || ((rbox->isValid !=0) && (box->isValid == 0))){
            printf("---box %d failed\n", i);
            continue;
        } 
         if (box->startx != rbox->startx) {
                printf("box %d startx not initialized correctly\n", i);
                //return 1;
            }
            if (box->starty != rbox->starty) {
                printf("box %d starty not initialized correctly\n", i);
                //return 1;
            }
            if (box->centroid_x != rbox->centroid_x) {
                printf("box %d centroid_x not initialized correctly (original, copied) -> (%d, %d)\n", i, box->centroid_x, rbox->centroid_x);
                //return 1;
            }
            if (box->centroid_y != rbox->centroid_y) {
                printf("box %d centroid_y not initialized correctly\n", i);
                ///return 1;
            }
            if (box->numPixels != rbox->numPixels) {
                printf("box %d center_x not initialized correctly\n", i);
                //return 1;
            }
            if (box->timeLastSeen != rbox->timeLastSeen) {
                printf("box %d center_y not initialized correctly\n", i);
                //return 1;
            }
            if (box->height != rbox->height) {
                printf("box %d height not initialized correctly\n", i);
                //return 1;
            }
            if (box->width != rbox->width) {
                printf("box %d width not initialized correctly\n", i);
                //return 1;
            }
            if (box->dir != rbox->dir) {
                printf("box %d dir not initialized correctly\n", i);
                //return 1;
            }
            if (box->tag != rbox->tag) {
                printf("box %d tag not initialized correctly\n", i);
                //return 1;
            }
            if (box->isValid != rbox->isValid) {
                printf("box %d isValid not initialized correctly\n", i);
                //return 1;
            }    
    }
    resBox = origBox;
    printf("End of test 1\n\n");


    // draw boxes on an image so we can view it.
    const char *filename = "frames/diff/frame_1.jpg";
    frame_t *frame = (frame_t *)malloc(sizeof(struct frame_s));
    if (readImageFrame(frame, filename) == 1) {
        printf("Unable to read frame at [%s]\n", filename);
        return 1;
    }
    frame->arBoxes = resBox;
    
    if (drawBoxOnImage(frame, frame) == 1) {
        printf("Error in draw Box on image\n");
        return 1;
    }

    if (frameToJPG(frame, "diff_frame1_boxTest.jpg") == 1){
        printf("Unable to save frame\n");
        return 1;
    }
    printf("----Displaying boxes on file diff_frame1_boxTest.jpg----\n"); 
 
    printf("----Freeing frame----\n");
    if (freeFrame(frame) == 1) {
        printf("Unable to free frame\n");
        return 1;
    }

    return 0;
    */
}
