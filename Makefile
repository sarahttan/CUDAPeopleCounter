all: readjpeg readImage blobDir boxCreDel frameSub copyFrame

readjpeg: readjpeg.c tests/loadjpeg.c
	gcc -ljpeg -Wall -o readjpeg readjpeg.c tests/loadjpeg.c

readImage: tests/readImageTest.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o readImageTest readjpeg.c peopleCounter.c tests/readImageTest.c

blobDir: tests/blobDirTest.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o blobDirTest readjpeg.c peopleCounter.c tests/blobDirTest.c

boxCreDel: tests/boxCreateDelete.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o boxCreateDelete readjpeg.c peopleCounter.c tests/boxCreateDelete.c

frameSub: tests/frameSubTest.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o frameSubTest readjpeg.c peopleCounter.c tests/frameSubTest.c

copyFrame: tests/copyFrameTest.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o copyFrameTest readjpeg.c peopleCounter.c tests/copyFrameTest.c

clean:
	rm readjpeg readImageTest blobDirTest boxCreateDelete frameSubTest copyFrame
