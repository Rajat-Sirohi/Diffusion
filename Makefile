CC = g++
CFLAGS = -g -Wall -ldl -lGL -lOpenGL -lGLU -lglfw
OBJ = glad.c main.o shader.o particle.o

diffusion: $(OBJ)
	$(CC) $(CFLAGS) -o diffusion $(OBJ)

main.o: main.cpp shader.h particle.h
	$(CC) -g -c main.cpp
shader.o: shader.cpp shader.h
	$(CC) -g -c shader.cpp
particle.o: particle.cpp particle.h
	$(CC) -g -c particle.cpp

clean:
	rm -f *.o diffusion
