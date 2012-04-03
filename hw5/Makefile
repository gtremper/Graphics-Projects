#makefile

#Basic stuff
CC = g++ -g -Wall -O2 -fmessage-length=0

#Libraries
CCOPTS = -c -I./headers/ -I./glm-0.9.3.2
LDOPTS = -L./lib/mac -lfreeimage

#Final Files and Intermediate .o Files
SOURCES = main.cpp Scene.cpp
OBJECTS = main.o Scene.o Transform.o Shapes.o
TARGET = raytracer

#------------------------------------------------------
all: raytracer

raytracer: $(OBJECTS)
	$(CC) $(LDOPTS) $(OBJECTS) -o $(TARGET)

main.o: main.cpp Scene.cpp
	$(CC) $(CCOPTS) main.cpp

Scene.o: Scene.cpp 
	$(CC) $(CCOPTS) Scene.cpp

Transform.o: Transform.cpp
	$(CC) $(CCOPTS) Transform.cpp

Shapes.o: Shapes.cpp
	$(CC) $(CCOPTS) Shapes.cpp

default: $(TARGET)

clean:
	rm -f *.o $(TARGET)