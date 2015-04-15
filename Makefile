all: readjpeg readImage blobDir boxCreDel

readjpeg: readjpeg.c tests/loadjpeg.c
	gcc -ljpeg -Wall -o readjpeg readjpeg.c tests/loadjpeg.c

readImage: tests/readImageTest.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o readImageTest readjpeg.c peopleCounter.c tests/readImageTest.c

blobDir: tests/blobDirTest.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o blobDirTest readjpeg.c peopleCounter.c tests/blobDirTest.c

boxCreDel: tests/boxCreateDelete.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o boxCreateDelete readjpeg.c peopleCounter.c tests/boxCreateDelete.c

clean:
	rm readjpeg readImageTest blobDirTest boxCreateDelete
