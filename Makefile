all: readjpeg readImage blobDir boxCreDel frameSub copyFrame writeImage stack segmentImage

readjpeg: readjpeg.c tests/loadjpeg.c
	gcc -ljpeg -Wall -o readjpeg readjpeg.c tests/loadjpeg.c

readImage: tests/readImageTest.c stack.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o readImageTest readjpeg.c stack.c peopleCounter.c tests/readImageTest.c

blobDir: tests/blobDirTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o blobDirTest readjpeg.c stack.c peopleCounter.c tests/blobDirTest.c

boxCreDel: tests/boxCreateDelete.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o boxCreateDelete readjpeg.c stack.c peopleCounter.c tests/boxCreateDelete.c

frameSub: tests/frameSubTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o frameSubTest readjpeg.c stack.c peopleCounter.c tests/frameSubTest.c

copyFrame: tests/copyFrameTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o copyFrameTest readjpeg.c stack.c peopleCounter.c tests/copyFrameTest.c

writeImage: tests/writeImageTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o writeImageTest readjpeg.c stack.c peopleCounter.c tests/writeImageTest.c

stack: tests/stackTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o stackTest readjpeg.c stack.c peopleCounter.c tests/stackTest.c

segmentImage: tests/segmentImageTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o segmentImageTest readjpeg.c stack.c peopleCounter.c tests/segmentImageTest.c

clean:
	rm readjpeg readImageTest blobDirTest boxCreateDelete frameSubTest copyFrame writeImageTest stackTest segmentImageTest
