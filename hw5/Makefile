#makefile

#Basic stuff
CC = g++ -g -Wall -O2 -fmessage-length=0

#Libraries
CCOPTS = -c -I./headers/ -I./glm-0.9.3.2 -fopenmp
LDOPTS = -L./lib/mac -lfreeimage -fopenmp

#Final Files and Intermediate .o Files
OBJECTS = main.o Scene.o Transform.o Shapes.o Light.o KDTree.o
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

Light.o: Light.cpp
	$(CC) $(CCOPTS) Light.cpp

KDTree.o: KDTree.cpp
	$(CC) $(CCOPTS) KDTree.cpp

default: $(TARGET)

clean:
	rm -f *.o $(TARGET)