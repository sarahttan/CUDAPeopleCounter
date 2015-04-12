all: readjpeg pplcounter

readjpeg: readjpeg.c
	gcc -ljpeg -Wall -o readjpeg readjpeg.c

pplcounter: peopleCounter.c readjpeg.c
	gcc -ljpeg -Wall -std=c99 peopleCounter.c readjpeg.c
	
clean:
	rm readjpeg
