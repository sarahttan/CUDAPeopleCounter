g++ -ljpeg -Wall -lm -o fullTest -fopenmp readjpeg.c stack.c peopleCounter2.c tests/fullTest.c
readjpeg.c: In function ‘int saveJpg(const char*, Image_t*)’:
readjpeg.c:67:42: warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
     while (cinfo.next_scanline < pImage->height) {
                                          ^
readjpeg.c:69:26: warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
         for (i=0;i<cinfo.image_width; i++) {
                          ^
peopleCounter2.c: In function ‘int blobDetection(frame_t*)’:
peopleCounter2.c:650:32: warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
                 if (p.label == tag) {
                                ^
peopleCounter2.c:669:27: warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
                 if (tag > largestLabel) {
                           ^
tests/fullTest.c: In function ‘int main(int, char**)’:
tests/fullTest.c:25:52: error: invalid conversion from ‘void*’ to ‘frame_t* {aka frame_s*}’ [-fpermissive]
     frame_t *frame1 = malloc(sizeof(struct frame_s));
                                                    ^
tests/fullTest.c:26:52: error: invalid conversion from ‘void*’ to ‘frame_t* {aka frame_s*}’ [-fpermissive]
     frame_t *frame2 = malloc(sizeof(struct frame_s));    
                                                    ^
tests/fullTest.c:27:50: error: invalid conversion from ‘void*’ to ‘frame_t* {aka frame_s*}’ [-fpermissive]
     frame_t *res1 = malloc(sizeof(struct frame_s));
                                                  ^
tests/fullTest.c:28:50: error: invalid conversion from ‘void*’ to ‘frame_t* {aka frame_s*}’ [-fpermissive]
     frame_t *res2 = malloc(sizeof(struct frame_s));
                                                  ^
tests/fullTest.c:29:50: error: invalid conversion from ‘void*’ to ‘frame_t* {aka frame_s*}’ [-fpermissive]
     frame_t *res3 = malloc(sizeof(struct frame_s));
                                                  ^
tests/fullTest.c:30:50: error: invalid conversion from ‘void*’ to ‘frame_t* {aka frame_s*}’ [-fpermissive]
     frame_t *res4 = malloc(sizeof(struct frame_s));
                                                  ^
tests/fullTest.c:41:35: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
     frameToJPG(frame1, "full1.jpg");
                                   ^
tests/fullTest.c:42:35: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
     frameToJPG(frame2, "full2.jpg");
                                   ^
tests/fullTest.c:50:33: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
     frameToJPG(res1, "full3.jpg");
                                 ^
tests/fullTest.c:57:33: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
     frameToJPG(res2, "full4.jpg");
                                 ^
tests/fullTest.c:72:33: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
     frameToJPG(res3, "full5.jpg");
                                 ^
tests/fullTest.c:86:33: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
     frameToJPG(res4, "full6.jpg");
                                 ^
make: *** [fullTest] Error 1
