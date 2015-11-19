CXX=g++
CC=gcc
CPPFLAGS=-fpermissive -Wdeprecated-declarations -Wall -O3
CPPSOURCES=$(wildcard *.cpp)
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
CSOURCES=$(wildcard *.c)
COBJECTS=$(CSOURCES:.c=.o)
TARGET=cameraclient

.PHONY: all clean

all: .dcpp .dc $(CPPSOURCES) $(CSOURCES) $(TARGET)

.dcpp: $(CPPSOURCES)
	$(CXX) $(CPPFLAGS) -MM $(CPPSOURCES) >.dcpp
.dc: $(CSOURCES)
	$(CC) $(CFLAGS) -MM $(CSOURCES) >.dc
-include .dcpp
-include .dc
$(TARGET): $(CPPOBJECTS) $(COBJECTS)
	$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -L/usr/local/lib -lSDL2 -lavcodec -lavfilter -lavutil -lswresample -lswscale -lavdevice -lavformat -lpostproc -lopencv_core -lopencv_highgui -lopencv_imgproc

clean:
	rm $(CPPOBJECTS) $(COBJECTS) .dcpp .dc
