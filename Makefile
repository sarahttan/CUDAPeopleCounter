all: readjpeg readImage

readjpeg: readjpeg.c tests/loadjpeg.c
	gcc -ljpeg -Wall -o readjpeg readjpeg.c tests/loadjpeg.c

readImage: tests/readImageTest.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -o readImageTest readjpeg.c peopleCounter.c tests/readImageTest.c

clean:
	rm readjpeg readImageTest
