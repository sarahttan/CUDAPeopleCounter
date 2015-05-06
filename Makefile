all: readjpeg readImageTest blobDirTest boxCreateDelete frameSubTest frameSubTestOmp copyFrameTest writeImageTest stackTest segmentImageTest drawTest drawBoxOnImageTest saveLoadBBoxesTest fullTest

drawTest: readjpeg.c tests/drawTest.c peopleCounter.c stack.c
	gcc -ljpeg -Wall -std=c99 -lm -fopenmp -o drawTest stack.c peopleCounter.c readjpeg.c tests/drawTest.c

readjpeg: readjpeg.c tests/loadjpeg.c
	gcc -ljpeg -Wall -o readjpeg -fopenmp readjpeg.c tests/loadjpeg.c

readImageTest: tests/readImageTest.c stack.c readjpeg.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o readImageTest -fopenmp readjpeg.c stack.c peopleCounter.c tests/readImageTest.c

blobDirTest: tests/blobDirTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o blobDirTest -fopenmp readjpeg.c stack.c peopleCounter.c tests/blobDirTest.c

boxCreateDelete: tests/boxCreateDelete.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o boxCreateDelete -fopenmp readjpeg.c stack.c peopleCounter.c tests/boxCreateDelete.c

frameSubTest: tests/frameSubTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o frameSubTest -fopenmp readjpeg.c stack.c peopleCounter.c tests/frameSubTest.c

frameSubTestOmp: tests/frameSubTestOmp.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o frameSubTestOmp -fopenmp readjpeg.c stack.c peopleCounter.c tests/frameSubTestOmp.c

copyFrameTest: tests/copyFrameTest.cpp readjpeg.c stack.c peopleCounterOMP.c
	g++ -ljpeg -Wall -lm -o copyFrameTest -fopenmp readjpeg.c stack.c peopleCounterOMP.c tests/copyFrameTest.cpp

writeImageTest: tests/writeImageTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o writeImageTest -fopenmp readjpeg.c stack.c peopleCounter.c tests/writeImageTest.c

stackTest: tests/stackTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o stackTest readjpeg.c -fopenmp stack.c peopleCounter.c tests/stackTest.c

segmentImageTest: tests/segmentImageTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o segmentImageTest -fopenmp readjpeg.c stack.c peopleCounter.c tests/segmentImageTest.c

drawBoxOnImageTest: tests/drawBoxOnImageTest.c readjpeg.c stack.c peopleCounter.c
	gcc -ljpeg -Wall -std=c99 -lm -o drawBoxOnImageTest -fopenmp readjpeg.c stack.c peopleCounter.c tests/drawBoxOnImageTest.c

saveLoadBBoxesTest: tests/saveLoadBBoxesTest.cpp readjpeg.c stack.c peopleCounter2.c
	g++ -ljpeg -Wall -lm -o saveLoadBBoxes -fopenmp readjpeg.c stack.c peopleCounter2.c tests/saveLoadBBoxesTest.cpp

fullTestSOL: tests/fullTestSOL.cpp readjpeg.c stack.c peopleCounter.c
	g++ -ljpeg -Wall -lm -fopenmp -o fullTestSOL readjpeg.c stack.c peopleCounter.c tests/fullTestSOL.cpp

fullTest: tests/fullTest.cpp readjpeg.c stack.c peopleCounter2.c
	g++ -ljpeg -Wall -lm -fopenmp -o fullTest readjpeg.c stack.c peopleCounter2.c tests/fullTest.cpp

fullTestOMP: tests/fullTestOMP.cpp readjpeg.c stack.c peopleCounterOMP.c
	g++ -ljpeg -Wall -lm -fopenmp -o fullTestOMP readjpeg.c stack.c peopleCounterOMP.c tests/fullTestOMP.cpp


clean:
	rm readjpeg readImageTest blobDirTest boxCreateDelete frameSubTest frameSubTestOmp copyFrameTest writeImageTest stackTest segmentImageTest drawBoxOnImageTest fullTest



OBJDIR=objs
COBJDIR=objs
CXX=g++ -m64
CXXFLAGS=-O3 -Wall

LDFLAGS=-L/usr/local/cuda/lib64/ -lcudart -ljpeg
NVCC=nvcc
NVCCFLAGS=-O3 -m64


OBJS=$(OBJDIR)/fullTestCUDA.o  $(OBJDIR)/readjpeg.o $(OBJDIR)/stack.o $(OBJDIR)/peopleCounterCUDA.o

OBJScpTest=$(OBJDIR)/copyFrameTestCUDA.o  $(OBJDIR)/readjpeg.o $(OBJDIR)/stack.o $(OBJDIR)/peopleCounterCUDA.o

fullTestCUDA: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	
copyFrameTestCUDA: $(OBJScpTest)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJScpTest) $(LDFLAGS)

$(OBJDIR)/%.o: %.c
	$(CXX) $< $(CXXFLAGS) -c -o $@
	
$(OBJDIR)/%.o: %.cu
	$(NVCC) $< $(NVCCFLAGS) -c -o $@
			
$(OBJDIR)/%.o: tests/%.cu
	$(NVCC) $< $(NVCCFLAGS) -c -o $@

$(OBJDIR)/%.o: tests/%.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@
