CC = g++
CFLAGS = -g -Wall -ldl -lGL -lOpenGL -lGLU -lglfw
OBJ = glad.c main.o shader.o

diffusion: $(OBJ)
	$(CC) $(CFLAGS) -o diffusion $(OBJ)

main.o: main.cpp shader.h
	$(CC) -g -c main.cpp
shader.o: shader.cpp shader.h
	$(CC) -g -c shader.cpp

clean:
	rm -f *.o diffusion
