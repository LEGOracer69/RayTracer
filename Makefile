# Przykladowy Makefile dla tutorialu06 dzialajacy na pracowni
# (c) anl 2015-2017 
#
CXX= g++ -fopenmp
CFLAGS= -I/usr/local/include/GLFW/ -I./include/ -I./  -std=c++14  -Wall -Wextra -O3
LIBS=-lglfw3 -lGL  -ldl -lX11 -lXxf86vm -lXrandr -lXi -lXinerama -lXcursor -lpthread -lassimp -lfreeimage

default: main

.cpp.o:
	${CXX} -c ${CFLAGS} $<

main: main.o glad.o scene.o mesh.o model.o viewer.o camera.o renderer.o kdtree.o screen.o
	${CXX} -Wall -Wextra main.o glad.o mesh.o scene.o model.o viewer.o camera.o renderer.o kdtree.o screen.o -o main ${COMMON} ${LIBS}

main.o: main.cpp
	${CXX} ${CFLAGS} -c main.cpp -o main.o   ${COMMON} ${LIBS}

glad.o: src/glad.c
	${CXX} ${CFLAGS} -c src/glad.c -o glad.o  ${COMMON} ${LIBS}


scene.o: src/scene.cpp
	${CXX} ${CFLAGS} -c src/scene.cpp -o scene.o  ${COMMON} ${LIBS}

mesh.o: src/mesh.cpp
	${CXX} ${CFLAGS} -c src/mesh.cpp -o mesh.o  ${COMMON} ${LIBS}

model.o: src/model.cpp
	${CXX} ${CFLAGS} -Wno-unused-parameter -c src/model.cpp -o model.o  ${COMMON} ${LIBS}

viewer.o: src/viewer.cpp
	${CXX} ${CFLAGS} -c src/viewer.cpp -o viewer.o  ${COMMON} ${LIBS}

camera.o: src/camera.cpp
	${CXX} ${CFLAGS} -c -Wno-unused-parameter src/camera.cpp -o camera.o  ${COMMON} ${LIBS}

renderer.o: src/renderer.cpp
	${CXX} ${CFLAGS} -c src/renderer.cpp -o renderer.o  ${COMMON} ${LIBS}

kdtree.o: src/kdtree.cpp
	${CXX} ${CFLAGS} -c src/kdtree.cpp -o kdtree.o  ${COMMON} ${LIBS}

screen.o: src/screen.cpp
	${CXX} ${CFLAGS} -c src/screen.cpp -o screen.o  ${COMMON} ${LIBS}

clean:
	rm -f main *.o


